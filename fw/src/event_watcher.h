#ifndef _EVENT_WATCHER
#define _EVENT_WATCHER

#include "fw_data_struct.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct _ThreadArgs{
    Events* events_ptr;
    LogicConfig* logic_config;
    pid_t parent_pid;
    int* client_socket_ptr;
    pthread_t* main_thread;
} ThreadArgs;

#define EVENT_OCCUER SIGUSR1
#define CLIENT_WANTS_TO_CLOSE SIGUSR2

void init_thread_args_struct(ThreadArgs* thread_args, LogicConfig* logic_config, pid_t parent_pid, pthread_t* main_thread, int* client_socket_ptr, Events* events);

void* mainEventThread(void* args);
void monitorEvents(Events* event_struct);

void events_signals_handler(int sig);

#endif