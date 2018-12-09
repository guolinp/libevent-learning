#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "socket_helper.h"

int op_read(int epollfd, int fd)
{
	int ret;
	char buff[128];
	struct epoll_event ev;

	memset(buff, 0, 128);

	ret = read(fd, buff, 128);
	if (ret > 0)
		printf("epoll_tcp_server: receive message: %s\n", buff);
	else
		printf("epoll_tcp_server: receive message ERROR!\n");

	printf("epoll_tcp_server: remove client fd=%d to event lists\n", fd);

	ev.events = EPOLLIN;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);

	return 0;
}

int op_accept(int epollfd, int server_fd)
{
	int client_fd;
	struct sockaddr_in sock_addr;
	socklen_t sock_addr_len = sizeof(sock_addr);
	struct epoll_event ev;

	client_fd = accept(server_fd, (struct sockaddr *) &sock_addr, &sock_addr_len);
	printf("epoll_tcp_server: new client, fd=%d\n", client_fd);

	if (client_fd > 0) {
		printf("epoll_tcp_server: add client fd=%d to event lists\n", client_fd);
		ev.events = EPOLLIN;
		ev.data.fd = client_fd;
		epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev);
		return 0;
	}

	return -1;
}

void do_epoll(int server_fd)
{
	int epollfd;
	struct epoll_event events[4];
	struct epoll_event ev;

	epollfd = epoll_create(4);

	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev);

	for (;;) {
		int i;
		int ret = epoll_wait(epollfd, events, 4, -1);
		if (ret < 0)
			break;

		for (i = 0; i < ret; i++) {
			int fd = events[i].data.fd;
			if ((fd == server_fd) && (events[i].events & EPOLLIN))
				op_accept(epollfd, server_fd);
			else if (events[i].events & EPOLLIN)
				op_read(epollfd, fd);
		}
	}

	close(epollfd);
}

int main(int argc, char **argv)
{
	int server_fd = new_tcp_server_socket("127.0.0.1", 20000);

	listen(server_fd, 1);

	do_epoll(server_fd);

	close(server_fd);

	return 0;
}
