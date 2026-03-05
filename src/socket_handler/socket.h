#ifndef SOCKET_H
#define SOCKET_H
#include <stdlib.h>
#include <netinet/in.h>

#define PORTSTRLEN 6


// get ip addr by sockaddr struct
void* get_in_addr(struct sockaddr *sa);

// get port number by sockaddr struct
u_int16_t get_in_port(struct sockaddr *sa);

// handles socket creation and binding
// returns socket file descriptor
int initialise_socket(int port);

// starts socket listening
int start_listening(int sockfd, int backlog, int port);

// runs the blocking accept function
// returns new socket file descriptor
int accept_connection(int sockfd, char* ipstr, size_t ipstr_len);

// get peer by the connection's socket file descriptor
int get_peer_by_sockfd(int sockfd, char* ipstr, size_t ipstr_len, char* portstr, size_t portstr_len);

// sends message to client
int send_response(int sockfd, const void* msg, size_t len, int flags);

// reveives message from client
int receive_request(int sockfd, void* buf, size_t len, int flags);

#endif
