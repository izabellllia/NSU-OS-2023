#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main() {
	int fd = open("output.txt", O_WRONLY | O_CREAT);
	sleep(5);
	write(fd, "Printed", 7);
	close(fd);
	return 0;
}
