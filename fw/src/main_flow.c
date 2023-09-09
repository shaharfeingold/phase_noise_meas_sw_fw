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

// file:        main_flow.c
// owner:       shahar
// description: main flow for fw. manage and coordinate.
// comment:     //todo shahar need to define error handling and memory error 

// todo shahar need to add all modules.


void print_hello_msg(){
    time_t current_time;
    struct tm *time_info;
    time(&current_time);
    time_info = localtime(&current_time);

    verb_print(NONE, "Current date and time: %s\n", asctime(time_info));
    verb_print(NONE, "Start FW simulation\n");
}

void print_exit_msg(){
    verb_print("Exit Fw simulation\n");
}

int main(int argc, char** argv){
    //var structs:
    LogicConfig logic_config;
    Events events;
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // main flow:
    print_hello_msg();
    // init structs
    init_logic_config_struct(&logic_config);
    init_events_struct(&events);

    // init socket
    server_socket = create_server_socket();
    set_server_addr_struct(&server_addr);
    bind_server_socket(server_socket, &server_addr);
    listen_server_socket(server_socket);

    // wait for a client to connect
    client_socket = accept_connection(server_socket, &client_addr);
    
    // rcev and send echo.
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
    	perror("Error receiving data");
    	exit(1);
   	}
    verb_print(MED, "DEBUG | Received message from client: %s\n", buffer);

	verb_print(MED, "trying to echo the msg\n");
	if (send(client_socket, buffer, strlen(buffer), 0) == -1){
		perror("Error sending data to client");
	}

    //close sockets
    close(server_socket);
    close(client_socket);
    print_exit_msg();
    return 0;
}