#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 1024


typedef struct Node {
    char *str;
    struct Node *nxt;
} Node;

Node *addNode(char *str, Node *prev);
void print_node_list(Node *node);
void free_node_list(Node * start);

int main() {
    Node *node = NULL;
    Node *prev = NULL;
    char buff[BUFSIZE + 1];

    while (1) {
        if (fgets(buff, BUFSIZE + 1, stdin) == NULL) {
            break;
        }
        if (buff[0] == '.') {
            break;
        }

        prev = addNode(buff, prev);
        if (node == NULL) {
            node = prev;
        }
    }

    print_node_list(node);
    free_node_list(node);

    exit(EXIT_SUCCESS);
}

void print_node_list(Node *node) {
    Node *pntr;
    pntr = node;
    while (pntr != NULL) {
        if ((write(1, pntr->str, strlen(pntr->str))) == -1) {
            perror("Write failure");
            exit(EXIT_FAILURE);
        }
        pntr = pntr->nxt;
    }
}

Node *addNode(char *str, Node *prev) {
    Node *new_node = NULL;

    if ((new_node = (Node*) malloc(sizeof(Node))) == NULL) {
        perror("Malloc for node failure");
        exit(EXIT_FAILURE);
    }
    if ((new_node->str = malloc(strlen(str) + 1)) == NULL) {
        perror("Malloc for str failure");
        exit(EXIT_FAILURE);
    }

    if (prev != NULL) {
        prev->nxt = new_node;
    }

    new_node->nxt = NULL;
    strcpy(new_node->str, str);

    return new_node;
}

void free_node_list(Node * start) {
    Node *tmp;
    Node *to_free;

    tmp = start;
    while (tmp != NULL) {
        to_free = tmp;
        tmp = tmp->nxt;
        free(to_free->str);
        free(to_free);
    }
}
