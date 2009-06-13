#ifndef LIST_H
#define LIST_H

/* Iterator */
typedef struct _iterator {
    void* element;
    struct _iterator* next;
} Iterator;
/* Linked List */
typedef struct _list {
    int element_size;
    int size;
    void* (*construstor)();
    void (*destructor)(void*);
    struct _iterator* head;
    struct _iterator* tail;
} List;

/* foreach */
#define foreach(it, list) for(it = get_iterator(list); iterator_has_value(it); it = iterator_next(it))

/* Linked Listの生成 */
#define list_alloc(list, type, construct_func, destory_func) \
    list = xalloc(sizeof(List)); \
    list->element_size = sizeof(type); \
    list->construstor = (void*)construct_func; \
    list->destructor = (void*)destory_func;

void* list_new_element(List*);
Iterator* get_iterator(List*);
int iterator_has_value(Iterator*);
void* iterator_next(Iterator*);
void list_add(List*, void*);
void list_free(List*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
