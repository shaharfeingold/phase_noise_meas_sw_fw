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
void init_events_struct(Events* new_event_struct, uint32_t event_mask){
    new_event_struct->EventsBitsVector = 0x00000000;
    new_event_struct->EventMask = event_mask;
    new_event_struct->EventVectorMasked = 0x00000000;
    new_event_struct->test_event0 = 0;
    new_event_struct->test_event1 = 1;
    //todo shahar init more fields upon their addition.
}

void UpdateEventsVec(Events* event_strcut, uint32_t new_event_vec){
    event_strcut->EventsBitsVector = new_event_vec;
}

void handle_read_new_event_vector(Events* event_struct, uint32_t new_event_vector){
    UpdateEventsVec(event_struct, new_event_vector);
    decode_event_vector(event_struct);
}

void decode_event_vector(Events* event_struct) {
    uint32_t event_vec_maksed = event_struct->EventMask & event_struct->EventsBitsVector;
    event_struct->EventVectorMasked = event_vec_maksed;
    event_struct->test_event0 = 0x00000001 & event_vec_maksed; //pos bit 0
    event_struct->test_event1 = (0x00000002 & event_vec_maksed) >> 1; //pos bit 1
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
    //2. payload one by one (in case read+img, real + img)
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
                send_uint32_t_to_client(client_socket_ptr, data_array->ImgDataArray[index]);
            }
            // for (index = 0 ; index < len ; index++){
            //     send_uint32_t_to_client(client_socket_ptr, data_array->ImgDataArray[index]);
            // }
            // break;
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

void get_config_header(LogicConfig* logic_config, int* client_socket_ptr){
    char header[MAX_DATA_LEN];
    memset(header, 0, MAX_DATA_LEN);
    recv_bytes_from_client(client_socket_ptr, header);
    decode_header(logic_config, header, client_socket_ptr);
}

void decode_header(LogicConfig* logic_config, char header[], int* client_socket_ptr){
    uint64_t header_as_uint = convert_string_to_hex_uint64_t(header);
    char pkt_type = (header_as_uint & 0xFF00000000000000) >> 56; //1B long
    verb_print(HIGH, "DEBUG | pkt_type recv from header = %d", pkt_type);
    uint32_t phase_inc = (header_as_uint & 0x00FFFFFFFF000000) >> 24; //4B long
    verb_print(HIGH, "DEBUG | phase_inc recv from header = %d", phase_inc);
    char control_byte = (header_as_uint & 0x0000000000FF0000) >> 16; //4B long;//1B long
    verb_print(HIGH, "DEBUG | control_byte recv from header = %d", control_byte);
    if ((pkt_type == CONFIG_PKT) && (control_byte == 255)){ //meaning good packet
        UpdateConfigSent(logic_config, TRUE);
        UpdatePhaseInc(logic_config, phase_inc);
        send_config_ack(logic_config, pkt_type, phase_inc, control_byte, client_socket_ptr);
    }
}

void send_config_ack(LogicConfig* logic_config, char pkt_type, uint32_t phase_inc, char control_byte, int* client_socket_ptr){
    verb_print(HIGH, "entered send_config_ack\n");
    uint64_t header_as_uint = 0x0000000000000000;
    uint64_t pkt_type_casted = (uint64_t)pkt_type << 56;
    verb_print(HIGH, "DEBUG | pkt_type = 0x%08x \n", pkt_type_casted);
    uint64_t phase_inc_casted = (uint64_t)phase_inc << 24;
    verb_print(HIGH, "DEBUG | phase_inc = 0x%08x \n", phase_inc_casted);
    uint64_t control_byte_casted = 0x00 << 16;
    verb_print(HIGH, "DEBUG | control_byte = 0x%08x \n", control_byte_casted);
    header_as_uint = header_as_uint | pkt_type_casted | phase_inc_casted | control_byte_casted;
    char header[MAX_DATA_LEN];
    memset(header, 0, MAX_DATA_LEN);
    convert_hex_to_string_unformnatted(header_as_uint, header);
    send_data_as_string_to_client(client_socket_ptr, header);
}

//todo shahar connect to vars/defines
void init_buffer_info(BufferInfo* buffer_info, uint32_t buffer_len, uint64_t buffer_base_address){
    buffer_info->buffer_base_address = buffer_base_address;
    buffer_info->buffer_len = buffer_len;
}