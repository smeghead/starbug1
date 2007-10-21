#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sqlite3.h>
#include "data.h"
#include "util.h"
#include "dbutil.h"

extern const char* db_name;
extern sqlite3 *db;

bt_element_type* db_get_element_types(int all)
{
    bt_element_type* elements = NULL;
    bt_element_type* e;
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    if (all) {
        sql = "select id, type, ticket_property, reply_property, required, element_name, description, display_in_list, sort from element_type order by sort";
    } else {
        sql = "select id, type, ticket_property, reply_property, required, element_name, description, display_in_list, sort from element_type where display_in_list = 1 order by sort";
    }
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);

    e = NULL;
    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        d("e->id \n");
        if (e == NULL) {
            e = elements = (bt_element_type*)xalloc(sizeof(bt_element_type));
        } else {
            e->next = (bt_element_type*)xalloc(sizeof(bt_element_type));
            e = e->next;
        }
        e->id = sqlite3_column_int(stmt, 0);
        e->type = sqlite3_column_int(stmt, 1);
        e->ticket_property = sqlite3_column_int(stmt, 2);
        e->reply_property = sqlite3_column_int(stmt, 3);
        e->required = sqlite3_column_int(stmt, 4);
        strcpy(e->name, sqlite3_column_text(stmt, 5));
        strcpy(e->description, sqlite3_column_text(stmt, 6));
        e->display_in_list = sqlite3_column_int(stmt, 7);
        e->sort = sqlite3_column_int(stmt, 8);
        e->next = NULL;
    }
    d("sssss: \n");
    if (SQLITE_DONE != r)
        goto error;

    d("sssss: \n");
    // stmt を開放
    sqlite3_finalize(stmt);

    return elements;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_element_types.");
}
bt_element_type* db_get_element_type(int id)
{
    bt_element_type* e;
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    d("%d\n", id);
    sql = "select id, type, ticket_property, reply_property, required, element_name, description, display_in_list, sort "
        "from element_type "
        "where id = ? "
        "order by sort";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);

    e = NULL;
    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        e = (bt_element_type*)xalloc(sizeof(bt_element_type));
        e->id = sqlite3_column_int(stmt, 0);
        e->type = sqlite3_column_int(stmt, 1);
        e->ticket_property = sqlite3_column_int(stmt, 2);
        e->reply_property = sqlite3_column_int(stmt, 3);
        e->required = sqlite3_column_int(stmt, 4);
        strcpy(e->name, sqlite3_column_text(stmt, 5));
        strcpy(e->description, sqlite3_column_text(stmt, 6));
        e->display_in_list = sqlite3_column_int(stmt, 7);
        e->sort = sqlite3_column_int(stmt, 8);
        e->next = NULL;
        break;
    }

    // stmt を開放
    sqlite3_finalize(stmt);

    return e;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_element_type.");
}

bt_list_item* db_get_list_item(int element_type)
{
    bt_list_item* items = NULL;
    bt_list_item* i = NULL;
    int r;
    const char *sql = "select id, name, close, sort from list_item where element_type_id = ? order by sort";
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, element_type);

    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        if (i == NULL) {
            i = items = (bt_list_item*)xalloc(sizeof(bt_list_item));
        } else {
            i->next = (bt_list_item*)xalloc(sizeof(bt_list_item));
            i = i->next;
        }
        i->id = sqlite3_column_int(stmt, 0);
        strcpy(i->name, sqlite3_column_text(stmt, 1));
        i->close = sqlite3_column_int(stmt, 2);
        i->sort = sqlite3_column_int(stmt, 3);
        i->next = NULL;
    }
    if (SQLITE_DONE != r)
        goto error;

    // stmt を開放
    sqlite3_finalize(stmt);

    return items;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_list_item.");
}

