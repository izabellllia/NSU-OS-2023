#include "unistd.h"
#include "stdio.h"
#include "errno.h"

void printIds() {
  uid_t uid = getuid(), euid = geteuid();
  printf("UID: %d / EUID: %d\n", uid, euid);
}

void tryOpen(char* fileName) {
  printIds();
  printf("Reading file \"%s\"...\n", fileName);
  FILE* fStream = fopen(fileName, "r");
  if (fStream) {
    printf("File was read successfully\n");
    if (fclose(fStream) != 0)
      perror("Failed to close file");
    else
      printf("File was closed\n");
  }
  else
    perror("Failed to read file");
}

void setuidWrapper(uid_t newEUID) {
  printf("Setting EUID to %d...\n", newEUID);
  if (setuid(newEUID) == 0)
    printf("Success!\n");
  else 
    perror("Failed to set EUID");
}

int main(int argc, char** argv) {
  char fileName[5] = "file";
  tryOpen(fileName);
  printf("\n");
  setuidWrapper(getuid());
  printf("\n");
  tryOpen(fileName);
}