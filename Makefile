LIBEVENT=../libevent-2.1.8-stable-libs

INC=-I$(LIBEVENT)/include
LIB=-L$(LIBEVENT)/lib

all:
	gcc $(INC) $(LIB)  socket_helper.c  udp_server.c            -o  udp_server
	gcc $(INC) $(LIB)  socket_helper.c  udp_client.c            -o  udp_client
	gcc $(INC) $(LIB)  socket_helper.c  tcp_server.c            -o  tcp_server
	gcc $(INC) $(LIB)  socket_helper.c  epoll_tcp_server.c      -o  epoll_tcp_server
	gcc $(INC) $(LIB)  socket_helper.c  select_tcp_server.c     -o  select_tcp_server
	gcc $(INC) $(LIB)  socket_helper.c  tcp_client.c            -o  tcp_client
	gcc $(INC) $(LIB)  socket_helper.c  ev_basic.c              -o  ev_basic               -levent
	gcc $(INC) $(LIB)  socket_helper.c  ev_bufferevent.c        -o  ev_bufferevent         -levent
	gcc $(INC) $(LIB)  socket_helper.c  ev_bufferevent_pair.c   -o  ev_bufferevent_pair    -levent
	gcc $(INC) $(LIB)  socket_helper.c  ev_bufferevent_socket.c -o  ev_bufferevent_socket  -levent
	gcc $(INC) $(LIB)  socket_helper.c  ev_evbuffer.c           -o  ev_evbuffer            -levent
	gcc $(INC) $(LIB)                   ev_timer.c              -o  ev_timer               -levent
	gcc $(INC) $(LIB)                   ev_signal.c             -o  ev_signal              -levent


clean:
	rm -rf udp_server
	rm -rf udp_client
	rm -rf tcp_server
	rm -rf tcp_client
	rm -rf epoll_tcp_server
	rm -rf select_tcp_server
	rm -rf ev_basic
	rm -rf ev_bufferevent
	rm -rf ev_bufferevent_pair
	rm -rf ev_bufferevent_socket
	rm -rf ev_evbuffer
	rm -rf ev_timer
	rm -rf ev_signal
