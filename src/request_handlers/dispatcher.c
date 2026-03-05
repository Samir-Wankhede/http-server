#include "dispatcher.h"
#include "log_handler/logger.h"
#include "socket_handler/socket.h"
#include "../socket_handler/socket.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void *dispatch_request(void *arg){
    int client_fd = (int)(intptr_t)arg;
    char ip[INET6_ADDRSTRLEN];
    char port[PORTSTRLEN];
    if(get_peer_by_sockfd(client_fd, ip, sizeof(ip), port, sizeof(port)) == -1){
        LOG_ERROR("Could not get ip and port from peer");
    }
    char *response = "HTTP/2.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 42\r\nConnection: close\r\n\r\n<html><body><h1>Hello World!</body></html>";
    if(send_response(client_fd, response, strlen(response), 0) == -1){
        LOG_ERROR("sending error: ");
    }
    pthread_exit(NULL);
    close(client_fd);
    return NULL;
}
