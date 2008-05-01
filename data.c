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
char* get_element_value_by_id(List* elements, int type)
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
void set_element_value(Element* e, char* val)
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
    return xalloc(sizeof(SearchResult));
}
void search_result_free(SearchResult* sr)
{
    if (sr->messages)
        xfree(sr->messages);
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
    return xalloc(sizeof(Message));
}
void message_free(Message* m)
{
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
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
