#ifndef _CLIENT_CONNECTION
#define _CLIENT_CONNECTION

#include <stdint.h>
#include "defines.h"

#define MAX_MSG_SIZE 1024 //in terms of bytes. //todo shahar move to defines file //todo shahar review this number

int create_server_socket();
void set_server_addr_struct(_sockaddr_in* server_addr);
void bind_server_socket(int server_socket, _sockaddr_in* server_addr);
void listen_server_socket(int server_socket);
int accept_connection(int server_socket, _sockaddr_in* client_addr);
void init_connection(int* server_socket, int* client_socket, _sockaddr_in* server_addr, _sockaddr_in* client_addr);

void send_data_as_string_to_client(int* client_socket, char data[]); //this function gets the data as encoded string. only sends bytes
void send_uint32_t_to_client(int* client_socket, float data);

void recv_bytes_from_client(int* client_socket_ptr, char data[]);
#endif