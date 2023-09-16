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

typedef struct sockaddr_in _sockaddr_in;

#endif