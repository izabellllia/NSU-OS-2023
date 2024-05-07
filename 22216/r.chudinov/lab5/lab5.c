#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define INIT_CAPACITY_OFF_T 100
#define SIZE_OF_BUFFER 512

typedef struct {
	off_t * data;
	size_t size;
	size_t capacity;
} DynamicArrayOffT;

void initDynamicArrayOffT(DynamicArrayOffT* arr, size_t initialCapacity) {
	arr->data = malloc(sizeof(off_t) * initialCapacity);
	arr->size = 0;
	arr->capacity = initialCapacity;
}

void addToOffTArray(DynamicArrayOffT* arr, off_t value) {
	if (arr->size == arr->capacity) {
		arr->capacity *= 2;
		arr->data = realloc(arr->data, sizeof(off_t) * arr->capacity);
	}
	arr->data[arr->size] = value;
	arr->size++;
}


int main(int argc,char* argv[]) {
	if (argc < 2) {
		printf("Filename not passed\n");
		exit(-1);
	}

	int file_descriptor = open(argv[1], O_RDONLY);
	if(file_descriptor == -1) {
		perror("Error");
		exit(-1);
	}

	DynamicArrayOffT offsetBeforeString;
	initDynamicArrayOffT(&offsetBeforeString, INIT_CAPACITY_OFF_T);

	offsetBeforeString.data[0] = 0;
	offsetBeforeString.data[1] = 0;
	offsetBeforeString.size = 2;
	char buffer[SIZE_OF_BUFFER];

	while(1) {
		int numberCountBite = read(file_descriptor, buffer, SIZE_OF_BUFFER);
		if(numberCountBite) {
			for (int i = 0; i < numberCountBite; i++) {
				offsetBeforeString.data[offsetBeforeString.size - 1]++;
				if(buffer[i] == '\n') {
					addToOffTArray(&offsetBeforeString, offsetBeforeString.data[offsetBeforeString.size - 1]);
				}
				if(buffer[i] == '\0') {
					break;
				}
			}
		} else {
			break;
		}
	}

	int request = 0;
	char bufferForOutput[SIZE_OF_BUFFER + 1];

	while(1) {
		printf("Enter number of string that you want to see\n");
		int result = scanf("%d", &request);
		if(result != 1) {
			printf("Please, enter only number of string\n");
			int c;
			while ((c = getchar()) != '\n' && c != EOF);
			continue;
		}

		if(request >= offsetBeforeString.size - 1) {
			printf("There is no such number\n");
			continue;
		}

		if (request == 0) {
			close(file_descriptor);
			free(offsetBeforeString.data);
			return 0;
		}
		lseek(file_descriptor, offsetBeforeString.data[request - 1], SEEK_SET);
		int countOfElement = offsetBeforeString.data[request] - offsetBeforeString.data[request - 1];
		printf("String:");
		while(countOfElement > 0) {
			if(countOfElement <= SIZE_OF_BUFFER) {
				read(file_descriptor, bufferForOutput, countOfElement);
				bufferForOutput[countOfElement] = '\0';
				printf("%s", bufferForOutput);
				countOfElement = 0;
			} else {
				read(file_descriptor, bufferForOutput, SIZE_OF_BUFFER);
				bufferForOutput[SIZE_OF_BUFFER] = '\0';
				printf("%s", bufferForOutput);
				countOfElement -= SIZE_OF_BUFFER;
			}
		}
	}
}
