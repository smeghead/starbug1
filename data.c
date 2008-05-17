#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "util.h"

char* get_element_value(List* elements, ElementType* et)
{
    Iterator* it;
    if (elements == NULL) return "";
    foreach (it, elements) {
        Element* e = it->element;
        if (et->id == e->element_type_id && e->str_val != NULL)
            return e->str_val;
    }
    return "";
}
char* get_element_value_by_id(List* elements, const int type)
{
    Iterator* it;
    if (elements == NULL) return "";
    foreach (it, elements) {
        Element* e = it->element;
        if (type == e->element_type_id && e->str_val != NULL)
            return e->str_val;
    }
    return "";
}
void set_element_value(Element* e, const char* val)
{
    e->str_val = xalloc(sizeof(char) * strlen(val) + 1);
    strcpy(e->str_val, val);
}
void set_condition_values(Condition* c, int element_type_id, int condition_type, char* value)
{
    c->element_type_id = element_type_id;
    c->condition_type = condition_type;
    strcpy(c->value, value);
}
char* get_condition_value(List* conditions, int element_type_id, int condition_type)
{
    Iterator* it;
    foreach (it, conditions) {
        Condition* c = it->element;
        if (c->element_type_id == element_type_id && c->condition_type == condition_type) {
            return c->value;
        }
    }
    return "";
}
int valid_condition_size(List* conditions)
{
    int size = 0;
    Iterator* it;
    foreach (it, conditions) {
        Condition* c = it->element;
        if (strlen(c->value))
            size++;
    }
    return size;
}
Project* project_new()
{
    return xalloc(sizeof(Project));
}
void project_free(Project* p)
{
    xfree(p);
}
SearchResult* search_result_new()
{
    SearchResult* result =  xalloc(sizeof(SearchResult));
    list_alloc(result->messages, Message);
    list_alloc(result->states, State);
    return result;
}
void search_result_free(SearchResult* sr)
{
    if (sr->messages) {
        Iterator* it;
        foreach (it, sr->messages) {
            Message* m = it->element;
            if (m->elements)
                free_element_list(m->elements); /* リスト自体は、list_freeで開放するので、リスト要素だけ開放する。 */
        }
        list_free(sr->messages);
    }
    if (sr->states) {
        list_free(sr->states);
    }
    xfree(sr);
}
ListItem* list_item_new()
{
    return xalloc(sizeof(ListItem));
}
void list_item_free(ListItem* li)
{
    xfree(li);
}
ElementFile* element_file_new()
{
    return xalloc(sizeof(ElementFile));
}
void element_file_free(ElementFile* ef)
{
    if (ef->blob)
        xfree(ef->blob);
    xfree(ef);
}
Message* message_new()
{
    Message* message = xalloc(sizeof(Message));
    list_alloc(message->elements, Element);
    return message;
}
void message_free(Message* m)
{
    if (m->elements)
        free_element_list(m->elements);
    xfree(m);
}
Condition* condition_new()
{
    return xalloc(sizeof(Condition));
}
void condition_free(Condition* c)
{
    xfree(c);
}
void free_element_list(List* elements)
{
    Iterator* it;
    foreach (it, elements) {
        Element* e = it->element;
/*         d("str_val:%s\n", e->str_val); */
        if (e->str_val)
            xfree(e->str_val);
    }
    list_free(elements);
}
Wiki* wiki_new()
{
/*     d("wiki size: %d\n", sizeof(Wiki)); */
    return xalloc(sizeof(Wiki));
}
void wiki_free(Wiki* wiki)
{
    xfree(wiki->content);
    xfree(wiki);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
