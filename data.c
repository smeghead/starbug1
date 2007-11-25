#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "util.h"


void* xalloc(size_t sz)
{
    void* p;
    p = calloc(1, sz);
    if (!p) {
        d("memory error!!");
        die("memory error.");
    }
    return p;
}
char* get_element_value(bt_element* elements, bt_element_type* e_type)
{
    bt_element* e = elements;
    for (; e != NULL; e = e->next) {
        if (e_type->id == e->element_type_id && e->str_val != NULL)
            return e->str_val;
    }
    return "";
}
char* get_element_value_by_id(bt_element* elements, int type)
{
    bt_element* e = elements;
    for (; e != NULL; e = e->next) {
        if (type == e->element_type_id && e->str_val != NULL)
            return e->str_val;
    }
    return "";
}
int get_element_lid_by_id(bt_element* elements, int id)
{
    bt_element* e = elements;
    for (; e != NULL; e = e->next) {
        if (id == e->element_type_id)
            return e->list_item_id;
    }
    return -1;
}
