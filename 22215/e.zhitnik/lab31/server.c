#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>


#define MAX_BUFFER_SIZE 512

char *path = "./socket";

int client_reader(int descriptor) {
    char buff[MAX_BUFFER_SIZE];
    ssize_t reader;
    reader = read(descriptor, buff, sizeof(buff));
    if (reader < 0) {
        perror("error while reading");
        return -1;
    } else {
        if (reader == 0) {
            return -1;
        } else {
            for (int i = 0; i < reader; i++) {
                putchar(toupper((unsigned char)buff[i]));
            }
        }
    }
    return 0;
}

void sig_handler(int signo) {
    unlink(path);
    exit(0);
}

void unlinker() {
    unlink(path);
}

int array_descriptors[FD_SETSIZE];
int count_busy = 0;

void add_descriptor(int desc) {
    if (count_busy < FD_SETSIZE) {
        array_descriptors[count_busy++] = desc;
        array_descriptors[count_busy] = -1;
    } else {
        fprintf(stderr, "Descriptor array is full\n");
    }
}

void delete_descriptor(int desc) {
    for (int i = 0; i < count_busy; i++) {
        if (array_descriptors[i] == desc) {
            for (int j = i; j < count_busy - 1; j++) {
                array_descriptors[j] = array_descriptors[j + 1];
            }
            count_busy--;
            array_descriptors[count_busy] = -1;
            return;
        }
    }
    fprintf(stderr, "Descriptor not found in the array\n");
}

int main() {
    signal(SIGINT, sig_handler);

    int descriptor;
    struct sockaddr_un address;
    fd_set active_set, read_set;

    if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Can't create socket");
        return -1;
    }

    for (int i = 0; i < FD_SETSIZE; i++) {
        array_descriptors[i] = -1;
    }
    add_descriptor(descriptor);

    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);
    address.sun_path[sizeof(address.sun_path) - 1] = '\0';

    unlink(path);

    if (bind(descriptor, (struct sockaddr *) &address, sizeof(address)) == -1) {
        perror("error while binding");
        return -1;
    }

    if (atexit(unlinker) != 0) {
        perror("error while registering atexit handler");
        return -1;
    }

    if (listen(descriptor, 50) == -1) {
        perror("error while listening");
        return -1;
    }

    FD_ZERO(&active_set);
    FD_SET(descriptor, &active_set);

    while (1) {
        read_set = active_set;
        if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
            perror("error while selecting");
            return -1;
        }

        for (int i = 0; i < count_busy; i++) {
            if (FD_ISSET(array_descriptors[i], &read_set)) {
                if (array_descriptors[i] == descriptor) {
                    int current_client = accept(descriptor, NULL, NULL);
                    if (current_client < 0) {
                        perror("error while accepting");
                    } else {
                        add_descriptor(current_client);
                        FD_SET(current_client, &active_set);
                    }
                } else {
                    if (client_reader(array_descriptors[i]) < 0) {
                        close(array_descriptors[i]);
                        FD_CLR(array_descriptors[i], &active_set);
                        delete_descriptor(array_descriptors[i]);
                    }
                }
            }
        }
    }

    return 0;
}
