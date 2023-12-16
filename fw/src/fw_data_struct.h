#ifndef _FW_DATA_STRUCT
#define _FW_DATA_STRUCT

#include <stdint.h>
#include "defines.h"

//struct declaration
typedef struct _LogicConfig{
    uint32_t phase_inc;
    uint32_t config_sent;
    uint32_t start_sent;
    uint32_t got_finish;
    uint64_t config_address;
}LogicConfig;

typedef struct _Events{
    uint32_t EventsBitsVector;
    uint32_t EventMask;
    uint32_t EventVectorMasked;
    int test_event0; //pos bit 0
    int test_event1; //pos bit 1
    //todo shahar add evenets signals according to logic designs
}Events;

typedef struct _DataArray{
    uint32_t Len;
    uint32_t TargetLen; //can be at most MAX_DATA_LEN
    uint32_t RealDataArray[MAX_DATA_LEN];
    uint32_t ImgDataArray[MAX_DATA_LEN];
    int RecvAllExpectedData; //bool
}DataArray;

typedef struct _BufferInfo{
    uint32_t buffer_len;
    uint64_t buffer_base_address;
}BufferInfo;

//struct functions:
void init_logic_config_struct(LogicConfig* new_logic_config);
void get_config_header(LogicConfig* logic_config, int* client_socket_ptr);
void get_start_header(LogicConfig* logic_config, int* client_socket_ptr);
void decode_header(LogicConfig* logic_config, char header[], int* client_socket_ptr);
void decode_start_header(LogicConfig* logic_config, char header[], int* client_socket_ptr);
void send_config_ack(LogicConfig* logic_config, char pkt_type, uint32_t phase_inc, char control_byte, int* client_socket_ptr);
void send_start_ack(LogicConfig* logic_config,char pkt_type, uint32_t start_bit, char control_byte, int* client_socket_ptr);
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data);
void UpdateGotFinish(LogicConfig* logic_config, uint32_t data);
void UpdatePhaseInc(LogicConfig* logic_config, uint32_t data);
void UpdateStartSent(LogicConfig* logic_config, uint32_t data);
void UpdateConfigAddress(LogicConfig* logic_config, uint64_t config_address);

//todo shahar below function need to implement. job: need to write to logic start operation + read when operation finish + collect data and send to client
//todo shahar need to review the width of the data is read from logic
void send_to_logic_start_operation(LogicConfig* logic_config);
char wait_to_finish_operation(LogicConfig* logic_config); //todo shahar here while loop up until we read finish. + consider add a feature to have time out
//void unload_data_from_logic(DataArray* data_array, BufferInfo* buffer_info);
void get_from_client_start_operation(LogicConfig* logic_config);
void start_operation_wrapper(LogicConfig* logic_config, BufferInfo* buffer_info); //todo shahar wrapper to all above.

void init_events_struct(Events* new_event_struct, uint32_t event_mask);
void UpdateEventsVec(Events* event_strcut, uint32_t new_event_vec);
void handle_read_new_event_vector(Events* event_struct, uint32_t new_event_vector); //todo implemt
void decode_event_vector(Events* event_struct); //todo implemenet

void init_data_array_struct(DataArray* new_data_array, uint32_t expected_data_len);
void store_new_data(DataArray* data_array, uint32_t RealData, uint32_t ImgData);
void send_data_array_to_client(DataArray* data_array, int type ,int* client_socket_ptr);
void send_data_array_to_client_according_to_type(DataArray* data_array, int type ,int* client_socket_ptr);

void clear_uint32_array(uint32_t* array_head, uint32_t len);
void print_uint32_array(uint32_t* array_head, uint32_t len);

void init_buffer_info(BufferInfo* buffer_info, uint32_t buffer_len, uint64_t buffer_base_address);

int unload_data_from_logic(DataArray* data_array);

#endif