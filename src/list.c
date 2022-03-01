#include <stdlib.h>
#include <assert.h>
#include "include/list.h"

list_t* list_create() {
    list_t* list = calloc(1, sizeof(list_t));
    assert(list != NULL);

    list->count = 0;
    return list;
}

void list_additem(list_t* list, void* item) {
    assert(list != NULL && item != NULL);

    list->items = realloc(list->items, sizeof(void**) * ++list->count);
    list->items[list->count - 1] = item;
}