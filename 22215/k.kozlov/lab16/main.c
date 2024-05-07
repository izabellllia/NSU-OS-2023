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
  struct termios modifiedTerminal, savedTerminal;
  if (tcgetattr(terminalDescriptor, &modifiedTerminal) == -1) {
    perror("Couldn't get terminal's attributes");
    exit(-1);
  }
  savedTerminal = modifiedTerminal;
  modifiedTerminal.c_lflag &= ~ICANON;
  modifiedTerminal.c_cc[VMIN] = 1;
  modifiedTerminal.c_cc[VTIME] = 0;
  if (tcsetattr(terminalDescriptor, TCSANOW, &modifiedTerminal) == -1) {
    perror("Couldn't set terminal's changed attributes");
    exit(-1);
  }

  char answer[2] = "", *question = "Do you like operating systems? ";
  write(terminalDescriptor, question, strlen(question));
  read(terminalDescriptor, answer, 1);
  while (answer[0] != 'y' && answer[0] != 'Y') {
    printf("\nWrong answer! Go out and think more... \n");
    write(terminalDescriptor, question, strlen(question));
    read(terminalDescriptor, answer, 1);
  }
  printf("\nRight answer!\n");

  if (tcsetattr(terminalDescriptor, TCSAFLUSH, &savedTerminal) == -1) {
    perror("Couldn't restore terminal's original attributes");
  }
  return 0;
}