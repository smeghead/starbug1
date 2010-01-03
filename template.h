#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "simple_string.h"
#include "list.h"

typedef struct {
    String* name;
    String* path;
} Template;
Template* template_new();
void template_free(Template*);

List* get_templates(List*, String*);
void create_db_from_template(int, char*);
void save_template(char*, String*);
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
