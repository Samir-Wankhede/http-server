#include "sig_handler.h"
#include "log_handler/logger.h"
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile sig_atomic_t stop_server = 0;

void sigint_handler(int s){
    (void) s;
    stop_server = 1;
}

void sigterm_handler(int s){
    (void) s;
    stop_server = 1;
}

int setup_signal_handlers(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = 0; // Keeping as zero and not SA_RESTART for now
                     // when polling and global socket state is created then update to restart
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigint_handler;
    if(sigaction(SIGINT, &sa, NULL) == -1){
        LOG_ERROR("SigInt Sigaction error: %s", strerror(errno));
        return -1;
    }
    sa.sa_handler = sigterm_handler;
    if(sigaction(SIGTERM, &sa, NULL) == -1){
        LOG_ERROR("SigTerm Sigaction error: %s", strerror(errno));
        return -1;
    }
    sa.sa_handler = SIG_IGN;
    if(sigaction(SIGPIPE, &sa, NULL) == -1){
        LOG_ERROR("SigPipe Sigaction error: %s", strerror(errno));
        return -1;
    }
    return 0;
}
