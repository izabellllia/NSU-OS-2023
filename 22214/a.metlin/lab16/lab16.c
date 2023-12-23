#include <stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>


int main(){
    struct termios old_settings;

    if(tcgetattr(0, &old_settings)==-1) {
        perror("error tcgetattr\n");
        exit(errno);
    }

    struct termios new_settings;
    new_settings = old_settings;

    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
        new_settings.c_cc[VMIN] = 1;

    if(tcsetattr(0, TCSANOW, &new_settings)==-1) {
        perror("error tcsetattr\n");
        exit(errno);
    }

    char buf;
    printf("?\n");
    read(0, &buf, 1);

    if(tcsetattr(0, TCSANOW, &old_settings)==-1) {
        perror("error tcsetattr\n");
        exit(errno);
    }

    return 0;
}
