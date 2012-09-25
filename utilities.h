#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#define MAX_COMMAND_LENGTH 256

#define MAX_FILE_NAME_LENGTH 1024

char * get_response(int sock);
int valid_message(char *message);
char ** parse_message(char *message);

char ** split(char *input, char *delimiter);

#endif