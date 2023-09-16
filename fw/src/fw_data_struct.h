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
}LogicConfig;

typedef struct _Events{
    uint32_t EventsBitsVector;
    //todo shahar add evenets signals according to logic designs
}Events;

typedef struct _DataArray{
    uint32_t Len;
    uint32_t TargetLen; //can be at most MAX_DATA_LEN
    uint32_t RealDataArray[MAX_DATA_LEN];
    uint32_t ImgDataArray[MAX_DATA_LEN];
    int RecvAllExpectedData; //bool
}DataArray;

//struct functions:
void init_logic_config_struct(LogicConfig* new_logic_config);
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data);
void UpdateGotFinish(LogicConfig* logic_config, uint32_t data);
void UpdatePhaseInc(LogicConfig* logic_config, uint32_t data);
void UpdateStartSent(LogicConfig* logic_config, uint32_t data);

void init_events_struct(Events* new_event_struct);

void init_data_array_struct(DataArray* new_data_array, uint32_t expected_data_len);
void store_new_data(DataArray* data_array, uint32_t RealData, uint32_t ImgData);
void send_data_array_to_client(DataArray* data_array, int type ,int* client_socket_ptr);
void send_data_array_to_client_according_to_type(DataArray* data_array, int type ,int* client_socket_ptr);

void clear_uint32_array(uint32_t* array_head, uint32_t len);
void print_uint32_array(uint32_t* array_head, uint32_t len);

#endif