#include <stdio.h>
#include <stdlib.h>

#include <termios.h>


int main() {
    struct termios tdes;
    tcgetattr(0, &tdes);

    tdes.c_lflag &= ~ICANON;
    tdes.c_cc[VMIN] = 1;

    tcsetattr(0, TCSAFLUSH, &tdes);


    char inp;
    printf("Please say yes\n");
    scanf("%c", &inp);

    if (inp == 'y' || inp == 'Y')
    {
        printf("\nOh yeah...\n");
    } else
    {
        printf("\nOh no(((\n");
    }

    return 0;
}
