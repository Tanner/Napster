#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define RECEIVE_BUFFER_SIZE 32

void HandleTCPClient(int client_socket)
{
	char echoBuffer[RECEIVE_BUFFER_SIZE];
	int recvMsgSize;

	// Receive message from client
	if ((recvMsgSize = recv(client_socket, echoBuffer, RECEIVE_BUFFER_SIZE, 0)) < 0) {
		fprintf(stderr, "Receive failed.");
	}

	// Send received string and receive again until end of transmission
	while (recvMsgSize > 0) {
		// Echo message back to client
		if (send(client_socket, echoBuffer, recvMsgSize, 0) != recvMsgSize) {
			fprintf(stderr, "Send failed.");
		}

		// See if there is more data to receive
		if ((recvMsgSize = recv(client_socket, echoBuffer, RECEIVE_BUFFER_SIZE, 0)) < 0) {
			fprintf(stderr, "Receive failed.");
		}
	}

	close(client_socket);
}