#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "utilities.h"

#define MAX_INPUT_LENGTH 1024

#define SEND_MESSAGE_SIZE 512
#define RECEIVE_BUFFER_SIZE 32

#define DEFAULT_SERVER_PORT 7

#define PROMPT "(napster) "

void add_file(int sock, char *string);

int arguments_exist(char *args);
int check_file_name_length(char *file_name);

int server_command(char *server_ip, unsigned int server_port, int (*pre_connect_function)(char*), void (*command_function)(int, char*), char *args);

int server_connect(char *server_ip, unsigned int server_port);
int server_disconnect(int sock);

int main(int argc, char *argv[]) {
	char *server_ip;
	unsigned int server_port;

	if ((argc < 3) || (argc > 3)) {
	   fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>]\n", argv[0]);
	   exit(1);
	}

	server_ip = argv[1]; // First argument – Server IP (dotted quad)

	// Use a given port if one is provided, otherwise the default
	if (argc == 3) {
		server_port = atoi(argv[2]);
	} else {
		server_port = DEFAULT_SERVER_PORT;
	}

	char *input = malloc(sizeof(char) * MAX_INPUT_LENGTH);
	assert(input);

	char **args = 0;

	while(1) {
		char *console_status;

		// Get input from the user
		printf("%s", PROMPT);
		console_status = fgets(input, MAX_INPUT_LENGTH, stdin);

		if (console_status) {
			if (args) {
				free(args);
			}

			args = split(input, " \n");

			char *command = args[0];

			if (command) {
				if (strcmp(command, "quit") == 0) {
					break;
				} else if (strcmp(command, "add") == 0) {
					server_command(server_ip, server_port, check_file_name_length, add_file, args[1]);
				}
			}
		}
	}

	free(input);

	for (int i = 0; i < sizeof(args) / sizeof(char *); i++) {
		free(args[i]);
	}
	free(args);

	exit(0);
}

/**
 * Add a file command.
 * @param sock   Socket descriptor
 * @param string File name to add to server's list
 */
void add_file(int sock, char *string) {
	char *message = calloc(1, sizeof(char) * SEND_MESSAGE_SIZE);

	snprintf(message, SEND_MESSAGE_SIZE, "ADD %s\n", string);

	send(sock, message, strlen(message), 0);

	printf("Sent %s\n", message);
}

/**
 * Returns 1 if the arguments exist and are not of size 0.
 * @param  args Arguments string
 * @return 1 for true, 0 for false
 */
int arguments_exist(char *args) {
	if (args && strlen(args) > 0) {
		return 1;
	} else {
		printf("Command must have argument(s).");

		return 0;
	}
}

/**
 * Makes sure the file name length is valid.
 * @param  file_name File name string
 * @return 1 for true, 0 for false
 */
int check_file_name_length(char *file_name) {
	if (!arguments_exist(file_name)) {
		return 0;
	}

	if (file_name && strlen(file_name) > 0 && strlen(file_name) <= MAX_FILE_NAME_LENGTH) {
		return 1;
	} else {
		printf("File name cannot be longer than %d characters.", MAX_FILE_NAME_LENGTH);

		return 0;
	}
}

/**
 * Connects to the server and runs a command using that socket.
 *
 * @param server_ip				Server IP (dotted quad)
 * @param server_port			Port Number
 * @param pre_connect_function	Function that validates the input before connecting
 * @param command_function		Function that takes in socket descriptor and char*
 * @param args					Args to pass to command_function
 * @return Result of socket creation
 */
int server_command(char *server_ip, unsigned int server_port, int (*pre_connect_function)(char*), void (*command_function)(int, char*), char *args) {
	// If args don't pass the test, don't do anything
	if (pre_connect_function && pre_connect_function(args) == 0) {
		printf(" Try 'help'.\n");

		return -1;
	}	

	int sock = server_connect(server_ip, server_port);

	if (sock == -1) {
		return -1;
	} else {
		command_function(sock, args);
		close(sock);

		return 0;
	}
}

/**
 * Create a socket and connect to the server.
 * 
 * @param server_ip		Server IP (dotted quad)
 * @param server_port	Port Number
 * @return Socket descriptor
 */
int server_connect(char *server_ip, unsigned int server_port) {
	// Create a socket using TCP
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Could not create socket.\n");
		return -1;
	}

	// Construct the server address structure
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(server_ip);
	server_address.sin_port = htons(server_port);

	// Establish the connection to the server
	if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "Could not connect to server.\n");
		return -1;
	}

	printf("Connected to the server!\n");

	return sock;
}

/**
 * Close the socket.
 * 
 * @param sock Socket
 * @return Result of close
 */
int server_disconnect(int sock) {
	printf("Disconnecting from server!\n");

	return close(sock);
}