#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <poll.h>

int server_socket;

void cleanup() {
    unlink("/tmp/task32_socket");
    exit(0);
}

int main() {
    struct sockaddr_un addr;
    struct pollfd fds[10 + 1];
    int client_fds[10];
    int nfds = 1;
    
    signal(SIGINT, (void (*)(int))cleanup);
    
    for (int i = 0; i < 10; i++) {
        client_fds[i] = -1;
    }
    
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/task32_socket");
    unlink("/tmp/task32_socket");
    bind(server_socket, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_socket, 5);
    
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;
    
    printf("Сервер запущен на сокете /tmp/task32_socket\n");
    
    while (1) {
        int ret = poll(fds, nfds, -1);
        
        if (ret < 0) {
            perror("poll");
            break;
        }
        
        if (fds[0].revents & POLLIN) {
            int client_fd = accept(server_socket, NULL, NULL);
            if (client_fd >= 0) {
                for (int i = 0; i < 10; i++) {
                    if (client_fds[i] == -1) {
                        client_fds[i] = client_fd;
                        
                        fds[nfds].fd = client_fd;
                        fds[nfds].events = POLLIN;
                        nfds++;
                        
                        printf("Клиент %d подключен\n", i);
                        break;
                    }
                }
            }
        }
        
        for (int i = 1; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                char buffer[1024];
                int bytes = read(fds[i].fd, buffer, 1024 - 1);
                
                if (bytes <= 0) {
                    close(fds[i].fd);
                    
                    for (int j = 0; j < 10; j++) {
                        if (client_fds[j] == fds[i].fd) {
                            printf("Клиент %d отключен\n", j);
                            client_fds[j] = -1;
                            break;
                        }
                    }
                    
                    fds[i].fd = fds[nfds - 1].fd;
                    fds[i].events = fds[nfds - 1].events;
                    fds[i].revents = fds[nfds - 1].revents;
                    nfds--;
                    i--; 
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