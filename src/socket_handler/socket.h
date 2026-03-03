#ifndef SOCKET_H
#define SOCKET_H

// handles socket creation and binding
// returns socket file descriptor
int initialise_socket(int port);

// starts socket listening
int start_listening(int sockfd, int backlog, int port);

// runs the blocking accept function
// returns new socket file descriptor
int accept_connection(int sockfd);

// sends message to client
void respond(int sockfd, const void* msg, int len, int flags);

// reveives message from client
void receive(int sockfd, void* buf, int len, int flags);

#endif
