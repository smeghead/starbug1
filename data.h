#ifndef DATA_H
#define DATA_H
/* 使用するデータ型を定義する */

#include <stdio.h>
#include "list.h"
#include "simple_string.h"

#define VALUE_LENGTH 1048575
#define MODE_LENGTH 10
#define DEFAULT_LENGTH 1024
#define NUM_LENGTH 20
#define DATE_LENGTH 20
#define LIST_COUNT_PER_LIST_PAGE 15
#define LIST_COUNT_PER_SEARCH_PAGE 30

typedef enum _bool {
    false = 0,
    true
} bool;

typedef struct {
    int id;
    String* code;
    int sort;
    String* project_type;
    int deleted;
} ProjectInfo;
ProjectInfo* project_info_new();
void project_info_free(ProjectInfo*);

typedef struct {
    String* name;
    String* home_description;
    String* home_url;
    int upload_max_size;
    String* locale;
} Project;
Project* project_new();
void project_free(Project*);

typedef struct {
    int element_type_id;
    String* str_val;
    int is_file;
} Element;
Element* element_new();
void element_free(Element*);

typedef struct {
    String* name;
    String* file_name;
    int size;
    String* mime_type;
    char* content;
} SettingFile;
SettingFile* setting_file_new();
void setting_file_free(SettingFile*);

typedef struct {
    int id;
    List* elements;
} Message;
Message* message_new();
void message_free(Message*);

typedef struct {
    int id;
    int element_type_id;
    String* name;
    int close;
    int sort;
} ListItem;
ListItem* list_item_new();
void list_item_free(ListItem*);

typedef struct {
    int id;
    int type;
    int ticket_property;
    int reply_property;
    int required;
    String* name;
    String* description;
    int auto_add_item;
    String* default_value;
    int display_in_list;
    int sort;
} ElementType;
ElementType* element_type_new();
void element_type_free(ElementType*);
void element_type_copy(ElementType*, ElementType*);

typedef struct {
    int id;
    int element_type_id;
    String* name;
    int size;
    String* mime_type;
    char* content;
} ElementFile;
ElementFile* element_file_new();
void element_file_free(ElementFile*);

enum ELEM_TYPE {
    ELEM_TYPE_TEXT,
    ELEM_TYPE_TEXTAREA,
    ELEM_TYPE_CHECKBOX,
    ELEM_TYPE_LIST_SINGLE,
    ELEM_TYPE_LIST_MULTI,
    ELEM_TYPE_UPLOADFILE,
    ELEM_TYPE_DATE,
    ELEM_TYPE_LIST_SINGLE_RADIO,
    ELEM_TYPE_NUM
    /* DBの値と連動しているので、追加する場合は、後に追加する必要がある。*/
};
enum ELEM_ID {
    ELEM_ID_ID = -1,
    ELEM_ID_TITLE = 1,
    ELEM_ID_SENDER = 2,
    ELEM_ID_STATUS = 3,
    ELEM_ID_REGISTERDATE = -2,
    ELEM_ID_LASTREGISTERDATE = -3,
    ELEM_ID_ORG_SENDER = -4,
    ELEM_ID_LASTREGISTERDATE_PASSED = -5
};
#define BASIC_ELEMENT_MAX 3

typedef struct {
    int element_type_id;
    int condition_type;
    String* value;
    String* cookie_value;
} Condition;
enum CONDITION_TYPE {
    CONDITION_TYPE_NORMAL,
    CONDITION_TYPE_DATE_FROM,
    CONDITION_TYPE_DATE_TO,
    CONDITION_TYPE_DAYS
};
Condition* condition_new();
void condition_free(Condition*);
void set_condition_values(Condition*, int, int, char*, char*, bool);
char* get_condition_value(List*, int, int);
int valid_condition_size(List*);
bool valid_condition(Condition*);
char* get_condition_valid_value(Condition*);

typedef struct {
    int id;
    String* name;
    int count;
} State;
State* state_new();
void state_free(State*);

typedef struct {
    int hit_count;
    int page;
    List* messages;
    List* states;
    List* sums;
} SearchResult;
SearchResult* search_result_new();
void search_result_free(SearchResult*);

typedef struct {
    int id;
    String* name;
    char* content;
} Wiki;
Wiki* wiki_new();
void wiki_free(Wiki*);

char* get_element_value_by_id(List*, const int);
char* get_element_value(List*, ElementType*);
// int get_element_lid_by_id(List*, int);
void set_element_value(Element*, const char*);

typedef struct {
    int project_id;
    String* project_code;
    String* project_name;
    int id;
    String* title;
} Ticket;
Ticket* ticket_new();
void ticket_free(Ticket*);

typedef struct {
    int id;
    int field_count;
} DbInfo;

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
