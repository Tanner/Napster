#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define RECEIVE_BUFFER_SIZE 32

#define DEFAULT_SERVER_PORT 7

#define PROMPT "(napster) "

void split(char *input, char **split, char *delimiter, int count);
void echo(int sock, char *string);

int connect_to_server(char *server_ip, unsigned int server_port);
int disconnect(int sock);

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

	char *input = malloc(sizeof(char) * 100);
	assert(input);

	int max_args = 2;
	char **args = malloc(sizeof(char *) * (max_args + 1));
	assert(args);

	while(1) {
		char *console_status;

		// Get input from the user
		printf("%s", PROMPT);
		console_status = fgets(input, 100, stdin);

		if (console_status) {
			split(input, args, " \n", max_args + 1);

			char *command = args[0];

			if (command) {
				if (strcmp(command, "quit") == 0) {
					break;
				} else if (strcmp(command, "send") == 0) {
					int sock = connect_to_server(server_ip, server_port);

					if (sock != -1) {
						echo(sock, args[1]);
						close(sock);
					}
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

void echo(int sock, char *string) {
	char buffer[RECEIVE_BUFFER_SIZE];
	unsigned int string_length;

	string_length = strlen(string);

	// Send the string to the server
	if (send(sock, string, string_length, 0) != string_length) {
		fprintf(stderr, "Sent a different number of bytes than expected.\n");
	}

	// Receive data from the server
	printf("Received: ");

	int total_bytes_received = 0;
	int bytes_received = 0;
	while (total_bytes_received < string_length) {
		if ((bytes_received = recv(sock, buffer, RECEIVE_BUFFER_SIZE - 1, 0)) <= 0) {
			fprintf(stderr, "Received failed or connection closed prematurely.\n");
		}

		total_bytes_received += bytes_received;
		buffer[bytes_received] = '\0';
		printf("%s", buffer);
	}

	printf("\n");
}

/**
 * Create a socket and connect to the server.
 * 
 * @param  server_ip   Server IP (dotted quad)
 * @param  server_port Port Number
 * @return             Socket descriptor
 */
int connect_to_server(char *server_ip, unsigned int server_port) {
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
 * @param  sock Socket
 * @return      Result of close
 */
int disconnect(int sock) {
	return close(sock);
}

/**
 * Split a string into x variable.
 *
 * @param *input String to split up
 * @param **split Where to store results of split
 * @param *delimiter Delimiter to use
 * @param count Number of splits to make
 */
void split(char *input, char **split, char *delimiter, int count) {
    char *temp = strtok(input, delimiter);

    for (int i = 0; i < count; i++) {
        if (split[i] != NULL) {
            free(split[i]);

            split[i] = NULL;
        }

        if (temp != NULL) {
            split[i] = malloc(strlen(temp) + 1);
            assert(split[i]);

            strcpy(split[i], temp);
        }

        temp = strtok(NULL, delimiter);
    }
}