void set_date_string(char* buf)
{
    struct timeval tv;
    struct tm *tp;

    gettimeofday(&tv, NULL);
    tp = localtime(&tv.tv_sec);
    sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d",
            tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
            tp->tm_hour, tp->tm_min, tp->tm_sec);
}
void db_register_ticket(bt_message* ticket)
{
    int ticket_id;
    char registerdate[20];
    bt_element* elements;

    set_date_string(registerdate);
    d("%s\n", registerdate);

    exec_query("insert into ticket(id, registerdate, closed, deleted) "
            "values (NULL, ?, 0, 0)",
            COLUMN_TYPE_TEXT, registerdate,
            COLUMN_TYPE_END);

    ticket_id = sqlite3_last_insert_rowid(db);

    d("pass2");
    elements = ticket->elements;
    for (; elements != NULL; elements = elements->next) {
        exec_query("insert into element(id, ticket_id, element_type_id, str_val) values (NULL, ?, ?, ?)",
                COLUMN_TYPE_INT, ticket_id,
                COLUMN_TYPE_INT, elements->element_type_id,
                COLUMN_TYPE_TEXT, elements->str_val,
                COLUMN_TYPE_END);
    }
}
void db_reply_ticket(bt_message* ticket)
{
    int reply_id, closed = 0;
    char registerdate[20];
    bt_element* elements;

    elements = ticket->elements;
    for (; elements != NULL; elements = elements->next) {
        int c = exec_query_scalar_int("select close from list_item "
                "inner join element_type on element_type.type = list_item.element_type_id "
                "where list_item.element_type_id = ? and list_item.name = ?",
                COLUMN_TYPE_INT, elements->element_type_id,
                COLUMN_TYPE_TEXT, elements->str_val,
                COLUMN_TYPE_END);
        if (c != INVALID_INT && c != 0) {
            closed = 1;
            break;
        }
    }
    set_date_string(registerdate);
    if (exec_query("update ticket set closed = ? where id = ?",
            COLUMN_TYPE_INT, closed,
            COLUMN_TYPE_INT, ticket->id,
            COLUMN_TYPE_END) != 1)
        die("no ticket to update?");

    exec_query("insert into reply (id, ticket_id, registerdate) values (NULL, ?, ?)",
            COLUMN_TYPE_INT, ticket->id,
            COLUMN_TYPE_TEXT, registerdate,
            COLUMN_TYPE_END);

    reply_id = sqlite3_last_insert_rowid(db);

    elements = ticket->elements;
    for (; elements != NULL; elements = elements->next) {
        exec_query("insert into element(id, ticket_id, reply_id, element_type_id, str_val)"
                "values (NULL, ?, ?, ?, ?)",
                COLUMN_TYPE_INT, ticket->id,
                COLUMN_TYPE_INT, reply_id,
                COLUMN_TYPE_INT, elements->element_type_id,
                COLUMN_TYPE_TEXT, elements->str_val,
                COLUMN_TYPE_END);
    }
}
void db_delete_ticket(bt_message* ticket)
{
    if (exec_query("update ticket set deleted = 1 where id = ?",
            COLUMN_TYPE_INT, ticket->id,
            COLUMN_TYPE_END) != 1)
        die("no ticket to update?");
}

