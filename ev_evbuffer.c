#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <event.h>
#include <event2/util.h>

#include "socket_helper.h"

void socket_read_cb(evutil_socket_t fd, short what, void *arg)
{
	int len;
	char *str;
	struct evbuffer *evbuf = (struct evbuffer *) arg;
	char data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	len = evbuffer_read(evbuf, fd, 128);
	printf("%s: read data to evbuffer, %d\n", __func__, len);

	len = evbuffer_get_length(evbuf);
	printf("%s: data length in evbuffer, %d\n", __func__, len);

	str = evbuffer_pullup(evbuf, 4);
	printf("%s: pullup the first 4 bytes, %c,%c,%c,%c\n", __func__, str[0], str[1], str[2], str[3]);

	str = evbuffer_pullup(evbuf, 4);
	printf("%s: pullup the first 4 bytes, %c,%c,%c,%c\n", __func__, str[0], str[1], str[2], str[3]);

	printf("%s: drain evbuffer, 4 bytes\n", __func__);
	evbuffer_drain(evbuf, 4);

	len = evbuffer_get_length(evbuf);
	printf("%s: data length in evbuffer, %d\n", __func__, len);

	str = evbuffer_pullup(evbuf, 4);
	printf("%s: pullup the first 4 bytes, %c,%c,%c,%c\n", __func__, str[0], str[1], str[2], str[3]);

	evbuffer_remove(evbuf, (void *) data, 7);
	printf("%s: remove data, %s\n", __func__, data);

	len = evbuffer_get_length(evbuf);
	printf("%s: data length in evbuffer, %d\n", __func__, len);
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct event_base *base;
	struct event *ev_read;
	struct evbuffer *evbuf;

	base = event_base_new();
	evbuf = evbuffer_new();
	sockfd = new_udp_server_socket("127.0.0.1", 30000);
	ev_read = event_new(base, sockfd, EV_READ | EV_PERSIST, socket_read_cb, evbuf);

	event_add(ev_read, NULL);

	event_base_dispatch(base);

	return 0;
}
