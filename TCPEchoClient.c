#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RECEIVE_BUFFER_SIZE 32

#define DEFAULT_SERVER_PORT 7

int main(int argc, char *argv[]) {
	int sock;							// Socket Descriptor
	struct sockaddr_in server_address;
	unsigned short server_port;
	char *server_ip;
	char *echoString;                /* String to send to echo server */
	char echoBuffer[RECEIVE_BUFFER_SIZE];     /* Buffer for echo string */
	unsigned int echoStringLen;      /* Length of string to echo */
	int bytes_received, total_bytes_received;   /* Bytes read in single recv() 
										and total bytes read */

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

	// Create a socket using TCP
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Could not create socket.");
		exit(2);
	}

	/* Construct the server address structure */
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(server_ip);
	server_address.sin_port = htons(server_port);

	// Establish the connection to the server
	if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "Could not connect to server.");
		exit(2);
	}

	// Send the string to the server
	if (send(sock, echoString, echoStringLen, 0) != echoStringLen) {
		fprintf(stderr, "Sent a different number of bytes than expected.");
		exit(2);
	}

	// Receive data from the server
	printf("Received: ");

	total_bytes_received = 0;
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
