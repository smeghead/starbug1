#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include "list.h"

#define VALUE_LENGTH 1048575
#define MODE_LENGTH 10
#define DEFAULT_LENGTH 1024
#define MAX_FILE_SIZE 512
#define LIST_PER_PAGE 30

typedef struct _project {
  char name[DEFAULT_LENGTH];
  char description[DEFAULT_LENGTH];
  char home_url[DEFAULT_LENGTH];
  char smtp_server[DEFAULT_LENGTH];
  int smtp_port;
  char notify_address[DEFAULT_LENGTH];
  char admin_address[DEFAULT_LENGTH];
} Project;


typedef struct _element {
  int element_type_id;
  char* str_val;
  int is_file;
  int list_item_id;
  struct _element* next;
} Element;

typedef struct _message {
  int id;
  List* elements;
} Message;

typedef struct _list_item {
  int id;
  int element_type_id;
  char name[256];
  int close;
  int sort;
} ListItem;

typedef struct _element_type {
  int id;
  int type;
  int ticket_property;
  int reply_property;
  int required;
  char name[DEFAULT_LENGTH];
  char description[DEFAULT_LENGTH];
  int auto_add_item;
  char default_value[DEFAULT_LENGTH];
  int display_in_list;
  int sort;
} ElementType;

typedef struct _element_file {
    int id;
    int element_type_id;
    char name[DEFAULT_LENGTH];
    int size;
    char mime_type[DEFAULT_LENGTH];
    char* blob;
} ElementFile;

enum ELEM_TYPE {
  ELEM_TYPE_TEXT,
  ELEM_TYPE_TEXTAREA,
  ELEM_TYPE_CHECKBOX,
  ELEM_TYPE_LIST_SINGLE,
  ELEM_TYPE_LIST_MULTI,
  ELEM_TYPE_UPLOADFILE
};
enum ELEM_ID {
    ELEM_ID_ID = -1,
    ELEM_ID_TITLE = 1,
    ELEM_ID_SENDER = 2,
    ELEM_ID_STATUS = 3,
    ELEM_ID_REGISTERDATE = -2,
    ELEM_ID_LASTREGISTERDATE = -3,
    ELEM_ID_ORG_SENDER = -4
};
#define BASIC_ELEMENT_MAX 3

typedef struct _condition {
  int element_type_id;
  char value[DEFAULT_LENGTH];
} Condition;

typedef struct _search_result {
  int hit_count;
  int page;
  List* messages;
} SearchResult;

typedef struct _state {
  int id;
  char name[DEFAULT_LENGTH];
  int count;
} State;


char* get_element_value_by_id(List*, int);
char* get_element_value(List*, ElementType*);
int get_element_lid_by_id(List*, int);



#endif
/* vim: set ts=4 sw=4 sts=4 expandtab: */
