#include "server.h"
#include "utilities.h"

file_source** file_sources;
int file_source_count;

void handle_client(int client_socket, struct sockaddr_in client_address);

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

	// Create the array to hold file sources
	file_sources = calloc(1, sizeof(file_source));
	assert(file_sources);

	file_source_count = 0;

	printf("Server started...\n");

	while(1) {
		// Set the size of the in-out parameter
		client_address_size = sizeof(client_address);

		// Wait for a client to connect
		if ((client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_size)) < 0) {
			fprintf(stderr, "Could not accept a client.");
		}

		// Handle the client
		handle_client(client_socket, client_address);
	}
}

void handle_client(int client_socket, struct sockaddr_in client_address) {
	char *message = get_response(client_socket);

	printf("Got request from %s – %s – Size: %d\n", inet_ntoa(client_address.sin_addr), message, (int) strlen(message));

	char **parse = parse_message(message);

	if (parse) {
		printf("Command: '%s'\n", parse[0]);

		if (strcmp(parse[0], "ADD") == 0) {
			printf("File Name: '%s' (length %d)\n", parse[1], (int) strlen(parse[1]));
		}

		for (int i = 0; i < sizeof(parse) / sizeof(char *); i++) {
			free(parse[i]);
		}

		free(parse);
	} else {
		printf("Invalid request.");
	}

	free(message);

	close(client_socket);
}