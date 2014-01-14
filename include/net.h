#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define MAX_TCP_OPTS_LEN	40
#define PKT_LEN			80

#define NC_SYN			1
#define NC_ACK			2
#define NC_FIN			4

struct tcp_header {
	uint16_t		srcp;	/* Source port */
	uint16_t		dstp;	/* Destination port */
	uint32_t		seqn;	/* Sequence number */
	uint32_t		ackn;	/* Acknowledgement number */
	uint16_t		ns:1;	/* ECN nonce concealment protection */
	uint16_t		res:3;	/* Reserved */
	uint16_t		doff:4;	/* Data offset */
	uint16_t		fin:1;	/* No more data from sender */
	uint16_t		syn:1;	/* Synchronize sequence numbers. */
	uint16_t		rst:1;	/* Reset the connection */
	uint16_t		psh:1;	/* Push function. */
	uint16_t		ack:1;	/* Indicates ackn is relevant. */
	uint16_t		urg:1;	/* Indicates urgp is relevant. */
	uint16_t		ece:1;	/* ECN Echo: SYN = 1 => TCP peer is ECN capable
						     SYN = 0 => CE flag set in IP header
					   ECN = explicit congestion notification
					   CE = congestion experienced
					*/
	uint16_t		cwr:1;	/* Congestion window reduced */
	uint16_t		win_sz;	/* Window size */
	uint16_t		chksum;	/* Checksum */
	uint16_t		urgp;	/* Offset from seqn, marks last urgent data byte */
	char			opts[MAX_TCP_OPTS_LEN];
};

struct net_comm {

	struct sockaddr_in sin, din;
	struct iphdr *iph;
	struct tcp_header *tcph;
	char buffer[PKT_LEN];
	int sock;
};

struct net_comm *nc_init(char *src_ip, char *src_port, char *dst_ip, char *dst_port);

void nc_send_data(struct net_comm *nc, const char *data, int flags);

void nc_recv_data(struct net_comm *nc, char *data, int len);
