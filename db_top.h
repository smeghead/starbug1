#ifndef DB_TOP_H
#define DB_TOP_H
#include <sqlite3.h>
#include "list.h"
#include "data.h"
#include "dbutil.h"

List* db_top_get_all_project_infos(Database*, List*);
int db_top_register_project_info(Database*, ProjectInfo*);
void db_top_update_project_info(Database*, ProjectInfo*);
void db_top_delete_project_info(Database*, ProjectInfo*);
char* db_top_get_project_db_name(char*, char*);
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
