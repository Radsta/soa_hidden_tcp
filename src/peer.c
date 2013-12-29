#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net.h>

int main(int argc, char **argv)
{
	struct net_comm *nc;
	char data[PKT_LEN];
	
	if (argc !=5) {
		printf("Usage: %s <srcIP> <srcPort> <dstIP> <dstPort>\n", argv[0]);
		return EXIT_FAILURE;
	}

	nc = nc_init(argv[1], argv[2], argv[3], argv[4]);

	if (strcmp(&argv[0][2], "client") == 0) {
		nc_send_data(nc, "", NC_SYN);		/* send SYN */
		nc_recv_data(nc, data, PKT_LEN);	/* recv SYNACK */
		nc_send_data(nc, "", NC_ACK);		/* send ACK */
		nc_send_data(nc, "test", 0);		/* send data */
	} else {
		nc_recv_data(nc, data, PKT_LEN);	/* recv SYN */
		nc_send_data(nc, "", NC_SYN | NC_ACK);	/* send SYNACK */
		nc_recv_data(nc, data, PKT_LEN);	/* recv ACK */
		nc_recv_data(nc, data, PKT_LEN);	/* recv data */
		struct tcp_header *data_tcph = (struct tcp_header*)(data + sizeof(struct iphdr));
		printf("%s\n", data_tcph->opts);
	}
	
	close(nc->sock);
	free(nc);

	return EXIT_SUCCESS;
}
