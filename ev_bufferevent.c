#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <event.h>
#include <event2/util.h>

#include "socket_helper.h"

void buffer_event_event_cb(struct bufferevent *bev, short what, void *ctx)
{
	printf("%s: buffer event is received\n", __func__);
}

void buffer_event_read_cb(struct bufferevent *bev, void *ctx)
{
	int ret = 0;
	char buff[128] = { 0 };

	static int n = 0;
	if (n < 2) {
		printf("%s: do not read message from the buffer, %d time\n", __func__, n);
		n++;
		return;
	}

	ret = bufferevent_read(bev, buff, 128);
	if (ret > 0)
		printf("%s: bufferevent receive message: %s\n", __func__, buff);
	else
		printf("%s: bufferevent receive messag ERROR!\n", __func__);
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct event_base *base;
	struct bufferevent *bev;

	base = event_base_new();
	sockfd = new_udp_server_socket("127.0.0.1", 30000);
	bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, buffer_event_read_cb, NULL, buffer_event_event_cb, NULL);
	bufferevent_enable(bev, EV_READ | EV_PERSIST);

	event_base_dispatch(base);

	return 0;
}
