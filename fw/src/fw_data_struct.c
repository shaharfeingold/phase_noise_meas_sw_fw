#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "utils_function.h"
#include "fw_data_struct.h"
#include "client_connection.h"
#include "read_write.h"
#include "logic_config.h"
#include "error_handling.h"
// file:        fw_data_struct.c
// owner:       shahar
// description: data structs for the fw and their corosponding functions.
// comment:     


//strcut function
//get a pointer for a new allocated strct and modify it inside.
//modify all field to zero.
void init_logic_config_struct(LogicConfig* new_logic_config){
    if (new_logic_config == NULL) {
        handle_fatal_error("LogicConfig pointer is NULL");
    }
    new_logic_config->config_sent = 0;
    new_logic_config->got_finish = 0;
    new_logic_config->phase_inc = 0;
    new_logic_config->start_sent = 0;
}

//update field config_sent.
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data){
    if (logic_config == NULL) {
        handle_fatal_error("LogicConfig pointer is NULL");
    }
    logic_config->config_sent = data;
}

//update field got_finish.
void UpdateGotFinish(LogicConfig* logic_config, uint32_t data){
    if (logic_config == NULL) {
        handle_fatal_error("LogicConfig pointer is NULL");
    }
    logic_config->got_finish = data;
}

//update field phase_inc.
void UpdatePhaseInc(LogicConfig* logic_config, uint32_t data){
    if (logic_config == NULL) {
        handle_fatal_error("LogicConfig pointer is NULL in UpdateConfigSent");
    }
    logic_config->phase_inc = data;
}

//update field start_sent.
void UpdateStartSent(LogicConfig* logic_config, uint32_t data){
    if (logic_config == NULL) {
        handle_fatal_error("LogicConfig pointer is NULL in UpdateConfigSent");
    } 
    logic_config->start_sent = data;
}

//get a pointer for a new allocated strct and modify it inside.
void init_events_struct(Events* new_event_struct, uint32_t event_mask){
    if (new_event_struct == NULL) {
        handle_fatal_error("Events pointer is NULL");
    }
    new_event_struct->EventsBitsVector = 0x00000000;
    new_event_struct->EventMask = event_mask;
    new_event_struct->EventVectorMasked = 0x00000000;
    new_event_struct->test_event0 = 0;
    new_event_struct->test_event1 = 1;
    //todo shahar init more fields upon their addition.
}

void UpdateEventsVec(Events* event_strcut, uint32_t new_event_vec){
    if (event_strcut == NULL) {
        handle_fatal_error("Events pointer is NULL");
    }
    event_strcut->EventsBitsVector = new_event_vec;
}

void handle_read_new_event_vector(Events* event_struct, uint32_t new_event_vector){
    verb_print(HIGH, "entered handle_read_new_event_vector\n");
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
    if (new_data_array == NULL) {
        handle_fatal_error("DataArray pointer is NULL");
    }
    verb_print(HIGH, "entered init_data_array function\n");
    new_data_array->Len = 0;
    new_data_array->TargetLen = expected_data_len;
    clear_uint32_array(new_data_array->RealDataArray, MAX_DATA_LEN);
    clear_uint32_array(new_data_array->ImgDataArray, MAX_DATA_LEN);
}

void store_new_data(DataArray* data_array, float RealData, float ImgData){
    verb_print(HIGH, "enterd store_new_data\n");
    data_array->RealDataArray[data_array->Len] = RealData;
    data_array->ImgDataArray[data_array->Len] = ImgData;
    data_array->Len += 1;
    verb_print(HIGH, "DEBUG | going to store new record:\nRealData = %f, ImgData = %f, updated array Len = %d", RealData, ImgData, data_array->Len);
}

