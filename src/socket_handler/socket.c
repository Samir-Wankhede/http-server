#include "socket.h"
#include "log_handler/logger.h"

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

void* get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

u_int16_t get_in_port(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return (((struct sockaddr_in *)sa)->sin_port);
    }
    return (((struct sockaddr_in6 *)sa)->sin6_port);
}

int initialise_socket(int port){
    char port_string[6];
    snprintf(port_string, sizeof(port_string), "%d", port);
    struct addrinfo hints, *servinfo, *p;

    int sockfd;
    int rv;
    int sockopt_yes = 1;
    
    LOG_DEBUG("Setting hints addrinfo");
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    LOG_DEBUG("Fetching addr info");
    if((rv = getaddrinfo(NULL, port_string, &hints, &servinfo)) != 0){
        LOG_FATAL("getaddrinfo error: %s", gai_strerror(rv));
        return -1;
    }
    
    for(p = servinfo; p!=NULL; p=p->ai_next){
        LOG_DEBUG("Attempting to for a socket connection");
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            LOG_ERROR("Server socket error: %s", strerror(errno));
            continue;
        }
        
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt_yes, sizeof(int)) == -1){
            LOG_FATAL("setsockopt error: %s", strerror(errno));
            freeaddrinfo(servinfo);
            return -1;
        }

        LOG_DEBUG("Attempting to bind socket");
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            LOG_ERROR("Server bind error: %s", strerror(errno));
            continue;
        }
        break;
    }
    LOG_DEBUG("Freeing addrinfo structures");
    freeaddrinfo(servinfo);

    LOG_DEBUG("Checking Server socket bind status");
    if(p == NULL){
        LOG_FATAL("Server: socket failed to bind");
        return -1;
    }
    LOG_INFO("Server: bound on port %d", port);
    return sockfd;
}

int start_listening(int sockfd, int backlog, int port){
    if(listen(sockfd, backlog) == -1){
        LOG_FATAL("Server: listen error: %s", strerror(errno));
        return -1;
    }
    LOG_INFO("Server: waiting for connections on port: %d", port);
    return 0;
}


int accept_connection(int sockfd, char* ipstr, size_t ipstr_len){
    struct sockaddr_storage client_addr;
    socklen_t sin_size;

    sin_size = sizeof client_addr;
    int client_sock_fd = accept(sockfd, (struct sockaddr *) &client_addr, &sin_size);
    if(client_sock_fd == -1){
        LOG_ERROR("Server: connection acceptance error: %s", strerror(errno));
        return -1;
    }
    if(inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), ipstr, (socklen_t) ipstr_len)==NULL){
        LOG_ERROR("inet_ntop error: %s", strerror(errno));
    }
    LOG_INFO("Server: received connection from: %s", ipstr);
    return client_sock_fd;
}

int get_peer_by_sockfd(int sockfd, char* ipstr, size_t ipstr_len, char* portstr, size_t portstr_len){
    struct sockaddr_storage client_addr;
    socklen_t sin_size = sizeof(client_addr);

    if(getpeername(sockfd, (struct sockaddr *) &client_addr, &sin_size) == 0){
        if(inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), ipstr, (socklen_t) ipstr_len) == NULL){
			LOG_ERROR("inet_ntop error: %s", strerror(errno));
			return -1;
        }
        int port = ntohs(get_in_port((struct sockaddr *) &client_addr));
        snprintf(portstr, portstr_len, "%d", port);
        return 0;
    }
    LOG_ERROR("getpeername error: %s", strerror(errno));
    return -1;
}

int send_response(int sockfd, const void* msg, size_t len, int flags){
    char *buf = (char *)msg;
    ssize_t remaining_len = (ssize_t)len;
    while(remaining_len > 0){
        ssize_t sent = send(sockfd, buf, (size_t)remaining_len, flags);
        if(sent == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK) continue;
            char ip[INET6_ADDRSTRLEN];
            char port[PORTSTRLEN];
            if(get_peer_by_sockfd(sockfd, ip, sizeof(ip), port, sizeof(port)) == -1){
                LOG_ERROR("Failed to fetch socket's ip and port");
                LOG_ERROR("send error: %s", strerror(errno));
                return -1;
            }
            LOG_ERROR("Error while sending to %s:%s", ip, port);
            LOG_ERROR("send error: %s", strerror(errno));
            return -1;
        }
        if(sent == 0){
            LOG_WARN("Connection close by peer\n Remaining bytes to send: %zu", remaining_len);
            return 0;
        }
        buf += sent;
        remaining_len -= sent;
    }
    return 0;
}

// int receive_request(int sockfd, void* buf, size_t len, int flags){

//}
