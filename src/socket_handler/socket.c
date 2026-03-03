#include "socket.h"
#include "log_handler/logger.h"
#include "../log_handler/logger.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int initialise_socket(int port){
    char port_string[5];
    snprintf(port_string, sizeof(port_string), "%d", port);
    int sockfd;
    struct addrinfo base, *servinfo, *p;
    int rv;
    int sockopt_yes = 1;
    
    LOG_DEBUG("Setting base addrinfo");
    memset(&base, 0, sizeof base);
    base.ai_family = AF_INET;
    base.ai_socktype = SOCK_STREAM;
    base.ai_flags = AI_PASSIVE;

    LOG_INFO("Fetching addr info");
    if((rv = getaddrinfo(NULL, port_string, &base, &servinfo)) != 0){
        LOG_FATAL("getaddrinfo error: %s", gai_strerror(rv));
        return -1;
    }
    
    for(p = servinfo; p!=NULL; p=p->ai_next){
        LOG_INFO("Attempting to for a socket connection");
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            LOG_ERROR("server socket error: %s", strerror(errno));
            continue;
        }
        
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt_yes, sizeof(int)) == -1){
            LOG_FATAL("setsockopt error: %s", strerror(errno));
            freeaddrinfo(servinfo);
            return -1;
        }

        LOG_INFO("Attempting to bind socket");
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            LOG_ERROR("server bind error: %s", strerror(errno));
            continue;
        }
        LOG_INFO("Server: bound on port %d", port);
        break;
    }
    LOG_DEBUG("Freeing addrinfo structures");
    freeaddrinfo(servinfo);

    LOG_DEBUG("Checking server socket bind status");
    if(p == NULL){
        LOG_FATAL("Server: socket failed to bind");
        return -1;
    }
    return sockfd;
}

int start_listening(int sockfd, int backlog, int port){
    if(listen(sockfd, backlog) == -1){
        LOG_FATAL("server: listen error: %s", strerror(errno));
        return -1;
    }
    LOG_INFO("Server: waiting for connections on port: %d", port);
    return 0;
}

void* get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int accept_connection(int sockfd){
    int client_sock_fd;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    char ipstr[INET6_ADDRSTRLEN];
    sin_size = sizeof client_addr;
    client_sock_fd = accept(sockfd, (struct sockaddr *) &client_addr, &sin_size);
    if(client_sock_fd == -1){
        LOG_ERROR("server: connection acceptance error: %s", strerror(errno));
        return -1;
    }
    inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), ipstr, sizeof ipstr);
    LOG_INFO("server: received connection from: %s", ipstr);
    return client_sock_fd;
}