void send_data_array_to_client(DataArray* data_array_ch0, DataArray* data_array_ch1, int type ,int NumOfChannels ,int* client_socket_ptr){
    if (client_socket_ptr == NULL || data_array_ch0 == NULL || data_array_ch1 == NULL) {
        handle_fatal_error("Null pointer passed to send_data_array_to_client");
    }
    if (data_array_ch0->Len != data_array_ch0->TargetLen){
        handle_medium_error("Data length mismatch in send_data_array_to_client");
    }
    if(NumOfChannels == 2){
        if (data_array_ch1->Len != data_array_ch1->TargetLen){
            handle_medium_error("Data length mismatch in send_data_array_to_client");
        }
    }
    verb_print(HIGH, "entered send_data_array_to_client NumOfChannels = %d\n", NumOfChannels);
    verb_print(MED, "Goind to send data collected from the array to client. msg type = %d\n", type);
    //how data is going to be send:
    //* all data is send as bytes (string) need to encode it on server and decode it (convert back to num) on client side
    //* goint to send item by item.
    //1. first msg: type of msg = data, array_len, array_type [real, img, real+img]
    //2. payload one by one (in case read+img, real + img)
    //3. in case of 2 channels: first channel and then the other channel
    //implement:
    //print warning in case expected len is not equal to len.
    uint32_t len = data_array_ch0->Len;
    uint32_t expected_len = data_array_ch0->TargetLen;
    int client_socket = *client_socket_ptr;
    //int index = 0;
    char data[MAX_MSG_SIZE];
    memset(data, 0, MAX_MSG_SIZE);
    char header[MAX_MSG_SIZE];
    memset(header, 0, MAX_MSG_SIZE);
    if (len != expected_len){ //seems unrelevent due to if at the function head
        verb_print(MED, "Going to send data to client but not recv all the expcted data from logic.\n");
        // todo shahar review. now it is just a warning. maybe need to switch to exit. according to flow
        // we will defined later.
    }
   // implement here
   // build header of packet;
   build_data_packet_header(header, len, type);
   send_data_as_string_to_client(client_socket_ptr, header);
   send_data_array_to_client_according_to_type(data_array_ch0, type ,client_socket_ptr);
   if (NumOfChannels == 2){
        send_data_array_to_client_according_to_type(data_array_ch1, type ,client_socket_ptr);
   }
   //updata + print that all data is sent from client side.
   verb_print(HIGH, "finish sending to client data accroding to data type = %d", type);
}

