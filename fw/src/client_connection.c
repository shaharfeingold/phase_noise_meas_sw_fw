#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "utils_function.h"
#include "defines.h"


int create_server_socket(){
	verb_print(MED, "create server socket\n");
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1){ //todo shahar define handling error and implement.
		perror("Error creating socket");
        exit(1);
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
        	perror("Error binding");  //todo shahar define handling error and implement.
        	exit(1);
    	}
}

void listen_server_socket(int server_socket){
	verb_print(MED, "server start listing on port = %d\n", PORT); // todo shahar consider set port as arg rather the define
	if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        	perror("Error listening"); //todo shahar define handling error and implement.
        	exit(1);
   	 	}
}

int accept_connection(int server_socket, _sockaddr_in* client_addr){
	verb_print(MED, "entered accept_connection function\n");
	verb_print(HIGH, "waiting for a client to connect to server\n");
	int client_socket;
	socklen_t client_addr_len = sizeof(*client_addr);
    if ((client_socket = accept(server_socket, (struct sockaddr*)client_addr, &client_addr_len)) == -1) {
        perror("Error accepting connection"); //todo shahar define handling error and implement.
        exit(1);
    }	
	verb_print(MED, "DEBUG | connection accepted, return client socket\n");
	return client_socket;
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

