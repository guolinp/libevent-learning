#ifndef __MISC_H_
#define __MISC_H_

// sokcet helper functions
// for server: bind the given ip:port
// for client: connect the given server ip:port

int new_tcp_server_socket(const char *ip, int port);
int new_tcp_client_socket(const char *ip, int port);

int new_udp_server_socket(const char *ip, int port);
int new_udp_client_socket(const char *ip, int port);

#endif
