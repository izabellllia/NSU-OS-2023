#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#define MAX 100
int main()
{
  struct pollfd fds[MAX];
  fds[0].events = POLLIN;

  struct sockaddr_un addr;
  char buf[BUFSIZ];
  int fd, cl, rc;
  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  fds[0].fd = fd;
  if ((fd) == -1)
  {
    perror("socket error");
    return -1;
  }
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, "socket", sizeof(addr.sun_path) - 1);
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    perror("bind error");
    return -1;
  }
  if (listen(fd, MAX) == -1)
  {
    perror("listen error");
    unlink("socket");
    return -1;
  }
  int count = 1;
  int ret = 0;
  while (1)
  {
    if (cl == -1)
    {
      perror("accept error");
      unlink("socket");
      return -1;
    }

    ret = poll(fds, MAX, 50 * 1000);
    if (ret > 0)
    {
      if (fds[0].revents & POLLIN)
      {
        cl = accept4(fd, NULL, NULL, SOCK_NONBLOCK);
        if (cl == -1)
        {
          perror("New connection error");
          unlink("socket");
          return -1;
        }
        if (count == MAX)
        {
          printf("Max opened connections limit\n");
        }
        else
        {
          printf("Connected\n");
          for (int i = 1; i < MAX; i++)
          {
            if (fds[i].revents & POLLNVAL)
            {
              fds[i].fd = cl;
              count++;
              fds[i].events = POLLIN;
              break;
            }
          }
        }
      }

      for (int i = 1; i < MAX; i++)
      {
        if (fds[i].revents & POLLIN)
        {
          int x = read(fds[i].fd, buf, BUFSIZ);
          if (x == 0)
          {
            close(fds[i].fd);
            count--;
            if (count == 1)
            {
              unlink("socket");
              return 0;
            }
          }
          else if (x == -1)
          {
            unlink("socket");
            close(fds[i].fd);
            perror("Couldn't read socket");
            return -1;
          }
          else
          {
            for (int i = 0; i < x; i++)
            {
              printf("%c", toupper(buf[i]));
            }
          }
        }
      }
    }
    else if (ret == -1)
    {
      perror("Poll error");
      unlink("socket");
      return -1;
    }
  }
  return 0;
}