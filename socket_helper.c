#include <string.h>
#include <arpa/inet.h>
#include <assert.h>

#include "socket_helper.h"

// service type
#define SERVICE_SERVER 0
#define SERVICE_CLIENT 1

static int new_socket_fd(int service_type, int sock_type, const char *ip, int port)
{
	int sockfd;
	struct sockaddr_in sock_addr;

	bzero(&sock_addr, sizeof(sock_addr));

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_addr.s_addr = inet_addr(ip);

	sockfd = socket(AF_INET, sock_type, 0);

	// go die, gua le, gun du zi, wtf...
	assert(sockfd > 0);

	if (service_type == SERVICE_SERVER)
		bind(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
	else
		connect(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));

	return sockfd;
}

int new_tcp_server_socket(const char *ip, int port)
{
	return new_socket_fd(SERVICE_SERVER, SOCK_STREAM, ip, port);
}

int new_tcp_client_socket(const char *ip, int port)
{
	return new_socket_fd(SERVICE_CLIENT, SOCK_STREAM, ip, port);
}

int new_udp_server_socket(const char *ip, int port)
{
	return new_socket_fd(SERVICE_SERVER, SOCK_DGRAM, ip, port);
}

int new_udp_client_socket(const char *ip, int port)
{
	return new_socket_fd(SERVICE_CLIENT, SOCK_DGRAM, ip, port);
}
