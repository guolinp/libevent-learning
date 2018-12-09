#include <stdio.h>

#include "socket_helper.h"

int main(int argc, char **argv)
{
	char buff[128] = { 0 };
	int sockfd = new_udp_server_socket("127.0.0.1", 30000);
	int len = read(sockfd, buff, 128);

	if (len > 0)
		printf("udp_server: receive message: %s\n", buff);
	else
		printf("udp_server: receive message ERROR!\n");

	close(sockfd);

	return 0;
}
