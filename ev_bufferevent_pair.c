#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <event.h>
#include <event2/util.h>
#include <event2/bufferevent.h>

#include "socket_helper.h"

struct bufferevent *bev[2] = { NULL, NULL };

void buffer_event_event_cb(struct bufferevent *bev, short what, void *ctx)
{
	printf("%s: bufferevent event is received\n", __func__);
}

void buffer_event_read_cb(struct bufferevent *bev, void *ctx)
{
	int ret = 0;
	char buff[128] = { 0 };

	ret = bufferevent_read(bev, buff, 128);
	if (ret > 0)
		printf("%s: bufferevent receive message: %s\n", __func__, buff);
	else
		printf("%s: bufferevent receive message ERROR!\n", __func__);
}

void socket_read_cb(evutil_socket_t fd, short what, void *arg)
{
	char buff[128] = { 0 };
	int len = read(fd, buff, 128);
	if (len > 0) {
		printf("%s: receive message: %s\n", __func__, buff);
		bufferevent_write(bev[0], buff, strlen(buff));
	} else {
		printf("%s: receive message ERROR!\n", __func__);
		bufferevent_write(bev[0], "ERROR!", strlen("ERROR!"));
	}
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

	bufferevent_pair_new(base, BEV_OPT_CLOSE_ON_FREE, bev);
	bufferevent_setcb(bev[1], buffer_event_read_cb, NULL, buffer_event_event_cb, NULL);
	bufferevent_enable(bev[1], EV_READ | EV_PERSIST);

	event_base_dispatch(base);

	return 0;
}
