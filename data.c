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
int get_element_lid_by_id(List* elements, int id)
{
    Iterator* it;
    if (elements == NULL) return -1;
    foreach (it, elements) {
        Element* e = it->element;
        if (id == e->element_type_id)
            return e->list_item_id;
    }
    return -1;
}
/* vim: set ts=4 sw=4 sts=4 expandtab: */
