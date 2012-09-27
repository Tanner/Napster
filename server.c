#include "server.h"
#include "utilities.h"

list* file_sources_list;

void handle_client(int client_socket, struct sockaddr_in client_address);

void add_file_from_source(struct sockaddr_in client_address, char *file_name);

void print_source(void *data);
void print_files(void *data);

int compare_source_addresses(const void *a, const void *b);
int compare_file_names(const void *a, const void *b);

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

	// Create file source array linked list
	file_sources_list = create_list();

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

	printf("Got request from %s\n", inet_ntoa(client_address.sin_addr));

	char **parse = parse_message(message);

	if (parse) {
		printf("Command: '%s'\n", parse[0]);

		if (strcmp(parse[0], "ADD") == 0) {
			printf("File Name: '%s' (length %d)\n", parse[1], (int) strlen(parse[1]));

			add_file_from_source(client_address, parse[1]);
		}

		traverse(file_sources_list, print_source);

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

void add_file_from_source(struct sockaddr_in client_address, char *file_name) {
	if (size(file_sources_list) != 0) {
		file_source *source = find_occurrence(file_sources_list, &client_address, compare_source_addresses);

		if (source) {
			char *file = find_occurrence(source->files, file_name, compare_file_names);

			if (!file) {
				push_front(source->files, file_name);
			}

			return;
		}
	}

	file_source *new_source = malloc(sizeof(file_source));
	assert(new_source);

	new_source->address = client_address;
	new_source->files = create_list();

	push_front(new_source->files, file_name);
	push_front(file_sources_list, new_source);
}

int compare_source_addresses(const void *a, const void *b) {
	struct sockaddr_in *needle = (struct sockaddr_in *) a;
	struct sockaddr_in haystack = ((file_source *) b)->address;

	return strcmp(inet_ntoa(needle->sin_addr), inet_ntoa(haystack.sin_addr)) == 0;
}

int compare_file_names(const void *a, const void *b) {
	return strcmp(a, b) == 0;
}

void print_source(void *data) {
	file_source *source = (file_source *) data;

	printf("%s:\n", inet_ntoa(source->address.sin_addr));

	traverse(source->files, print_files);
}

void print_files(void *data) {
	printf("\t%s\n", (char *)data);
}