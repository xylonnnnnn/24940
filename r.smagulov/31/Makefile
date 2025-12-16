CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: server client

server31: server.c
	$(CC) $(CFLAGS) -o server server.c

client31: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client
	rm -f /tmp/task31_socket

run_server:
	./server

run_client:
	./client

.PHONY: all clean run_server run_client