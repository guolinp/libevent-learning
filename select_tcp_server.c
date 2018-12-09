#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "socket_helper.h"

int op_read(int fd)
{
	int ret;
	char buff[128];

	memset(buff, 0, 128);

	ret = read(fd, buff, 128);
	if (ret > 0)
		printf("select_tcp_server: receive message: %s\n", buff);
	else
		printf("select_tcp_server: receive message ERROR!\n");

	return 0;
}

int op_accept(int server_fd)
{
	int client_fd;
	struct sockaddr_in sock_addr;
	socklen_t sock_addr_len = sizeof(sock_addr);

	client_fd = accept(server_fd, (struct sockaddr *) &sock_addr, &sock_addr_len);
	printf("select_tcp_server: new client, fd=%d\n", client_fd);

	return client_fd > 0 ? client_fd : -1;
}

void do_select(int client_fd)
{
	int ret;
	fd_set readfds;

	FD_ZERO(&readfds);
	FD_SET(client_fd, &readfds);

	ret = select(client_fd + 1, &readfds, NULL, NULL, NULL);
	if (ret < 0)
		return;

	printf("select_tcp_server: some socket(s) has data in\n");
	if (FD_ISSET(client_fd, &readfds))
		op_read(client_fd);

	FD_CLR(client_fd, &readfds);
}

int main(int argc, char **argv)
{
	int client_fd;
	int server_fd = new_tcp_server_socket("127.0.0.1", 20000);

	listen(server_fd, 1);

	client_fd = op_accept(server_fd);

	do_select(client_fd);

	close(client_fd);
	close(server_fd);

	return 0;
}
