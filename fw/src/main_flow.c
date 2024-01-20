#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "utils_function.h"
#include "fw_data_struct.h"
#include "client_connection.h"
#include "event_watcher.h"
#include "main.h"
#include "defines.h"
#include "logic_config.h"
#include "error_handling.h"

// file:        main_flow.c
// owner:       shahar
// description: main flow for fw. manage and coordinate.
// comment:     todo shahar need to define error handling and memory error
//              todo need to catch the event when the client wants to close  
//              todo review all >> or << operation

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
            handle_easy_error("Unexpected signal received in main_signals_handler");
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
    else {
        handle_medium_error("Invalid end of operation packet received");
        }
    return pkt_type;
}

void send_end_of_op_ack(char pkt_type, char control_byte, int* client_socket_ptr){
    if (client_socket_ptr == NULL) {
        handle_fatal_error("Null pointer passed to send_end_of_op_ack");
    }
    verb_print(HIGH, "entered send_end_of_op_ack\n");
    uint64_t ack_pkt_as_uint = 0x0000000000000000;
    uint64_t pkt_type_casted = (uint64_t)pkt_type << 56;
    verb_print(HIGH, "DEBUG | pkt_type = 0x%016llx \n", pkt_type_casted);
    uint64_t control_byte_casted = 0x00 << 16;
    verb_print(HIGH, "DEBUG | control_byte = 0x%016llx \n", control_byte_casted);
    ack_pkt_as_uint = ack_pkt_as_uint | pkt_type_casted | control_byte_casted;
    char ack_pkt[MAX_MSG_SIZE];
    memset(ack_pkt, 0, MAX_MSG_SIZE);
    convert_hex_to_string_unformnatted(ack_pkt_as_uint, ack_pkt);
    send_data_as_string_to_client(client_socket_ptr, ack_pkt);
}

int GetNumOfChannels(int argc, char** argv){
    // check for default --> channel 1
    if (argc == 1){
        return 1; //defualt value
    }
    if (argc > 2){
        printf("ERROR! | Should provid only one argument to script\n");
        printf("Usage:\n");
        printf("<script_name> <num_of_ch>\n");
        printf("Please try again\n");
        exit(1);
    }
    else{
        //should check that the argument is a number
        char Num = argv[1][0];
        int result = (int) Num - 48;
        printf("NumOfChannels = %d\n", result);
        return result; // 48 is ascii code for zero
    }
}

