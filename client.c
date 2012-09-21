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

int main(int argc, char *argv[]) {
	char *server_ip;
	unsigned int server_port;

	char *echoString;                /* String to send to echo server */
	char echoBuffer[RECEIVE_BUFFER_SIZE];     /* Buffer for echo string */
	unsigned int echoStringLen;      /* Length of string to echo */

	if ((argc < 3) || (argc > 4)) {
	   fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
	   exit(1);
	}

	server_ip = argv[1];	// First argument – Server IP (dotted quad)
	echoString = argv[2];	// Second argument – String to echo

	echoStringLen = strlen(echoString);

	// Use a given port if one is provided, otherwise the default
	if (argc == 4) {
		server_port = atoi(argv[3]);
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
				}
			}
		}
	}

	free(input);

	for (int i = 0; i < sizeof(args) / sizeof(char *); i++) {
		free(args[i]);
	}
	free(args);

	// Create a socket using TCP
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Could not create socket.");
		exit(2);
	}

	// Construct the server address structure
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(server_ip);
	server_address.sin_port = htons(server_port);

	// Establish the connection to the server
	if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "Could not connect to server.");
		exit(2);
	}

	printf("Connected to the server!\n");

	// Send the string to the server
	if (send(sock, echoString, echoStringLen, 0) != echoStringLen) {
		fprintf(stderr, "Sent a different number of bytes than expected.");
		exit(2);
	}

	// Receive data from the server
	printf("Received: ");

	int total_bytes_received = 0;
	int bytes_received = 0;
	while (total_bytes_received < echoStringLen) {
		if ((bytes_received = recv(sock, echoBuffer, RECEIVE_BUFFER_SIZE - 1, 0)) <= 0) {
			fprintf(stderr, "Received failed or connection closed prematurely.");
			exit(2);
		}

		total_bytes_received += bytes_received;
		echoBuffer[bytes_received] = '\0';
		printf("%s", echoBuffer);
	}

	printf("\n");

	close(sock);
	exit(0);
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