#ifndef UTIL_H
#define UTIL_H
#include <sqlite3.h>
#include "list.h"
#include "data.h"
#include "dbutil.h"
#include "simple_string.h"

#define o(...) fprintf(cgiOut, __VA_ARGS__)
#ifdef DEBUG
#define d(...) {\
    char time[20];\
    FILE *fp = fopen("debug.log", "a");\
    set_date_string(time);\
    fprintf(fp, "[%s] ", time);\
    fprintf(fp, __VA_ARGS__);\
    fclose(fp);\
}
#else
#define d(...) {\
    FILE *fp = fopen("debug.log", "a");\
    fprintf(fp, __VA_ARGS__);\
    fclose(fp);\
}
#endif
#define die(msg)    {d("ERROR: %s(%d) %s %s\n", __FILE__, __LINE__, __FUNCTION__, msg); exit(1);}
#define h(str)      cgiHtmlEscape(str == NULL ? "" : str)
#define v(str)      cgiValueEscape(str == NULL ? "" : str)
void u(char*);
typedef struct _action {
    char* action_name;
    void (*action_func)(void);
    struct _action* next;
} Action;
void register_action_actions(char*, void func(void));
void free_action_actions();
void exec_action();
void hm(char*);
void hmail(char*);
char* get_filename_without_path(char*);
char* get_upload_filename(const int, char*);
int get_upload_size(const int);
char* get_upload_content_type(const int, char*);
ElementFile* get_upload_content(const int);
unsigned long url_encode(unsigned char*, unsigned char*, unsigned long);
void redirect(const char*, const char*);
void redirect_with_hook_messages(const char*, const char*, List*);
void* xalloc(size_t);
void xfree(void*);
void csv_field(char*);
void css_field(char*);
String* get_base_url(String*);
bool contains(char* const, const char*);
void set_cookie(char*, char*);
void clear_cookie(char*);
void get_cookie_string(char*, char*);
int base64_decode(const unsigned char*, unsigned char*);
void set_date_string(char*);
char* get_ext(char*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
