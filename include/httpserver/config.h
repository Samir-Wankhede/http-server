#ifndef HTTPSERVER_CONFIG_H
#define HTTPSERVER_CONFIG_H

#ifndef SERVER_ROOT
#define SERVER_ROOT "./http-server-static"
#endif

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 8080
#endif

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 128
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

#ifndef LOGGING_LEVEL
#define LOGGING_LEVEL "INFO"
#endif

#ifndef BACKLOG
#define BACKLOG 15
#endif

#endif
