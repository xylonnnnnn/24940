#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void refresh_line(char *buffer, int len) {
    printf("\r\033[K"); 
    
    for (int i = 0; i < len; i++) {
        putchar(buffer[i]);
        if ((i + 1) % 40 == 0 && i != len - 1) {
            printf("\n");
        }
    }
    fflush(stdout);
}

int main() {
    enableRawMode();
    char buffer[401] = {0}; 
    int length = 0;
    while (1) {
        char c = getchar();
        if (c == 4) {
            if (length == 0) {
                break;
            } else {
                putchar(7);
            }
        }
        else if (c == 127) {
            if (length > 0) {
                length--;
                refresh_line(buffer, length);
            } else {
                putchar(7);
            }
        }
        else if (c == 21) {
            length = 0;
            refresh_line(buffer, length);
        }
        else if (c == 23) {
            int i = length - 1;
            while (i >= 0 && isspace(buffer[i])) {
                i--;
            }
            while (i >= 0 && !isspace(buffer[i])) {
                i--;
            }
            length = i + 1;
            refresh_line(buffer, length);
        } else if (isprint(c)) {
            if (length < (int)(sizeof(buffer) - 1)) {
                buffer[length++] = c;
                refresh_line(buffer, length);
            } else {
                putchar(7);
            }
        }
        else {
            putchar(7);
        }
        fflush(stdout);
    }
    printf("\n");
    return 0;
}