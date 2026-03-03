#include "httpserver/config.h"
#include "log_handler/logger.h"
#include "socket_handler/socket.h"
#include <unistd.h>
#include <stdlib.h>
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
        int client_sock_fd = accept_connection(sockfd);
        if(client_sock_fd == -1){
            continue;
        }
        if(!fork()){
            close(sockfd);
            if(send(client_sock_fd, "BLOOP LESGOO", 12, 0) == -1){
                LOG_ERROR("sending error: ");
            }
            close(client_sock_fd);
            exit(EXIT_FAILURE);
        }
        close(client_sock_fd);
    }
    return 0;
}
