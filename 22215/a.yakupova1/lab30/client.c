#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_FILE "mysocket.sock"

int main(int argc, char *argv[]) {

    int fileDesc = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fileDesc == -1) {
        perror("error in socket");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_FILE);

    if (connect(fileDesc, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("error in connect");
        close(fileDesc);
        return -1;
    }

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int charsRead = read(STDIN_FILENO, buffer, sizeof(buffer));
    while(charsRead > 0) {
        int charsWrite;
        int done = 0;
        int diff = charsRead;
        do {
	    charsWrite = write(fileDesc, buffer+done, diff);
	    if (charsWrite == -1){
	        perror("error in write");
	        close(fileDesc);
	        return -1;
	    }
	    diff -= charsWrite;
	    done += charsWrite;
	} while (diff != 0);

	charsRead = read(STDIN_FILENO, buffer, sizeof(buffer));
    }
    if (charsRead == -1){
    	perror("error in read");
	close(fileDesc);
	return -1;
    }
    close(fileDesc);

    return 0;
}
