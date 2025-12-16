CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=600
LDFLAGS = -lrt -lsocket -lnsl

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) client.c -o client $(LDFLAGS)

clean:
	rm -f server client
	rm -f /tmp/task32_socket

run_server:
	./server

run_client:
	./client

.PHONY: all clean run_server run_client