#include<stdio.h>
#include<string.h>
#include<errno.h>


typedef struct Node {
	struct Node* next;
	char* string;
} Node;

Node* first = NULL,* last = NULL;

void addNode(Node* node) {
	node->next = NULL;
	if (first == NULL) {
		first = node;
		last = node;
	} else {
		last->next = node;
		last = last->next;
	}
}

void freeNodes() {
	while(first != NULL) {
		last = first->next;
		
		free(first->string);
		free(first);

		first = last;
	}
}

int main() {
	
	char buf[BUFSIZ];
	while(buf[0] != '.') {
		char* res = fgets(buf, BUFSIZ - 1, stdin);
		if (res == NULL) {
			perror("Error while reading string");
			exit(errno);
		}
		size_t length = strlen(buf);

		Node* newNode =(Node*)malloc(sizeof(Node));
		if (newNode == NULL) {
			perror("Can't allocate memory\n");
			exit(ENOMEM);
		}

		newNode->string = malloc(length + 1);
		if (newNode->string == NULL) {
			perror("Can't allocate memory\n");
			exit(ENOMEM);
		}

		memcpy(newNode->string, buf, length + 1);
		
		addNode(newNode);
	}
	
	printf("\n");
	
	Node* cur = first;	
	while(cur != NULL) {
		printf("%s", cur->string);
		cur = cur->next;
	}
	
	freeNodes();

	return 0;
}
