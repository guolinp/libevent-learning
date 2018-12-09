#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <event2/event.h>
#include <event2/event_struct.h>

void signal_handler(int signo, short events, void *arg)
{
	printf("signal_handler: receive signal %d\n", signo);
}

int main(int argc, char *argv[])
{
	int signo = SIGINT;
	struct event_base *base;
	struct event *sig_event;

	base = event_base_new();
	sig_event = evsignal_new(base, signo, signal_handler, NULL);

	evsignal_add(sig_event, NULL);

	event_base_dispatch(base);

	return 0;
}
