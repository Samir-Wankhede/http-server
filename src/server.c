#include "httpserver/config.h"
#include "log_handler/logger.h"
#include "socket_handler/socket.h"
#include "request_handlers/dispatcher.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(void){
    // set logger level from config
    for(int i=LOG_INFO; i<=LOG_FATAL; i++){
        if(strcmp(LOGGING_LEVEL, levels[i]) == 0){
            log_set_level((log_level_types)i);
            break;
        }
    }
    
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

        pthread_t thread;
        char ip[INET6_ADDRSTRLEN];
        char port[PORTSTRLEN];
        if(pthread_create(&thread, NULL, dispatch_request, (void *)(intptr_t)client_sock_fd) == 0){
            if(get_peer_by_sockfd(client_sock_fd, ip, sizeof(ip), port, sizeof(port)) == 0)
                LOG_DEBUG("Thread: %lu is handling client: %s:%s", thread, ip, port);
            pthread_detach(thread);
        }
        else{
            LOG_ERROR("Failed to create thread for client: %d", client_sock_fd);
            close(client_sock_fd);
        }
    }
    close(sockfd);
    return 0;
}
