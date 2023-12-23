#include<signal.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int beepCnt = 0;

int main() {
	
	void beeper();
	void quiter();
	sigset(SIGINT, beeper);
	sigset(SIGQUIT, quiter);

	while(1){
	}
}

void beeper() {
	beepCnt++;
	write(STDOUT_FILENO, "\a\n", 2); 
}

void quiter() {
	write(STDOUT_FILENO, "\n", 1);
	
	char number[11];
	int ans = beepCnt;
	
	int i = 0;
	while(ans) {
		number[i] = '0' + ans % 10;
		i++;
		ans /= 10;
	}

	number[i] = 0;
	for(int j = 0; j < i / 2; j++) {
		char tmp = number[j];
		number[j] = number[i - j - 1];
		number[i - j - 1] = tmp;
	}

	write(STDOUT_FILENO, number, i);
	
	write(STDOUT_FILENO, "\n", 1);
	exit(0);
}

