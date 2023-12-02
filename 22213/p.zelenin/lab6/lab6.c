#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define BUF_LEN 20
#define READBUF_LEN 20
#define SECONDS 5

void makeLinesTable(int fd, off_t *count, int *count_n, off_t *buf_n, int* maxLen) {
    int len = 0;
    buf_n[0] = -1;
    char *readBuffer = (char *) malloc(sizeof(char) * READBUF_LEN);
    if (readBuffer == NULL) {
        perror("Memory allocation error (readBuffer) ");
        exit(EXIT_FAILURE);
    }
    int capacity_n = BUF_LEN;
    ssize_t readBytes;
    while ((readBytes = read(fd, readBuffer, READBUF_LEN)) > 0) {
        for (ssize_t i = 0; i < readBytes; ++i) {
            if (*(count_n) >= capacity_n) {
                buf_n = (off_t *) realloc(buf_n, sizeof(off_t) * capacity_n * 2);
                capacity_n *= 2;
            }
            if (readBuffer[i] == '\n') {
                buf_n[++*(count_n)] = *(count) + (off_t) i;
                (*maxLen) = *(maxLen) < len ? len : *(maxLen);
                len = 0;
            }
            else {
                len++;
            }
        }
        *(count) += (off_t) readBytes;
    }
    if (readBytes < 0) {
        perror("read() caused an error ");
        exit(EXIT_FAILURE);
    }

    buf_n[++*(count_n)] = *(count);
    free(readBuffer);
}

int fd;

void handler() {
    char buf[BUF_LEN];
    write(STDIN_FILENO, "\n", 1);
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek() caused an error (handler) ");
        exit(EXIT_FAILURE);
    }

    ssize_t readBytes;
    while ((readBytes = read(fd, buf, BUF_LEN)) > 0) {
        buf[readBytes] = '\0';
        write(STDOUT_FILENO, buf, readBytes);
    }
    if (readBytes < 0) {
        perror("read() caused an error (handler)) ");
        exit(EXIT_FAILURE);
    }

    close(fd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    int count_n = 0, maxLen = 0;
    off_t count = 0;
    int capacity = BUF_LEN;
    int line;
    char* buf = (char*)malloc(sizeof(char) * BUF_LEN);
    off_t* buf_n = (off_t*)calloc(BUF_LEN, sizeof(off_t));

    if (signal(SIGALRM, handler) == SIG_ERR) {
        perror("signal(2) caused an error ");
        exit(EXIT_FAILURE);
    }

    if (buf == NULL) {
        perror("Memory allocation error (buf) ");
        exit(EXIT_FAILURE);
    }

    if (buf_n == NULL) {
        perror("Memory allocation error (buf_n) ");
        exit(EXIT_FAILURE);
    }

    if (((fd = open(argv[1], O_RDONLY)) == -1)) {
        perror("Could not open the file - ");
        exit(EXIT_FAILURE);
    }

    makeLinesTable(fd, &count, &count_n, buf_n, &maxLen);

    if (capacity < maxLen) {
        buf = (char* ) realloc(buf, sizeof(char) * (maxLen + 1));
        if (buf == NULL) {
            perror("realloc() caused an error (buf) ");
            exit(EXIT_FAILURE);
        }
    }

    int c;
    while (1) {
        printf("Enter the line number - ");
        alarm(SECONDS);

        if ((scanf("%d", &line) != 1) || ((c = getchar()) != '\n')) {
            alarm(0);
            while ((c = getchar()) != '\n');
            printf("Could not read the line number\n");
            continue;
        }

        if (line >= count_n || line < 0) {
            printf("There's no such line\n");
            continue;
        }
        if (line == 0) {
            break;
        }

        size_t len = buf_n[line] - buf_n[line - 1] - 1;
        if (lseek(fd, buf_n[line - 1] + 1, SEEK_SET) == -1) {
            perror("lseek() caused an error trying to get the line-");
            exit(EXIT_FAILURE);
        }
        if (read(fd, buf, len) != (ssize_t)len) {
            perror("read() caused an error (line reading)");
            exit(EXIT_FAILURE);
        }
        buf[len] = '\0';
        printf("%s\n", buf);
    }

    free(buf_n);
    free(buf);
    close(fd);
    exit(EXIT_SUCCESS);
}
