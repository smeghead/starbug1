#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    sqlite3_reset(stmt);

    e = NULL;
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
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
    if (SQLITE_DONE != r)
        goto error;

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

    sql = "select id, type, ticket_property, reply_property, required, element_name, description, display_in_list, sort "
        "from element_type "
        "where id = ? "
        "order by sort";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);

    e = NULL;
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

    sqlite3_finalize(stmt);

    return e;
}

bt_list_item* db_get_list_item(int element_type)
{
    bt_list_item* items = NULL;
    bt_list_item* i = NULL;
    int r;
    const char *sql = "select id, name, close, sort from list_item where element_type_id = ? order by sort";
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, element_type);

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
void create_message_insert_sql(bt_element* elements, char* buf)
{
    bt_element* e;
    strcpy(buf, "insert into message(id, ticket_id, registerdate");
    for (e = elements; e != NULL; e = e->next) {
        char statement[16];
        sprintf(statement, ", field%d", e->element_type_id);
        strcat(buf, statement);
    }
    strcat(buf, ") values (NULL, ?, ?");
    for (e = elements; e != NULL; e = e->next) {
        strcat(buf, ", ?");
    }
    strcat(buf, ")");
}
int db_register_ticket(bt_message* ticket)
{
    char registerdate[20];
    bt_element* elements;
    bt_element* e;
    char sql[DEFAULT_LENGTH];
    int i, loop = 0, closed = 0;
    sqlite3_stmt *stmt = NULL;
    int message_id;
    int register_mode = ticket->id == -1;

    set_date_string(registerdate);

    if (register_mode) {
        /* 新規の場合は、ticketテーブルにレコードを挿入する。 */
        exec_query("insert into ticket(id, registerdate, closed) "
                "values (NULL, ?, 0)",
                COLUMN_TYPE_TEXT, registerdate,
                COLUMN_TYPE_END);

        ticket->id = sqlite3_last_insert_rowid(db);
    }
    /* クローズの状態に変更されたかどうかを判定する。 */
    elements = ticket->elements;
    for (; elements != NULL; elements = elements->next) {
        int c = exec_query_scalar_int("select close from list_item "
                "where list_item.element_type_id = ? and list_item.name = ?",
                COLUMN_TYPE_INT, elements->element_type_id,
                COLUMN_TYPE_TEXT, elements->str_val,
                COLUMN_TYPE_END);
        d("element_type_id:%d str_val %s\n", elements->element_type_id, elements->str_val);
        if (c != INVALID_INT && c != 0) {
            closed = 1;
            break;
        }
    }
    d("closed:%d\n", closed);
    set_date_string(registerdate);
    /* クローズ状態に変更されていた場合は、closedに1を設定する。 */
    if (exec_query("update ticket set closed = ? where id = ?",
            COLUMN_TYPE_INT, closed,
            COLUMN_TYPE_INT, ticket->id,
            COLUMN_TYPE_END) != 1)
        die("no ticket to update?");

    elements = ticket->elements;
    create_message_insert_sql(elements, sql);
    d("%s\n", sql);

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    i = 1;
    sqlite3_bind_int(stmt, i++, ticket->id);
    sqlite3_bind_text(stmt, i++, registerdate, strlen(registerdate), NULL);
    for (e = elements; e != NULL; e = e->next) {
        sqlite3_bind_text(stmt, i++, e->str_val, strlen(e->str_val), NULL);
    }
    while (SQLITE_DONE != sqlite3_step(stmt)){
        if (loop++ > 1000)
            goto error;
    }
    message_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    /* message_id を更新する。 */
    if (register_mode) {
        exec_query("update ticket set original_message_id = ?, last_message_id = ? "
                "where id = ?",
                COLUMN_TYPE_INT, message_id,
                COLUMN_TYPE_INT, message_id,
                COLUMN_TYPE_INT, ticket->id,
                COLUMN_TYPE_END);
    } else {
        exec_query("update ticket set last_message_id = ? "
                "where id = ?",
                COLUMN_TYPE_INT, message_id,
                COLUMN_TYPE_INT, ticket->id,
                COLUMN_TYPE_END);
    }

    elements = ticket->elements;
    /* 添付ファイルの登録 */
    for (; elements != NULL; elements = elements->next) {
        if (elements->is_file) {
            int size;
            char filename[DEFAULT_LENGTH];
            char content_type[DEFAULT_LENGTH];
            char* fname;
            char* ctype;
            bt_element_file* content;
            fname = get_upload_filename(elements->element_type_id, filename);
            size = get_upload_size(elements->element_type_id);
            ctype = get_upload_content_type(elements->element_type_id, content_type);
            content = get_upload_content(elements->element_type_id);
            d("fname: %s size: %d\n", fname, size);
            if (exec_query(
                        "insert into element_file("
                        " id, message_id, element_type_id, filename, size, content_type, content"
                        ") values (NULL, ?, ?, ?, ?, ?, ?) ",
                    COLUMN_TYPE_INT, message_id,
                    COLUMN_TYPE_INT, elements->element_type_id,
                    COLUMN_TYPE_TEXT, fname,
                    COLUMN_TYPE_INT, size,
                    COLUMN_TYPE_TEXT, content_type,
                    COLUMN_TYPE_BLOB, content,
                    COLUMN_TYPE_END) == 0)
                die("insert failed.");
        }
    }
    return ticket->id;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    die("failed to db_register_ticket.");
}
void db_delete_ticket(bt_message* ticket)
{
    if (exec_query("update ticket set deleted = 1 where id = ?",
            COLUMN_TYPE_INT, ticket->id,
            COLUMN_TYPE_END) != 1)
        die("no ticket to update?");
}

