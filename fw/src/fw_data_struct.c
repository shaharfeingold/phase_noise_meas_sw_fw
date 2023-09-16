#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "utils_function.h"
#include "fw_data_struct.h"
#include "client_connection.h"

// file:        fw_data_struct.c
// owner:       shahar
// description: data structs for the fw and their corosponding functions.
// comment:     


//strcut function
//get a pointer for a new allocated strct and modify it inside.
//modify all field to zero.
void init_logic_config_struct(LogicConfig* new_logic_config){
    new_logic_config->config_sent = 0;
    new_logic_config->got_finish = 0;
    new_logic_config->phase_inc = 0;
    new_logic_config->start_sent = 0;
}

//update field config_sent.
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data){
    logic_config->config_sent = data;
}

//update field got_finish.
void UpdateGotFinish(LogicConfig* logic_config, uint32_t data){
    logic_config->got_finish = data;
}

//update field phase_inc.
void UpdatePhaseInc(LogicConfig* logic_config, uint32_t data){
    logic_config->phase_inc = data;
}

//update field start_sent.
void UpdateStartSent(LogicConfig* logic_config, uint32_t data){
    logic_config->start_sent = data;
}

//get a pointer for a new allocated strct and modify it inside.
//modify all field to zero.
void init_events_struct(Events* new_event_struct){
    new_event_struct->EventsBitsVector = 0x0000;
    //todo shahar init more fields upon their addition.
}

//todo shahar make function to set bit event.

void init_data_array_struct(DataArray* new_data_array, uint32_t expected_data_len){
    verb_print(HIGH, "entered init_data_array function\n");
    new_data_array->Len = 0;
    new_data_array->TargetLen = expected_data_len;
    clear_uint32_array(new_data_array->RealDataArray, MAX_DATA_LEN);
    clear_uint32_array(new_data_array->ImgDataArray, MAX_DATA_LEN);
}

void store_new_data(DataArray* data_array, uint32_t RealData, uint32_t ImgData){
    verb_print(HIGH, "enterd store_new_data\n");
    data_array->RealDataArray[data_array->Len] = RealData;
    data_array->ImgDataArray[data_array->Len] = ImgData;
    data_array->Len += 1;
    verb_print(HIGH, "DEBUG | going to store new record:\nRealData = %d, ImgData = %d, updated array Len = %d", RealData, ImgData, data_array->Len);
}

void send_data_array_to_client(DataArray* data_array, int type ,int* client_socket_ptr){
    verb_print(HIGH, "entered send_data_array_to_client\n");
    verb_print(MED, "Goind to send data collected from the array to client. msg type = %d\n", type);
    //how data is going to be send:
    //* all data is send as bytes (string) need to encode it on server and decode it (convert back to num) on client side
    //* goint to send item by item.
    //1. first msg: type of msg = data, array_len, array_type [real, img, real+img]
    //2. payload one by one (in case read+img, first real)
    //implement:
    //print warning in case expected len is not equal to len.
    uint32_t len = data_array->Len;
    uint32_t expected_len = data_array->TargetLen;
    int client_socket = *client_socket_ptr;
    int index = 0;
    char data[MAX_MSG_SIZE];
    memset(data, 0, MAX_MSG_SIZE);
    char header[MAX_MSG_SIZE];
    memset(header, 0, MAX_MSG_SIZE);
    if (len != expected_len){
        verb_print(MED, "Going to send data to client but not recv all the expcted data from logic.\n");
        // todo shahar review. now it is just a warning. maybe need to switch to exit. according to flow
        // we will defined later.
    }
   // implement here
   // build header of packet;
   build_data_packet_header(header, len, type);
   send_data_as_string_to_client(client_socket_ptr, header);
   send_data_array_to_client_according_to_type(data_array, type ,client_socket_ptr);
   //updata + print that all data is sent from client side.
   verb_print(HIGH, "finish sending to client data accroding to data type = %d", type);
}

void send_data_array_to_client_according_to_type(DataArray* data_array, int type ,int* client_socket_ptr){
    verb_print(HIGH, "send_data_array_to_client_according_to_type with data_type =%d\n", type);
    int index = 0;
    uint32_t len = data_array->Len;
    switch (type){
        case REAL_DATA_MSG:
            for(index = 0; index < len ; index++){
                send_uint32_t_to_client(client_socket_ptr, data_array->RealDataArray[index]);
            }
            break;
        case IMG_DATA_MSG:
            for (index = 0 ; index < len ; index++){
                send_uint32_t_to_client(client_socket_ptr, data_array->ImgDataArray[index]);
            }
            break;
        case REAL_IMG_DATA_MSG:
            for(index = 0; index < len ; index++){
                send_uint32_t_to_client(client_socket_ptr, data_array->RealDataArray[index]);
            }
            for (index = 0 ; index < len ; index++){
                send_uint32_t_to_client(client_socket_ptr, data_array->ImgDataArray[index]);
            }
            break;
    }
}

void clear_uint32_array(uint32_t* array_head, uint32_t len){
    verb_print(HIGH, "entered clear_uint32_array\n");
    uint32_t index = 0;
    for (index = 0; index < len ; index++){
        *(array_head + index) = 0;
    }
    print_uint32_array(array_head, len);
}

void print_uint32_array(uint32_t* array_head, uint32_t len){
    int index = 0;
    verb_print(HIGH,"print array = %p\n", array_head);
    for (index = 0; index < len; index++){
        verb_print(HIGH, "item %d | value = %d\t", index, *(array_head + index));
        if(index % 4 == 3){
            verb_print(HIGH, "\n");
        }
    }
}