#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "socket_helper.h"

int main(int argc, char **argv)
{
	int client_fd;
	int server_fd;

	struct sockaddr_in sock_addr;
	socklen_t sock_addr_len = sizeof(sock_addr);

	char buff[128] = { 0 };
	int len;

	server_fd = new_tcp_server_socket("127.0.0.1", 20000);
	listen(server_fd, 1);

	client_fd = accept(server_fd, (struct sockaddr *) &sock_addr, &sock_addr_len);
	printf("tcp_server: new client, fd=%d\n", client_fd);

	len = read(client_fd, buff, 128);
	if (len > 0)
		printf("tcp_server: receive message: %s\n", buff);
	else
		printf("tcp_server: receive message ERROR!\n");

	close(client_fd);
	close(server_fd);

	return 0;
}
