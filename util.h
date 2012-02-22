#ifndef UTIL_H
#define UTIL_H
/* define utility functions. */
#include <sqlite3.h>
#include <libintl.h>
#include "list.h"
#include "data.h"
#include "dbutil.h"
#include "simple_string.h"

#define _(s) gettext(s)

#ifdef _WIN32
  #define PATH_SEPERATOR "\\"
#else
  #define PATH_SEPERATOR "/"
#endif

#define REG_ACTION(action) register_action_actions(#action, action ## _action)
#define o(...) fprintf(cgiOut, __VA_ARGS__)
#ifdef DEBUG
#define d(...) {\
    char time[20];\
    FILE *fp = fopen(get_log_filename(), "a");\
    if (fp) { \
    set_date_string(time);\
        fprintf(fp, "[%s] {%s/%s}\t", time, g_project_code, g_action_name);\
        fprintf(fp, __VA_ARGS__);\
        fclose(fp);\
    } \
}
#else
#define d(...) {\
    FILE *fp = fopen(get_log_filename(), "a");\
    if (fp) { \
        fprintf(fp, __VA_ARGS__);\
        fclose(fp);\
    } \
}
#endif
#define die(msg)    { \
    d("ERROR: %s(%d) %s %s\n", __FILE__, __LINE__, __FUNCTION__, msg);\
    print_error_page(__FILE__, __LINE__, msg);\
    exit(1);\
}
#define h(str)      cgi_escape(str == NULL ? "" : str)
#define hs(str)      cgi_escape(string_rawstr(str))
#define v(str)      cgi_escape(str == NULL ? "" : str)
#define vs(str)      cgi_escape(string_rawstr(str))
void u(char*);
#define us(str)      u(string_rawstr(str))

typedef enum {
    ACTION_TYPE_NONE,
    ACTION_TYPE_INDEX_TOP,
    ACTION_TYPE_INDEX,
    ACTION_TYPE_ADMIN_TOP,
    ACTION_TYPE_ADMIN
} ActionType;

typedef struct _action {
    char* action_name;
    void (*action_func)(void);
    struct _action* next;
} Action;
void print_error_page(char*, int, char*);
void register_action_actions(char*, void func(void));
void free_action_actions();
ActionType analysis_action();
void exec_action();
void hm(char*);
void hmail(char*);
char* get_filename_without_path(char*);
char* get_upload_filename(const int, char*);
int get_upload_size(const int);
char* get_upload_content_type(const int, char*);
ElementFile* get_upload_content(const int);
void redirect(const char*, const char*);
void redirect_with_hook_messages(const char*, const char*, List*);
void csv_field(char*);
void css_field(char*);
String* get_base_url(String*);
bool contains(char* const, const char*);
void set_cookie(char*, char*);
void clear_cookie(char*);
void get_cookie_string(char*, char*);
void set_timestamp_string(char*);
void set_date_string(char*);
char* get_ext(char*);
void page_not_found();
#define MIN(a,b) (((a)<(b))?(a):(b)) 
#define MAX(a,b) (((a)>(b))?(a):(b))
double get_microseconds();
char* get_script_dir(char*);
char* get_log_filename();


extern char g_project_code[DEFAULT_LENGTH];
extern char g_project_code_4_url[DEFAULT_LENGTH];
extern char g_action_name[DEFAULT_LENGTH];
extern char g_path_info[DEFAULT_LENGTH];
void cgi_escape(char*);
void print_field_help();
void set_locale(char*);
#ifdef _WIN32
struct tm* localtime_r(const time_t* timer, struct tm* result);
#define mkstemp(p) mktemp(p)
#endif
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
