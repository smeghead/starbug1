#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "util.h"

bt_element* bt_element_create()
{
  bt_element* e;
  if ((e = (bt_element*)malloc(sizeof(bt_element))) == 0) {
    d("memory error.");
    die("memory error.");
  }
  e->ticket_id = -1;
  e->element_type_id = -1;
  e->str_val = (char*)xalloc(sizeof(char) * 1);
  strcpy(e->str_val, "");
  return e;
}

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
    bt_element* elems = elements;
    for (; elems != NULL; elems = elems->next) {
        if (e_type->id == elems->element_type_id)
            return elems->str_val;
    }
    return "";
}
char* get_element_value_by_id(bt_element* elements, int type)
{
    bt_element* elems = elements;
    for (; elems != NULL; elems = elems->next) {
        if (type == elems->element_type_id)
            return elems->str_val;
    }
    return "";
}
int get_element_lid_by_id(bt_element* elements, int id)
{
    bt_element* elems = elements;
    for (; elems != NULL; elems = elems->next) {
        if (id == elems->element_type_id)
            return elems->list_item_id;
    }
    return -1;
}
