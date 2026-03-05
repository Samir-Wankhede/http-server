#include "httpserver/config.h"
#include "log_handler/logger.h"
#include "socket_handler/socket.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(void){
    int sockfd;
    if((sockfd = initialise_socket(DEFAULT_PORT)) == -1){
        exit(EXIT_FAILURE);
    }
    int listening_status;
    if((listening_status = start_listening(sockfd, BACKLOG, DEFAULT_PORT)) == -1){
        exit(EXIT_FAILURE);
    } 
    while(1){
        char ipstr[INET6_ADDRSTRLEN];
        int client_sock_fd = accept_connection(sockfd, ipstr, sizeof(ipstr));
        if(client_sock_fd == -1){
            continue;
        }
        if(!fork()){
            close(sockfd);
            char ip[INET6_ADDRSTRLEN];
            char port[PORTSTRLEN];
            if(get_peer_by_sockfd(client_sock_fd, ip, sizeof(ip), port, sizeof(port)) == -1){
                LOG_ERROR("Could not get ip and port from peer");
            }
            LOG_INFO("Attempting to send status to %s:%s", ip, port);
            char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 42\r\nConnection: close\r\n\r\n<html><body><h1>Hello World!</body></html>";
            if(send_response(client_sock_fd, response, strlen(response), 0) == -1){
                LOG_ERROR("sending error: ");
            }
            close(client_sock_fd);
            exit(EXIT_FAILURE);
        }
        close(client_sock_fd);
    }
    return 0;
}
