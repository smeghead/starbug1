#ifndef UTIL_H
#define UTIL_H
#include <sqlite3.h>
#include "data.h"

#define o(...) fprintf(cgiOut, __VA_ARGS__)
#define d(...) fprintf(stderr, __VA_ARGS__)
#define die(msg)    {d("ERROR: %s:%d %s\n", __FILE__, __LINE__, msg); exit(1);}
#define h(str)      cgiHtmlEscape(str)
#define v(str)      cgiValueEscape(str)
void u(char*);
typedef struct _action {
    char* action_name;
    void (*action_func)(void);
    struct _action* next;
} action;
void register_action_actions(char*, void func(void));
void exec_action();
void hm(char*);
char* get_element_value(bt_element*, bt_element_type*);

#endif
