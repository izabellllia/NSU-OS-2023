#include <stdio.h> //fopen, printf
#include <unistd.h>    //getuid, setuid,
#include <sys/types.h> //geteuid, seteuid

void printUserIDs(){
	printf("Real user ID: %d\n",getuid());
	printf("Effective user ID: %d\n",geteuid());
}

void workWithFile(const char* file_path){
	FILE* used_file = fopen(file_path, "r");
	if (used_file == NULL){
		perror("Failed to open file.");
		return;
	}
	printf("File opened successfully.\n");
	fclose(used_file);
	printf("File closed successfully.\n");
}

int main(int argc, char** argv){
	printf("User IDs at start:\n");
	printUserIDs();
	workWithFile(argv[1]);
	
	if (seteuid(getuid())!=0){
		perror("Failed to change effective ID.");
	}

	printf("User IDs after change:\n");
	printUserIDs();
	workWithFile(argv[1]);

	return 0;
}
