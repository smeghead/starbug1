#ifndef DB_PROJECT_H
#define DB_PROJECT_H
#include <sqlite3.h>
#include "list.h"
#include "data.h"
#include "dbutil.h"

List* db_get_element_types_4_list(Database*, DbInfo*, List*);
List* db_get_element_types_all(Database*, DbInfo*, List*);
ElementType* db_get_element_type(Database*, int, ElementType*);
List* db_get_list_item(Database*, const int, List*);
int db_get_list_item_id(Database*, const int, char*);
int db_register_ticket(Database*, Message*);
void db_delete_ticket(Database*, Message*);
SearchResult* db_search_tickets(Database*, List*, char*, Condition*, const int, SearchResult*);
SearchResult* db_search_tickets_4_report(Database*, List*, char*, Condition*, SearchResult*);
SearchResult* db_get_tickets_by_status(Database*, const char*, SearchResult*);
List* db_get_elements(Database*, const int, List*);
List* db_get_last_elements_4_list(Database*, const int, List*);
List* db_get_last_elements(Database*, const int, List*);
int* db_get_message_ids_a(Database*, const int);
Project* db_get_project(Database*, Project*);
void db_update_project(Database*, Project*);
void db_update_element_type(Database*, ElementType*);
void db_update_list_item(Database*, ListItem*);
void db_delete_list_item(Database*, const int);
void db_register_list_item(Database*, ListItem*);
int db_register_element_type(Database*, ElementType*);
void db_delete_element_type(Database*, const int);
List* db_get_states_has_not_close(Database*, List*);
List* db_get_states(Database*, List*);
List* db_get_statictics_multi(Database*, List*, const int);
List* db_get_statictics(Database*, List*, const int);
ElementFile* db_get_element_file(Database*, int, ElementFile*);
List* db_get_newest_information(Database*, const int, List*);
bool db_get_element_file_deleted(Database*, int);
char* db_get_element_file_mime_type(Database*, const int, const int, char*);
int db_get_element_file_id(Database*, const int, const int);
void db_delete_element_file(Database*, const int, const bool);
void db_register_wiki(Database*, Wiki*);
Wiki* db_get_newest_wiki(Database*, char*, Wiki*);
void db_setting_file_save(Database*, SettingFile*);
SettingFile* db_get_setting_file(Database*, char*, SettingFile*);
void db_update_top_image(Database*, SettingFile*);
List* db_get_burndownchart(Database*, List*);
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
