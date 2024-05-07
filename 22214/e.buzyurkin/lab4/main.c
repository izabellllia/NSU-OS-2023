#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LEN 1024

typedef struct node_s Node;

struct node_s {
    char* buffer;
    Node* next;
};

Node* initList(){
    Node* head = malloc(sizeof(Node));
    if (head == NULL){
        perror("An error has occured");
        exit(0);
    }
    head->buffer = NULL;
    head->next = NULL;

    return head;
}

void pushList(Node* last, char* buffer){

    Node* new = malloc(sizeof(Node));
    if (new == NULL){
        perror("An error has occured");
        exit(0);
    }

    new->buffer = buffer;
    new->next = NULL;

    last->next = new;
}

void printList(Node* head){
    while (head->next){
        head = head->next;
        printf("%s", head->buffer);
    }
}

int main(){
    char* input_buffer = malloc(MAX_LEN);
    Node* head = initList();
    Node* last = head;

    while (1){
        if (fgets(input_buffer, MAX_LEN, stdin) == NULL){
            perror("An error has occured");
            exit(0);
        }

        if (input_buffer[0] == '.')
            break;

        int len = strlen(input_buffer) + 1;

        if (len == 2 && input_buffer[0] == '\n')
            continue;

        char* string = malloc(len);
        if (string == NULL){
            perror("An error has occured");
            exit(0);
        }

        memcpy(string, input_buffer, len);

        pushList(last, string);
        last = last->next;
    }

    printList(head);

    Node* temp;
    while (head->next){
        temp = head;
        head = head->next;
        free(temp->buffer);
        free(temp);
    }

    free(input_buffer);

    return 0;
}