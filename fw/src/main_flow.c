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
#include "defines.h"

// file:        main_flow.c
// owner:       shahar
// description: main flow for fw. manage and coordinate.
// comment:     todo shahar need to define error handling and memory error
//              todo need to catch the event when the client wants to close  
//              todo review all >> or << operation
//              need to implemenet "wait to start operation"
//              need to implemenet "wait to stop operation"

// todo shahar need to add all modules.
//kill to send signal to process.
//pthread_kill to send signal to a thread within the process.


// global vars.
Events events;


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
    verb_print(HIGH, "main_signals_handler\n");
    //todo shahar continue implement
    switch (sig){
        case EVENT_OCCUER:{
            verb_print(MED, "event watcher sends to main thread EVENT_OCCUER signal\n");
            //tdoo shahar consider init a semaphore until main finish hadling also  main reset the vec in logic 
            // prio 1
            seek_event_from_vec();
            break;
        }

        //case CLIENT_WANTS_TO_CLOSE:{
        //    break;
        //}

        default:{
            verb_print(HIGH, "Error sig number within main signals handler\n");
            break;
        }
    }
}

void seek_event_from_vec(){
    //todo shahar contiune implement as the events grows prio 2
    int IsEvent0Up = events.test_event0;
    int IsEvent1Up = events.test_event1;
    verb_print(MED, "Event0Up ? %d , Event1Up ? %d\n", IsEvent0Up, IsEvent1Up);
}

char get_end_of_operation_pkt(int* client_socket_ptr){
    verb_print(HIGH, "entered get_end_of_operation_pkt\n");
    char pkt[MAX_MSG_SIZE];
    memset(pkt, 0, MAX_MSG_SIZE);
    char pkt_type;
    recv_bytes_from_client(client_socket_ptr, pkt);
    pkt_type = decode_end_of_op_pkt(pkt, client_socket_ptr);
    return pkt_type;
}

char decode_end_of_op_pkt(char* pkt, int* client_socket_ptr){
    verb_print(HIGH, "entered decode_end_of_op_pkt\n");
    uint64_t pkt_as_uint = convert_string_to_hex_uint64_t(pkt);
    char pkt_type = (pkt_as_uint & 0xFF00000000000000) >> 56; //1B long
    verb_print(HIGH, "DEBUG | pkt_type recv from header = %d\n", pkt_type);
    char control_byte = (pkt_as_uint & 0x0000000000FF0000) >> 16; //4B long;//1B long
    verb_print(HIGH, "DEBUG | control_byte recv from header = %d\n", control_byte);
    if ((pkt_type >= RESTART) && (control_byte == 255)){ //meaning good packet end of operation pkt
        send_end_of_op_ack(pkt_type, control_byte, client_socket_ptr);
    }
    return pkt_type;
}

void send_end_of_op_ack(char pkt_type, char control_byte, int* client_socket_ptr){
    verb_print(HIGH, "entered send_end_of_op_ack\n");
    uint64_t ack_pkt_as_uint = 0x0000000000000000;
    uint64_t pkt_type_casted = (uint64_t)pkt_type << 56;
    verb_print(HIGH, "DEBUG | pkt_type = 0x%08lx \n", pkt_type_casted);
    uint64_t control_byte_casted = 0x00 << 16;
    verb_print(HIGH, "DEBUG | control_byte = 0x%08lx \n", control_byte_casted);
    ack_pkt_as_uint = ack_pkt_as_uint | pkt_type_casted | control_byte_casted;
    char ack_pkt[MAX_MSG_SIZE];
    memset(ack_pkt, 0, MAX_MSG_SIZE);
    convert_hex_to_string_unformnatted(ack_pkt_as_uint, ack_pkt);
    send_data_as_string_to_client(client_socket_ptr, ack_pkt);
}

int main(int argc, char** argv){
    //var structs:
    LogicConfig logic_config;
    // Events events; //todo shahar prio3 review | moved to be global.
    DataArray data_array;
    BufferInfo buffer_info;

    //vars socket
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    //var pid
    pid_t parent_pid = getpid();
    pthread_t main_pthread = pthread_self();
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
    init_thread_args_struct(&thread_args, &logic_config, parent_pid, &main_pthread, &client_socket, &events);

    // Register the signals to thier handlers
    signal(EVENT_OCCUER, main_signals_handler);
    // signal(CLIENT_WANTS_TO_CLOSE, main_signals_handler); //todo only the thread should catch this signal

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
    init_data_array_struct(&data_array, MAX_DATA_LEN); //note here we put the expected len. the actual len is according to method that fill the array
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

    //wait to rcve end of operation pakcet
    int loop = TRUE;
    while (loop){
        char pkt_type = get_end_of_operation_pkt(&client_socket);
        verb_print(HIGH, "pkt_type_rcev = %d\n", pkt_type);
        switch (pkt_type){
            case END_CONNECTION:
                loop = FALSE;
                break;
            case REDO:
                //todo shahar need to support this type
                break;
            case RESTART:
                //todo shahar need to support this type
                break;
            default:
                verb_print(MED, "recv pkt type here which is not valid end of operation pkt\n");
                //todo need to handle this
                break;
        }
    }
    // ******** end test section ********
    //close sockets
    close(server_socket);
    close(client_socket);

    sleep(2); //todo shahar for debug only

    // ### this section to signal the child (event_monitor) that client wants to end connection ***
    pthread_kill(event_monitor_pthread, CLIENT_WANTS_TO_CLOSE);
    // ********************************************************************************************

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
