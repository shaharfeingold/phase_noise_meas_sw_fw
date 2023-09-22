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

// file:        event_watcher.c
// owner:       shahar
// description: 
// comment:    todo shaharf need to continue and develop this + end the actualy thred call + signal handling for both. 

void mainEventThread(void* args){
    verb_print(HIGH, "entered mainEventThread\n");
    //cast args struct to vars.
}


void monitorEvents(){
    verb_print(HIGH, "entered monitorEvents\n");
    
}

void terminate_signal_handler(int sig){
    verb_print(HIGH, "entered terminate_signal_handler\n");
    switch (sig) {
        case EVENT_OCCUER:{
            // when an fatal events happends we exit the program. and send to client the events
            // if no fatal need to set the signal watch again and continue executaion
            break;
        }
        
        case CLIENT_WANTS_TO_CLOSE:{

        }

        default:{
            printf("Error !! no a valid signal to handle by user in terminate_signal_handler\n");
            break;
        }

    }
}