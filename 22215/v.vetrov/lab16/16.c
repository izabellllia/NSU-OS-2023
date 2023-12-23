#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

struct termios oldt;

void signal_handler(int signum) {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  printf("\nПрограмма прервана, настройки терминала восстановлены\n");
  exit(signum);
}

int main() {
  struct termios newt;
  int ch;

  tcgetattr(STDIN_FILENO, &oldt);

  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);

  signal(SIGINT, signal_handler);
  
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  printf("Введите символ: ");
  ch = getchar();
  printf("\nВведенный символ: %c\n", ch);

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return 0;
}