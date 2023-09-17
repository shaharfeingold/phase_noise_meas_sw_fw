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
    verb_print(NONE, "Exit Fw simulation\n");
}

// global for multithred case.
DataArray data_array;

int main(int argc, char** argv){
    //var structs:
    LogicConfig logic_config;
    Events events;
    // DataArray data_array;
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // main flow:
    print_hello_msg();
    // init structs
    init_logic_config_struct(&logic_config);
    init_events_struct(&events);
    // init_data_array_struct(&data_array, MAX_DATA_LEN); //todo shahar need to review this defines and change if needed //todo shahar review this after finishing debug.

    // init socket
    init_connection(&server_socket, &client_socket, &server_addr, &client_addr);

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
    init_data_array_struct(&data_array, 3);
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
    print_exit_msg();
    return 0;
}