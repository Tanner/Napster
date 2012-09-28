#include "server.h"
#include "utilities.h"

list* file_sources_list;

void handle_client(int client_socket, struct sockaddr_in client_address);

void list_all_files(int client_socket, struct sockaddr_in client_address);
void add_file_from_source(struct sockaddr_in client_address, char *file_name);
void remove_file_from_source(struct sockaddr_in client_address, char *file_name);

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

	printf("Got request from %s – ", inet_ntoa(client_address.sin_addr));

	int size = 0;
	char **parse = parse_message(message, &size);

	if (parse) {
		printf("%s\n", parse[0]);

		if (strcmp(parse[0], "ADD") == 0) {
			add_file_from_source(client_address, parse[1]);

			printf("Updated file source list:\n");

			traverse(file_sources_list, print_source);
		} else if (strcmp(parse[0], "REMOVE") == 0) {
			remove_file_from_source(client_address, parse[1]);

			printf("Updated file source list:\n");

			traverse(file_sources_list, print_source);
		} else if (strcmp(parse[0], "LIST") == 0) {
			list_all_files(client_socket, client_address);
		}

		for (int i = 0; i < size; i++) {
			free(parse[i]);
		}

		free(parse);
	} else {
		printf("Invalid request.\n");
	}

	free(message);

	close(client_socket);
}

void list_all_files(int client_socket, struct sockaddr_in client_address) {
	if (size(file_sources_list) == 0) {
		char *message = "LIST 0\n";

		send(client_socket, message, strlen(message), 0);
	} else {
		int number_file_sources = 0;
		int total_number_files = 0;

		number_file_sources = size(file_sources_list);
		for (int i = 0; i < number_file_sources; i++) {
			file_source *source = (file_source *) get_index(file_sources_list, i);

			total_number_files += size(source->files);
		}

		char *file_list = "";

		for (int i = 0; i < number_file_sources; i++) {
			file_source *source = (file_source *) get_index(file_sources_list, i);

			int number_files = size(source->files);
			for (int j = 0; j < number_files; j++) {
				char *file_name = (char *) get_index(source->files, j);

				if (asprintf(&file_list, "%s %s", file_list, file_name) == -1) {
					fprintf(stderr, "Could not create a list of files - too many files.\n");

					return;
				}
			}
		}

		char *message;
		if (asprintf(&message, "LIST %d%s\n", total_number_files, file_list) == -1) {
			fprintf(stderr, "Could not create LIST command - too many files.\n");

			return;
		}

		printf("Sending response back to %s – LIST\n", inet_ntoa(client_address.sin_addr));

		send(client_socket, message, strlen(message), 0);
	}
}

/**
 * Removes file_name from the client_address.
 * @param client_address sockaddr_in struct for client
 * @param file_name      File name to remove
 */
void remove_file_from_source(struct sockaddr_in client_address, char *file_name) {
	if (size(file_sources_list) != 0) {
		file_source *source = find_occurrence(file_sources_list, &client_address, compare_source_addresses);

		if (source) {
			int remove_data(list* llist, const void* data, equal_op compare_func, list_op free_func);

			int number_files_removed = remove_data(source->files, file_name, compare_file_names, free);

			if (number_files_removed == 0) {
				printf("\tNo file found by that name to delete.\n");
			} else {
				printf("\tRemoved %d files.\n", number_files_removed);
			}
		}
	} else {
		printf("\tNo file sources to remove files from. Bailing out.\n");
	}
}

/**
 * Adds file_name to the client_address.
 * @param client_address sockaddr_in struct for client
 * @param file_name      File name to add
 */
void add_file_from_source(struct sockaddr_in client_address, char *file_name) {
	if (size(file_sources_list) != 0) {
		file_source *source = find_occurrence(file_sources_list, &client_address, compare_source_addresses);

		if (source) {
			char *file = find_occurrence(source->files, file_name, compare_file_names);

			if (!file) {
				printf("\tAdding file to existing source...\n");

				push_front(source->files, file_name);
			} else {
				printf("\tFile already exists in source; not adding again...\n");
			}

			return;
		}
	}

	printf("\tCreating new source...\n");

	file_source *new_source = malloc(sizeof(file_source));
	assert(new_source);

	new_source->address = client_address;
	new_source->files = create_list();

	push_front(new_source->files, file_name);
	push_front(file_sources_list, new_source);
}

/**
 * Compares the source addresses given an address and a file source.
 * @param  a Source address to match to
 * @param  b File source to determine if it is matching
 * @return 1 for true, 0 for false
 */
int compare_source_addresses(const void *a, const void *b) {
	struct sockaddr_in *needle = (struct sockaddr_in *) a;
	struct sockaddr_in haystack = ((file_source *) b)->address;

	return strcmp(inet_ntoa(needle->sin_addr), inet_ntoa(haystack.sin_addr)) == 0;
}

/**
 * Compares two files names to see if they are equal.
 * @param  a First file name
 * @param  b Second file name
 * @return 1 for true, 0 for false
 */
int compare_file_names(const void *a, const void *b) {
	return strcmp(a, b) == 0;
}

/**
 * Prints out a source and all of its files.
 * @param data File source
 */
void print_source(void *data) {
	file_source *source = (file_source *) data;

	printf("\t%s:\n", inet_ntoa(source->address.sin_addr));

	traverse(source->files, print_files);
}

/**
 * Prints out a file.
 * @param data File name
 */
void print_files(void *data) {
	printf("\t\t%s\n", (char *)data);
}