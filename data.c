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
        char* str = string_rawstr(e->str_val);
        if (et->id == e->element_type_id && str != NULL)
            return str;
    }
    return "";
}

char* get_element_value_by_id(List* elements, const int type)
{
    Iterator* it;
    if (elements == NULL) return "";
    foreach (it, elements) {
        Element* e = it->element;
        char* str = string_rawstr(e->str_val);
        if (type == e->element_type_id && str != NULL)
            return str;
    }
    return "";
}

void set_element_value(Element* e, const char* val)
{
    string_set(e->str_val, val);
}

void set_condition_values(Condition* c, int element_type_id, int condition_type, char* value, char* cookie_value, bool cookie_restore)
{
    c->element_type_id = element_type_id;
    c->condition_type = condition_type;
    string_set(c->value, value);
    if (cookie_restore)
        string_set(c->cookie_value, cookie_value);
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
    return (string_len(c->value) || string_len(c->cookie_value)) ? true : false;
}

char* get_condition_valid_value(Condition* c)
{
    return string_len(c->value) ? string_rawstr(c->value) : string_rawstr(c->cookie_value);
}

ProjectInfo* project_info_new()
{
    ProjectInfo* pi =  xalloc(sizeof(ProjectInfo));
    pi->code = string_new();
    pi->project_type = string_new();
    return pi;
}

void project_info_free(ProjectInfo* pi)
{
    string_free(pi->code);
    string_free(pi->project_type);
    xfree(pi);
}

Project* project_new()
{
    Project* p = xalloc(sizeof(Project));
    p->name = string_new();
    p->home_description = string_new();
    p->home_url = string_new();
    p->locale = string_new();
    return p;
}

void project_free(Project* p)
{
    string_free(p->name);
    string_free(p->home_description);
    string_free(p->home_url);
    string_free(p->locale);
    xfree(p);
}

State* state_new()
{
    State* s = xalloc(sizeof(State));
    s->name = string_new();
    return s;
}

void state_free(State* s)
{
    string_free(s->name);
    xfree(s);
}

SearchResult* search_result_new()
{
    SearchResult* result =  xalloc(sizeof(SearchResult));
    list_alloc(result->messages, Message, message_new, message_free);
    list_alloc(result->states, State, state_new, state_free);
    list_alloc(result->sums, Element, element_new, element_free);
    return result;
}

void search_result_free(SearchResult* sr)
{
    if (sr->messages) {
        list_free(sr->messages);
    }
    if (sr->states) {
        list_free(sr->states);
    }
    if (sr->sums) {
        list_free(sr->sums);
    }
    xfree(sr);
}

ListItem* list_item_new()
{
    ListItem* li = xalloc(sizeof(ListItem));
    li->name = string_new();
    return li;
}

void list_item_free(ListItem* li)
{
    string_free(li->name);
    xfree(li);
}

SettingFile* setting_file_new()
{
    SettingFile* sf = xalloc(sizeof(SettingFile));
    sf->name = string_new();
    sf->file_name = string_new();
    sf->mime_type = string_new();
    return sf;
}

void setting_file_free(SettingFile* file)
{
    string_free(file->name);
    string_free(file->file_name);
    string_free(file->mime_type);
    if (file->content)
        xfree(file->content);
    xfree(file);
}

Element* element_new()
{
    Element* e = xalloc(sizeof(Element));
    e->str_val = string_new();
    return e;
}

void element_free(Element* e)
{
    string_free(e->str_val);
    xfree(e);
}

ElementType* element_type_new()
{
    ElementType* et = xalloc(sizeof(ElementType));
    et->name = string_new();
    et->description = string_new();
    et->default_value = string_new();
    return et;
}

void element_type_free(ElementType* e)
{
    string_free(e->name);
    string_free(e->description);
    string_free(e->default_value);
    xfree(e);
}

void element_type_copy(ElementType* src, ElementType* dest)
{
    dest->id = src->id;
    dest->type = src->type;
    dest->ticket_property = src->ticket_property;
    dest->reply_property = src->reply_property;
    dest->required = src->required;
    dest->name = string_new();
    string_set(dest->name, string_rawstr(src->name));
    dest->description = string_new();
    string_set(dest->description, string_rawstr(src->description));
    dest->auto_add_item = src->auto_add_item;
    dest->default_value = string_new();
    string_set(dest->default_value, string_rawstr(src->default_value));
    dest->display_in_list = src->display_in_list;
    dest->sort = src->sort;
}

ElementFile* element_file_new()
{
    ElementFile* ef = xalloc(sizeof(ElementFile));
    ef->name = string_new();
    ef->mime_type = string_new();
    return ef;
}

void element_file_free(ElementFile* ef)
{
    string_free(ef->name);
    string_free(ef->mime_type);
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
        list_free(m->elements);
    xfree(m);
}

Condition* condition_new()
{
    Condition* c = xalloc(sizeof(Condition));
    c->value = string_new();
    c->cookie_value = string_new();
    return c;
}

void condition_free(Condition* c)
{
    string_free(c->value);
    string_free(c->cookie_value);
    xfree(c);
}

Wiki* wiki_new()
{
/*     d("wiki size: %d\n", sizeof(Wiki)); */
    Wiki* w = xalloc(sizeof(Wiki));
    w->name = string_new();
    return w;
}

void wiki_free(Wiki* wiki)
{
    string_free(wiki->name);
    xfree(wiki->content);
    xfree(wiki);
}

Ticket* ticket_new()
{
    Ticket* t = xalloc(sizeof(Ticket));
    t->project_code = string_new();
    t->project_name = string_new();
    t->title = string_new();
    return t;
}

void ticket_free(Ticket* t)
{
    string_free(t->project_code);
    string_free(t->project_name);
    string_free(t->title);
    xfree(t);
}

BurndownChartPoint* burndown_chart_point_new()
{
    BurndownChartPoint* b = xalloc(sizeof(BurndownChartPoint));
    b->day = string_new();
    return b;
}

void burndown_chart_point_free(BurndownChartPoint* b)
{
    string_free(b->day);
    xfree(b);
}

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
