#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Node {
    char *line;
    struct Node *next;
} Node;

Node* createNode() {
    Node *node = (Node *) malloc(sizeof(Node));
    if (node == NULL) {
        perror("malloc failed");
        exit(1);
    }
    node->line = NULL;
    node->next = NULL;
    return node;
}

void addNode(Node* head, char* line) {
    Node *new = (Node *) malloc(sizeof(Node));
    if (new == NULL) {
        perror("malloc failed");
        exit(1);
    }
    head->next = new;
    new->line = line;
    new->next = NULL;
}

void printList(Node* head) {
    while (head != NULL) {
        printf("%s", head->line);
        head = head->next;
    }
}

void freeList(Node* head) {
    Node *tmp;
    while (head != NULL) {
        tmp = head->next;
        free(head->line);
        free(head);
        head = tmp;
    }
}

int main() {
    char *fstr = (char *) malloc(BUFSIZ * sizeof(char));
    if (fstr == NULL) {
        perror("malloc failed");
        exit(1);
    }
    Node *head = createNode();
    Node *last = head;
    while (1) {
        if (fgets(fstr, BUFSIZ, stdin) == NULL) {
            perror("Error");
            exit(1);
        }
        if (fstr[0] == '.') {
            break;
        }
        int len = strlen(fstr) + 1;
        char *str = (char *) malloc(len * sizeof(char));
        if (str == NULL) {
            perror("malloc failed");
            exit(1);
        }
        memcpy(str, fstr, len);
        addNode(last, str);
        last = last->next;
    }
    printList(head->next);
    freeList(head);
    free(fstr);
    return 0;
}
