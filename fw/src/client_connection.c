#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h> // Required for TCP_NODELAY
#include "error_handling.h"
#include "client_connection.h"
#include "utils_function.h"
#include "defines.h"


int create_server_socket() {
    verb_print(MED, "create server socket\n");
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        handle_fatal_error("Error creating socket");
    }
    return server_socket;
}


void set_server_addr_struct(_sockaddr_in* server_addr){ //todo shahar consider to pass the port number as arg.
	verb_print(MED, "set server addr strcut\n");
	server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr.s_addr = INADDR_ANY;
}

void bind_server_socket(int server_socket, _sockaddr_in* server_addr){
	verb_print(MED, "bind server socket to addr and port\n");
	if (bind(server_socket, (struct sockaddr*)server_addr, sizeof(*server_addr)) == -1) {
			//todo shahar check syntax (struct sockaddr*)server_addr if correct.
			//todo shahar check syntax sizeof(*server_addr) if correct.
        	close(server_socket); // Close the socket before error handling
			handle_fatal_error("Error binding");
    	}
}

void listen_server_socket(int server_socket){
	verb_print(MED, "server start listing on port = %d\n", PORT); // todo shahar consider set port as arg rather the define
	if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        	handle_fatal_error("Error listening");
   	 	}
}

int accept_connection(int server_socket, _sockaddr_in* client_addr){
	verb_print(MED, "entered accept_connection function\n");
	verb_print(HIGH, "waiting for a client to connect to server\n");
	int client_socket;
	socklen_t client_addr_len = sizeof(*client_addr);
    if ((client_socket = accept(server_socket, (struct sockaddr*)client_addr, &client_addr_len)) == -1) {
        close(server_socket); // Close the server socket before error handling
		handle_fatal_error("Error accepting connection");
    }	

	// Enable TCP_NODELAY
    int flag = 1;
    if (setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int)) == -1) {
        handle_fatal_error("TCP_NODELAY setting failed");
    }

	verb_print(MED, "DEBUG | connection accepted, return client socket\n");
	return client_socket;
}

void init_connection(int* server_socket, int* client_socket, _sockaddr_in* server_addr, _sockaddr_in* client_addr){
	*server_socket = create_server_socket();
    set_server_addr_struct(server_addr);
    bind_server_socket(*server_socket, server_addr);
    listen_server_socket(*server_socket);

    // wait for a client to connect
    *client_socket = accept_connection(*server_socket, client_addr);
}

void send_data_as_string_to_client(int* client_socket, char data[]){
	verb_print(HIGH, "send_data_as_string_to_client with data = %s\n", data);
	//todo shahar need to make sure that the data send is ended with string null to work properly.
	int bytes_sent = 0;
	int bytes_left = 0;
	// bytes_sent += send(*client_socket, data, strlen(data), 0); //here we are sending only up until null char
	bytes_sent += send(*client_socket, data, MAX_MSG_SIZE, 0); //here we send the entire buffer include the null chars
	verb_print(HIGH, "DEBUG | first attempt, #bytes sends = %d\n", bytes_sent);
	if (bytes_sent == -1){
 		handle_medium_error("Error sending data to client");
		close(*client_socket);  // Close the client socket
    	return;  // Exit the function
	}
	//todo shahar code review this section
	while(bytes_sent < strlen(data)){
		bytes_left = strlen(data) - bytes_sent;
		bytes_sent += send(*client_socket, data + bytes_sent, bytes_left, 0);
		verb_print(HIGH, "DEBUG | other attempt attempt, #bytes sends = %d, bytes left = %d\n", bytes_sent, bytes_left);
	}
}

void send_uint32_t_to_client(int* client_socket, float data){
	uint32_t data_in_network_order = encode_uint_data_to_send(data);
	verb_print(HIGH, "send_data_as_uint32_to_client with data = %f | 0x%x\n", data, data_in_network_order);
	int bytes_sent = 0;
	int bytes_left = 0;
	bytes_sent += send(*client_socket, &data_in_network_order, sizeof(data_in_network_order), 0);
	verb_print(HIGH, "DEBUG | first attempt, #bytes sends = %d\n", bytes_sent);
	if (bytes_sent == -1){
 		handle_medium_error("Error sending data to client");
		close(*client_socket);  // Close the client socket
    	return;  // Exit the function
	}
	//todo shahar review this section.
	while(bytes_sent < sizeof(data_in_network_order)){
		bytes_left = sizeof(data_in_network_order) - bytes_sent;
		bytes_sent += send(*client_socket, &data_in_network_order + bytes_sent, bytes_left, 0);
		verb_print(HIGH, "DEBUG | other attempt attempt, #bytes sends = %d, bytes left = %d\n", bytes_sent, bytes_left);
	}
}
// todo - if null -> retry
void recv_bytes_from_client(int* client_socket_ptr, char data[]){
	int bytes_recv = recv(*client_socket_ptr, data, MAX_MSG_SIZE, 0);
	if (bytes_recv == -1) {
        handle_easy_error("Error receiving bytes from client");
		// todo - Decide if you need to close the socket here
    }
	verb_print(HIGH, "got the following bytes from socket : %s\n", data);
}

// int main(){
	// int server_socket, client_socket;
    	// struct sockaddr_in server_addr, client_addr;
   	// char buffer[BUFFER_SIZE];

    	// Create a socket
    	// if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        // 	perror("Error creating socket");
        // 	exit(1);
    	// }

    	// Configure server address
    	// server_addr.sin_family = AF_INET;
    	// server_addr.sin_port = htons(PORT);
    	// server_addr.sin_addr.s_addr = INADDR_ANY;

    	// Bind the socket to the specified address and port
    	// if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        // 	perror("Error binding");
        // 	exit(1);
    	// }
	


	// Listen for incoming connections
    	// if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        // 	perror("Error listening");
        // 	exit(1);
   	 	// }	

    	// printf("Server listening on port %d\n", PORT);

    	// socklen_t client_addr_len = sizeof(client_addr);

    	// // Accept incoming connections
    	// if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
        // 	perror("Error accepting connection");
        // 	exit(1);
    	// }	

    	// printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	// Receive data from the client
//     	memset(buffer, 0, sizeof(buffer));
//     	if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
//         	perror("Error receiving data");
//         	exit(1);
//    	 }

//     	printf("Received message from client: %s\n", buffer);
	
// 	// Echo the msg back to client
// 	printf("trying to echo the msg\n");

// 	if (send(client_socket, buffer, strlen(buffer), 0) == -1){
// 		perror("Error sending data to client");
// 	}
		

//     	// Close the sockets
//     	close(client_socket);
//     	close(server_socket);

// 	return 0;
// }

