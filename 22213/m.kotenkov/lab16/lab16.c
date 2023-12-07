#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <termios.h>


int main() {
    char inp;
    struct termios prev_term, new_term;

    tcgetattr(0, &prev_term);

    new_term = prev_term;
    new_term.c_lflag &= ~ICANON;
    new_term.c_cc[VMIN] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term) == -1) {
        perror("Standard input is not a terminal!!! Aborting");
        exit(EXIT_FAILURE);
    }

    if ((write(1, "Please say yes\n", 16)) == -1) {
        perror("Write failure");
        exit(EXIT_FAILURE);
    }
    if ((read(0, &inp, 1)) == -1) {
        perror("Read failure");
        exit(EXIT_FAILURE);
    }

    if (inp == 'y' || inp == 'Y') {
        if ((write(1, "\nOh yeah...\n", 13)) == -1) {
            perror("Write failure");
            exit(EXIT_FAILURE);
        }
    } else {
        if ((write(STDIN_FILENO, "\nOh no(((\n", 11)) == -1) {
            perror("Write failure");
            exit(EXIT_FAILURE);
        }
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &prev_term) != 0) {
        perror("Tcsetattr failure");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