char* get_search_sql_string(bt_condition* conditions, bt_condition* sort, char* q, char* sql_string)
{
/*     bt_condition* cond; */
/*     int i; */
    strcpy(sql_string, 
            "select  "
            " t.id, t.registerdate, m.registerdate as last_registerdate "
            "from ticket as t "
            "inner join message as m on m.id = t.last_message_id ");



    if (!conditions)
        strcat(sql_string, "where t.closed = 0 ");










/*     if (strlen(q)) */
/*         strcat(sql_string, "inner join element as e on t.id = e.ticket_id "); */

/*             "inner join element as e_4_sender " */
/*             " on (t.id = e_4_sender.ticket_id " */
/*             "    and e_4_sender.reply_id is null) " */
/*     strcat(sql_string, "left join reply as r on t.id = r.ticket_id "); */
/*     for (i = 0, cond = conditions; cond != NULL; cond = cond->next, i++) { */
/*         char val[DEFAULT_LENGTH]; */
/*         int n = i + 1; */
/*         sprintf(val, */
/*                 " inner join element as e%d on t.id = e%d.ticket_id and " */
/*                 " (e%d.reply_id in (select max(id) from reply group by ticket_id)" */
/*                 " or " */
/*                 " (not exists (select id from reply where ticket_id = t.id) and e%d.reply_id is null)) ", */
/*                 n, n, n, n); */
/*         strcat(sql_string, val); */
/*     } */
/*     if (sort && sort->element_type_id > 0) { |+特殊項目の場合は、省く+| */
/*         char val[DEFAULT_LENGTH]; */
/*         int n = i + 1; */
/*         sprintf(val, */
/*                 " inner join element as e%d on t.id = e%d.ticket_id and e%d.element_type_id = %d and " */
/*                 " (e%d.reply_id in (select max(id) from reply group by ticket_id)" */
/*                 " or " */
/*                 " (not exists (select id from reply where ticket_id = t.id) and e%d.reply_id is null)) ", */
/*                 n, n, n, sort->element_type_id, n, n); */
/*         strcat(sql_string, val); */
/*     } */
/*     if (conditions) { */
/*         strcat(sql_string, " where "); */
/*         for (i = 0, cond = conditions; cond != NULL; cond = cond->next, i++) { */
/*             char val[DEFAULT_LENGTH]; */
/*             if (i) strcat(sql_string, " and "); */
/*             sprintf(val, " (e%d.element_type_id = ? and e%d.str_val like '%%' || ? || '%%') ", i + 1, i + 1); */
/*             strcat(sql_string, val); */
/*         } */
/*     } else { */
/*         strcat(sql_string, " where "); */
/*         if (!strlen(q)) { */
/*             strcat(sql_string, "  t.closed = 0 "); */
/*         } */
/*     } */
/*     if (strlen(q)) { */
/*         if (conditions) |+conditionsが指定れていれば、1つ以上条件が設定されているとみなす。綺麗じゃないので修正するつもり。 +| */
/*             strcat(sql_string, " and "); */
/*         strcat(sql_string, " e.str_val like '%' || ? || '%' "); */
/*     } */
/*     strcat(sql_string, " group by t.id, t.registerdate "); */
    strcat(sql_string, " order by ");
/*     if (sort) { */
/*         char column[DEFAULT_LENGTH]; */
/*         char sort_type[DEFAULT_LENGTH]; */
/*         sprintf(sort_type, "%s", strstr(sort->value, "reverse") ? "desc" : "asc"); */
/*         switch (sort->element_type_id) { */
/*             case -1: */
/*                 sprintf(column, "t.id %s, ", sort_type); */
/*                 break; */
/*             case -2: */
/*                 sprintf(column, "t.registerdate %s, ", sort_type); */
/*                 break; */
/*             case -3: */
/*                 sprintf(column, "last_registerdate %s, ", sort_type); */
/*                 break; */
/*             default: */
/*                 sprintf(column, "e%d.str_val %s, ", i + 1, sort_type); */
/*                 break; */
/*         } */
/*         strcat(sql_string, column); */
/*     } */
    strcat(sql_string, "t.registerdate desc ");
    d("sql: %s\n", sql_string);

    return sql_string;
}
bt_message* db_search_tickets(bt_condition* conditions, char* q, bt_condition* sorts)
{
    bt_message* tichets = NULL;
    bt_message* i = NULL;
    int r, n;
    char buffer[VALUE_LENGTH];
    char *sql = get_search_sql_string(conditions, sorts, q, buffer);
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    for (n = 1; conditions != NULL; conditions = conditions->next) {
        sqlite3_bind_int(stmt, n++, conditions->element_type_id);
        sqlite3_bind_text(stmt, n++, conditions->value, strlen(conditions->value), NULL);
    }
    if (q) {
        sqlite3_bind_text(stmt, n++, q, strlen(q), NULL);
    }

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* registerdate;
        if (i == NULL) {
            i = tichets = (bt_message*)xalloc(sizeof(bt_message));
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

    sqlite3_finalize(stmt);


    return tichets;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_search_tickets.");
}
void create_columns_exp(bt_element_type* element_types, char* table_name, char* buf)
{
    for (; element_types != NULL; element_types = element_types->next) {
        char column_name[DEFAULT_LENGTH];
        sprintf(column_name, ", %s.field%d ", table_name, element_types->id);
        strcat(buf, column_name);
    }
}
static void set_str_val(bt_element* e, const unsigned char* str_val)
{
    if (str_val != NULL) {
        e->str_val = (char*)xalloc(sizeof(char) * strlen(str_val) + 1);
        strcpy(e->str_val, str_val);
    }
}
bt_element* db_get_last_elements_4_list(int ticket_id)
{
    bt_element* elements = NULL;
    bt_element* e = NULL;
    char sql[DEFAULT_LENGTH] = "";
    char sql_suf[DEFAULT_LENGTH] = "";
    sqlite3_stmt *stmt = NULL;
    int r;
    char columns[DEFAULT_LENGTH] = "";
    bt_element_type* element_types;

    element_types = db_get_element_types(0);
    create_columns_exp(element_types, "last_m", columns);
    sprintf(sql, "select t.id, org_m.field%d, l.id ", ELEM_ID_SENDER);
    strcat(sql, columns);
    sprintf(sql_suf, 
            "  , t.registerdate, last_m.registerdate "
            "from ticket as t "
            "inner join message as last_m on last_m.id = t.last_message_id "
            "inner join message as org_m on org_m.id = t.original_message_id "
            "left join list_item as l on l.element_type_id = %d and l.name = last_m.field%d "
            "where t.id = ?", ELEM_ID_STATUS, ELEM_ID_STATUS);
    strcat(sql, sql_suf);
    d("sql:%s\n", sql);
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        int i = 0;
        const unsigned char* status_id;
        /* ID */
        e = elements = (bt_element*)xalloc(sizeof(bt_element));
        e->element_type_id = ELEM_ID_ID;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        /* 初回投稿者 */
        e = e->next = (bt_element*)xalloc(sizeof(bt_element));
        e->element_type_id = ELEM_ID_ORG_SENDER;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        /* status item_list id */
        status_id = sqlite3_column_text(stmt, i++);
        for (; element_types != NULL; element_types = element_types->next) {
            e = e->next = (bt_element*)xalloc(sizeof(bt_element));
            e->element_type_id = element_types->id;
            set_str_val(e, sqlite3_column_text(stmt, i++));
            if (element_types->id == ELEM_ID_STATUS && status_id != 0)
                e->list_item_id = atoi(status_id); /* 状態のスタイルシートのために、list_item.idを設定 */
        }
        /* 投稿日時 */
        e = e->next = (bt_element*)xalloc(sizeof(bt_element));
        e->element_type_id = ELEM_ID_REGISTERDATE;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        /* 最終更新日時 */
        e = e->next = (bt_element*)xalloc(sizeof(bt_element));
        e->element_type_id = ELEM_ID_LASTREGISTERDATE;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        e->next = NULL;
    }
    if (SQLITE_DONE != r)
        goto error;

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
    bt_element* elements = NULL;
    bt_element* e = NULL;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;
    int r;
    bt_element_type* element_types;
    char columns[DEFAULT_LENGTH] = "";

    element_types = db_get_element_types(1);
    create_columns_exp(element_types, "m", columns);
    strcpy(sql, "select t.id");
    strcat(sql, columns);
    strcat(sql,
            "from ticket as t "
            "inner join message as m on m.id = t.last_message_id "
            "where ticket_id = ?");
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* str_val;
        int i = 1;
        for (; element_types != NULL; element_types = element_types->next) {
            if (e == NULL) {
                e = elements = (bt_element*)xalloc(sizeof(bt_element));
            } else {
                e->next = (bt_element*)xalloc(sizeof(bt_element));
                e = e->next;
            }
/*             e->id = sqlite3_column_int(stmt, 0); */
            e->element_type_id = element_types->id;
            str_val = sqlite3_column_text(stmt, i++);
            if (str_val != NULL) {
                e->str_val = (char*)xalloc(sizeof(char) * strlen(str_val) + 1);
                strcpy(e->str_val, str_val);
            }
            e->next = NULL;
        }
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return elements;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_last_elements.");
    return NULL;
}
bt_element* db_get_elements(int message_id)
{
    bt_element* elements = NULL;
    bt_element* e = NULL;
    char sql[DEFAULT_LENGTH] = "";
    sqlite3_stmt *stmt = NULL;
    int r;
    char columns[DEFAULT_LENGTH] = "";
    bt_element_type* element_types;

    element_types = db_get_element_types(1);
    create_columns_exp(element_types, "m", columns);

    strcpy(sql, "select m.id");
    strcat(sql, columns);
    strcat(sql,
            "from message as m "
            "where m.id = ? ");
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, message_id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* str_val;
        int i = 1;
        for (; element_types != NULL; element_types = element_types->next) {
            if (e == NULL) {
                e = elements = (bt_element*)xalloc(sizeof(bt_element));
            } else {
                e->next = (bt_element*)xalloc(sizeof(bt_element));
                e = e->next;
            }
            e->element_type_id = element_types->id;
            str_val = sqlite3_column_text(stmt, i++);
            if (str_val != NULL) {
                e->str_val = (char*)xalloc(sizeof(char) * strlen(str_val) + 1);
                strcpy(e->str_val, str_val);
            }
            e->next = NULL;
        }
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return elements;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_elements.");
}
bt_message* db_get_ticket(int ticket_id)
{
    bt_message* ticket = NULL;
    int r;
    const char *sql = "select id, registerdate from ticket where id = ?";
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    if (SQLITE_ROW != (r = sqlite3_step(stmt))) {
        d("no ticket.\n");
        return NULL;
    } else {
        const unsigned char* registerdate;
        ticket = (bt_message*)xalloc(sizeof(bt_message));
        ticket->id = sqlite3_column_int(stmt, 0);
        registerdate = sqlite3_column_text(stmt, 1);
        if (registerdate != NULL)
            strcpy(ticket->registerdate, registerdate);
        ticket->next = NULL;
    }

    sqlite3_finalize(stmt);
    return ticket;
}
bt_message* db_get_message(int reply_id)
{
    bt_message* message = NULL;
    int r;
    const char *sql = "select id, registerdate from message where id = ?";
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, reply_id);

    if (SQLITE_ROW != (r = sqlite3_step(stmt))) {
        goto error;
    } else {
        const unsigned char* registerdate;
        message = (bt_message*)xalloc(sizeof(bt_message));
        message->id = sqlite3_column_int(stmt, 0);
        registerdate = sqlite3_column_text(stmt, 1);
        if (registerdate != NULL)
            strcpy(message->registerdate, registerdate);
        message->next = NULL;
    }

    sqlite3_finalize(stmt);
    return message;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_reply.");
}
int* db_get_message_ids(int ticket_id)
{
    int* message_ids = NULL;
    int r, i = 0;
    const char *sql = "select count(*) from message as m where m.ticket_id = ?";
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    if (SQLITE_ROW != (r = sqlite3_step(stmt))) {
        goto error;
    }
    message_ids = (int*)xalloc(sizeof(int) * (sqlite3_column_int(stmt, 0) + 1));
    sqlite3_finalize(stmt);

    sql = "select id from message as m where m.ticket_id = ? order by m.registerdate";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        message_ids[i++] = sqlite3_column_int(stmt, 0);
    }
    message_ids[i] = 0;
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return message_ids;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_message_ids.");
}

