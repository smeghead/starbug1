#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "data.h"
#include "db_project.h"
#include "alloc.h"
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
        ProjectInfo* pi = list_new_element(project_infos);

        pi->id = sqlite3_column_int(stmt, 0);
/*    d("db_top_get_all_project_infos 1 [%x]\n", pi->name);*/
        string_set(pi->name, (char*)sqlite3_column_text(stmt, 1));
        pi->sort = sqlite3_column_int(stmt, 2);
        pi->deleted = sqlite3_column_int(stmt, 3);
        list_add(project_infos, pi);
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
        string_set(project_info->name, (char*)sqlite3_column_text(stmt, 1));
        project_info->sort = sqlite3_column_int(stmt, 2);
        project_info->deleted = sqlite3_column_int(stmt, 3);
        break;
    }
    sqlite3_finalize(stmt);

    return project_info;

ERROR_LABEL(db->handle)
}
void db_top_update_project(Database* db, Project* project)
{
    exec_query(db, "update setting set value = ? where name = 'project_name'",
            COLUMN_TYPE_TEXT, string_rawstr(project->name),
            COLUMN_TYPE_END);
    exec_query(db, "update setting set value = ? where name = 'home_description'",
            COLUMN_TYPE_TEXT, string_rawstr(project->home_description),
            COLUMN_TYPE_END);
    exec_query(db, "update setting set value = ? where name = 'home_url'",
            COLUMN_TYPE_TEXT, string_rawstr(project->home_url),
            COLUMN_TYPE_END);
}
void db_top_update_project_infos(Database* db, List* project_infos)
{
    Iterator* it;

    foreach (it, project_infos) {
        ProjectInfo* pi = it->element;
        exec_query(db, "update project_info set name = ?, sort = ? , deleted = ? where id = ?",
                COLUMN_TYPE_TEXT, string_rawstr(pi->name),
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
            COLUMN_TYPE_TEXT, string_rawstr(project_info->name),
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
typedef struct {
    int id;
    int field_count;
} DbInfo;
List* db_top_search(Database* db, char* q, List* tickets)
{
    List* projects_a;
    Iterator* it;
    List* db_infos_a;
    sqlite3_stmt *stmt = NULL;
    /* プロジェクトの検索 */
    list_alloc(db_infos_a, DbInfo, NULL, NULL);
    list_alloc(projects_a, ProjectInfo, project_info_new, project_info_free);
    projects_a = db_top_get_all_project_infos(db, projects_a);
    foreach (it, projects_a) {
        char sql[DEFAULT_LENGTH];
        char db_name[DEFAULT_LENGTH];
        ProjectInfo* pi = it->element;
        if (pi->id == 1) continue; /* トップなので飛ばす。 */
        if (pi->deleted) continue; /* 削除されたサブプロジェクトなので飛ばす。 */
        d("%s\n", string_rawstr(pi->name));
        sprintf(sql, "attach ? as db%d", pi->id);
        sprintf(db_name, "db/%d.db", pi->id);
        exec_query(db, sql,
                COLUMN_TYPE_TEXT, db_name,
                COLUMN_TYPE_END);
        sprintf(sql, "select count(*) from db%d.element_type", pi->id);
        if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
        sqlite3_reset(stmt);

        if (SQLITE_ROW == sqlite3_step(stmt)) {
            DbInfo* db_info = list_new_element(db_infos_a);
            db_info->id = pi->id;
            db_info->field_count = sqlite3_column_int(stmt, 0);
            list_add(db_infos_a, db_info);
            break;
        }
        sqlite3_finalize(stmt);
    }
    /* 検索用sql作成 */

    list_free(db_infos_a);
    return tickets;

ERROR_LABEL(db->handle)
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
