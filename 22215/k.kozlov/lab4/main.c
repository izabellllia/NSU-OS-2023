#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"

/*
1. Что такое виртуальная память
2. Что такое диспетчер памяти?
3. Как работает malloc?
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