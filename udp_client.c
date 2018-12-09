#include <stdio.h>
#include <string.h>

#include "socket_helper.h"

int main(int argc, char **argv)
{
	const char *message = "this is a message from udp client";

	int sockfd = new_udp_client_socket("127.0.0.1", 30000);

	int ret = write(sockfd, message, strlen(message));

	if (ret > 0)
		printf("udp_client: send message: %s\n", message);
	else
		printf("udp_client: failed to send message: %s\n", message);

	close(sockfd);

	return 0;
}
