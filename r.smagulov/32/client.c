#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int sock;
    struct sockaddr_un addr;
    char buffer[1024];
    
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/task32_socket");
    
    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    
    while (fgets(buffer, 1024, stdin)) {
        write(sock, buffer, strlen(buffer));
    }
    
    close(sock);
    return 0;
}