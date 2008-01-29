#include <stdlib.h>
#include "util.h"
#include "list.h"

/* Iteratorに格納する要素のalloc */
void* list_new_element(List* list)
{
    return xalloc(list->element_size);
}
/* Iteratorの取得 */
Iterator* get_iterator(List* list)
{
    return list->head;
}
/* 要素を持っているか？ */
int iterator_has_value(Iterator* ite)
{
    return ite != NULL;
}
/* 次の要素を取得する */
void* iterator_next(Iterator* ite)
{
    return ite->next;
}
/* Linked Listにオブジェクトを追加する */
void list_add(List* list, void* new_element)
{
    if (list->tail == NULL) {
        /* 最初の要素 */
        list->tail = list->head = xalloc(sizeof(Iterator));
    } else {
        Iterator* old_tail = list->tail;
        old_tail->next = xalloc(sizeof(Iterator));
        list->tail = old_tail->next;
    }
    list->size++;
    list->tail->element = new_element;
}
/* Linked Listを開放する */
void list_free(List* list)
{
    Iterator* it = get_iterator(list);
    while (1) {
        Iterator* old_it = it;
        if (it == NULL) break;
        if (it->element != NULL) xfree(it->element);
        it = it->next;
        xfree(old_it);
    }
}
