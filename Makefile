CC = gcc
CFLAGS = -Wall -Werror -ansi -pedantic -std=c99

RELEASE_FLAGS = -02
DEBUG_FLAGS = -g -DDEBUG

CLIENT_OBJ = client.o utilities.o
CLIENT_TARGET = client

SERVER_OBJ = server.o utilities.o
SERVER_TARGET = server

all: CFLAGS += $(RELEASE_FLAGS)
all: $(CLIENT_TARGET) $(SERVER_TARGET)

debug-client: CFLAGS += $(DEBUG_FLAGS)
debug-client: $(CLIENT_TARGET)

debug-server: CFLAGS += $(DEBUG_FLAGS)
debug-server: $(SERVER_TARGET)

$(CLIENT_TARGET) : $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJ)

$(SERVER_TARGET) : $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJ)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean: clean-client clean-server

clean-client:
	rm -f $(CLIENT_OBJ) $(CLIENT_TARGET)

clean-server:
	rm -f $(SERVER_OBJ) $(SERVER_TARGET)
