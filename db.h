#ifndef DB_H
#define DB_H
#include <sqlite3.h>
#include "list.h"
#include "data.h"

List* db_get_element_types_4_list(List*);
List* db_get_element_types_all(List*);
ElementType* db_get_element_type(int, ElementType*);
List* db_get_list_item(int, List*);
int db_register_ticket(Message*);
void db_delete_ticket(Message*);
SearchResult* db_search_tickets(List*, char*, Condition*, int, List* messages, SearchResult*);
SearchResult* db_search_tickets_4_report(List*, char*, Condition*, List*, SearchResult*);
SearchResult* db_get_tickets_by_status(char*, List*, SearchResult*);
List* db_get_elements(int, List*);
List* db_get_last_elements_4_list(int, List*);
List* db_get_last_elements(int, List*);
int* db_get_message_ids_a(int);
Project* db_get_project(Project*);
void db_update_project(Project*);
void db_update_element_type(ElementType*);
void db_update_list_item(ListItem*);
void db_delete_list_item(int);
void db_register_list_item(ListItem*);
int db_register_element_type(ElementType*);
void db_delete_element_type(int);
List* db_get_states_has_not_close(List*);
List* db_get_states(List*);
List* db_get_statictics_multi(List*, int);
List* db_get_statictics(List*, int);
ElementFile* db_get_element_file(int, ElementFile*);
List* db_get_newest_information(int, List*);
char* db_get_element_file_mime_type(int, int, char*);
int db_get_element_file_id(int, int);
void db_register_wiki(Wiki*);
Wiki* db_get_newest_wiki(char*, Wiki*);
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