bt_project* db_get_project()
{
    bt_project* project = NULL;
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select name, description, home_url, host_name, smtp_server, smtp_port, notify_address, admin_address from project";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        project = (bt_project*)xalloc(sizeof(bt_project));
        strcpy(project->name, sqlite3_column_text(stmt, 0));
        strcpy(project->description, sqlite3_column_text(stmt, 1));
        strcpy(project->home_url, sqlite3_column_text(stmt, 2));
        strcpy(project->host_name, sqlite3_column_text(stmt, 3));
        strcpy(project->smtp_server, sqlite3_column_text(stmt, 4));
        project->smtp_port = sqlite3_column_int(stmt, 5);
        strcpy(project->notify_address, sqlite3_column_text(stmt, 6));
        strcpy(project->admin_address, sqlite3_column_text(stmt, 7));
        break;
    }

    sqlite3_finalize(stmt);

    return project;
}
void db_update_project(bt_project* project)
{
    if (exec_query(
            "update project set "
            "name = ?, description = ?, home_url = ?, host_name = ?, "
            "smtp_server = ?, smtp_port = ?, notify_address = ?, admin_address = ?",
            COLUMN_TYPE_TEXT, project->name,
            COLUMN_TYPE_TEXT, project->description,
            COLUMN_TYPE_TEXT, project->home_url,
            COLUMN_TYPE_TEXT, project->host_name,
            COLUMN_TYPE_TEXT, project->smtp_server,
            COLUMN_TYPE_INT, project->smtp_port,
            COLUMN_TYPE_TEXT, project->notify_address,
            COLUMN_TYPE_TEXT, project->admin_address,
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
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;

    sprintf(sql, 
            "select m.field%d as name, count(t.id) as count "
            "from ticket as t "
            "inner join message as m "
            " on m.id = t.last_message_id ", ELEM_ID_STATUS);
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        if (s == NULL) {
            s = states = (bt_state*)xalloc(sizeof(bt_state));
        } else {
            s->next = (bt_state*)xalloc(sizeof(bt_state));
            s = s->next;
        }
        strcpy(s->name, sqlite3_column_text(stmt, 0));
        s->count = sqlite3_column_int(stmt, 1);
        s->next = NULL;
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_states.");
}
bt_element_file* db_get_element_file(int element_id)
{
    bt_element_file* file;
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select id, element_id, filename, size, content_type, content "
        "from element_file "
        "where element_id = ? ";
    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, element_id);

    file = NULL;
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        int len;
        char* p_src;
        char* p_dist;
        file = (bt_element_file*)xalloc(sizeof(bt_element_file));
        file->id = sqlite3_column_int(stmt, 0);
        file->element_id = sqlite3_column_int(stmt, 1);
        strcpy(file->name, sqlite3_column_text(stmt, 2));
        file->size = sqlite3_column_int(stmt, 3);
        strcpy(file->content_type, sqlite3_column_text(stmt, 4));
        len = sqlite3_column_bytes(stmt, 5);
        p_dist = file->blob = (char*)xalloc(sizeof(char) * len);
        p_src = (char*)sqlite3_column_blob(stmt, 5);
        while (len--) {
            *p_dist = *p_src;
            p_dist++;
            p_src++;
        }
        break;
    }

    sqlite3_finalize(stmt);

    return file;
}
bt_message* db_get_newest_information(int limit)
{
    bt_message* tichets = NULL;
    bt_message* i = NULL;
    int r;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;
    
    sprintf(sql, 
            "select id, reply_id, registerdate "
            "from ( "
            "    select id as id, null as reply_id, registerdate from ticket "
            "    union "
            "    select ticket_id as id, id as reply_id, registerdate  from reply "
            " ) "
            "order by registerdate desc "
            "limit %d ", limit);

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* registerdate;
        if (i == NULL) {
            i = tichets = (bt_message*)xalloc(sizeof(bt_message));
        } else {
            i->next = (bt_message*)xalloc(sizeof(bt_message));
            i = i->next;
        }
        i->id = sqlite3_column_int(stmt, 0);
        i->reply_id = sqlite3_column_int(stmt, 1);
        registerdate = sqlite3_column_text(stmt, 2);
        if (registerdate != NULL)
            strcpy(i->registerdate, registerdate);
        i->next = NULL;
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);
    return tichets;

error:
    d("ERR: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    die("failed to db_get_newest_information.");
}
