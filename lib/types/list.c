#ifndef __LIST__

#include <stdlib.h>
#include <string.h>

#define __LIST__
#define EXPANSION_LENGTH 10

typedef struct _list* LIST;

struct _list;

LIST newList();
BOOL freeList(LIST list);
BOOL push(LIST list, void* item);
int length(LIST list);
void* get(LIST list, int index);


struct _list {
	void** items;
	int length;
	int reservedLength;
};

LIST newList() {
	LIST list = (LIST) malloc(sizeof(struct _list));
	list->items = (void**) malloc(sizeof(void*) * EXPANSION_LENGTH);
	list->length = 0;
	list->reservedLength = EXPANSION_LENGTH;
	return list;
}

BOOL freeList(LIST list) {
	if (list == NULL) {
		return FALSE;
	}
	for (int i = 0; i < list->length; i++) {
		free(list->items[i]);
	}
	free(list->items);
	free(list);
	return TRUE;
}

BOOL push(LIST list, void* item) {
	if (list == NULL || list->items == NULL) {
		return FALSE;
	}
	if (list->length >= list->reservedLength) {
		void** reallocItems = list->items;
		reallocItems = (void**) realloc(reallocItems, (list->length + EXPANSION_LENGTH) * sizeof(void*));
		if (reallocItems == NULL) {
			return FALSE;
		}
		list->items = reallocItems;
		list->reservedLength += EXPANSION_LENGTH;
	}
	list->items[list->length] = item;
	list->length++;
	return TRUE;
}

int length(LIST list) {
	return list == NULL ? -1: list->length;
}

void* get(LIST list, int index) {
	if (list == NULL || index >= list->length) {
		return NULL;
	}
	return list->items[index];
}

#endif