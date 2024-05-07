#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

typedef struct vector {
	off_t* arr;
        int cap;
	int sz;	
} vector; 

void init(vector *cur) {
	cur->cap = 2;
	cur->sz = 0;
	cur->arr = (off_t*)malloc(sizeof(off_t)*2);
	if (cur->arr == NULL) {
		perror("Can't allocate initial vector array");
		exit(ENOMEM);
	}
}

void myScanf(int *query) {
	while(1) {
		int scanned = scanf("%d", query);
		if (scanned == 0 || getchar() != '\n') {
			printf("ERROR: only int allowed\n");
			while(getchar() != '\n');
		} else {
			return;
		}
	}
}

void push_back(vector *cur, off_t value) {
	if (cur->sz == cur->cap) {
		cur->cap *= 2;
		cur->arr = (off_t*)realloc(cur->arr, sizeof(off_t)*cur->cap);
		if (cur->arr == NULL) {
			perror("Can't reallocate vector array");
			exit(ENOMEM);
		}	
	}
	cur->arr[cur->sz] = value;
	cur->sz++;
}


int main() {
	
	int fd = open("text.txt", O_RDONLY);
	if (fd == -1) {
		perror("Can't open file");
		exit(errno);
	}

	vector *positions = (vector*)malloc(sizeof(vector));
	if(positions == NULL) {
		perror("Can't allocate memory for vector structure");
		exit(ENOMEM);
	}
	init(positions);
	push_back(positions, (off_t) 0);
	

	char buf;
	int cnt = read(fd, &buf, 1);
	while(cnt){
		if (buf == '\n') {
			off_t offset = lseek(fd, 0L, SEEK_CUR);
			push_back(positions, offset);
		}
		cnt = read(fd, &buf, 1);
	}


	int query;
	myScanf(&query);

	while (query){
		if (query >= positions->sz) {
			printf("ERROR: too big number\n"); 
			myScanf(&query);
			continue;
		}
		if (query < 0) {
			printf("ERROR: negative number\n");
			myScanf(&query);
			continue;
		}
		
		
		lseek(fd, positions->arr[query - 1], SEEK_SET);
		int len = positions->arr[query] - positions->arr[query - 1];
		char string[len];
		
		read(fd, &string, len);
		printf("%s", string);

		myScanf(&query);
	}

	close(fd);
	free(positions->arr);
	free(positions);

	return 0;
}
