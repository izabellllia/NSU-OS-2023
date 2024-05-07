#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <termios.h>


int main() {
    char inp;
    struct termios prev_term, new_term;

    if (isatty(STDIN_FILENO) == 0) {
        perror("Standard input is not a terminal!!! Aborting");
        exit(EXIT_FAILURE);
    }

    tcgetattr(STDIN_FILENO, &prev_term);

    new_term = prev_term;
    new_term.c_lflag &= ~ICANON;
    new_term.c_cc[VMIN] = 1;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term) == -1) {
        perror("Setattr failure");
        exit(EXIT_FAILURE);
    }

    if ((read(STDIN_FILENO, &inp, 1)) == -1) {
        perror("Read failure");
        if (tcsetattr(STDIN_FILENO, TCSANOW, &prev_term) != 0) {
            perror("Back to the old termios failed");
        }
        exit(EXIT_FAILURE);
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &prev_term) != 0) {
        perror("Back to the old termios failed");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
