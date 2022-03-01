#ifndef LIST_H
#define LIST_H

typedef struct list {
    void** items;
    int count;
} list_t;

list_t* list_create();
void list_additem(list_t* list, void* item);

#endif