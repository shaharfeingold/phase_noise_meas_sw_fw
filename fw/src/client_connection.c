#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024 //in terms of byte //todo shaharf see that it is match with client
#define PORT 8080

int main(){
	int server_socket, client_socket;
    	struct sockaddr_in server_addr, client_addr;
   	char buffer[BUFFER_SIZE];

    	// Create a socket
    	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        	perror("Error creating socket");
        	exit(1);
    	}

    	// Configure server address
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_port = htons(PORT);
    	server_addr.sin_addr.s_addr = INADDR_ANY;

    	// Bind the socket to the specified address and port
    	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        	perror("Error binding");
        	exit(1);
    	}
	


	// Listen for incoming connections
    	if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        	perror("Error listening");
        	exit(1);
   	 }	

    	printf("Server listening on port %d\n", PORT);

    	socklen_t client_addr_len = sizeof(client_addr);

    	// Accept incoming connections
    	if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
        	perror("Error accepting connection");
        	exit(1);
    	}	

    	printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	// Receive data from the client
    	memset(buffer, 0, sizeof(buffer));
    	if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
        	perror("Error receiving data");
        	exit(1);
   	 }

    	printf("Received message from client: %s\n", buffer);
	
	// Echo the msg back to client
	printf("trying to echo the msg\n");

	if (send(client_socket, buffer, strlen(buffer), 0) == -1){
		perror("Error sending data to client");
	}
		

    	// Close the sockets
    	close(client_socket);
    	close(server_socket);

	return 0;
}

