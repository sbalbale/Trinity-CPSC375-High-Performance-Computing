# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall

# Define libraries for the server
SERVER_LIBS = -lpthread -lrt

# The 'all' target builds both server and client
all: server client

# Rule to build the server
# Note: server.c includes mydb.c directly
server: server.c mydb.c
	$(CC) $(CFLAGS) -o server server.c $(SERVER_LIBS)

# Rule to build the client
client: client.c
	$(CC) $(CFLAGS) -o client client.c

# Rule to clean up the directory by removing binaries
clean:
	rm -f server client

# Phony targets to prevent conflicts with files named 'all' or 'clean'
.PHONY: all clean