char* get_search_sql_string(bt_condition* conditions, char* sql_string)
{
    bt_condition* cond;
    int i;
    strcpy(sql_string, 
            "select distinct "
            " t.id, t.registerdate "
            "from ticket as t "
            "inner join element as e on t.id = e.ticket_id ");
    for (i = 0, cond = conditions; cond != NULL; cond = cond->next, i++) {
        char val[DEFAULT_LENGTH];
        sprintf(val, " inner join element as e%d on t.id = e%d.ticket_id ", i + 1, i + 1);
        strcat(sql_string, val);
    }
    if (conditions != NULL) {
        strcat(sql_string, " where ");
        for (i = 0, cond = conditions; cond != NULL; cond = cond->next, i++) {
            char val[DEFAULT_LENGTH];
            if (i) strcat(sql_string, " and ");
            sprintf(val, " (e%d.element_type_id = ? and e%d.str_val like '%' || ? || '%') ", i + 1, i + 1);
            strcat(sql_string, val);
        }
    }
    strcat(sql_string, " order by registerdate desc ");
    d("sql: %s\n", sql_string);

    return sql_string;
}
bt_message* db_search_tickets(bt_condition* conditions)
{
    bt_message* incidnets = NULL;
    bt_message* i = NULL;
    int r, n;
    char buffer[VALUE_LENGTH];
    char *sql = get_search_sql_string(conditions, buffer);
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    for (n = 1; conditions != NULL; conditions = conditions->next) {
        sqlite3_bind_int(stmt, n++, conditions->element_type_id);
        sqlite3_bind_text(stmt, n++, conditions->value, strlen(conditions->value), NULL);
    }

    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* registerdate;
        if (i == NULL) {
            i = incidnets = (bt_message*)xalloc(sizeof(bt_message));
        } else {
            i->next = (bt_message*)xalloc(sizeof(bt_message));
            i = i->next;
        }
        i->id = sqlite3_column_int(stmt, 0);
        registerdate = sqlite3_column_text(stmt, 1);
        if (registerdate != NULL)
            strcpy(i->registerdate, registerdate);
        i->next = NULL;
    }
    if (SQLITE_DONE != r)
        goto error;

    // stmt を開放
    sqlite3_finalize(stmt);


    return incidnets;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_search_tickets.");
}
bt_element* db_get_last_elements_4_list(int ticket_id)
{
    int reply_id = 0;
    bt_element* elements = NULL;
    bt_element* e = NULL;
    const char *sql;
    sqlite3_stmt *stmt = NULL;
    int r;

    reply_id = exec_query_scalar_int("select max(id) from reply where ticket_id = ?",
            COLUMN_TYPE_INT, ticket_id,
            COLUMN_TYPE_END);
    if (reply_id == INVALID_INT)
        die("failed to reply_id.");

    d("reply_id: %d\n", reply_id);
    if (reply_id == 0)
        sql = "select element.id, element_type_id, str_val "
            "from element "
            "inner join element_type on element.element_type_id = element_type.id "
            "where ticket_id = ? and reply_id is null and element_type.display_in_list = 1";
    else
        sql = "select element.id, element_type_id, str_val "
            "from element "
            "inner join element_type on element.element_type_id = element_type.id "
            "where ticket_id = ? and reply_id = ? and element_type.display_in_list = 1";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);
    if (reply_id != -1)
        sqlite3_bind_int(stmt, 2, reply_id);

    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* str_val;
        if (e == NULL) {
            e = elements = (bt_element*)xalloc(sizeof(bt_element));
        } else {
            e->next = (bt_element*)xalloc(sizeof(bt_element));
            e = e->next;
        }
        e->id = sqlite3_column_int(stmt, 0);
        e->element_type_id = sqlite3_column_int(stmt, 1);
        str_val = sqlite3_column_text(stmt, 2);
        if (str_val != NULL) {
            e->str_val = (char*)xalloc(sizeof(char) * strlen(str_val) + 1);
            strcpy(e->str_val, str_val);
        }
        e->next = NULL;
    }
    if (SQLITE_DONE != r)
        goto error;

    // stmt を開放
    sqlite3_finalize(stmt);

    return elements;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_last_elements_4_list.");
    return NULL;
}
bt_element* db_get_last_elements(int ticket_id)
{
    int reply_id = 0;
    bt_element* elements = NULL;
    bt_element* e = NULL;
    const char *sql;
    sqlite3_stmt *stmt = NULL;
    int r;

    reply_id = exec_query_scalar_int("select max(id) from reply where ticket_id = ?",
            COLUMN_TYPE_INT, ticket_id,
            COLUMN_TYPE_END);
    if (reply_id == INVALID_INT)
        die("failed to reply_id.");

    d("reply_id: %d\n", reply_id);
    if (reply_id == 0)
        sql = "select element.id, element_type_id, str_val "
            "from element "
            "inner join element_type on element.element_type_id = element_type.id "
            "where ticket_id = ? and reply_id is null";
    else
        sql = "select element.id, element_type_id, str_val "
            "from element "
            "inner join element_type on element.element_type_id = element_type.id "
            "where ticket_id = ? and reply_id = ?";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);
    if (reply_id != -1)
        sqlite3_bind_int(stmt, 2, reply_id);

    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* str_val;
        if (e == NULL) {
            e = elements = (bt_element*)xalloc(sizeof(bt_element));
        } else {
            e->next = (bt_element*)xalloc(sizeof(bt_element));
            e = e->next;
        }
        e->id = sqlite3_column_int(stmt, 0);
        e->element_type_id = sqlite3_column_int(stmt, 1);
        str_val = sqlite3_column_text(stmt, 2);
        if (str_val != NULL) {
            e->str_val = (char*)xalloc(sizeof(char) * strlen(str_val) + 1);
            strcpy(e->str_val, str_val);
        }
        e->next = NULL;
    }
    if (SQLITE_DONE != r)
        goto error;

    // stmt を開放
    sqlite3_finalize(stmt);

    return elements;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_last_elements.");
    return NULL;
}
bt_element* db_get_elements(int ticket_id, int reply_id)
{
    bt_element* elements = NULL;
    bt_element* e = NULL;
    const char *sql;
    sqlite3_stmt *stmt = NULL;
    int r;

    d("inc: %d, reply %d\n", ticket_id, reply_id);
    if (reply_id == 0) {
        sql = "select e.id, e.element_type_id, e.str_val "
            "from element as e "
            "inner join element_type as et on et.id = e.element_type_id "
            "where e.ticket_id = ? and e.reply_id is null "
            "order by et.sort";
    } else {
        sql = "select e.id, e.element_type_id, e.str_val "
            "from element as e "
            "inner join element_type as et on et.id = e.element_type_id "
            "where e.reply_id = ? "
            "order by et.sort";
    }
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    if (reply_id == 0)
        sqlite3_bind_int(stmt, 1, ticket_id);
    else
        sqlite3_bind_int(stmt, 1, reply_id);

    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* str_val;
        if (e == NULL) {
            e = elements = (bt_element*)xalloc(sizeof(bt_element));
        } else {
            e->next = (bt_element*)xalloc(sizeof(bt_element));
            e = e->next;
        }
        e->id = sqlite3_column_int(stmt, 0);
        e->element_type_id = sqlite3_column_int(stmt, 1);
        str_val = sqlite3_column_text(stmt, 2);
        d("id: %d  val: %s\n", e->id, str_val);
        if (str_val != NULL) {
            e->str_val = (char*)xalloc(sizeof(char) * strlen(str_val) + 1);
            strcpy(e->str_val, str_val);
        }
        e->next = NULL;
    }
    if (SQLITE_DONE != r)
        goto error;

    // stmt を開放
    sqlite3_finalize(stmt);

    return elements;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_elements.");
}
bt_message* db_get_ticket(int ticket_id)
{
    bt_message* incidnet = NULL;
    int r;
    const char *sql = "select id, registerdate from ticket where id = ?";
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    // stmtのSQLを実行し、結果を一列づつ取得
    if (SQLITE_ROW != (r = sqlite3_step(stmt))) {
        goto error;
    } else {
        const unsigned char* registerdate;
        incidnet = (bt_message*)xalloc(sizeof(bt_message));
        incidnet->id = sqlite3_column_int(stmt, 0);
        registerdate = sqlite3_column_text(stmt, 1);
        if (registerdate != NULL)
            strcpy(incidnet->registerdate, registerdate);
        incidnet->next = NULL;
    }

    // stmt を開放
    sqlite3_finalize(stmt);
    return incidnet;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_ticket.");
}
bt_message* db_get_reply(int reply_id)
{
    bt_message* incidnet = NULL;
    int r;
    const char *sql = "select id, registerdate from reply where id = ?";
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, reply_id);

    // stmtのSQLを実行し、結果を一列づつ取得
    if (SQLITE_ROW != (r = sqlite3_step(stmt))) {
        goto error;
    } else {
        const unsigned char* sender;
        const unsigned char* registerdate;
        incidnet = (bt_message*)xalloc(sizeof(bt_message));
        incidnet->id = sqlite3_column_int(stmt, 0);
        registerdate = sqlite3_column_text(stmt, 1);
        if (registerdate != NULL)
            strcpy(incidnet->registerdate, registerdate);
        incidnet->next = NULL;
    }

    // stmt を開放
    sqlite3_finalize(stmt);
    return incidnet;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_reply.");
}
int* db_get_reply_ids(int ticket_id)
{
    int* reply_ids = NULL;
    int r, i = 0;
    const char *sql = "select count(id) from reply where ticket_id = ?";
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    if (SQLITE_ROW != (r = sqlite3_step(stmt))) {
        goto error;
    }
    reply_ids = (int*)xalloc(sizeof(int) * (sqlite3_column_int(stmt, 0) + 1));
    // stmt を開放
    sqlite3_finalize(stmt);

    sql = "select id from reply where ticket_id = ? order by id";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);
    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        reply_ids[i++] = sqlite3_column_int(stmt, 0);
    }
    reply_ids[i] = 0;
    if (SQLITE_DONE != r)
        goto error;

    // stmt を開放
    sqlite3_finalize(stmt);

    return reply_ids;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_reply_ids.");
}