void send_data_array_to_client_according_to_type(DataArray* data_array, int type ,int* client_socket_ptr){
    if (data_array == NULL || client_socket_ptr == NULL) {
        handle_fatal_error("Null pointer passed to send_data_array_to_client_according_to_type");
    }
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

void clear_uint32_array(float* array_head, uint32_t len){
    verb_print(HIGH, "entered clear_uint32_array\n");
    uint32_t index = 0;
    for (index = 0; index < len ; index++){
        // *(array_head + index) = 0;
        array_head[index] = 0;
    }
    print_uint32_array(array_head, len);
}

void print_uint32_array(float* array_head, uint32_t len){
    int index = 0;
    verb_print(HIGH,"print array = %p\n", array_head);
    for (index = 0; index < len; index++){
        verb_print(HIGH, "item %d | value = %f\t", index, *(array_head + index));
        if(index % 4 == 3){
            verb_print(HIGH, "\n");
        }
    }
}

void get_start_header(LogicConfig* logic_config, int* client_socket_ptr){
    if (logic_config == NULL || client_socket_ptr == NULL) {
        handle_fatal_error("Null pointer passed to get_start_header");
    }
    char header[MAX_DATA_LEN];
    memset(header, 0, MAX_DATA_LEN);
    recv_bytes_from_client(client_socket_ptr, header);
    decode_start_header(logic_config, header, client_socket_ptr);
}

uint32_t get_end_header(LogicConfig* logic_config, int* client_socket_ptr){
    if (logic_config == NULL || client_socket_ptr == NULL) {
        handle_fatal_error("Null pointer passed to get_end_header");
    }
    char header[MAX_DATA_LEN];
    memset(header, 0, MAX_DATA_LEN);
    recv_bytes_from_client(client_socket_ptr, header);
    return (decode_end_header(logic_config, header, client_socket_ptr));
}

void get_config_header(LogicConfig* logic_config, int* client_socket_ptr){
    if (logic_config == NULL || client_socket_ptr == NULL) {
        handle_fatal_error("Null pointer passed to get_config_header");
    }
    char header[MAX_DATA_LEN];
    memset(header, 0, MAX_DATA_LEN);
    recv_bytes_from_client(client_socket_ptr, header);
    decode_header(logic_config, header, client_socket_ptr);
}

uint32_t decode_end_header(LogicConfig* logic_config, char header[], int* client_socket_ptr){
    if (logic_config == NULL || header == NULL || client_socket_ptr == NULL) {
        handle_fatal_error("Null pointer passed to decode_end_header");
    }
    uint64_t header_as_uint = convert_string_to_hex_uint64_t(header);
    char pkt_type = (header_as_uint & 0xFF00000000000000) >> 56; //1B long
    verb_print(HIGH, "DEBUG | pkt_type recv from header = %d\n", pkt_type);
    uint32_t end_type_byte = (header_as_uint & 0x00FFFFFFFF000000) >> 24; //4B long
    verb_print(HIGH, "DEBUG | end_type_byte recv from header = %d\n", end_type_byte);
    char control_byte = (header_as_uint & 0x0000000000FF0000) >> 16; //4B long;//1B long
    verb_print(HIGH, "DEBUG | control_byte recv from header = %d\n", control_byte);
    if ((pkt_type == CONFIG_PKT) && (end_type_byte >= RESTART) && (control_byte == 255)){ //meaning good packet
        //UpdateStartSent(logic_config, TRUE); //should we need this
        send_end_ack(logic_config, pkt_type, end_type_byte, control_byte, client_socket_ptr);
        return end_type_byte;
    }
    //todo shahar need to define routine for what to do if bad packet is recived.    
}

void decode_start_header(LogicConfig* logic_config, char header[], int* client_socket_ptr){
    if (logic_config == NULL || header == NULL || client_socket_ptr == NULL) {
        handle_fatal_error("Null pointer passed to decode_start_header");
    }
    uint64_t header_as_uint = convert_string_to_hex_uint64_t(header);
    char pkt_type = (header_as_uint & 0xFF00000000000000) >> 56; //1B long
    verb_print(HIGH, "DEBUG | pkt_type recv from header = %d\n", pkt_type);
    uint32_t start_bit = (header_as_uint & 0x00FFFFFFFF000000) >> 24; //4B long
    verb_print(HIGH, "DEBUG | start_bit recv from header = %d\n", start_bit);
    char control_byte = (header_as_uint & 0x0000000000FF0000) >> 16; //4B long;//1B long
    verb_print(HIGH, "DEBUG | control_byte recv from header = %d\n", control_byte);
    if ((pkt_type == START_PKT) && (start_bit == 1) && (control_byte == 255)){ //meaning good packet
        UpdateStartSent(logic_config, TRUE);
        send_start_ack(logic_config, pkt_type, start_bit, control_byte, client_socket_ptr);
    }
    //todo shahar need to define routine for what to do if bad packet is recived.    
}

//todo shahar change name to decode_config_header
void decode_header(LogicConfig* logic_config, char header[], int* client_socket_ptr){
    if (logic_config == NULL || header == NULL || client_socket_ptr == NULL) {
        handle_fatal_error("Null pointer passed to decode_header");
    }
    uint64_t header_as_uint = convert_string_to_hex_uint64_t(header);
    char pkt_type = (header_as_uint & 0xFF00000000000000) >> 56; //1B long
    verb_print(HIGH, "DEBUG | pkt_type recv from header = %d\n", pkt_type);
    uint32_t phase_inc = (header_as_uint & 0x00FFFFFFFF000000) >> 24; //4B long
    verb_print(HIGH, "DEBUG | phase_inc recv from header = %d\n", phase_inc);
    char control_byte = (header_as_uint & 0x0000000000FF0000) >> 16; //4B long;//1B long
    verb_print(HIGH, "DEBUG | control_byte recv from header = %d\n", control_byte);
    if ((pkt_type == CONFIG_PKT) && (control_byte == 255)){ //meaning good packet
        UpdateConfigSent(logic_config, TRUE);
        UpdatePhaseInc(logic_config, phase_inc);
        send_config_ack(logic_config, pkt_type, phase_inc, control_byte, client_socket_ptr);
    }
    //todo shahar need to define routine for what to do if bad packet is recived.
}

void send_end_ack(LogicConfig* logic_config, char pkt_type, uint32_t end_type_byte, char control_byte, int* client_socket_ptr){
    verb_print(HIGH, "entered send_end_ack\n");
    uint64_t header_as_uint = 0x0000000000000000;
    uint64_t pkt_type_casted = (uint64_t)pkt_type << 56;
    verb_print(HIGH, "DEBUG | pkt_type = 0x%016llx \n", pkt_type_casted);
    uint64_t end_type_byte_casted = (uint64_t)end_type_byte << 24;
    verb_print(HIGH, "DEBUG | phase_inc = 0x%016llx \n", end_type_byte_casted);
    uint64_t control_byte_casted = 0x00 << 16;
    verb_print(HIGH, "DEBUG | control_byte = 0x%016llx \n", control_byte_casted);
    header_as_uint = header_as_uint | pkt_type_casted | end_type_byte_casted | control_byte_casted;
    char header[MAX_DATA_LEN];
    memset(header, 0, MAX_DATA_LEN);
    convert_hex_to_string_unformnatted(header_as_uint, header);
    send_data_as_string_to_client(client_socket_ptr, header);
}

void send_start_ack(LogicConfig* logic_config, char pkt_type, uint32_t start_bit, char control_byte, int* client_socket_ptr){
    verb_print(HIGH, "entered send_start_ack\n");
    uint64_t header_as_uint = 0x0000000000000000;
    uint64_t pkt_type_casted = (uint64_t)pkt_type << 56;
    verb_print(HIGH, "DEBUG | pkt_type = 0x%016llx \n", pkt_type_casted);
    uint64_t start_bit_casted = (uint64_t)start_bit << 24;
    verb_print(HIGH, "DEBUG | phase_inc = 0x%016llx \n", start_bit_casted);
    uint64_t control_byte_casted = 0x00 << 16;
    verb_print(HIGH, "DEBUG | control_byte = 0x%016llx \n", control_byte_casted);
    header_as_uint = header_as_uint | pkt_type_casted | start_bit_casted | control_byte_casted;
    char header[MAX_DATA_LEN];
    memset(header, 0, MAX_DATA_LEN);
    convert_hex_to_string_unformnatted(header_as_uint, header);
    send_data_as_string_to_client(client_socket_ptr, header);
}

void send_config_ack(LogicConfig* logic_config, char pkt_type, uint32_t phase_inc, char control_byte, int* client_socket_ptr){
    verb_print(HIGH, "entered send_config_ack\n");
    uint64_t header_as_uint = 0x0000000000000000;
    uint64_t pkt_type_casted = (uint64_t)pkt_type << 56;
    verb_print(HIGH, "DEBUG | pkt_type = 0x%016llx \n", pkt_type_casted);
    uint64_t phase_inc_casted = (uint64_t)phase_inc << 24;
    verb_print(HIGH, "DEBUG | phase_inc = 0x%016llx \n", phase_inc_casted);
    uint64_t control_byte_casted = 0x00 << 16;
    verb_print(HIGH, "DEBUG | control_byte = 0x%016llx \n", control_byte_casted);
    header_as_uint = header_as_uint | pkt_type_casted | phase_inc_casted | control_byte_casted;
    char header[MAX_DATA_LEN];
    memset(header, 0, MAX_DATA_LEN);
    convert_hex_to_string_unformnatted(header_as_uint, header);
    send_data_as_string_to_client(client_socket_ptr, header);
}

//todo shahar connect to vars/defines
void init_buffer_info(BufferInfo* buffer_info, uint32_t buffer_len, uint64_t buffer_base_address){
    if (buffer_info == NULL) {
        handle_fatal_error("BufferInfo pointer is NULL");
    }
    buffer_info->buffer_base_address = buffer_base_address;
    buffer_info->buffer_len = buffer_len;
}

int unload_data_from_logic(DataArray* data_array, int Ch){
    // int index = 0;
    // int offset = 0;
    int result = TRUE;
    // uint32_t data_from_logic = 0x00000000;
    // for (index = 0; index < data_array->TargetLen; index++){
    //     offset = index * 4;
    //     data_from_logic = read_from_logic(BUFFER_BASE_ADDR + offset);
    //     store_new_data(data_array, data_from_logic, 0);
    // }
    // Assuming read_from_array returns a status code or boolean indicating success/failure
    if (!read_from_array(data_array, Ch)) {
        handle_medium_error("Failed to read data from array in unload_data_from_logic");
        // Decide if you need to stop processing or can continue
        result = FALSE;
    }
    //read_from_array(data_array);
    if (!check_all_data_read(data_array)) {
        handle_easy_error("Not all expected data was read in unload_data_from_logic");
        // This might not be critical, so logged as an easy error
        result = FALSE;
    }
    //result = check_all_data_read(data_array);
    return result;
}

int check_all_data_read(DataArray* data_array){
    if (data_array->TargetLen != data_array->Len){
        return FALSE;
    }
    return TRUE;
}

int unload_data_from_logic_ch1(DataArray* data_array, int Ch){
    int result = TRUE;
    
    if (!read_from_array_ch1(data_array, Ch)) {
        handle_medium_error("Failed to read data from array in unload_data_from_logic ch1");
        // Decide if you need to stop processing or can continue
        result = FALSE;
    }
    //read_from_array(data_array);
    if (!check_all_data_read(data_array)) {
        handle_easy_error("Not all expected data was read in unload_data_from_logic ch1");
        // This might not be critical, so logged as an easy error
        result = FALSE;
    }
    //result = check_all_data_read(data_array);
    return result;
}