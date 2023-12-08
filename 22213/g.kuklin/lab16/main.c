#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main() {
    if (!isatty(STDIN_FILENO)) {
        printf("Stdin is not a terminal\n");
        return 1;
    }

    struct termios previous, current; 
    if (tcgetattr(STDIN_FILENO, &previous) != 0) {
        perror("Failed to get terminal attributes"); 
    }
    current = previous;

    current.c_lflag &= ~ICANON;
    current.c_cc[VMIN] = 1;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &current) != 0) {
        perror("Failed to set new terminal attributes");    
    }
    
    printf("Press a button to accept this lab: ");
    if (fflush(stdout) != 0) {
        perror("Failed to flush stdout");
    };
    char sym;
    if (read(STDIN_FILENO, &sym, 1) < 0) {
        perror("Failed to read a byte from terminal");        
    }
    printf("\nThanks\n");

    if (tcsetattr(STDIN_FILENO, TCSANOW, &previous) != 0) {
        perror("Failed to set terminal attributes back");
    }
    return 0;
}
