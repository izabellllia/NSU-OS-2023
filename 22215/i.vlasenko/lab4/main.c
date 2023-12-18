#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char str[256];

typedef struct Node_s {
	char *data;
	struct Node_s *next;
} Node;

int main() {
	Node* head = malloc(sizeof(Node));
	if (head == NULL) {
		perror("Failed to malloc.");
    	return -1;
	}
	head->data = NULL;
	Node* store = head;

	while (fgets(str, sizeof(str), stdin) != NULL) {
		if (str[0] == '.') {
			break;
		}
		Node *new = malloc(sizeof(Node));
		if (new == NULL) {
			perror("Failed to malloc.");
        	return -1;
		}
		new->data = malloc(sizeof(char) * (strlen(str) + 1));
		strcpy(new->data, str);
		new->next = NULL;
		head->next = new;
		head = head->next;
	}

	head = store->next;
	while (head != NULL) {
		printf("%s", head->data);
		free(store);
		store = head;
		head = head->next;
	}

	return 0;
}