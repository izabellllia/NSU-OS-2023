#include <unistd.h>
#include <sys/termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define LINE_LENGTH 40

typedef struct Line_s {
    int len;
    char* str;
} Line;

typedef struct Stack_s {
    struct Stack_s *next;
    Line line;
} Stack;


Line stack_pop(Stack* *head) {
    if ((*head) == NULL) {
        printf("trying to pop from an empty stack\n");
        exit(1);
    }

    Line prev_line = {(*head)->line.len, 
                      (*head)->line.str};

    Stack* saved_pointer = *head;
    
    *head = (*head)->next;
    free(saved_pointer);

    return prev_line;
}


Stack* stack_push(Stack *head, int value, char* str) {
    Stack* newHead = malloc(sizeof(Stack));
    if (newHead == NULL) {
        perror("failed to alloc memory");
        exit(1);
    }

    newHead->line.len = value;
    newHead->line.str = str;

    newHead->next = NULL;
    if (head != NULL) {
        newHead->next = head;
    }

    return newHead;
}


struct termios tty, savetty;

void reset_terminal() {
    tcsetattr(0, TCSANOW, &savetty);
}

int main() {
    if (!isatty(0)) {
        fprintf(stderr, "no stdin\n");
        exit(1);
    }

    if (tcgetattr(0, &tty) == -1) {
        perror("failed to get attributes");
        exit(1);
    }
    
    savetty = tty;
    atexit(reset_terminal);

    tty.c_lflag &= ~(ECHO | ICANON);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &tty) == -1) {
        perror("failed to set attributes");
        exit(1);
    }

    Stack* head = NULL;

    char c;
    char *line = malloc(LINE_LENGTH + 1);
    int pos = 0;
    char temp_string[8];

    while (read(0, &c, 1) == 1) {

        // got CTRL+D
        if (c == CEOF && pos == 0) {
            break;
        }
    
        // got backspace
        if (c == tty.c_cc[VERASE]) {
            // ESC+[+K - erase from cursor to end of the line
            if (pos > 0) {
                write(1, "\b\x1b[K", 4);
                pos--;
                continue;
            }

            if (pos == 0 && head != NULL) {
                free(line);
                Line prev_line = stack_pop(&head);
                line = prev_line.str;
                pos = prev_line.len;
                // ESC+[+A - moves cursor up 1 line
                write(1, "\x1b[A", 3);

                // ESC+[+#+G - moves cursor to column #
                int len = sprintf(temp_string, "\x1b[%dG", pos + 1);
                write(1, &temp_string, len);
            }
            continue;
        }

        // got CTRL+U
        // ESC+[+2+K - erase the entire line
        if (c == tty.c_cc[VKILL]) {
            write(1, "\x1b[2K\r", 5);
            line[0] = 0;
            pos = 0;
            continue;
        }

        // got CTRL+W
        if (c == tty.c_cc[VWERASE]) {
            while (pos > 0 && isspace(line[pos - 1])) {
                write(1, "\b", 1);
                pos--;
            }

            while (pos > 0 && !isspace(line[pos - 1])) {
                write(1, "\b", 1);
                pos--;
            }

            // ESC+[+K - erase from cursor to end of the line
            write(1, "\x1b[K", 3);

            continue;
        }

        if (c == '\n') {
            write(1, &c, 1);

            head = stack_push(head, pos, line);
            line = malloc(LINE_LENGTH + 1);
            pos = 0;
    
            continue;
        }

        if (!isprint(c)) {
            write(1, "\a", 1);
            continue;
        }

        if (pos == LINE_LENGTH) {
            if (isspace(c)) {
                write(1, "\n", 1);
                head = stack_push(head, LINE_LENGTH, line);
                pos = 0;
                line = malloc(LINE_LENGTH + 1);
                continue;
            }

            while (pos > 0 && !isspace(line[pos - 1])) {
                write(1, "\b", 1);
                pos--;
            }

            char* temp = malloc(LINE_LENGTH + 1);
            if (pos != 0) {
                write(1, "\x1b[K", 3);
                
                memcpy(temp, &line[pos], LINE_LENGTH - pos);
                pos = LINE_LENGTH - pos;
            }
            head = stack_push(head, LINE_LENGTH - pos, line);
            temp[pos++] = c;
            write(1, "\n", 1);
            write(1, temp, pos);
            line = temp;

            continue;
        }

        if (isprint(c)) {
            write(1, &c, 1);
            line[pos++] = c;
            continue;
        }

    }

    return 0;
}