int main(int argc, char** argv){
    //var structs:
    LogicConfig logic_config;
    // Events events; //todo shahar prio3 review | moved to be global.
    DataArray data_array_ch0;
    DataArray data_array_ch1; //in case of 2 channael
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

    //num of chaneels:
    int NumOfChannels = GetNumOfChannels(argc, argv);

    //vars for shared data
    ThreadArgs thread_args;  

    // main flow:
    print_hello_msg();
    // init structs
    init_logic_config_struct(&logic_config);
    init_events_struct(&events, EVENT_MASK);
    // if (!init_logic_config_struct(&logic_config) || !init_events_struct(&events, EVENT_MASK)) {
    //     handle_fatal_error("Initialization failed in main");
    // }
    init_buffer_info(&buffer_info, BUFFER_LEN, BUFFER_BASE_ADDR_CH0);
    init_data_array_struct(&data_array_ch0, MAX_DATA_LEN); //todo shahar need to review this defines and change if needed //todo shahar review this after finishing debug.
    init_data_array_struct(&data_array_ch1, MAX_DATA_LEN); //todo shahar need to review this defines and change if needed //todo shahar review this after finishing debug.

    // init socket
    init_connection(&server_socket, &client_socket, &server_addr, &client_addr);

    //init shared args for data shared 
    // todo shahar need to init this struct last becuase it uses other prog. pointer !!
    init_thread_args_struct(&thread_args, &logic_config, parent_pid, &main_pthread, &client_socket, &events);

    // Register the signals to thier handlers
    signal(EVENT_OCCUER, main_signals_handler);
    // signal(CLIENT_WANTS_TO_CLOSE, main_signals_handler); //todo only the thread should catch this signal

    //create the events_watcher theard
    //todo shahar close for now the thread open if we want to have events to design
    //todo shahar for now not supporting events
    // if (pthread_create(&event_monitor_pthread, NULL, mainEventThread, &thread_args) != 0) {
    //     perror("pthread_create");
    //     exit(EXIT_FAILURE);
    // }

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

    //wait for config header to arrive.
    //todo need to handle a case where the config header is delayed. (maybe loop)
    reconfig:
    verb_print(MED, "DEBUG | Get config packet from client\n");
    verb_print(MED, "DEBUG | decode config pakcet and send ack if needed\n");
    get_config_header(&logic_config, &client_socket);

    //start config the logic
    verb_print(MED, "DEBUG | start config logic\n");
    config_logic(&logic_config);

    restart:
    verb_print(MED, "DEBUG | Get start packet from client\n");
    verb_print(MED, "DEBUG | decode start pakcet and send ack if needed\n");
    get_start_header(&logic_config, &client_socket);

    //send start_op to design
    verb_print(MED, "DEBUG | Send start op to logic\n");
    config_start(&logic_config);

    //wait for stop
    verb_print(MED, "DEBUG | wait for finish op\n");
    wait_4_finish(&logic_config, NumOfChannels);

    //unload data from logic
    verb_print(MED, "DEBUG | unload data from logic\n");
    unload_data_from_logic(&data_array_ch0);
    if(NumOfChannels == 2){
        unload_data_from_logic_ch1(&data_array_ch1);
    };

    //send data to client
    
    verb_print(MED, "DEBUG | send data to client\n");
    send_data_array_to_client(&data_array_ch0, &data_array_ch1, REAL_DATA_MSG, NumOfChannels, &client_socket);

    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), MSG_WAITALL) == -1) {
    	perror("Error receiving data");
    	exit(1);
   	}
    verb_print(MED, "DEBUG | Received message from client: %s\n", buffer); //todo shahar need to remove this echo msg.

    //todo shahar stopped here develop //add send restrat vld+type to logic
    uint32_t end_type_byte = get_end_header(&logic_config, &client_socket); 

    //wait to rcve end of operation pakcet
    //int loop = TRUE;
    //while (loop){
        //char pkt_type = get_end_of_operation_pkt(&client_socket); //todo shahar need to remove this function and its decented not useful
        //verb_print(HIGH, "pkt_type_rcev = %d\n", pkt_type);
        verb_print(HIGH, "end_type_byte = %d\n", end_type_byte);
        send_restart_op_to_logic(&logic_config, end_type_byte);
        
        switch (end_type_byte){
            case END_CONNECTION:
                break;
            case REDO:
                init_data_array_struct(&data_array_ch0, MAX_DATA_LEN); //todo shahar need to review this defines and change if needed //todo shahar review this after finishing debug.
                init_data_array_struct(&data_array_ch1, MAX_DATA_LEN); //todo shahar need to review this defines and change if needed //todo shahar review this after finishing debug.
                goto restart;
                //todo shahar need to support this type jump to the relavent program point
                break;
            case RESTART:
                init_data_array_struct(&data_array_ch0, MAX_DATA_LEN); //todo shahar need to review this defines and change if needed //todo shahar review this after finishing debug.
                init_data_array_struct(&data_array_ch1, MAX_DATA_LEN); //todo shahar need to review this defines and change if needed //todo shahar review this after finishing debug.
                goto reconfig;
                //todo shahar need to support this type jump to the relavent program point
                break;
            default:
                verb_print(MED, "recv pkt type here which is not valid end of operation pkt\n");
                //todo need to handle this
                break;
        }
    //}
    // ******** end test section ********
    //close sockets
    close(server_socket);
    close(client_socket);

    sleep(2); //todo shahar for debug only

    // ### this section to signal the child (event_monitor) that client wants to end connection ***
    //pthread_kill(event_monitor_pthread, CLIENT_WANTS_TO_CLOSE); //todo shahar need to uncomment if we are using thread for event watching also need to uncomment code at start of file
    // ********************************************************************************************

    //todo shahar need to uncomment the section below if we are using thread for event watching also need to uncomment code at start of file
    //wait for thread to finish
    //todo shahar review maybe need to before closeing sokcets
    // if (pthread_join(event_monitor_pthread, (void **)&event_monitor_exist_status) != 0) {
    //     perror("pthread_join");
    //     exit(EXIT_FAILURE);
    // }

    // if (event_monitor_exist_status != NULL) {
    //     verb_print(HIGH, "Thread exited with status code: %d\n", *event_monitor_exist_status);
    //     free(event_monitor_exist_status); // Free the memory only if it's not NULL
    // }

    //end section

    print_exit_msg();
    return 0;
}
