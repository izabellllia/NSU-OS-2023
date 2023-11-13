#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

/*
Что такое сокет?
Сокет - специальное псевдоустройство подсистемы STREAMS, используемое для связи между процессами на одной машине или на разных.
  Являются улучшенной версией труб (pipe), которые также позволяют передавать данные между процессами, но обладают рядом недостатков:
    - позволяют передавать данные лишь локально и между двумя процессами (тогда как сокеты позволяют общаться множеству процессов с одним, хранящим серверный сокет (listen(2)))
    - Однонаправленные, тогда как сокеты двунаправленные


Вахалия - 17.10

ПРИ КОМПИЛЯЦИИ НАДО ЗАГРУЖАТЬ ДОПОЛНИТЕЛЬНУЮ БИБЛИОТЕКУ socket:
  gcc -Wall -lsocket file.c

socket(2) - создаёт точку связи и возвращает его дескриптор
  int socket(int domain, int type, int protocol)
  domain:
    AF_UNIX / AF_LOCAL - Unix domain socket
      Unix Domain Socket позволяет связываться процессам через сокет в рамках одной системы. 
      Права доступа к сокетам будут определяться простыми правами доступа файлов
    AF_INET - IPv4
    AF_INET6 - IPv6
  type:
    SOCK_STREAM - двунаправленный сокет для передачи потока байтов
    SOCK_DGRAM - сокеты для передачи пакетов (датаграмм)
  Подробнее о доменах, типах и протоколах смотри man -s 2 socket

bind(2) - привязывает имя сокета с его дескриптором
  int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  struct sockaddr {
      sa_family_t sa_family; // domain
      char        sa_data[14]; // socket name
  }; - общая версия структуры
  struct sockaddr_un {
      sa_family_t sun_family;               // AF_UNIX
      char        sun_path[UNIX_PATH_MAX];  // pathname
  }; - версия для работы с доменом AF_UNIX

listen(2)
  int listen(int sockfd, int backlog);
  Помечает сокет по дескриптору sockfd как пассивный, то есть сокет, который будет принимать входящие подключения через accept(2)
    Иначе говоря, регистрирует сокет как серверный
  backlog показывает максимальную длину очереди ожидаемых подключений от клиентов
  Перед использованием listen должны быть вызваны socket(2) для создания сокета и bind(2) для возможности обращаться к сокету извне

accept(2)
  int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
  Принимает первый в очереди запрос на соединение с сокетом sockfd и возвращает дескриптор на новый сокет, который не будет в состоянии прослушивания и будет связан с обрабатываемым клиентом
  Если передать указатели вторым и третьим аргументами, они будут заполнены данными созданного сокета-клиента
  Если очередь запросов пуста и сокет не отмечен как неблокирующий, процесс будет заблокирован, пока не появится запрос к сокету. Если сокет неблокирующий, то accept завершится с ошибкой EAGAIN или EWOULDBLOCK
  Вместо ожидания в блоке можно использовать select(2) или poll(2), которые сообщат о появлении запроса в очереди

connect(2)
  int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  Соединяет дескриптор сокета sockfd с адресом, определённым в структуре addr

send(2)
  *Тут не используется, но имеет смысл изучить*

recv(2)
  *Тут не используется, но имеет смысл изучить*

read/write и другие базовые IO операции для сокетов работают через драйвера

как работает atexit()? В частности, как он работает, если в конце функции у нас return, а не exit?
*/

#define MAX_REQUESTS_QUEUE 5

void intPerrorTrigger(int value, char* errorMessage) {
  if ((value) == -1) {
    perror(errorMessage);
    exit(-1);
  }
}

void handleSigInt(int signalValue) {
  exit(signalValue);
}

void toUpperString(int sz, char* str) {
  for (int i = 0; i < sz; ++i) {
    str[i] = (char)toupper((int)str[i]);
  }
}

#define SERVER_SOCKET_NAME "socket.sock"
int serverSocketDescriptor;

void closeServerSocket() {
  printf("\nServer closed\n");
  intPerrorTrigger(close(serverSocketDescriptor), "Failed to close server socket");
  intPerrorTrigger(unlink(SERVER_SOCKET_NAME), "Failed to unlink server socket file");
}

int main(int argc, char** argv) {
  signal(SIGINT, handleSigInt);
  serverSocketDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
  intPerrorTrigger(serverSocketDescriptor, "Failed to create socket");
  atexit(closeServerSocket);
  struct sockaddr_un serverSocketAddr;
  memset(&serverSocketAddr, 0, sizeof(serverSocketAddr));
  serverSocketAddr.sun_family = AF_UNIX;
  strncpy(serverSocketAddr.sun_path, SERVER_SOCKET_NAME, sizeof(serverSocketAddr.sun_path)-1);
  intPerrorTrigger(
    bind(serverSocketDescriptor, (struct sockaddr*)&serverSocketAddr, sizeof(serverSocketAddr)), 
    "Failed to bind socket"
  );
  intPerrorTrigger(
    listen(serverSocketDescriptor, 0), 
    "Failed to mark socket as listener"
  );

  int clientSocketDescriptor = accept(serverSocketDescriptor, NULL, NULL);
  intPerrorTrigger(clientSocketDescriptor, "Failed to accept request from socket");
  char buf[256] = "";
  int bytesRead;
  while ((bytesRead = read(clientSocketDescriptor, buf, sizeof(buf))) != 0)
  {
    intPerrorTrigger(bytesRead, "Failed to read from socket");
    toUpperString(bytesRead, buf);
    intPerrorTrigger(write(1, buf, bytesRead), "Failed to write processed text");
  }
  return 0;
}
