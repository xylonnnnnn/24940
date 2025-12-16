#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[1024];
    ssize_t bytes_read;

    unlink("/tmp/task_socket");

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/task_socket", sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(client_fd, buffer, 1023)) > 0) {
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        printf("%s", buffer);
        fflush(stdout);
    }

    close(client_fd);
    close(server_fd);
    unlink("/tmp/task_socket");

    return 0;
}