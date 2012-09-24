#include "utilities.h"

/**
 * Split a string into x variable.
 *
 * @param *input		String to split up
 * @param *delimiter	Delimiter to use
 */
char ** split(char *input, char *delimiter) {
	char *input_copy = 0;
	char *temp = 0;

	// Count the number of chunks that exist in the string

	input_copy = malloc(sizeof(char) * strlen(input));
	assert(input_copy);

	strcpy(input_copy, input);

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

	char **split = malloc(sizeof(char *) * count);
	assert(split);

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