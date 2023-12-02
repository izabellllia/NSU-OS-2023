#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main() { 
    if (isatty(STDIN_FILENO) == 0) {
        perror("stdin isn't a terminal");
        exit(1);
    }
    struct termios current, prev;

    if (tcgetattr(STDIN_FILENO, &prev) == -1) {
        perror("failed to get terminal's attribute");
        exit(1);
    }
    current = prev; 

    current.c_lflag &= ~(ICANON);
    current.c_cc[VMIN] = 1;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &current) == -1) {
        perror("failed to set current terminal's attribute");
        exit(1);
    }
    printf("Press a button\n");

    char symbol;
    if (read(STDIN_FILENO, &symbol, 1) == -1) {
        perror("failed to read symbol");
        exit(1);
    }
    printf("\nGoodbye!\n");

    if (tcsetattr(STDIN_FILENO, TCSANOW, &prev) == -1) {
        perror("failed to set prev terminal's attribute");
        exit(1);
    }

    exit(0);
}

