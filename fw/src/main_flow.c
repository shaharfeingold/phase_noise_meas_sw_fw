#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

//for socket?
#include <unistd.h>
#include <arpa/inet.h>

#include "utils_function.h"
#include "fw_data_struct.h"
#include "client_connection.h"
#include "event_watcher.h"
#include "main.h"

// file:        main_flow.c
// owner:       shahar
// description: main flow for fw. manage and coordinate.
// comment:     todo shahar need to define error handling and memory error
//              need to create a thread to watch for events from logics
//              todo need to catch the event when the client wants to close  
//              todo review all >> or << operation

// todo shahar need to add all modules.
//kill to send signal to process.
//pthread_kill to send signal to a thread within the process.

void print_hello_msg(){
    time_t current_time;
    struct tm *time_info;
    time(&current_time);
    time_info = localtime(&current_time);

    verb_print(NONE, "Current date and time: %s\n", asctime(time_info));
    verb_print(NONE, "Start FW simulation\n");
}

void print_exit_msg(){
    verb_print(NONE, "Exit Fw simulation\n");
}

void main_signals_handler(int sig){
    //todo shahar continue implement
    switch (sig){
        case EVENT_OCCUER:{

        }

        case CLIENT_WANTS_TO_CLOSE:{

        }

        default:{
            verb_print(HIGH, "Error sig number within main signals handler\n");
        }
    }
}

// global for multithred case.


int main(int argc, char** argv){
    //var structs:
    LogicConfig logic_config;
    Events events;
    DataArray data_array;
    BufferInfo buffer_info;

    //vars socket
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    //var pid
    pid_t parent_pid = getpid();
    pthread_t event_monitor_pthread;
    int* event_monitor_exist_status;

    //vars for shared data
    ThreadArgs thread_args;  

    // main flow:
    print_hello_msg();
    // init structs
    init_logic_config_struct(&logic_config);
    init_events_struct(&events, EVENT_MASK);
    init_buffer_info(&buffer_info, BUFFER_LEN, BUFFER_BASE_ADDR);
    // init_data_array_struct(&data_array, MAX_DATA_LEN); //todo shahar need to review this defines and change if needed //todo shahar review this after finishing debug.

    // init socket
    init_connection(&server_socket, &client_socket, &server_addr, &client_addr);

    //init shared args for data shared 
    // todo shahar need to init this struct last becuase it uses other prog. pointer !!
    init_thread_args_struct(&thread_args, &logic_config, parent_pid, &client_socket, &events);

    // Register the signals to thier handlers
    signal(EVENT_OCCUER, main_signals_handler);
    signal(CLIENT_WANTS_TO_CLOSE, main_signals_handler);

    //create the events_watcher theard
    if (pthread_create(&event_monitor_pthread, NULL, mainEventThread, &thread_args) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // reached this point connection is established !!

    // server_socket = create_server_socket();
    // set_server_addr_struct(&server_addr);
    // bind_server_socket(server_socket, &server_addr);
    // listen_server_socket(server_socket);

    // // wait for a client to connect
    // client_socket = accept_connection(server_socket, &client_addr);
    
    // rcev and send echo.
    // memset(buffer, 0, sizeof(buffer));
    // if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
    // 	perror("Error receiving data");
    // 	exit(1);
   	// }
    // verb_print(MED, "DEBUG | Received message from client: %s\n", buffer);

	// verb_print(MED, "trying to echo the msg\n");
	// if (send(client_socket, buffer, strlen(buffer), 0) == -1){
	// 	perror("Error sending data to client");
	// }

    // ******* start test section *******
    verb_print(MED, "DEBUG | init new data_array struct\n");
    init_data_array_struct(&data_array, MAX_DATA_LEN);
    verb_print(MED, "DEBUG | store new data -> 1\n");
    store_new_data(&data_array, 1, 0);
    verb_print(MED, "DEBUG | store new data -> 2\n");
    store_new_data(&data_array, 2, 0);
    verb_print(MED, "DEBUG | store new data -> 3\n");
    store_new_data(&data_array, 3, 0);

    verb_print(MED, "DEBUG | Get config packet from client\n");
    verb_print(MED, "DEBUG | decode config pakcet and send ack if needed\n");
    get_config_header(&logic_config, &client_socket);

    verb_print(MED, "DEBUG | send data to client\n");
    send_data_array_to_client(&data_array, REAL_DATA_MSG, &client_socket);
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
    	perror("Error receiving data");
    	exit(1);
   	}
    verb_print(MED, "DEBUG | Received message from client: %s\n", buffer);


    // ******** end test section ********
    //close sockets
    close(server_socket);
    close(client_socket);

    //wait for thread to finish
    //todo shahar review maybe need to before closeing sokcets
    if (pthread_join(event_monitor_pthread, (void **)&event_monitor_exist_status) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (event_monitor_exist_status != NULL) {
        verb_print(HIGH, "Thread exited with status code: %d\n", *event_monitor_exist_status);
        free(event_monitor_exist_status); // Free the memory only if it's not NULL
    }
    print_exit_msg();
    return 0;
}