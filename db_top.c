#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "data.h"
#include "db_project.h"
#include "util.h"
#include "dbutil.h"
#include "simple_string.h"


extern const char* db_name;
extern sqlite3 *db;

List* db_top_get_all_project_infos(Database* db, List* project_infos)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select id, name from project_info where deleted = 0 order by sort";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))) {
        ProjectInfo* project_info = list_new_element(project_infos);

        project_info->id = sqlite3_column_int(stmt, 0);
        strncpy(project_info->name, (char*)sqlite3_column_text(stmt, 1), DEFAULT_LENGTH - 1);
        list_add(project_infos, project_info);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return project_infos;

ERROR_LABEL(db->handle)
}
ProjectInfo* db_top_get_project_info(Database* db, ProjectInfo* project_info, char* project_name)
{
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select id, name from project_info where deleted = 0 and name = ?";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, project_name, strlen(project_name), NULL);

    while (SQLITE_ROW == sqlite3_step(stmt)) {
        project_info->id = sqlite3_column_int(stmt, 0);
        strncpy(project_info->name, (char*)sqlite3_column_text(stmt, 1), DEFAULT_LENGTH - 1);
        break;
    }
    sqlite3_finalize(stmt);

    return project_info;

ERROR_LABEL(db->handle)
}
void db_top_update_project_info(List* project_infos)
{
    Iterator* it;
    sqlite3_stmt *stmt = NULL;


    foreach (it, project_infos) {
        ProjectInfo* pi = it->element;
        if (pi->id) {
        } else {
            /* 新規の場合は、ticketテーブルにレコードを挿入する。 */
/*             exec_query("insert into ticket(id, registerdate, closed) " */
/*                     "values (NULL, ?, 0)", */
/*                     COLUMN_TYPE_TEXT, registerdate, */
/*                     COLUMN_TYPE_END); */
        }
    }
    sqlite3_finalize(stmt);
    return;
}
char* db_top_get_project_db_name(char* project_name, char* buffer)
{
    Database* top_db_a = db_init("db/1.db");
    ProjectInfo* project_info = project_info_new();

    d("project_name: %s\n", project_name);
    project_info = db_top_get_project_info(top_db_a, project_info, project_name);
    if (!project_info->id) {
        die("ERROR: no such project found.");
    }
    sprintf(buffer, "db/%d.db", project_info->id);
    db_finish(top_db_a);
    return buffer;
}

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
