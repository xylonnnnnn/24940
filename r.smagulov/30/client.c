#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int sockfd;
    struct sockaddr_un addr;
    char buffer[1024];
    ssize_t bytes_read;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/task_socket", sizeof(addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(STDIN_FILENO, buffer, 1023)) > 0) {
        if (write(sockfd, buffer, bytes_read) != bytes_read) {
            perror("write");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        if (bytes_read < 1023) {
            break;
        }
    }

    close(sockfd);
    return 0;
}