#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void handleFile(const char* fileName) {
  FILE *fptr = fopen(fileName, "r");
  if (!fptr) {
    perror("Unable to open the file");
    return;
  }
  fclose(fptr);
}

int main(int argc, char *args[]) {
  if (argc < 2) {
    fprintf(stderr, "Please provide a file name.\n");
  }

  printf("Real User ID: %d\n", getuid());
  printf("Effective User ID: %d\n", geteuid());
  handleFile(args[1]);

  if (setuid(getuid()) != 0) {
    perror("Error setting the UID");
  }

  printf("After modifying UIDs:\n");

  printf("Real User ID: %d\n", getuid());
  printf("Effective User ID: %d\n", geteuid());
  handleFile(args[1]);

  return 0;
}
