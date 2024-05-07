#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char* string;
    struct Node* next;
} Node;

typedef struct List {
    Node* head;
    Node* tail;
} List;

List* CreateList() {
    List* list = (List*)malloc(sizeof(List));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    return list;
}

Node* CreateNode(const char* str) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->string = (char*)malloc(strlen(str) + 1);
    if (new_node->string == NULL) {
        return NULL;
    }
    strcpy(new_node->string, str);
    new_node->next = NULL;
    return new_node;
}

void insert_list(List* list, const char* str) {
    if (str == NULL || list == NULL) {
        return;
    }
    Node* new_node = CreateNode(str);
    if (new_node == NULL) {
        return;
    }
    if (list->head == NULL) {
        list->head = new_node;
    }
    else {
        list->tail->next = new_node;
    }
    list->tail = new_node;
}

void destroy_list(List* list) {
    if (list == NULL) {
        return;
    }
    Node* current = list->head;
    while (current != NULL) {
        Node* deleted_node = current;
        current = current->next;
        free(deleted_node->string);
        free(deleted_node);
    }
    free(list);
}

void print_list(List* list) {
    if (list->head) {
        Node* current = list->head;
        while (current != NULL) {
            printf("%s", current->string);
            current = current->next;
        }
    }
}

int main() {
    List* list = CreateList();
    if (list == NULL) {
        perror("Memory error");
        return -1;
    }
    char input[5];
    int end_of_str = 0;
    while (fgets(input, sizeof(input), stdin) != NULL) {
        char last = input[strlen(input) - 1];
        if (input[0] == '.' && end_of_str) {
            break;
        }
        if (last == '\n') {
            end_of_str = 1;
        }
        else {
            end_of_str = 0;
        }
        insert_list(list, input);
    }
    print_list(list);
    destroy_list(list);
    return 0;
}
