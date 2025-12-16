#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <ctype.h>
#include <signal.h>

int server_socket;

void cleanup() {
    unlink("/tmp/task31_socket");
    exit(0);
}

int main() {
    struct sockaddr_un addr;
    fd_set readfds;
    int max_fd;
    int client_sockets[FD_SETSIZE];
    int i;
    
    signal(SIGINT, (void (*)(int))cleanup);
    
    for (i = 0; i < FD_SETSIZE; i++) {
        client_sockets[i] = 0;
    }
    
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/task31_socket");
    unlink("/tmp/task31_socket");
    bind(server_socket, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_socket, 5);
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_fd = server_socket;
        
        for (i = 0; i < FD_SETSIZE; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
                if (client_sockets[i] > max_fd) {
                    max_fd = client_sockets[i];
                }
            }
        }
        
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        
        if (FD_ISSET(server_socket, &readfds)) {
            int new_client = accept(server_socket, NULL, NULL);
            for (i = 0; i < FD_SETSIZE; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_client;
                    break;
                }
            }
        }
        
        for (i = 0; i < FD_SETSIZE; i++) {
            if (client_sockets[i] > 0 && FD_ISSET(client_sockets[i], &readfds)) {
                char buffer[1024];
                int bytes = read(client_sockets[i], buffer, 1023);
                
                if (bytes <= 0) {
                    close(client_sockets[i]);
                    client_sockets[i] = 0;
                } else {
                    buffer[bytes] = '\0';
                    for (int j = 0; j < bytes; j++) {
                        buffer[j] = toupper(buffer[j]);
                    }
                    write(STDOUT_FILENO, buffer, bytes);
                }
            }
        }
    }
    return 0;
}