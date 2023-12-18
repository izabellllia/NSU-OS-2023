#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"

/*
Как работает malloc?
	/proc/sys/vm/overcommit_memory
	/proc/sys/vm/oom_adj
	proc(5)

	В обычно ситуации malloc выделяет память на куче и расширяет её, если это необходимо, использую sbrk(2)
		brk(2),sbrk(2) - уменьшают red zone памяти процесса, при обращении к которой процесс прекратится.
    int brk(void* addr) - увеличивает heap до указанного адреса
    void* sbrk(intptr_t increment) - увеличивает heap на указанное значение и возвращает указатель на начало этой области
  Если блок памяти больше MMAP_THRESHOLD (128 кБ по умолчанию), то используется mmap(2) с флагом MAP_ANONYMOUS
    mmap(2) - игнорирует RLIMIT_DATA - ограничение на память для процесса. Выделяет процессу новую разметку памяти
    void *mmap(void *addr, size_t length, int prot, int flags,
      int fd, off_t offset)
      void* addr указывает на начало области, где мы хотим выделить память. Воспринимается системой лишь как пожелание. 
        Если передать NULL, память будет выделяться по адресу на усмотрение системы
      int prot - определяет желаемы режим доступа к памяти (должен совпадать с ткущим). 
        Принимает одно или несколько (через побитовое |) из следующих значений:
        PROT_EXEC, PROT_READ, PROT_WRITE либо PROT_NONE (память может быть без доступа)
      int flags - побитовые флаги. Если передать флаг MAP_ANONYMOUS, то fd и offset будут игнорироваться, а область памяти выделится заполненная нулями.
      int fd, off_t offset - используются для отображения на память файла по полученному из open(2) дескриптору.
        При удачном выделении мы сможем обращаться к length байтам файла, начиная с позиции offset, но не дальше, чем конец файла (это желательно, но сам mmap не будет этого проверять)
*/

typedef struct ListElem_s {
	struct ListElem_s *prev, *next;
	void* value;
} ListE;

typedef struct ListStruct {
	int length;
	ListE *head, *last;
} List;

List* initList() {
	List *list = (List*) calloc(1, sizeof(List));
	list->head = (ListE*) calloc(1, sizeof(ListE));
	list->last = (ListE*) calloc(1, sizeof(ListE));
	list->head->prev = list->head;
	list->head->next = list->last;
	list->last->next = list->last;
	list->last->prev = list->head;
	list->length = 0;
	return list;
}

ListE* insertBetween(List *list, ListE *prevElem, ListE *newElem, ListE *nextElem) {
	newElem->prev = prevElem;
	newElem->next = nextElem;
	prevElem->next = newElem;
	nextElem->prev = newElem;
	list->length++;
	return newElem;
}


typedef ListE*(AddFunc)(List*, ListE*, void*);

ListE* addBefore(List *list, ListE *nextElem, void* value) {
	ListE *newElem = (ListE*)calloc(1, sizeof(ListE)),
			*prevElem = nextElem->prev;
	newElem->value = value;
	return insertBetween(list, prevElem, newElem, nextElem);
}

ListE* addAfter(List *list, ListE *prevElem, void* value) {
	ListE *newElem = (ListE*)calloc(1, sizeof(ListE)),
			*nextElem = prevElem->next;
	newElem->value = value;
	return insertBetween(list, prevElem, newElem, nextElem);
}

ListE* copyDataAndAddToList(void *data, size_t size, AddFunc add, List* list, ListE* relatedElem) {
	void *copiedData = malloc(size);
	memcpy(copiedData, data, size);
	return add(list, relatedElem, copiedData);
}

void* erase(List *list, ListE *elem) {
	ListE *nextElem = elem->next, *prevElem = elem->prev;
	void* value = elem->value;
	prevElem->next = nextElem;
	nextElem->prev = prevElem;
	list->length--;
	free(elem);
	return value;
}

void freeList(List *list) {
	ListE *elementForDeleting;
	ListE *elem = list->head->next;
	while (elem != list->last) {
		elementForDeleting = elem;
		elem = elem->next;
		free(elementForDeleting->value);
		free(elementForDeleting);
	}
	free(list->head);
	free(list->last);
	free(list);
}

/*
fgets(3) - char* fgets(char* s, int size, FILE* stream) - 
	читает из stream либо size - 1 символов, либо до конца файла или конца строки
	Возвращает NULL и записывает его в s, если возникла ошибка либо данные с потока закончились

	read(2) - ssize_t read(int fd, void* buf, size_t count) -
		пытается прочитать из дескриптора файла до count байтов и записать их в буфер по адресу buf
		возвращает количество считанных байтов
*/

#define BUFF_SZ 1025

int main(int argc, char** argv) {
	char* str = (char*)malloc(BUFF_SZ);
	List* list = initList();
	while (fgets(str, BUFF_SZ, stdin) != NULL) {
		if (str[0] == '.')
			break;
		copyDataAndAddToList(str, strlen(str)+1, addBefore, list, list->last);
	}
	ListE* elem = list->head->next;
	while (elem != list->last)
	{
		printf("%s", (char*)elem->value);
		elem = elem->next;
	}
	freeList(list);
	return 0;
}