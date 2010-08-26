#include <stdlib.h>
#include "alloc.h"
#include "util.h"
#include "list.h"

/* alloc for Iterator's element. */
void* list_new_element(List* list)
{
    void* new_element; 
    if (list->construstor) {
        /* appointed constructor, use it. */
        new_element = list->construstor();
    } else {
        new_element = xalloc(list->element_size);
    }
    return new_element;
}
/* Iteratorの取得 */
Iterator* get_iterator(List* list)
{
/*     d("get_iterator\n"); */
    return list->head;
}
/* 要素を持っているか？ */
int iterator_has_value(Iterator* ite)
{
/*     d("iterator_has_value\n"); */
    return ite != NULL;
}
/* 次の要素を取得する */
void* iterator_next(Iterator* ite)
{
/*     d("iterator_next\n"); */
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
        if (it->element != NULL) {
            if (list->destructor) {
                /* コンストラクタが指定されている場合はそれを利用する */
                list->destructor(it->element);
            } else {
                xfree(it->element);
            }
        }
        it = it->next;
        xfree(old_it);
    }
    xfree(list);
}
