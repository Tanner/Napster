#include "utilities.h"

#define RECEIVE_BUFFER_SIZE 32

char * get_response(int sock) {
	char received_buffer[RECEIVE_BUFFER_SIZE];
	int received_message_size = 0;

	int total_message_size = 0;
	int message_capacity = RECEIVE_BUFFER_SIZE;

	char *message = calloc(1, sizeof(char) * RECEIVE_BUFFER_SIZE);
	assert(message);

	// Send received string and receive again until end of transmission
	do {
		// Receive message from client
		if ((received_message_size = recv(sock, received_buffer, RECEIVE_BUFFER_SIZE, 0)) < 0) {
			free(message);

			return 0;
		}

		if (received_message_size > 0) {
			for (int i = 0; i < received_message_size; i++) {
				message[total_message_size + i] = received_buffer[i];
			}

			memset(received_buffer, 0, RECEIVE_BUFFER_SIZE);

			total_message_size += received_message_size;

			if (total_message_size >= message_capacity) {
				message_capacity += RECEIVE_BUFFER_SIZE;

				message = realloc(message, message_capacity);
			}
		}
	} while (!valid_message(message) && received_message_size > 0);

	return message;
}

/**
 * Check to see if a string is a valid request message.
 * @param  message String
 * @return 1 or 0 for validity
 */
int valid_message(char *message) {
	int length = strlen(message);

	// Valid message ends in \n

	if (length < 1) {
		return 0;
	}

	return message[length - 1] == '\n';
}

/**
 * Parse a message into command and args.
 * @param  message String
 * @param  size Pointer to a int that will contain the size of the array
 * @return Array of strings containing command and args
 */
char ** parse_message(char *message, int *size) {
	// Trim '\n' from end of message
	message[strlen(message) - 1] = '\0';

	// Alloc result array – start at 1 for minimum of just command
	char **result = calloc(1, sizeof(char *));
	assert(result);

	// Get the command
	char *command = calloc(MAX_COMMAND_LENGTH, sizeof(char));
	assert(command);

	int command_end_index = 0;

	for (int i = 0; i < strlen(message) && i < MAX_COMMAND_LENGTH; i++) {
		if (message[i] == ' ' || i == strlen(message) - 1) {
			command_end_index = i;

			if (i == strlen(message) - 1) {
				command_end_index++;
			}

			for (int j = 0; j < command_end_index; j++) {
//				memcpy(&command[j], &message[j], 1);
                command[j] = message[j];
			}

			break;
		}
	}

	// If the command is not there, return 0
	if (strlen(command) <= 0) {
		free(command);
		free(result);

		return 0;
	}

    if (message[command_end_index] == ' ') {
        command_end_index++;
    }
    
	// Otherwise, store the command and pick out arguments
	result[0] = command;
	*size = 1;
	if (strcmp(command, "ADD") == 0 || strcmp(command, "REMOVE") == 0) {
		// Get the file name
		char *file_name = calloc(MAX_FILE_NAME_LENGTH, sizeof(char));

		int file_name_length = 0;

		for (int i = command_end_index; i < strlen(message) && i <= MAX_FILE_NAME_LENGTH + command_end_index; i++) {
			file_name[i - command_end_index] = message[i];

			file_name_length = i;
		}

		// Add null terminator
		file_name[file_name_length] = '\0';

		*size += 1;

        result = realloc(result, sizeof(char *) * *size);
		result[1] = file_name;
	} else if (strcmp(command, "LIST") == 0) {
		// Ignore any other arguments
	} else {
		// If not matching command, return 0
		free(command);
		free(result);


		return 0;
	}

	return result;
}

/**
 * Split a string into x variable.
 *
 * @param *input		String to split up
 * @param *delimiter	Delimiter to use
 * @param *size			Pointer that will contain the number of arguments found
 */
char ** split(char *input, char *delimiter, int *size) {
	char *input_copy = 0;
	char *temp = 0;

	// Count the number of chunks that exist in the string

	input_copy = calloc(strlen(input) + 1, sizeof(char));
	assert(input_copy);

	strncpy(input_copy, input, strlen(input));

	int count = -1;
	do {
		if (temp == 0) {
			temp = strtok(input_copy, delimiter);
		} else {
			temp = strtok(NULL, delimiter);
		}

		count++;
	} while (temp);

	free(input_copy);

	// Create an array of that size (to hold all the chunks)

	char **split = calloc(count, sizeof(char *));
	assert(split);

	*size = count;

	// Begin to assign the chunks into the array

	temp = strtok(input, delimiter);

	for (int i = 0; i < count; i++) {
		if (temp != NULL) {
			split[i] = malloc(strlen(temp) + 1);
			assert(split[i]);

			strcpy(split[i], temp);
		}

		temp = strtok(NULL, delimiter);
	}

	return split;
}
