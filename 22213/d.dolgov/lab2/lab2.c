#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[], char *envp[]) {
    putenv("TZ=GMT+7"); // сеттим время на Калифорнию
    time_t rawtime; // структура под время
    time(&rawtime); // сеттим текущую дату и время в структуру
    printf("%s", asctime(localtime(&rawtime))); // выводим дату и время в Калифорнии

    exit(0);
}
