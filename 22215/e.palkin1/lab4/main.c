#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

void push_back(Node *head, char *value) {
    if (head->str == NULL) {
        head->str = value;
    } else {
        Node *new_elem = (Node *) malloc(sizeof(Node));
        new_elem->str = value;
        new_elem->next = NULL;
        Node *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_elem;
    }
}

void print_list(Node *head) {
    if (head->str != NULL) {
        Node *current = head;
        while (current->next != NULL) {
            printf("%s\n", current->str);
            current = current->next;
        }
        printf("%s\n", current->str);
    }
}

void free_list(Node *head){
    if (head->str != NULL) {
         Node *current = head;
	 Node *current1;
	 while (current != NULL) {
  	     current1 = current->next;
  	     free(current);
 	     current = current1;
	 }
	 head = NULL;
    }
}

int main() {
    Node head;
    head.str = NULL;
    head.next = NULL;
    while (1) {
        char *tmp = (char *) malloc(sizeof(char) * 5);
        int size = 5;
        int len = 0;
        char cur = getchar();
        if (cur == '.') {
            break;
        }
        while (1) {
            if (len < size) {
                tmp[len] = cur;
                len++;
            } else {
                size = size * 2;
                tmp = (char *) realloc(tmp, sizeof(char) * size);
                tmp[len] = cur;
                len++;
            }
            cur = getchar();
            if (cur == '\n') {
                break;
            }
        }
        if (len == size) {
            size = size * 2;
            tmp = (char *) realloc(tmp, sizeof(char) * size);
        }
        tmp[len] = '\0';
        push_back(&head, tmp);
    }
    print_list(&head);
    free_list(&head);
    return 0;
}

