#include <stdio.h>
#include <stdlib.h>
#include <string.h>
<<<<<<< HEAD
#include <ctype.h>

#ifndef LEN_BUFFER
#define LEN_BUFFER 10
#endif
=======
#include <termios.h>
#include <unistd.h>
>>>>>>> 9818394e4adeafff1631fd2a5725596004da41fa

typedef struct Node {
    char* string;
    struct Node* next;
} Node;

<<<<<<< HEAD
Node* add_node(Node* prev, char* string) {
    Node* ans;
    ans = malloc(sizeof(Node));
    if (ans == NULL) {
        return NULL;
=======
Node *history = NULL;
Node *current_history = NULL;
char current_input[1024] = {0};
int cursor_pos = 0;
int input_len = 0;

void add_node(Node **head, const char *str) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для узла\n");
        exit(1);
>>>>>>> 9818394e4adeafff1631fd2a5725596004da41fa
    }
    if (prev != NULL) {
        prev->next = ans;
    }

    ans->next = NULL;
    ans->string = string;
    return ans;
}


void print_sanitized(const char *s) {
    for (const unsigned char *p = (const unsigned char *)s; *p != '\0'; p++) {
        if (isprint(*p)) {
            putchar(*p);
        } else {
            printf("\\x%02X", *p);
        }
    }
    putchar('\n');
}

void print_list(Node* cur) {
    if (cur != NULL) {
        print_sanitized(cur->string);
        print_list(cur->next);
    }
}

void free_list(Node* cur) {
    if (cur != NULL) {
        free_list(cur->next);
        free(cur->string);
        free(cur);
    }
}

<<<<<<< HEAD
void exit_program(int code, char* message) {
    if (message != NULL) {
        perror(message);
=======
void print_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->str);
        current = current->next;
>>>>>>> 9818394e4adeafff1631fd2a5725596004da41fa
    }
    exit(code);
}

int get_list_from_user(Node** startList) {
    char buffer[LEN_BUFFER], flag_fullstop = 0, flag_start = 1;
    char* string = NULL;
    Node *prev = NULL, *start = NULL;

    while (fgets(buffer, LEN_BUFFER, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        if (buffer[0] == '.') {
            if (len == 1) {
                break;
            }
            string = strdup(&buffer[1]);
            flag_fullstop = 1;
        } else {
            string = strdup(buffer);
        }

        if (string == NULL) {
            return 1;
        }

        while (len == LEN_BUFFER - 1 && buffer[len - 1] != '\0') {
            if (!fgets(buffer, LEN_BUFFER, stdin)) {
                if (ferror(stdin)) {
                    free(string);
                    return 1;
                }
                break;
            }

            len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
                len--;
            }

            char* new_string = realloc(string, strlen(buffer) + strlen(string) + 1);
            if (new_string == NULL) {
                free(string);
                return 1;
            }
            string = new_string;
            strcat(string, buffer);
        }

        prev = add_node(prev, string);

        if (prev == NULL) {
            free(string);
            return 1;
        }

        if (flag_start) {
            flag_start = 0;
            start = prev;
        }

        if (flag_fullstop) {
            break;
        }
    }

    if (ferror(stdin)) {
        return 1;
    }

    if (feof(stdin)) {
        return 2;
    }

    *startList = start;
    return 0;
}

<<<<<<< HEAD
int main() {
    Node* start = NULL;
    Node** startPointer = &start;
    puts("ВВЕДИТЕ СТРОКИ:\n");

    switch (get_list_from_user(startPointer)) {
    case 1:
        exit_program(EXIT_FAILURE, "get_list_from_user failed");
        break;
    case 2:
        fprintf(stderr, "get_list_from_user failed: EOF\n");
        exit_program(EXIT_FAILURE, NULL);
    }

    puts("\nСТРОКИ:");
    print_list(start);
    free_list(start);
    exit_program(EXIT_SUCCESS, NULL);
}
=======
void set_noncanonical_mode(struct termios *original_termios) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, original_termios);
    new_termios = *original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VTIME] = 0;
    new_termios.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void restore_terminal_mode(struct termios *original_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, original_termios);
}

void handle_input(Node **head) {
    struct termios original_termios;
    set_noncanonical_mode(&original_termios);
    
    printf("Вводите строки (точка в начале строки для завершения):\n");
    
    while (1) {
        char c = getchar();
        
        if (c == '\n' || c == '\r') {
            printf("\n");
            
            if (input_len > 0) {
                if (current_input[0] == '.') {
                    break;
                }
                
                current_input[input_len] = '\0';
                add_node(&history, current_input);
                add_node(head, current_input);
            }
            
            memset(current_input, 0, sizeof(current_input));
            cursor_pos = 0;
            input_len = 0;
            current_history = NULL;
            
            continue;
        }
        
        if (c == '\033') {
            getchar(); 
            c = getchar();
            
            if (c == 'A' && history != NULL) { 
                if (current_history == NULL) {
                    current_history = history;
                    while (current_history->next != NULL) {
                        current_history = current_history->next;
                    }
                } else {
                    Node *prev = history;
                    while (prev != NULL && prev->next != current_history) {
                        prev = prev->next;
                    }
                    if (prev != NULL) {
                        current_history = prev;
                    }
                }
                
                strcpy(current_input, current_history->str);
                input_len = strlen(current_input);
                cursor_pos = input_len;
                
                printf("\r\033[K%s", current_input);
            } 
            else if (c == 'B' && current_history != NULL) { 
                if (current_history->next != NULL) {
                    current_history = current_history->next;
                    strcpy(current_input, current_history->str);
                    input_len = strlen(current_input);
                    cursor_pos = input_len;
                    printf("\r\033[K%s", current_input);
                } else {
                    current_history = NULL;
                    memset(current_input, 0, sizeof(current_input));
                    input_len = 0;
                    cursor_pos = 0;
                    printf("\r\033[K");
                }
            }
            
            continue;
        }
        
        if (c == 127 || c == 8) {
            if (cursor_pos > 0) {
                for (int i = cursor_pos - 1; i < input_len; i++) {
                    current_input[i] = current_input[i + 1];
                }
                input_len--;
                cursor_pos--;
                
                printf("\r\033[K%s", current_input);
                if (cursor_pos < input_len) {
                    printf("\033[%dD", input_len - cursor_pos);
                }
            }
            continue;
        }
        
        if (input_len < sizeof(current_input) - 1) {
            if (cursor_pos < input_len) {
                for (int i = input_len; i > cursor_pos; i--) {
                    current_input[i] = current_input[i - 1];
                }
            }
            
            current_input[cursor_pos] = c;
            input_len++;
            cursor_pos++;
            current_input[input_len] = '\0';
            
            printf("\r\033[K%s", current_input);
            
            if (cursor_pos < input_len) {
                printf("\033[%dD", input_len - cursor_pos);
            }
        }
    }
    
    restore_terminal_mode(&original_termios);
}

int main() {
    Node *head = NULL;
    
    handle_input(&head);
    
    printf("\nВведенные строки:\n");
    print_list(head);
    
    free_list(head);
    free_list(history);
    
    return 0;
}
>>>>>>> 9818394e4adeafff1631fd2a5725596004da41fa
