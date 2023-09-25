#ifndef _DEFINES
#define _DEFINES

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
#define RESTART 253
#define REDO 254
#define END_CONNECTION 255 //0xFF

typedef struct sockaddr_in _sockaddr_in;

//buffer info
//todo shahar need to review this defines after finish logic
#define BUFFER_LEN  MAX_DATA_LEN //buffer inside the logic
#define BUFFER_BASE_ADDR 0x0000000000000000

//Event Mask
//todo shahar need to make after define which events should be read by the fw according to severity
#define EVENT_MASK 0xffffffff
#define EVENT_ADDRESS 0x0000000000000000 //todo shahar change after implement logic 8B
//todo shahar consider pack the address in struct

#endif