bt_project* db_get_project()
{
    bt_project* project = NULL;
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select name, description, home_url, host_name, smtp_server, smtp_port, notify_address from project";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);

    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        project = (bt_project*)xalloc(sizeof(bt_project));
        strcpy(project->name, sqlite3_column_text(stmt, 0));
        strcpy(project->description, sqlite3_column_text(stmt, 1));
        strcpy(project->home_url, sqlite3_column_text(stmt, 2));
        strcpy(project->host_name, sqlite3_column_text(stmt, 3));
        strcpy(project->smtp_server, sqlite3_column_text(stmt, 4));
        project->smtp_port = sqlite3_column_int(stmt, 5);
        strcpy(project->notify_address, sqlite3_column_text(stmt, 6));
        break;
    }
    d("project->id \n");

    // stmt を開放
    sqlite3_finalize(stmt);

    d("project->name: %s \n", project->name);
    return project;
}
void db_update_project(bt_project* project)
{
    if (exec_query(
            "update project set "
            "name = ?, description = ?, home_url = ?, host_name = ?, smtp_server = ?, smtp_port = ?, notify_address = ?",
            COLUMN_TYPE_TEXT, project->name,
            COLUMN_TYPE_TEXT, project->description,
            COLUMN_TYPE_TEXT, project->home_url,
            COLUMN_TYPE_TEXT, project->host_name,
            COLUMN_TYPE_TEXT, project->smtp_server,
            COLUMN_TYPE_INT, project->smtp_port,
            COLUMN_TYPE_TEXT, project->notify_address,
            COLUMN_TYPE_END) != 1)
        die("no project to update? or too many?");
}
void db_update_element_type(bt_element_type* e_type)
{
    /* 基本項目の場合、ticket_propertyとreply_propertyは編集させない。 */
    switch (e_type->id) {
        case ELEM_ID_TITLE:
            e_type->ticket_property = 1;
            e_type->reply_property = 0;
            break;
        case ELEM_ID_SENDER:
            e_type->ticket_property = 0;
            e_type->reply_property = 0;
            break;
        case ELEM_ID_STATUS:
            e_type->ticket_property = 1;
            e_type->reply_property = 0;
            break;
    }
    if (exec_query(
            "update element_type set "
            "ticket_property = ?, reply_property = ?, required = ?, element_name = ?, description = ?, sort = ?, display_in_list = ? "
            "where id = ?",
            COLUMN_TYPE_INT, e_type->ticket_property,
            COLUMN_TYPE_INT, e_type->reply_property,
            COLUMN_TYPE_INT, e_type->required,
            COLUMN_TYPE_TEXT, e_type->name,
            COLUMN_TYPE_TEXT, e_type->description,
            COLUMN_TYPE_INT, e_type->sort,
            COLUMN_TYPE_INT, e_type->display_in_list,
            COLUMN_TYPE_INT, e_type->id,
            COLUMN_TYPE_END) != 1)
        die("no element_type to update?");
}
void db_update_list_item(bt_list_item* item)
{
    if (exec_query(
            "update list_item set "
            "name = ?, close = ?, sort = ? where id = ?",
            COLUMN_TYPE_TEXT, item->name,
            COLUMN_TYPE_INT, item->close,
            COLUMN_TYPE_INT, item->sort,
            COLUMN_TYPE_INT, item->id,
            COLUMN_TYPE_END) != 1)
        die("no list_item to update?");
}
void db_delete_list_item(int id)
{
    exec_query(
            "delete from list_item "
            "where id = ?",
            COLUMN_TYPE_INT, id,
            COLUMN_TYPE_END);
}
void db_register_list_item(bt_list_item* item)
{
    exec_query(
            "insert into list_item (id, element_type_id, name, close, sort)"
            "values (NULL, ?, ?, ?, ?)",
            COLUMN_TYPE_INT, item->element_type_id,
            COLUMN_TYPE_TEXT, item->name,
            COLUMN_TYPE_INT, item->close,
            COLUMN_TYPE_INT, item->sort,
            COLUMN_TYPE_END);
}
int db_register_element_type(bt_element_type* e_type)
{
    int ret;
    exec_query(
            "insert into element_type (id, type, ticket_property, reply_property, required, element_name, description, display_in_list, sort)"
            "values (NULL, ?, ?, ?, ?, ?, ?, ?, ?)",
            COLUMN_TYPE_INT, e_type->type,
            COLUMN_TYPE_INT, e_type->ticket_property,
            COLUMN_TYPE_INT, e_type->reply_property,
            COLUMN_TYPE_INT, e_type->required,
            COLUMN_TYPE_TEXT, e_type->name,
            COLUMN_TYPE_TEXT, e_type->description,
            COLUMN_TYPE_INT, e_type->display_in_list,
            COLUMN_TYPE_INT, e_type->sort,
            COLUMN_TYPE_END);

    ret = sqlite3_last_insert_rowid(db);
    return ret;
}
void db_delete_element_type(int id)
{
    exec_query(
            "delete from element_type where id = ?",
            COLUMN_TYPE_INT, id,
            COLUMN_TYPE_END);
}
bt_state* db_get_states()
{
    bt_state* states = NULL;
    bt_state* s = NULL;
    int r;
    char buffer[VALUE_LENGTH];
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;

    d("asdf: \n");
    sprintf(sql, 
            "select e.str_val as name, count(t.id) as count "
            "from ticket as t "
            "inner join element as e "
            " on t.id = e.ticket_id and "
            "  (e.reply_id = (select max(id) from reply where ticket_id = t.id)"
            "    or ((select count(id) from reply where ticket_id = t.id) = 0 and e.reply_id is null))"
            "inner join list_item as li on e.element_type_id = li.element_type_id and li.name = e.str_val "
            "where e.element_type_id = %d group by e.str_val order by li.sort", ELEM_ID_STATUS);
    d("sql: %s\n", sql);
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    // stmtの内部バッファを一旦クリア
    sqlite3_reset(stmt);

    d("aa: \n");
    // stmtのSQLを実行し、結果を一列づつ取得
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
    d("aa1: \n");
        if (s == NULL) {
            s = states = (bt_state*)xalloc(sizeof(bt_state));
        } else {
            s->next = (bt_state*)xalloc(sizeof(bt_state));
            s = s->next;
        }
        strcpy(s->name, sqlite3_column_text(stmt, 0));
        s->count = sqlite3_column_int(stmt, 1);
        s->next = NULL;
    d("aa2: \n");
    }
    if (SQLITE_DONE != r)
        goto error;

    d("aa3: \n");
    // stmt を開放
    sqlite3_finalize(stmt);

    return states;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_states.");
}
