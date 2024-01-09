#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char** argv, char** envp) {
  int terminalDescriptor = open("/dev/tty", O_RDWR);
  if (terminalDescriptor == -1 || !isatty(terminalDescriptor)) {
    perror("Terminal wasn't opened");
    exit(-1);
  }

  struct termios newTerminal, oldTerminal;
  if (tcgetattr(terminalDescriptor, &newTerminal) == -1) {
    perror("Couldn't get terminal's attributes");
    exit(-1);
  }
  oldTerminal = newTerminal;
  newTerminal.c_lflag &= ~ICANON;
  newTerminal.c_lflag &= ~ISIG;
  newTerminal.c_cc[VMIN] = 1;
  newTerminal.c_cc[VTIME] = 0;
  if (tcsetattr(terminalDescriptor, TCSANOW, &newTerminal) == -1) {
    perror("Couldn't set terminal's changed attributes");
    exit(-1);
  }
  const char* message = "Hello from the terminal!\n";
  const char* question = "I really like cats, do you like them too? [Y, N] \n";
  const char* retryMessage = "\nI'm upset with that, rethink and answer one more time :). [Y | N] \n";

  write(terminalDescriptor, message, strlen(message));
  write(terminalDescriptor, question, strlen(question));

  char answer[2] = "";
  read(terminalDescriptor, answer, sizeof(answer));

  while (answer[0] != 'Y' && answer[0] != 'y') {
    if (write(terminalDescriptor, retryMessage, strlen(retryMessage)) == -1) {
      perror("Failed to write to the terminal");
      close(terminalDescriptor);
      exit(-1);
    }

    read(terminalDescriptor, answer, sizeof(answer));
  }

  if (answer[0] == 'Y' || answer[0] == 'y') {
    const char* finalMessage = "\nThat's great that you love cats as much as I do. Thank you.\n";
    write(1, finalMessage, strlen(finalMessage));
  }

  printf("Have a great day, Goodbye!\n");

  if (tcsetattr(terminalDescriptor, TCSAFLUSH, &oldTerminal) == -1) {
    perror("Couldn't restore terminal's original attributes");
  }

  return 0;
}

