// line_lookup_mmap.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>

volatile sig_atomic_t timeout = 0;

void alarm_handler(int sig) {
    timeout = 1;
}

typedef struct {
    off_t offset;
    size_t length;
} LineEntry;

int main() {
    signal(SIGALRM, alarm_handler);
    
    int fd = open("text.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    char *file_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    LineEntry *lines = NULL;
    size_t capacity = 0;
    size_t line_count = 0;
    off_t offset = 0;

    for (off_t i = 0; i < file_size; i++) {
        if (file_data[i] == '\n') {
            if (line_count >= capacity) {
                capacity = capacity ? capacity * 2 : 128;
                lines = realloc(lines, capacity * sizeof(LineEntry));
            }
            lines[line_count].offset = offset;
            lines[line_count].length = i - offset + 1;
            offset = i + 1;
            line_count++;
        }
    }

    if (offset < file_size) {
        if (line_count >= capacity) {
            lines = realloc(lines, (line_count + 1) * sizeof(LineEntry));
        }
        lines[line_count].offset = offset;
        lines[line_count].length = file_size - offset;
        line_count++;
    }

    alarm(5);
    printf("Enter line number (0 to exit): ");
    int line_num;
    if (scanf("%d", &line_num) == 1) {
        alarm(0);
        if (line_num == 0) goto cleanup;
        if (line_num < 1 || line_num > line_count) {
            printf("Invalid line number!\n");
        } else {
            LineEntry *line = &lines[line_num - 1];
            printf("Line %d: %.*s", line_num, (int)line->length, 
                   file_data + line->offset);
        }
    } else {
        write(STDOUT_FILENO, file_data, file_size);
    }

cleanup:
    munmap(file_data, file_size);
    close(fd);
    free(lines);
    return 0;
}