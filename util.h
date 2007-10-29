#ifndef UTIL_H
#define UTIL_H
#include <sqlite3.h>
#include "data.h"
#include "dbutil.h"

#define cgi_uri cgiScriptName
#define app_uri cgiScriptName

#define o(...) fprintf(cgiOut, __VA_ARGS__)
#define d(...) {FILE *fp = fopen("debug.log", "a");fprintf(fp, __VA_ARGS__);fclose(fp);}
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
void hmail(char*);
char* get_element_value(bt_element*, bt_element_type*);
char* get_filename_without_path(char*);
char* get_upload_filename(int, char*);
int get_upload_size(int);
char* get_upload_content_type(int, char*);
bt_element_file* get_upload_content(int);
unsigned long url_encode(unsigned char*, unsigned char*, unsigned long);
void redirect(char*, char*);

#endif
