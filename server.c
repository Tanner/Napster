#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "utilities.h"

#define MAX_PENDING 5

void handleClient(int client_socket);

int main(int argc, char *argv[])
{
	int server_socket;
	int client_socket;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	unsigned int client_address_size;
	unsigned short server_port;

	if (argc != 2){
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}

	server_port = atoi(argv[1]); // First argument – port number

	// Create socket for incoming connections
	if ((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Could not create socket.");
		exit(2);
	}

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(server_port);

	// Bind to the server address
	if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "Could not bind to server address.");
		exit(2);
	}

	// Mark the socket so it will listen for incoming connections
	if (listen(server_socket, MAX_PENDING) < 0) {
		fprintf(stderr, "Could not listen for incoming connections.");
		exit(2);
	}

	printf("Server started...\n");

	while(1) {
		// Set the size of the in-out parameter
		client_address_size = sizeof(client_address);

		// Wait for a client to connect
		if ((client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_size)) < 0) {
			fprintf(stderr, "Could not accept a client.");
		}

		// Handle the client
		printf("Handling client %s\n", inet_ntoa(client_address.sin_addr));

		handleClient(client_socket);
	}
}

void handleClient(int client_socket) {
	char *message = get_response(client_socket);

	printf("Done – %s – Size: %d\n", message, (int) strlen(message));

	free(message);

	close(client_socket);
}