#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "alloc.h"
#include "util.h"
#include "simple_string.h"

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
void set_condition_values(Condition* c, int element_type_id, int condition_type, char* value, char* cookie_value, bool cookie_restore)
{
    c->element_type_id = element_type_id;
    c->condition_type = condition_type;
    strcpy(c->value, value);
    if (cookie_restore)
        strcpy(c->cookie_value, cookie_value);
}
char* get_condition_value(List* conditions, int element_type_id, int condition_type)
{
    Iterator* it;
    foreach (it, conditions) {
        Condition* c = it->element;
        if (c->element_type_id == element_type_id && c->condition_type == condition_type) {
            return get_condition_valid_value(c);
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
        if (valid_condition(c))
            size++;
    }
    return size;
}
bool valid_condition(Condition* c)
{
    return (strlen(c->value) || strlen(c->cookie_value)) ? true : false;
}
char* get_condition_valid_value(Condition* c)
{
    return strlen(c->value) ? c->value : c->cookie_value;
}
ProjectInfo* project_info_new()
{
    ProjectInfo* pi =  xalloc(sizeof(ProjectInfo));
    pi->name = string_new(0);
    return pi;
}
void project_info_free(ProjectInfo* pi)
{
    string_free(pi->name);
    xfree(pi);
}
Project* project_new()
{
    return xalloc(sizeof(Project));
}
void project_free(Project* p)
{
    xfree(p);
}
State* state_new()
{
    return xalloc(sizeof(State));
}
void state_free(State* s)
{
    xfree(s);
}
SearchResult* search_result_new()
{
    SearchResult* result =  xalloc(sizeof(SearchResult));
    list_alloc(result->messages, Message, message_new, message_free);
    list_alloc(result->states, State, state_new, state_free);
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

SettingFile* setting_file_new()
{
    return xalloc(sizeof(SettingFile));
}
void setting_file_free(SettingFile* file)
{
    if (file->content)
        xfree(file->content);
    xfree(file);
}
Element* element_new()
{
    return xalloc(sizeof(Element));
}
void element_free(Element* e)
{
    xfree(e);
}
ElementType* element_type_new()
{
    return xalloc(sizeof(ElementType));
}
void element_type_free(ElementType* e)
{
    xfree(e);
}

ElementFile* element_file_new()
{
    return xalloc(sizeof(ElementFile));
}
void element_file_free(ElementFile* ef)
{
    if (ef->content)
        xfree(ef->content);
    xfree(ef);
}
Message* message_new()
{
    Message* message = xalloc(sizeof(Message));
    list_alloc(message->elements, Element, element_new, element_free);
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
