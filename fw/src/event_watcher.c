#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

//for socket?
#include <unistd.h>
#include <arpa/inet.h>

#include "utils_function.h"
#include "fw_data_struct.h"
#include "client_connection.h"
#include "event_watcher.h"
#include "defines.h"
#include "read_write.h"
#include <pthread.h>

// file:        event_watcher.c
// owner:       shahar
// description: this modules handle the side thread that monitor the events and handle all defined routine
//              according to the sevarity of the event 
// comment:     todo shaharf need to continue and develop this + end the actualy thred call + signal handling for both. 
//              todo need to verify that all ptr are working and passing all the needed data
//              for example if event_mointor rise an event, the main process can also see which event rise. 

void init_thread_args_struct(ThreadArgs* thread_args, LogicConfig* logic_config, pid_t parent_pid, pthread_t* main_thread, int* client_socket_ptr, Events* events){
    thread_args->logic_config = logic_config;
    thread_args->parent_pid = parent_pid;
    thread_args->client_socket_ptr = client_socket_ptr;
    thread_args->events_ptr = events;
    thread_args->main_thread = main_thread;
}

// global vars
int client_running = TRUE; //bool, when client stop running, parent signal to thread to stop

void* mainEventThread(void* args){
    verb_print(HIGH, "entered mainEventThread\n");
    //cast args struct to vars.
    ThreadArgs* thread_args = (ThreadArgs*) args;
    Events* event_struct = thread_args->events_ptr;
    pid_t parent_pid = thread_args->parent_pid;
    LogicConfig* logic_config  = thread_args->logic_config;
    int* clinet_socket_ptr = thread_args->client_socket_ptr;
    pthread_t main_thread = *(thread_args->main_thread);


    // Register the signals to thier handlers
    // signal(EVENT_OCCUER, events_signals_handler); //todo only main thread should catch this
    signal(CLIENT_WANTS_TO_CLOSE, events_signals_handler);

    //event_watcher_main_logic
    int* exit_status = (int *)malloc(sizeof(int));
    if (exit_status == NULL) {
        perror("malloc");
        pthread_exit(NULL); // Terminate with an error status
    }
    while (client_running){
        //monitor the events for logic.
        monitorEvents(event_struct);
        if (event_struct->EventVectorMasked != 0){
            verb_print(HIGH, "EventVectorMasked != 0 | Going to signal to main thread\n");
            kill(parent_pid, EVENT_OCCUER);
            break;
        }
    }
    *exit_status = 0;
    pthread_exit(exit_status);
}


void monitorEvents(Events* event_strcut){
    verb_print(HIGH, "entered monitorEvents\n");
    // read vector from logic.
    uint32_t new_event_vec = read_from_logic(EVENT_ADDRESS); //read current event vec.
    int write_succeed = write_to_logic(0x00000000, EVENT_ADDRESS); //after reading events, rest all.
    handle_read_new_event_vector(event_strcut, new_event_vec);
    // handle_read_new_event_vector(event_strcut, 0x00000003); //todo prio1 remove for deubg only
}



//todo shahar review implementaion and verify the signal working and doing what we want
void events_signals_handler(int sig){
    verb_print(HIGH, "entered terminate_signal_handler\n");
    switch (sig) {
        //case EVENT_OCCUER:{
            //todo need to implement
            // when an fatal events happends we exit the program. and send to client the events
            // if no fatal need to set the signal watch again and continue executaion
        //    break;
       // }
        
        case CLIENT_WANTS_TO_CLOSE:{
            verb_print(HIGH, "client signal to server to close the session, event_watchers close_as_well\n");
            client_running = FALSE;
            break;
        }

        default:{
            printf("Error !! no a valid signal to handle by user in terminate_signal_handler\n");
            break;
        }

    }
}
