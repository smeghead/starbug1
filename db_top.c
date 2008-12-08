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

    sql = "select id, name, sort, deleted from project_info order by sort";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))) {
        ProjectInfo* project_info = list_new_element(project_infos);

        project_info->id = sqlite3_column_int(stmt, 0);
        strncpy(project_info->name, (char*)sqlite3_column_text(stmt, 1), DEFAULT_LENGTH - 1);
        project_info->sort = sqlite3_column_int(stmt, 2);
        project_info->deleted = sqlite3_column_int(stmt, 3);
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

    sql = "select id, name, sort, deleted from project_info where deleted = 0 and name = ?";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, project_name, strlen(project_name), NULL);

    while (SQLITE_ROW == sqlite3_step(stmt)) {
        project_info->id = sqlite3_column_int(stmt, 0);
        strncpy(project_info->name, (char*)sqlite3_column_text(stmt, 1), DEFAULT_LENGTH - 1);
        project_info->sort = sqlite3_column_int(stmt, 2);
        project_info->deleted = sqlite3_column_int(stmt, 3);
        break;
    }
    sqlite3_finalize(stmt);

    return project_info;

ERROR_LABEL(db->handle)
}
void db_top_update_project_infos(Database* db, List* project_infos)
{
    Iterator* it;

    foreach (it, project_infos) {
        ProjectInfo* pi = it->element;
        exec_query(db, "update project_info set name = ?, sort = ? , deleted = ? where id = ?",
                COLUMN_TYPE_TEXT, pi->name,
                COLUMN_TYPE_INT, pi->sort,
                COLUMN_TYPE_INT, pi->deleted,
                COLUMN_TYPE_INT, pi->id,
                COLUMN_TYPE_END);
    }
    return;
}
void db_top_register_project_info(Database* db, ProjectInfo* project_info)
{
    exec_query(db, "insert into project_info(id, name, sort) values (NULL, ?, ?)",
            COLUMN_TYPE_TEXT, project_info->name,
            COLUMN_TYPE_INT, project_info->sort,
            COLUMN_TYPE_END);
    return;
}
char* db_top_get_project_db_name(char* project_name, char* buffer)
{
    Database* top_db_a = db_init("db/1.db");
    ProjectInfo* project_info_a = project_info_new();

    d("project_name: %s\n", project_name);
    project_info_a = db_top_get_project_info(top_db_a, project_info_a, project_name);
    if (!project_info_a->id) {
        die("ERROR: no such project found.");
    }
    sprintf(buffer, "db/%d.db", project_info_a->id);
    project_info_free(project_info_a);
    db_finish(top_db_a);
    return buffer;
}

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */