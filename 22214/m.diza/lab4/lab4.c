#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

struct Node {
    char *str;
    struct Node *next;
};

struct Node* create_Node(char* str) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (!newNode) {
        perror("malloc failed to allocate memory");
        exit(1);
    }

    int n = strlen(str);
    newNode->str = (char*)malloc(n + 1);
    if (!newNode->str) {
        perror("malloc failed to allocate memory");
        exit(1);
    }

    memcpy(newNode->str, str, n + 1);
    newNode->next = NULL;
    
    return newNode;
}

void print_List(struct Node* head) {
    if (!head) return;

    printf("%s", head->str);

    print_List(head->next);
}

int main() {
    char buffer[BUFSIZ + 1];

    struct Node* head = NULL;
    struct Node* curr = NULL;

    while (fgets(buffer, BUFSIZ, stdin)) {
        if (buffer[0] == '.') break;

        if (curr) {
            curr->next = create_Node(buffer); 
            curr = curr->next;
        } else {
            head = create_Node(buffer);
            curr = head;
        }
  
    }
    
    print_List(head);
    
    while (head) {
        curr = head->next;
        free(head->str);
        free(head);
        head = curr;
    }
    
    return 0;
}