#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_STRING 1048320

typedef struct Node {
    char* buff;
    struct Node* next;
} Node;

void add_node(Node* s, char* buff) {
    Node* new = (Node*)malloc(sizeof(Node));
    if (new == NULL) {
        perror("No memory allocated");
        exit(1);
    }
    s->next = new;
    new->buff = buff;
    new->next = NULL;
}

int main() {
    char* input_string = (char*)malloc(MAX_STRING * sizeof(char));
    if (input_string == NULL) {
        perror("No memory allocated");
        exit(1);
    }

    Node* head = (Node*)malloc(sizeof(Node));
    if (head == NULL) {
        perror("No memory allocated");
        exit(1);
    }

    head->buff = NULL;
    head->next = NULL;

    Node* last = head;

    while (1) {
	if (fgets(input_string, MAX_STRING, stdin) == NULL) {
            perror("Error");
            exit(1);
        }
        if (input_string[0] == '.') {
            break;
        }
        int len = strlen(input_string) + 1;
        char* str = (char*)malloc(len * sizeof(char));
        if (str == NULL) {
            perror("No memory allocated");
            exit(1);
        }

        memcpy(str, input_string, len);
        add_node(last, str);
        last = last->next;
    }

    Node* tmp = head->next;
    while (tmp != NULL) {
        printf("%s", tmp->buff);
        tmp = tmp->next;
    }

    while (head != NULL) {
        tmp = head->next;
        free(head->buff);
        free(head);
        head = tmp;
    }

    free(input_string);

    return 0;
}
