#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <event.h>
#include <event2/util.h>

struct context {
	struct timeval tv;
	struct event *t_event;
};

void timeout_cb(int fd, short event, void *context)
{
	struct context *ctx = (struct context *) context;
	printf("%s: 1 second passed\n", __func__);
	evtimer_add(ctx->t_event, &ctx->tv);
}

int main(int argc, char *argv[])
{
	struct context ctx = { {1, 0}, NULL };
	struct event_base *base = event_base_new();

	ctx.t_event = evtimer_new(base, timeout_cb, &ctx);
	evtimer_add(ctx.t_event, &ctx.tv);

	event_base_dispatch(base);

	return 0;
}
