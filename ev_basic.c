#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include<event.h>
#include<event2/util.h>

#include "socket_helper.h"

void socket_read_cb(evutil_socket_t fd, short what, void *arg)
{
	char buff[128] = { 0 };
	int len = read(fd, buff, 128);
	if (len > 0)
		printf("%s: receive message: %s\n", __func__, buff);
	else
		printf("%s: receive message ERROR!\n", __func__);
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct event_base *base;
	struct event *ev_read;

	base = event_base_new();
	sockfd = new_udp_server_socket("127.0.0.1", 30000);
	ev_read = event_new(base, sockfd, EV_READ | EV_PERSIST, socket_read_cb, NULL);

	event_add(ev_read, NULL);

	event_base_dispatch(base);

	return 0;
}
