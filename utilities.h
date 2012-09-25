#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

char * get_response(int sock);
int valid_message(char *message);

char ** split(char *input, char *delimiter);

#endif