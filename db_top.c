#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "data.h"
#include "db_top.h"
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
        string_set(pi->code, (char*)sqlite3_column_text(stmt, 1));
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
        string_set(project_info->code, (char*)sqlite3_column_text(stmt, 1));
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
    exec_query(db, "update setting set value = ? where name = 'locale'",
            COLUMN_TYPE_TEXT, string_rawstr(project->locale),
            COLUMN_TYPE_END);
}
void db_top_update_project_infos(Database* db, List* project_infos)
{
    Iterator* it;

    foreach (it, project_infos) {
        ProjectInfo* pi = it->element;
        exec_query(db, "update project_info set name = ?, sort = ? , deleted = ? where id = ?",
                COLUMN_TYPE_TEXT, string_rawstr(pi->code),
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
            COLUMN_TYPE_TEXT, string_rawstr(project_info->code),
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
String* get_search_sql_string_per_project(Database* db, DbInfo* db_info, List* keywords, String* sql_string)
{
    string_appendf(sql_string,
            "select "
            " %d as project_id, t.id as id "
            "from db%d.ticket as t \n", db_info->id, db_info->id);
    
    if (keywords->size > 0)
        string_appendf(sql_string, "inner join db%d.message as m_all on m_all.ticket_id = t.id \n", db_info->id);

    if (keywords->size > 0)
        string_append(sql_string, "where ");
    if (keywords->size > 0) {
        String* columns_a = string_new();
        List* element_types_a;
        list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
        element_types_a = db_get_element_types_all(db, db_info, element_types_a);
        columns_a = create_columns_like_exp(element_types_a, "m_all", keywords, columns_a);
        string_appendf(sql_string, "(%s)", string_rawstr(columns_a));
        string_free(columns_a);
        list_free(element_types_a);
    }

    string_append(sql_string, " group by t.id ");

    d("sql: %s\n", string_rawstr(sql_string));
    return sql_string;
}
void set_project_code_by_ticket(Ticket* t, List* projects)
{
    Iterator* it;
    foreach (it, projects) {
        ProjectInfo* pi = it->element;
        if (pi->id == t->project_id) {
            string_set(t->project_code, string_rawstr(pi->code));
            break;
        }
    }
}
List* db_top_search(Database* db, char* q, List* tickets)
{
    List* projects_a;
    Iterator* it;
    List* db_infos_a;
    List* sqls_a;
    String* sql_search_a = string_new();
    sqlite3_stmt *stmt = NULL;
    List* keywords_a;
    int i, r;
    int index = 1;

    list_alloc(keywords_a, String, string_new, string_free);
    keywords_a = parse_keywords(keywords_a, q);
    /* プロジェクトの検索 */
    list_alloc(db_infos_a, DbInfo, NULL, NULL);
    list_alloc(projects_a, ProjectInfo, project_info_new, project_info_free);
    list_alloc(sqls_a, String, string_new, string_free);
    projects_a = db_top_get_all_project_infos(db, projects_a);
    foreach (it, projects_a) {
        char sql[DEFAULT_LENGTH];
        char db_name[DEFAULT_LENGTH];
        ProjectInfo* pi = it->element;

        if (pi->id == 1) continue; /* トップなので飛ばす。 */
        if (pi->deleted) continue; /* 削除されたサブプロジェクトなので飛ばす。 */
        sprintf(sql, "attach ? as db%d", pi->id);
        d("%s\n", sql);
        sprintf(db_name, "db/%d.db", pi->id);
        exec_query(db, sql,
                COLUMN_TYPE_TEXT, db_name,
                COLUMN_TYPE_END);
        sprintf(sql, "select count(*) from db%d.element_type", pi->id);
        if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
        sqlite3_reset(stmt);

        if (SQLITE_ROW == sqlite3_step(stmt)) {
            DbInfo* db_info = list_new_element(db_infos_a);
            String* s = list_new_element(sqls_a);
            db_info->id = pi->id;
            db_info->field_count = sqlite3_column_int(stmt, 0);
            list_add(db_infos_a, db_info);
            /* 検索用sql作成 */
            s = get_search_sql_string_per_project(db, db_info, keywords_a, s);
            list_add(sqls_a, s);
        }
        sqlite3_finalize(stmt);
    }
    /* sqlを合成する。 */
    string_append(sql_search_a, "select project_id, id from (\n");
    foreach (it, sqls_a) {
        String* sql = it->element;
        string_append(sql_search_a, string_rawstr(sql));
        if (iterator_next(it))
            string_append(sql_search_a, "\n union \n");
    }
    string_append(sql_search_a, "\n) order by project_id, id\n");
    d("%s\n", string_rawstr(sql_search_a));
    if (sqlite3_prepare(db->handle, string_rawstr(sql_search_a), string_len(sql_search_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    foreach (it, db_infos_a) { /* DBの数だけ */
        DbInfo* db_info = it->element;
        Iterator* it_k;
        foreach (it_k, keywords_a) { /* キーワードの数だけ */
            String* k = it_k->element;
            for (i = 0; i < db_info->field_count; i++) { /* カラムの数だけ */
                sqlite3_bind_text(stmt, index++, string_rawstr(k), string_len(k), NULL);
                d("settext %d:%s\n", index - 1, string_rawstr(k));
            }
        }
    }

    while (SQLITE_ROW == (r = sqlite3_step(stmt))) {
        Ticket* t = list_new_element(tickets);
        /* 検索結果を受け取る */
        t->project_id = sqlite3_column_int(stmt, 0);
        t->id = sqlite3_column_int(stmt, 1);
        d("get %d:%d\n", t->project_id, t->id);
        list_add(tickets, t);
    }
    d("finish \n");
    if (SQLITE_DONE != r)
        goto error;
    sqlite3_finalize(stmt);

    list_free(keywords_a);
    list_free(sqls_a);
    string_free(sql_search_a);

    list_free(db_infos_a);
    /* チケットのタイトルとプロジェクト名を取得する */
    foreach (it, tickets) {
        Ticket* t = it->element;
        set_project_code_by_ticket(t, projects_a);
        t->project_name = db_top_get_project_name(db, t, t->project_name);
        t->title = db_top_get_title(db, t, t->title);

    }
    list_free(projects_a);
    return tickets;

ERROR_LABEL(db->handle)
}
String* db_top_get_project_name(Database* db, Ticket* t, String* project_name)
{
    String* sql_a = string_new();
    sqlite3_stmt *stmt = NULL;

    string_appendf(sql_a,
            "select value "
            "from db%d.setting "
            "where name = 'project_name'", t->project_id);
    if (sqlite3_prepare(db->handle, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    if (SQLITE_ROW == sqlite3_step(stmt)) {
        string_set(project_name, (char*)sqlite3_column_text(stmt, 0));
    }
    string_free(sql_a);

    sqlite3_finalize(stmt);

    return project_name;

ERROR_LABEL(db->handle)
}
String* db_top_get_title(Database* db, Ticket* t, String* title)
{
    String* sql_a = string_new();
    sqlite3_stmt *stmt = NULL;

    string_appendf(sql_a,
            "select field%d "
            "from db%d.message "
            "where ticket_id = ? order by registerdate desc limit 1", ELEM_ID_TITLE, t->project_id);
    if (sqlite3_prepare(db->handle, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, t->id);

    if (SQLITE_ROW == sqlite3_step(stmt)) {
        string_set(title, (char*)sqlite3_column_text(stmt, 0));
    }
    string_free(sql_a);

    sqlite3_finalize(stmt);

    return title;

ERROR_LABEL(db->handle)
}
void db_top_set_locale()
{
    Project* top_project_a = project_new();
    char buffer[DEFAULT_LENGTH];
    Database* db_a;

    db_a = db_init(db_top_get_project_db_name("top", buffer));
    top_project_a = db_get_project(db_a, top_project_a);
    db_finish(db_a);
    set_locale(string_rawstr(top_project_a->locale));

    project_free(top_project_a);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
