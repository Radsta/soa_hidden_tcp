#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net.h>

static uint16_t nc_checksum(uint16_t *buf, unsigned int len)
{
	unsigned long sum;
	for (sum = 0; len > 0; len--)
		sum += *buf++;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (uint16_t)(~sum);
}

struct net_comm *nc_init(char *src_ip, char *src_port, char *dst_ip, char *dst_port)
{
	struct net_comm *result = (struct net_comm*)malloc(sizeof(struct net_comm));
	int one = 1;
	if (result == NULL) {
		perror("Failed to allocate memory for peer.");
		exit(EXIT_FAILURE);
	}

	memset(result->buffer, 0, PKT_LEN);

	result->sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	if (result->sock < 0) {
		perror("Failed to create raw socket.");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(result->sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
		perror("Failed to set socket options.");
		exit(EXIT_FAILURE);
	}

	result->sin.sin_family = result->din.sin_family = AF_INET;
	result->sin.sin_port = htons(atoi(src_port));
	result->din.sin_port = htons(atoi(dst_port));
	result->sin.sin_addr.s_addr = inet_addr(src_ip);
	result->din.sin_addr.s_addr = inet_addr(dst_ip);

	result->iph = (struct iphdr*)result->buffer;
	result->tcph = (struct tcp_header*)(result->buffer + sizeof(struct iphdr));

	/* syn, ack, seqn and ackn must be set each time */
	result->tcph->srcp = htons(atoi(src_port));
	result->tcph->dstp = htons(atoi(dst_port));
	result->tcph->doff = 15;
	result->tcph->win_sz = htons(32767);
	result->tcph->chksum = 0;
	result->tcph->urgp = 0;
	
	/* check must be calculated each time */
	result->iph->ihl = 5;
	result->iph->version = 4;
	result->iph->tos = 16;
	result->iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcp_header);
	result->iph->id = 1234;
	result->iph->frag_off = 0;
	result->iph->ttl = MAXTTL;
	result->iph->protocol = 6;
	result->iph->saddr = inet_addr(src_ip);
	result->iph->daddr = inet_addr(dst_ip);

	return result;
}

static void nc_reset_state(struct net_comm *nc)
{
	nc->iph->saddr = nc->sin.sin_addr.s_addr;
	nc->iph->daddr = nc->din.sin_addr.s_addr;
	nc->tcph->srcp = nc->sin.sin_port;
	nc->tcph->dstp = nc->din.sin_port;
	nc->tcph->ack = nc->tcph->syn = nc->tcph->fin = 0;
}

void nc_send_data(struct net_comm *nc, const char *data, int flags)
{
	ssize_t sent;
	if (flags & NC_SYN) {
		nc->tcph->syn = 1;
		nc->tcph->seqn = random();
		/* TCP Alternate Checksum Request */
		nc->tcph->opts[0] = 14; /* Type */
		nc->tcph->opts[1] = 3;  /* Length */
		nc->tcph->opts[2] = 2;  /* Value */
		nc->tcph->opts[3] = 0;  /* End of options */
	}
	if (flags & NC_ACK) {
		nc->tcph->ack = 1;
	}
	if (flags & NC_FIN) {
		nc->tcph->fin = 1;
	}
	if (flags == 0) {
		size_t len = strlen(data);
		if (len > MAX_TCP_OPTS_LEN - 3)
			len = MAX_TCP_OPTS_LEN - 3;
		/* TCP Alternate Checksum Data */
		nc->tcph->opts[0] = 15; 			/* Type */
		nc->tcph->opts[1] = MAX_TCP_OPTS_LEN - 1;	/* Length */
		memcpy(&nc->tcph->opts[2], data, len);		/* Value */
		nc->tcph->opts[MAX_TCP_OPTS_LEN - 1] = 0;	/* End of options list */
	}
	nc->iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcp_header);
	nc->iph->check = nc_checksum((uint16_t*)nc->buffer, (sizeof(struct iphdr) + sizeof(struct tcp_header)));
	struct sockaddr_in dst = nc->din;
	struct sockaddr_in *dst_ptr = &dst;
	sent = sendto(nc->sock, nc->buffer, nc->iph->tot_len,
		      0, (struct sockaddr*)dst_ptr, sizeof(dst));
	nc_reset_state(nc);
	nc->tcph->seqn += sent;
}

void nc_recv_data(struct net_comm *nc, char *data, int len)
{
	ssize_t received;
	socklen_t aux;
	struct sockaddr_in dst = nc->din;
	struct sockaddr_in *dst_ptr = &dst;
	received = recvfrom(nc->sock, nc->buffer, len,
			    0, (struct sockaddr*)dst_ptr, &aux);
	nc->tcph->ackn += received;
	memcpy(data, &nc->tcph->opts[2], MAX_TCP_OPTS_LEN - 2);
	nc_reset_state(nc);
}
