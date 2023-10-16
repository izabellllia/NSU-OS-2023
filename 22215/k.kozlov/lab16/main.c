#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Что такое терминал и псевдо-терминал? 
  Терминал - устройство ввода-вывод для работы с текстовой информацией. 
  Классические терминалы были предсталвены клавиатурой и телетайпом или видеотерминалом.
  
  Порт - аппаратный интерфейс ввода-вывода.
  Драйвер устройства - программный интерфейс доступа к порту. Драйвер содержит аппаратно-зависимые функции ядра для работы с портами

  Псевдоустройство - виртуальный объект, обслуживаниемый специальным драйвером, поддерживаемым все те же интерфейсы, что и обычный.
  


Что такое файл-устройство?

termios(3)

Структура struct termios содержит поля с флагами, отвечающими за различные параметры терминала:
struct termios {
  tcflag_t c_iflag;      // input modes
  tcflag_t c_oflag;      // output modes
  tcflag_t c_cflag;      // control modes
  tcflag_t c_lflag;      // local modes
  cc_t     c_cc[NCCS];   // special characters
}

tcflag_t - битовая маски

tcgetattr(int terminalDescriptor, termios_p structPointer)
  записывает по указателю structPointer данные структуры struct termios терминала по переданному дескриптору

tcsetattr(int terminalDescriptor, int when_set, termios_p structPointer)
  сохраняет параметры структуры struct termios по указателю termios_p в терминал по переданному дескриптору
  when_set указывает, в какой момент необходимо сохранить параметры:
    TCSANOW - немедленно
    TCSADRAIN - когда были переданы все направленные на вывод данные. Следует использовать при изменении флагов c_oflag
    TCSAFLUSH - когда были переданы все направленные на вывод данные и были обработаны все полученные на ввод данные.

*/

int main (int argc, char** argv, char** envp) {
  int terminalDescriptor = open("/dev/tty", O_RDWR);
  if (terminalDescriptor == -1) {
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