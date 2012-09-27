#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#ifndef __SERVER_H__
#define __SERVER_H__

#define MAX_PENDING 5

typedef struct struct_file_source {
	struct sockaddr_in address;
	int file_count;
	char **files;
} file_source;

#endif