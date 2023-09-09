#ifndef _CLIENT_CONNECTION
#define _CLIENT_CONNECTION

#include <stdint.h>
#include "defines.h"

int create_server_socket();
void set_server_addr_struct(_sockaddr_in* server_addr);
void bind_server_socket(int server_socket, _sockaddr_in* server_addr);
void listen_server_socket(int server_socket);
int accept_connection(int server_socket, _sockaddr_in* client_addr);
void init_connection(int* server_socket, int* client_socket, _sockaddr_in* server_addr, _sockaddr_in* client_addr);

#endif