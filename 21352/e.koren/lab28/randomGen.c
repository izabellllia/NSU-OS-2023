#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

int main(int argc, char** argv){
	srand(time(NULL));
	FILE* fptrs[2];

	if(p2open("/bin/sort -n", fptrs)==-1){
		perror("p2open failed");
		return 1;
	}

	int num;

	printf("generated nums:\n");
	for (int i =0; i<10;i++){
		for (int j =0 ; j<10;j++){
			num = rand()%100;
			fprintf(fptrs[0],"%d\n",num);
			printf("%d ",num);
		}
		printf("\n");
	}

	if(fclose(fptrs[0])==1){
		perror("fclose failed");
		return 1;
	}

	printf("sorted nums:\n");
	for (int i =0; i<10;i++){
		for (int j =0 ; j<10;j++){
			fscanf(fptrs[1],"%d",&num);
			printf("%d ",num);
		}
		printf("\n");
	}

	if(fclose(fptrs[1]) == 1){
		perror("fclose failed");
		return 1;
	}

	return 0;
}
