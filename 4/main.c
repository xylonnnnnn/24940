#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

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
    }
    
    new_node->str = (char *)malloc(strlen(str) + 1);
    if (new_node->str == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для строки\n");
        free(new_node);
        exit(1);
    }
    
    strcpy(new_node->str, str);
    new_node->next = NULL;
    
    if (*head == NULL) {
        *head = new_node;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void free_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp->str);
        free(temp);
    }
}

void print_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->str);
        current = current->next;
    }
}

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
