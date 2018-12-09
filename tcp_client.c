#include <stdio.h>
#include <string.h>

#include "socket_helper.h"

int main(int argc, char **argv)
{
	const char *message = "this is a message from tcp client";

	int sockfd = new_tcp_client_socket("127.0.0.1", 20000);

	int ret = write(sockfd, message, strlen(message));

	if (ret > 0)
		printf("tcp_client: send message: %s\n", message);
	else
		fprintf(stderr, "tcp_client: failed to send message: %s\n", message);

	close(sockfd);

	return 0;
}
