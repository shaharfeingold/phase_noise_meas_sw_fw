#ifndef _DEFINES
#define _DEFINES
//todo shahar review all the declare of the function with address maybe need only 4B address space.
#include <stdint.h>

#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024 //in terms of byte //todo shaharf see that it is match with client
#define PORT 8080
#define MAX_DATA_LEN 1024
#define MAX_DATA_WIDTH 32 //todo shahar TBD.

//data msg types defines:
#define REAL_DATA_MSG 0
#define IMG_DATA_MSG 1
#define REAL_IMG_DATA_MSG 2

// packet types:
#define IDLE_PKT 0
#define CONFIG_PKT 1
#define DATA_PKT 2
#define ERROR_PKT 3
#define START_PKT 4
#define RESTART 253
#define REDO 254
#define END_CONNECTION 255 //0xFF

typedef struct sockaddr_in _sockaddr_in;

//buffer info
//todo shahar need to review this defines after finish logic
#define BUFFER_LEN  MAX_DATA_LEN //buffer inside the logic
#define BUFFER_BASE_ADDR_CH0 0x40000000
#define BUFFER_BASE_ADDR_CH1 0x43C00000

//config addr
//todo shahar need to review this defines after finish logic
#define CONIG_BASE_ADDR 0x41210000 
#define START_CONFIG_BIT 0
#define START_OP_BIT 1
#define RESTART_VLD_BIT 2
#define RESTART_TYPE_BIT 3
#define RESTART_TYPE_WIDTH 2
//phase_inc
#define PHASE_INC_ADDR 0x41200000
#define PHASE_INC_WIDTH 16 

//finsih_op and data_count regs addr
#define FINISH_OP_DATA_COUNT_BASE_ADDR 0x41220000
#define FINISH_OP_MASK_1_CH 0x00000800 //in case we are using one channal
#define FINISH_OP_MASK_2_CH 0x00400000 //in case we are using two channal
#define DATA_COUNT_MASK_1 0x000007FF
#define DATA_CONUT_MASK_2 0x003FF800

//Event Mask
//todo shahar need to make after define which events should be read by the fw according to severity
#define EVENT_MASK 0xffffffff
#define EVENT_ADDRESS 0x0000000000000000 //todo shahar change after implement logic 8B
//todo shahar consider pack the address in struct

//array number configuration
//need to review in case format changes during development of fpga
#define ARRAY_FRCTIONAL_BIT 29
#define ARRAY_NUM_WIDTH 32

#define MAX_MSG_SIZE 1024 //in terms of bytes //todo shahar review this number
#define MAP_FAILED ((void *)-1)
#define INVALID_READ 0xFFFFFFFF 


#endif