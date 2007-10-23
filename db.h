#ifndef DB_H
#define DB_H
#include <sqlite3.h>
#include "data.h"

bt_element_type* db_get_element_types(int);
bt_element_type* db_get_element_type(int);
bt_list_item* db_get_list_item(int);
void db_register_ticket(bt_message*);
void db_reply_ticket(bt_message*);
void db_delete_ticket(bt_message*);
bt_message* db_search_tickets(bt_condition*);
bt_element* db_get_elements(int, int);
bt_element* db_get_last_elements_4_list(int);
bt_element* db_get_last_elements(int);
bt_message* db_get_ticket(int);
bt_message* db_get_reply(int);
int* db_get_reply_ids(int);
bt_project* db_get_project();
void db_update_project(bt_project*);
void db_update_element_type(bt_element_type*);
void db_update_list_item(bt_list_item*);
void db_delete_list_item(int);
void db_register_list_item(bt_list_item*);
int db_register_element_type(bt_element_type*);
void db_delete_element_type(int);
bt_state* db_get_states();
char* db_get_original_sender(int, char*);

#endif
