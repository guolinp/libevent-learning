#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <event.h>
#include <event2/util.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

void buffer_event_event_cb(struct bufferevent *bev, short what, void *ctx)
{
	printf("%s: buffer event is received, what=%d\n", __func__, what);
}

void buffer_event_read_cb(struct bufferevent *bev, void *ctx)
{
	int ret = 0;
	char buff[128] = { 0 };

	ret = bufferevent_read(bev, buff, 128);
	if (ret > 0)
		printf("%s: bufferevent receive message: %s\n", __func__, buff);
	else
		printf("%s: bufferevent receive messag ERROR!\n", __func__);
}

void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock_addr, int len, void *ctx)
{
	struct bufferevent *bev;
	struct event_base *base = (struct event_base *) ctx;

	printf("%s: new client, fd=%d\n", __func__, fd);

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, buffer_event_read_cb, NULL, buffer_event_event_cb, NULL);
	bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

int main(int argc, char *argv[])
{
	struct event_base *base;
	struct event *ev_read;
	struct sockaddr_in sock_addr;
	struct evconnlistener *listener;

	const char *ip = "127.0.0.1";
	int port = 20000;

	bzero(&sock_addr, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_addr.s_addr = inet_addr(ip);

	base = event_base_new();

	listener = evconnlistener_new_bind(base,
					   listener_cb,
					   base,
					   LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
					   4,
					   (struct sockaddr *) &sock_addr,
					   sizeof(sock_addr));

	event_base_dispatch(base);

	return 0;
}
