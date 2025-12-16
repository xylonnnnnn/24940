CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = server client
SOCKET_PATH = /tmp/task_socket

all: $(TARGETS)

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f $(TARGETS)
	rm -f $(SOCKET_PATH)

run: all
	./server &
	sleep 1
	./client

.PHONY: all clean run