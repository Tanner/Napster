CC = gcc
CFLAGS = -Wall -Werror -ansi -pedantic -std=c99 -O0 -g

CLIENT_OBJ = client.o
CLIENT_TARGET = client

SERVER_OBJ = server.o HandleTCPClient.o
SERVER_TARGET = server

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET) : $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJ)

$(SERVER_TARGET) : $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJ)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_TARGET) $(SERVER_TARGET)
