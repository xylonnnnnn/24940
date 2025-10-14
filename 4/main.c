#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

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
        printf("%s", current->str);
        current = current->next;
    }
}

int main() {
    Node *head = NULL;
    char buffer[1024]; 
    
    printf("Вводите строки (точка в начале строки для завершения):\n");
    
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        
        if (buffer[0] == '.') {
            break;
        }
        
        add_node(&head, buffer);
    }
    
    printf("\nВведенные строки:\n");
    print_list(head);
    free_list(head);
    return 0;
}