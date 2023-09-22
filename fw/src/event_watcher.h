#ifndef _EVENT_WATCHER
#define _EVENT_WATCHER

#include "fw_data_struct.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct _ThreadArgs
{
    Events* events_ptr;
    LogicConfig* logic_config;
    pid_t parent_pid;
    int* client_socket_ptr;
} ThreadArgs;

#define EVENT_OCCUER SIGUSR1
#define CLIENT_WANTS_TO_CLOSE SIGUSR2

void mainEventThread(void* args);
void monitorEvents();
void terminate_signal_handler(int sig);

#endif