#ifndef DATA_H
#define DATA_H

#include <stdio.h>

#define VALUE_LENGTH 1048575
#define MODE_LENGTH 128
#define DEFAULT_LENGTH 1024
#define MAX_FILE_SIZE 512

typedef struct _project {
  char name[DEFAULT_LENGTH];
  char description[DEFAULT_LENGTH];
  char home_url[DEFAULT_LENGTH];
  char host_name[DEFAULT_LENGTH];
  char smtp_server[DEFAULT_LENGTH];
  int smtp_port;
  char notify_address[DEFAULT_LENGTH];
  char admin_address[DEFAULT_LENGTH];
} bt_project;

bt_project project;

typedef struct _element {
  int ticket_id;
  int reply_id;
  int element_type_id;
  char* str_val;
  int is_file;
  int list_item_id;
  struct _element* next;
} bt_element;

typedef struct _message {
  int id;
  int reply_id;
  char registerdate[20];
  bt_element* elements;
  struct _message* next;
} bt_message;

typedef struct _list_item {
  int id;
  int element_type_id;
  char name[256];
  int close;
  int sort;
  struct _list_item* next;
} bt_list_item;

typedef struct _element_type {
  int id;
  int type;
  int ticket_property;
  int reply_property;
  int required;
  char name[DEFAULT_LENGTH];
  char description[DEFAULT_LENGTH];
  int display_in_list;
  int sort;
  bt_list_item* list_item;
  struct _element_type* next;
} bt_element_type;

typedef struct _element_file {
    int id;
    int element_id;
    char name[DEFAULT_LENGTH];
    int size;
    char content_type[DEFAULT_LENGTH];
    char* blob;
} bt_element_file;

enum bt_ELEMTYPE {
  ELEM_TEXT,
  ELEM_TEXTAREA,
  ELEM_CHECKBOX,
  ELEM_RADIO,
  ELEM_LIST_COMBO,
  ELEM_LIST_SINGLE,
  ELEM_LIST_MULTI,
  ELEM_UPLOADFILE
};
enum bt_ELEMENT_ID {
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
  struct _condition* next;
} bt_condition;

typedef struct _state {
  int id;
  char name[DEFAULT_LENGTH];
  int count;
  struct _state* next;
} bt_state;

bt_element* bt_element_create();
void bt_element_destroy(bt_element*);

bt_message* bt_ticket_create();
void bt_ticket_destroy(bt_message*);
char* get_element_value_by_id(bt_element*, int);
char* get_element_value(bt_element*, bt_element_type*);
// int get_element_id(bt_element*, bt_element_type*);
int get_element_lid_by_id(bt_element*, int);

void* xalloc(size_t);


#endif
