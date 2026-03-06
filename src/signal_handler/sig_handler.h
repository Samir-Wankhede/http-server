#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>
// server handling variable
extern volatile sig_atomic_t stop_server;

// setup sigaction and handle confirmation functionality
int setup_signal_handlers();

// Catch sigint signal and prompt for confirmation
void sigint_handler(int s);

// Catch sigterm signal and prompt for confirmation
void sigterm_handler(int s);

#endif
