#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include "data.h"
#include "db_project.h"
#include "alloc.h"
#include "util.h"
#include "dbutil.h"
#include "simple_string.h"


void create_columns_exp(List*, char*, char*);

static List* db_get_element_types(Database* db, DbInfo* db_info, bool all, List* element_types)
{
    int r;
    String* sql_a = string_new();
    sqlite3_stmt *stmt = NULL;
    char db_id[10] = "";

    if (db_info) {
        sprintf(db_id, "db%d.", db_info->id);
    }
    if (all) {
        string_appendf(sql_a,
                "select id, type, ticket_property, reply_property, required, name, "
                "  description, display_in_list, sort, default_value, auto_add_item "
                "from %selement_type "
                "where deleted = 0 order by sort", db_id);
    } else {
        string_appendf(sql_a,
                "select id, type, ticket_property, reply_property, required, name, "
                "  description, display_in_list, sort, default_value, auto_add_item "
                "from %selement_type "
                "where deleted = 0 and display_in_list = 1 order by sort", db_id);
    }
    if (sqlite3_prepare(db->handle, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        unsigned const char* value;
        ElementType* et = list_new_element(element_types);

        et->id = sqlite3_column_int(stmt, 0);
        et->type = sqlite3_column_int(stmt, 1);
        et->ticket_property = sqlite3_column_int(stmt, 2);
        et->reply_property = sqlite3_column_int(stmt, 3);
        et->required = sqlite3_column_int(stmt, 4);
        string_set(et->name, (char*)sqlite3_column_text(stmt, 5));
        string_set(et->description, (char*)sqlite3_column_text(stmt, 6));
        et->display_in_list = sqlite3_column_int(stmt, 7);
        et->sort = sqlite3_column_int(stmt, 8);
        value = sqlite3_column_text(stmt, 9);
        if (value != NULL) {
            string_set(et->default_value, (char*)value);
        }
        et->auto_add_item = sqlite3_column_int(stmt, 10);
        list_add(element_types, et);
    }
    if (SQLITE_DONE != r)
        goto error;

    string_free(sql_a);
    sqlite3_finalize(stmt);

    return element_types;

ERROR_LABEL(db->handle)
}
/*
 * 引数のdb_infoは、index_top.cから呼び出すようになったために追加した引数。
 * その他から呼び出す場合は、NULLを指定する。
 */
List* db_get_element_types_4_list(Database* db, DbInfo* db_info, List* elements)
{
    return db_get_element_types(db, db_info, false, elements);
}
/*
 * 引数のdb_infoは、index_top.cから呼び出すようになったために追加した引数。
 * その他から呼び出す場合は、NULLを指定する。
 */
List* db_get_element_types_all(Database* db, DbInfo* db_info, List* elements)
{
    return db_get_element_types(db, db_info, true, elements);
}
static List* db_get_num_element_types(Database* db, DbInfo* db_info, List* elements)
{
    List* es_a;
    Iterator* it;
    list_alloc(es_a, ElementType, element_type_new, element_type_free);
    es_a = db_get_element_types(db, db_info, false, es_a);
    foreach (it, es_a) {
        ElementType* et = it->element;
        if (et->type == ELEM_TYPE_NUM) {
            ElementType* et_copy = element_type_new();
            element_type_copy(et, et_copy);
            list_add(elements, et_copy);
        }
    }
    list_free(es_a);
    return elements;
}
ElementType* db_get_element_type(Database* db, int id, ElementType* e)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort "
        "from element_type "
        "where id = ? "
        " and deleted = 0 "
        "order by sort";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* value;
        e->id = sqlite3_column_int(stmt, 0);
        e->type = sqlite3_column_int(stmt, 1);
        e->ticket_property = sqlite3_column_int(stmt, 2);
        e->reply_property = sqlite3_column_int(stmt, 3);
        e->required = sqlite3_column_int(stmt, 4);
        string_set(e->name, (char*)sqlite3_column_text(stmt, 5));
        string_set(e->description, (char*)sqlite3_column_text(stmt, 6));
        e->auto_add_item = sqlite3_column_int(stmt, 7);
        value = sqlite3_column_text(stmt, 8);
        if (value != NULL)
            string_set(e->default_value, (char*)value);
        e->display_in_list = sqlite3_column_int(stmt, 9);
        e->sort = sqlite3_column_int(stmt, 10);
        break;
    }

    sqlite3_finalize(stmt);

    return e;
ERROR_LABEL(db->handle)
}

List* db_get_list_item(Database* db, const int element_type, List* items)
{
    int r;
    const char *sql = "select id, name, close, sort from list_item where element_type_id = ? order by sort";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, element_type);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        ListItem* item = list_new_element(items);
        item->id = sqlite3_column_int(stmt, 0);
        string_set(item->name, (char*)sqlite3_column_text(stmt, 1));
        item->close = sqlite3_column_int(stmt, 2);
        item->sort = sqlite3_column_int(stmt, 3);
        list_add(items, item);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return items;

ERROR_LABEL(db->handle)
}

int db_get_list_item_id(Database* db, const int element_type, char* name)
{
    int r, id = 0;
    const char *sql = "select id from list_item where element_type_id = ? and name = ?";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, element_type);
    sqlite3_bind_text(stmt, 2, name, strlen(name), NULL);

    if (SQLITE_ROW == (r = sqlite3_step(stmt))) {
        id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return id;

ERROR_LABEL(db->handle)
}
void create_message_insert_sql(List* elements, char* buf)
{
    Iterator* it;
    strcpy(buf, "insert into message(id, ticket_id, registerdate");
    foreach (it, elements) {
        Element* e = it->element;
        char statement[16];
        sprintf(statement, ", field%d", e->element_type_id);
        strcat(buf, statement);
    }
    strcat(buf, ") values (NULL, ?, ?");
    foreach (it, elements) {
        strcat(buf, ", ?");
    }
    strcat(buf, ")");
}
int db_register_ticket(Database* db, Message* ticket)
{
    char registerdate[20];
    List* elements;
    Iterator* it;
    char sql[DEFAULT_LENGTH];
    int i;
    bool closed = false;
    sqlite3_stmt *stmt = NULL;
    int message_id;
    int register_mode = ticket->id == -1;

    set_date_string(registerdate);

    d("1\n");
    if (register_mode) {
        /* 新規の場合は、ticketテーブルにレコードを挿入する。 */
        exec_query(
                db, 
                "insert into ticket(id, registerdate, closed) "
                "values (NULL, ?, 0)",
                COLUMN_TYPE_TEXT, registerdate,
                COLUMN_TYPE_END);

        ticket->id = sqlite3_last_insert_rowid(db->handle);
    }
    d("2\n");
    /* クローズの状態に変更されたかどうかを判定する。 */
    elements = ticket->elements;
    foreach (it, elements) {
        Element* e = it->element;
        int c = exec_query_scalar_int(
                db,
                "select close from list_item "
                "where list_item.element_type_id = ? and list_item.name = ?",
                COLUMN_TYPE_INT, e->element_type_id,
                COLUMN_TYPE_TEXT, string_rawstr(e->str_val),
                COLUMN_TYPE_END);
        if (c != INVALID_INT && c != 0) {
            closed = true;
            break;
        }
    }
    d("3\n");
    /* クローズ状態に変更されていた場合は、closedに1を設定する。 */
    if (exec_query(
                db,
                "update ticket set closed = ? where id = ?",
                COLUMN_TYPE_INT, closed,
                COLUMN_TYPE_INT, ticket->id,
                COLUMN_TYPE_END) != 1)
        die("no ticket to update?");

    elements = ticket->elements;
    create_message_insert_sql(elements, sql);

    d("4\n");
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    i = 1;
    sqlite3_bind_int(stmt, i++, ticket->id);
    sqlite3_bind_text(stmt, i++, registerdate, strlen(registerdate), NULL);
    foreach (it, elements) {
        Element* e = it->element;
        sqlite3_bind_text(stmt, i++, string_rawstr(e->str_val), string_len(e->str_val), NULL);
    }
    if (exec_and_wait_4_done(stmt) != SQLITE_RETURN_OK) goto error;
    message_id = sqlite3_last_insert_rowid(db->handle);
    sqlite3_finalize(stmt);
    /* message_id を更新する。 */
    d("5\n");
    if (register_mode) {
        exec_query(
                db,
                "update ticket set original_message_id = ?, last_message_id = ? "
                "where id = ?",
                COLUMN_TYPE_INT, message_id,
                COLUMN_TYPE_INT, message_id,
                COLUMN_TYPE_INT, ticket->id,
                COLUMN_TYPE_END);
    } else {
        exec_query(
                db,
                "update ticket set last_message_id = ? "
                "where id = ?",
                COLUMN_TYPE_INT, message_id,
                COLUMN_TYPE_INT, ticket->id,
                COLUMN_TYPE_END);
    }
    d("6\n");

    elements = ticket->elements;
    /* 添付ファイルの登録 */
    foreach (it, elements) {
        Element* e = it->element;
        if (e->is_file) {
            int size;
            char filename[DEFAULT_LENGTH];
            char mime_type[DEFAULT_LENGTH];
            char* fname;
            char* ctype;
            ElementFile* content_a;
            fname = get_upload_filename(e->element_type_id, filename);
            size = get_upload_size(e->element_type_id);
            ctype = get_upload_content_type(e->element_type_id, mime_type);
            content_a = get_upload_content(e->element_type_id);
            if (exec_query(
                        db,
                        "insert into element_file("
                        " id, message_id, element_type_id, filename, size, mime_type, content"
                        ") values (NULL, ?, ?, ?, ?, ?, ?) ",
                        COLUMN_TYPE_INT, message_id,
                        COLUMN_TYPE_INT, e->element_type_id,
                        COLUMN_TYPE_TEXT, fname,
                        COLUMN_TYPE_INT, size,
                        COLUMN_TYPE_TEXT, mime_type,
                        COLUMN_TYPE_BLOB_ELEMENT_FILE, content_a,
                        COLUMN_TYPE_END) == 0)
                die("insert failed.");
            element_file_free(content_a);
        }
    }
    d("7\n");
    return ticket->id;

ERROR_LABEL(db->handle)
}

static String* get_search_sql_string(Database* db, List* conditions, Condition* sort, List* keywords, String* sql_string)
{
    int i = 0;
    Iterator* it;
    List* element_types_a = NULL;

    list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
    element_types_a = db_get_element_types(db, NULL, false, element_types_a);
    string_appendf(sql_string,
            "select "
            " t.id as id, m.field%d as state, ", ELEM_ID_STATUS);
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        string_appendf(sql_string,
                " m.field%d as field%d ", et->id, et->id);
        if (iterator_next(it)) string_append(sql_string, ",");
    }
    list_free(element_types_a);
    string_append(sql_string,
            "from ticket as t "
            "inner join message as m on m.id = t.last_message_id "
            "inner join message as org_m on org_m.id = t.original_message_id ");
    d("sql: %s\n", string_rawstr(sql_string));
    
    if (keywords->size > 0)
        string_append(sql_string, "inner join message as m_all on m_all.ticket_id = t.id ");

    if (valid_condition_size(conditions) || keywords->size > 0)
        string_append(sql_string, "where ");
    if (valid_condition_size(conditions)) {
        foreach (it, conditions) {
            Condition* cond = it->element;
            char val[DEFAULT_LENGTH];
            if (cond->element_type_id < 0) continue;
            if (!valid_condition(cond)) continue;
            if (i++) string_append(sql_string, " and ");
            switch (cond->condition_type) {
                case CONDITION_TYPE_DATE_FROM:
                    sprintf(val, " (length(m.field%d) > 0 and m.field%d >= ?) ", cond->element_type_id, cond->element_type_id);
                    break;
                case CONDITION_TYPE_DATE_TO:
                    sprintf(val, " (length(m.field%d) > 0 and m.field%d <= ?) ", cond->element_type_id, cond->element_type_id);
                    break;
                default:
                    sprintf(val, " (%sm.field%d like '%%' || ? || '%%') ", 
                            cond->element_type_id == ELEM_ID_SENDER ? "org_" : "", /* 投稿者は初回投稿者が検索対象になる。 */
                            cond->element_type_id);
                    break;
            }
            string_append(sql_string, val);
        }
        foreach (it, conditions) {
            Condition* cond = it->element;
            char name[DEFAULT_LENGTH];
            char val[DEFAULT_LENGTH];
            if (cond->element_type_id > 0) continue;
            if (!valid_condition(cond)) continue;
            if (i++) string_append(sql_string, " and ");
            switch (cond->element_type_id) {
                case ELEM_ID_REGISTERDATE:
                    sprintf(name, "org_m.registerdate");
                    break;
                case ELEM_ID_LASTREGISTERDATE:
                    sprintf(name, "m.registerdate");
                    break;
            }
            switch (cond->condition_type) {
                case CONDITION_TYPE_DATE_FROM:
                    sprintf(val, " (%s >= ?) ", name);
                    break;
                case CONDITION_TYPE_DATE_TO:
                    sprintf(val, " (%s <= ?) ", name);
                    break;
                case CONDITION_TYPE_DAYS:
                    sprintf(val, " (%s >= datetime(current_timestamp, 'utc', '-%s days', 'localtime')) ",
                            name,
                            get_condition_valid_value(cond));
                    break;
            }
            string_append(sql_string, val);
        }
    }
    if (keywords->size > 0) {
        String* columns_a = string_new(0);
        List* element_types_a;
        list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
        element_types_a = db_get_element_types_all(db, NULL, element_types_a);
        columns_a = create_columns_like_exp(element_types_a, "m_all", keywords, columns_a);
        if (valid_condition_size(conditions))
            string_append(sql_string, " and ");
        string_appendf(sql_string, "(%s)", string_rawstr(columns_a));
        string_free(columns_a);
        list_free(element_types_a);
    }

    string_append(sql_string, " group by t.id ");
    string_append(sql_string, " order by ");
    if (sort != NULL && sort->element_type_id != 0) {
        char column[DEFAULT_LENGTH];
        char sort_type[DEFAULT_LENGTH];
        sprintf(sort_type, "%s", strstr(string_rawstr(sort->value), "reverse") ? "desc" : "asc");
        switch (sort->element_type_id) {
            case -1:
                sprintf(column, "t.id %s, ", sort_type);
                break;
            case -2:
                sprintf(column, "t.registerdate %s, ", sort_type);
                break;
            case -3:
                sprintf(column, "m.registerdate %s, ", sort_type);
                break;
            case ELEM_ID_SENDER:
                sprintf(column, "org_m.field%d %s, ", sort->element_type_id, sort_type);
                break;
            default:
                sprintf(column, "m.field%d %s, ", sort->element_type_id, sort_type);
                break;
        }
        string_append(sql_string, column);
    }
    string_append(sql_string, "t.registerdate desc, t.id desc ");

    return sql_string;
}
int set_conditions(Database* db, sqlite3_stmt* stmt, List* conditions, List* keywords)
{
    int n = 1;
    Iterator* it;
    foreach (it, conditions) {
        Condition* cond = it->element;
        char* v;
        if (!valid_condition(cond)) continue;
        if (cond->condition_type == CONDITION_TYPE_DAYS) continue; /* プレースフォルダが無いためスルーする */
        v = get_condition_valid_value(cond);
        sqlite3_bind_text(stmt, n++, v, strlen(v), NULL);
    }
    if (keywords->size > 0) {
        List* element_types_a;
        Iterator* it_keyword;
        Iterator* it;
        list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
        element_types_a = db_get_element_types_all(db, NULL, element_types_a);
        foreach (it_keyword, keywords) {
            String* word = it_keyword->element;
            foreach (it, element_types_a) {
                sqlite3_bind_text(stmt, n++, string_rawstr(word), string_len(word), NULL);
            }
        }
        list_free(element_types_a);
    }
    return n;
}
static void set_tickets_number_sum(Database* db, List* conditions, Condition* sorts, List* keywords, SearchResult* result)
{
    sqlite3_stmt* stmt = NULL;
    String* s_a = string_new();
    List* element_types_a = NULL;
    list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
    element_types_a = db_get_num_element_types(db, NULL, element_types_a);
    if (element_types_a->size) {
        int n, r;
        string_append(s_a, "select ");
        /* 数値項目の合計値を取得するためのカラムリストを付加する。 */
        Iterator* it;
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            string_appendf(s_a,
                    " sum(field%d) as sum%d ", et->id, et->id);
            if (iterator_next(it)) string_append(s_a, ",");
        }
        string_append(s_a, " from (");
        s_a = get_search_sql_string(db, conditions, sorts, keywords, s_a);
        string_append(s_a,
                ") as res ");
        d("sql : %s\n", string_rawstr(s_a));
        sqlite3_finalize(stmt);
        if (sqlite3_prepare(db->handle, string_rawstr(s_a), string_len(s_a), &stmt, NULL) == SQLITE_ERROR) goto error;
        sqlite3_reset(stmt);
        n = set_conditions(db, stmt, conditions, keywords);
        if (SQLITE_ROW == (r = sqlite3_step(stmt))) {
            foreach (it, element_types_a) {
                ElementType* et = it->element;
                Element* e = list_new_element(result->sums);
                e->element_type_id = et->id;
                string_appendf(e->str_val, "%f", sqlite3_column_double(stmt, 0));
                list_add(result->sums, e);
            }
        }
    }
    list_free(element_types_a);
    string_free(s_a);
    sqlite3_finalize(stmt);
    return;

ERROR_LABEL(db->handle)
}
SearchResult* db_get_tickets_by_status(Database* db, const char* status, SearchResult* result)
{
    int r, n, hit_count = 0;
    String* sql_a = string_new();
    List* conditions;
    Condition* cond_status;
    sqlite3_stmt *stmt = NULL;
    List* keywords_a;

    list_alloc(keywords_a, char, NULL, NULL);
    list_alloc(conditions, Condition, condition_new, condition_free);
    cond_status = list_new_element(conditions);
    cond_status->element_type_id = ELEM_ID_STATUS;
    string_set(cond_status->value, status);
    list_add(conditions, cond_status);

    sql_a = get_search_sql_string(db, conditions, NULL, keywords_a, sql_a);
    string_append(sql_a, " limit ? ");
    d("sql_a: %s\n", string_rawstr(sql_a));

    if (sqlite3_prepare(db->handle, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    n = set_conditions(db, stmt, conditions, keywords_a);
    sqlite3_bind_int(stmt, n++, LIST_COUNT_PER_LIST_PAGE);

    /* hitした分のticket_idを取得する。 */
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        Message* message = list_new_element(result->messages);
        message->id = sqlite3_column_int(stmt, 0);
        list_add(result->messages, message);
        hit_count++;
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);
    /* 数値項目の合計値を取得する。 */
    set_tickets_number_sum(db, conditions, NULL, keywords_a, result);
    string_free(sql_a);
    list_free(keywords_a);

    list_free(conditions);
    result->hit_count = hit_count;
    return result;
ERROR_LABEL(db->handle)
}
SearchResult* db_search_tickets(Database* db, List* conditions, char* q, Condition* sorts, const int page, SearchResult* result)
{
    int r, n;
    String* sql_a = string_new();
    sqlite3_stmt *stmt = NULL;
    List* keywords_a;

    list_alloc(keywords_a, String, string_new, string_free);
    keywords_a = parse_keywords(keywords_a, q);
    sql_a = get_search_sql_string(db, conditions, sorts, keywords_a, sql_a);

    string_append(sql_a, " limit ? offset ? ");
    if (sqlite3_prepare(db->handle, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    n = set_conditions(db, stmt, conditions, keywords_a);
    sqlite3_bind_int(stmt, n++, LIST_COUNT_PER_SEARCH_PAGE);
    sqlite3_bind_int(stmt, n++, page * LIST_COUNT_PER_SEARCH_PAGE);

    /* 1ページ分のticket_idを取得する。 */
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        Message* message = list_new_element(result->messages);
        message->id = sqlite3_column_int(stmt, 0);
        list_add(result->messages, message);
    }
    if (SQLITE_DONE != r)
        goto error;

    string_free(sql_a);
    sqlite3_finalize(stmt);
    /* hit件数を取得する。 */
    {
        String* s_a = string_new();
        string_append(s_a, "select count(res.id), res.state from (");
        s_a = get_search_sql_string(db, conditions, sorts, keywords_a, s_a);
        string_appendf(s_a,
                ") as res "
                "inner join list_item as l on l.element_type_id = %d and l.name = res.state "
                "group by res.state "
                "order by l.sort", ELEM_ID_STATUS);
        if (sqlite3_prepare(db->handle, string_rawstr(s_a), string_len(s_a), &stmt, NULL) == SQLITE_ERROR) goto error;
        sqlite3_reset(stmt);
        n = set_conditions(db, stmt, conditions, keywords_a);
        while (SQLITE_ROW == (r = sqlite3_step(stmt))) {
            State* s = list_new_element(result->states);
            result->hit_count += s->count = sqlite3_column_int(stmt, 0);
            string_set(s->name, (char*)sqlite3_column_text(stmt, 1));
            list_add(result->states, s);
        }
        string_free(s_a);
        if (SQLITE_DONE != r)
            goto error;
    }
    sqlite3_finalize(stmt);
    /* 数値項目の合計値を取得する。 */
    set_tickets_number_sum(db, conditions, sorts, keywords_a, result);

    list_free(keywords_a);

    result->page = page;
    return result;
ERROR_LABEL(db->handle)
}

SearchResult* db_search_tickets_4_report(Database* db, List* conditions, char* q, Condition* sorts, SearchResult* result)
{
    int r, n;
    String* sql_a = string_new();
    sqlite3_stmt *stmt = NULL;
    List* keywords_a;

    list_alloc(keywords_a, String, string_new, string_free);
    keywords_a = parse_keywords(keywords_a, q);
    sql_a = get_search_sql_string(db, conditions, sorts, keywords_a, sql_a);

    if (sqlite3_prepare(db->handle, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    n = set_conditions(db, stmt, conditions, keywords_a);

    /* ticket_idを取得する。 */
    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        Message* message = list_new_element(result->messages);
        message->id = sqlite3_column_int(stmt, 0);
        list_add(result->messages, message);
        result->hit_count++;
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);
    string_free(sql_a);
    list_free(keywords_a);

    return result;
ERROR_LABEL(db->handle)
}
void create_columns_exp(List* element_types, char* table_name, char* buf)
{
    Iterator* it;
    foreach (it, element_types) {
        ElementType* et = it->element;
        char column_name[DEFAULT_LENGTH];
        sprintf(column_name, ", %s.field%d ", table_name, et->id);
        strcat(buf, column_name);
    }
}
static void set_str_val(Element* e, const unsigned char* str_val)
{
    if (str_val != NULL) {
        string_set(e->str_val, (const char*)str_val);
    }
}
List* db_get_last_elements_4_list(Database* db, const int ticket_id, List* elements)
{
    char sql[DEFAULT_LENGTH] = "";
    char sql_suf[DEFAULT_LENGTH] = "";
    sqlite3_stmt *stmt = NULL;
    int r;
    char columns[DEFAULT_LENGTH] = "";
    List* element_types_a = NULL;
    Iterator* it;

    list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
    element_types_a = db_get_element_types_4_list(db, NULL, element_types_a);
    create_columns_exp(element_types_a, "last_m", columns);
    sprintf(sql, "select t.id, org_m.field%d ", ELEM_ID_SENDER);
    strcat(sql, columns);
    sprintf(sql_suf, 
            "  , substr(t.registerdate, 1, 16), substr(last_m.registerdate, 1, 16),  "
            "  julianday(current_date) - julianday(date(last_m.registerdate)) as passed_date "
            "from ticket as t "
            "inner join message as last_m on last_m.id = t.last_message_id "
            "inner join message as org_m on org_m.id = t.original_message_id "
            "where t.id = ?");
    strcat(sql, sql_suf);
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))) {
        int i = 0;
        Element* e;
        /* ID */
        e = list_new_element(elements);
        e->element_type_id = ELEM_ID_ID;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
        /* 初回投稿者 */
        e = list_new_element(elements);
        e->element_type_id = ELEM_ID_ORG_SENDER;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            e = list_new_element(elements);
            e->element_type_id = et->id;
            set_str_val(e, sqlite3_column_text(stmt, i++));
            list_add(elements, e);
        }
        /* 投稿日時 */
        e = list_new_element(elements);
        e->element_type_id = ELEM_ID_REGISTERDATE;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
        /* 最終更新日時 */
        e = list_new_element(elements);
        e->element_type_id = ELEM_ID_LASTREGISTERDATE;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
        /* 最終更新日時からの経過日数 */
        e = list_new_element(elements);
        e->element_type_id = ELEM_ID_LASTREGISTERDATE_PASSED;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);
    list_free(element_types_a);

    return elements;
ERROR_LABEL(db->handle)
}
List* db_get_last_elements(Database* db, const int ticket_id, List* elements)
{
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;
    int r;
    List* element_types_a = NULL;
    char columns[DEFAULT_LENGTH] = "";

    list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
    element_types_a = db_get_element_types_all(db, NULL, element_types_a);
    create_columns_exp(element_types_a, "m", columns);
    strcpy(sql, "select m.id");
    strcat(sql, columns);
    strcat(sql, 
            "  , t.registerdate, last_m.registerdate,  "
            "  julianday(current_date) - julianday(date(last_m.registerdate)) as passed_date ");
    strcat(sql,
            "from ticket as t "
            "inner join message as last_m on last_m.id = t.last_message_id "
            "inner join message as m on m.id = t.last_message_id "
            "where t.id = ?");
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        int i = 0;
        Iterator* it;
        Element* e;
        Element* e_id = list_new_element(elements);
        e_id->element_type_id = ELEM_ID_ID;
        set_str_val(e_id, sqlite3_column_text(stmt, i++));
        list_add(elements, e_id);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            e = list_new_element(elements);
            e->element_type_id = et->id;
            set_str_val(e, sqlite3_column_text(stmt, i++));
            list_add(elements, e);
        }
        /* 投稿日時 */
        e = list_new_element(elements);
        e->element_type_id = ELEM_ID_REGISTERDATE;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
        /* 最終更新日時 */
        e = list_new_element(elements);
        e->element_type_id = ELEM_ID_LASTREGISTERDATE;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
        /* 最終更新日時からの経過日数 */
        e = list_new_element(elements);
        e->element_type_id = ELEM_ID_LASTREGISTERDATE_PASSED;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);
    list_free(element_types_a);

    return elements;
ERROR_LABEL(db->handle)
}
List* db_get_elements(Database* db, int message_id, List* elements)
{
    char sql[DEFAULT_LENGTH] = "";
    sqlite3_stmt *stmt = NULL;
    int r;
    char columns[DEFAULT_LENGTH] = "";
    List* element_types_a = NULL;

    list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
    element_types_a = db_get_element_types_all(db, NULL, element_types_a);
    create_columns_exp(element_types_a, "m", columns);

    strcpy(sql, "select m.registerdate");
    strcat(sql, columns);
    strcat(sql,
            "from message as m "
            "where m.id = ? ");
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, message_id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        Iterator* it;
        int i = 0;
        Element* e = list_new_element(elements);
        e->element_type_id = ELEM_ID_LASTREGISTERDATE;
        set_str_val(e, sqlite3_column_text(stmt, i++));
        list_add(elements, e);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            e = list_new_element(elements);
            e->element_type_id = et->id;
            set_str_val(e, sqlite3_column_text(stmt, i++));
            list_add(elements, e);
        }
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);
    list_free(element_types_a);

    return elements;

ERROR_LABEL(db->handle)
}
int* db_get_message_ids_a(Database* db, const int ticket_id)
{
    int* message_ids = NULL;
    int r, i = 0;
    const char *sql = "select count(*) from message as m where m.ticket_id = ?";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    if (SQLITE_ROW != (r = sqlite3_step(stmt))) {
        goto error;
    }
    message_ids = xalloc(sizeof(int) * (sqlite3_column_int(stmt, 0) + 1));
    sqlite3_finalize(stmt);

    sql = "select id from message as m where m.ticket_id = ? order by m.registerdate";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
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

ERROR_LABEL(db->handle)
}

Project* db_get_project(Database* db, Project* project)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select name, value from setting";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        char* name = (char*)sqlite3_column_text(stmt, 0);
        if (strcmp(name, "project_name") == 0)
            string_set(project->name, (char*)sqlite3_column_text(stmt, 1));
        else if (strcmp(name, "home_description") == 0)
            string_set(project->home_description, (char*)sqlite3_column_text(stmt, 1));
        else if (strcmp(name, "home_url") == 0)
            string_set(project->home_url, (char*)sqlite3_column_text(stmt, 1));
        else if (strcmp(name, "upload_max_size") == 0)
            project->upload_max_size = sqlite3_column_int(stmt, 1);
        else if (strcmp(name, "locale") == 0)
            string_set(project->locale, (char*)sqlite3_column_text(stmt, 1));
    }
    /* アップロードファイルサイズの上限値が有効な値でない場合は、512kbを設定する。 */
    project->upload_max_size = (project->upload_max_size <= 0) ? 512 : project->upload_max_size;

    sqlite3_finalize(stmt);

    return project;
ERROR_LABEL(db->handle)
}
void db_update_project(Database* db, Project* project)
{
    if (exec_query(
            db,
            "update setting set "
            "value = ? "
            "where name = 'project_name'",
            COLUMN_TYPE_TEXT, string_rawstr(project->name),
            COLUMN_TYPE_END) != 1)
        die("no seting to update? or too many?");
    if (exec_query(
            db,
            "update setting set "
            "value = ? "
            "where name = 'home_description'",
            COLUMN_TYPE_TEXT, string_rawstr(project->home_description),
            COLUMN_TYPE_END) != 1)
        die("no seting to update? or too many?");
    if (exec_query(
            db,
            "update setting set "
            "value = ? "
            "where name = 'home_url'",
            COLUMN_TYPE_TEXT, string_rawstr(project->home_url),
            COLUMN_TYPE_END) != 1)
        die("no seting to update? or too many?");
    if (exec_query(
            db,
            "update setting set "
            "value = ? "
            "where name = 'upload_max_size'",
            COLUMN_TYPE_INT, project->upload_max_size,
            COLUMN_TYPE_END) != 1)
        die("no seting to update? or too many?");
}
void db_update_element_type(Database* db, ElementType* et)
{
    /* 基本項目の場合、ticket_propertyとreply_propertyは編集させない。 */
    switch (et->id) {
        case ELEM_ID_TITLE:
            et->ticket_property = 1;
            et->reply_property = 0;
            break;
        case ELEM_ID_SENDER:
            et->ticket_property = 0;
            et->reply_property = 0;
            break;
        case ELEM_ID_STATUS:
            et->ticket_property = 1;
            et->reply_property = 0;
            break;
    }
    if (exec_query(
            db,
            "update element_type set "
            " ticket_property = ?,"
            " reply_property = ?,"
            " required = ?,"
            " name = ?,"
            " description = ?,"
            " sort = ?,"
            " display_in_list = ?,"
            " default_value = ?, "
            " auto_add_item = ? "
            "where id = ?",
            COLUMN_TYPE_INT, et->ticket_property,
            COLUMN_TYPE_INT, et->reply_property,
            COLUMN_TYPE_INT, et->required,
            COLUMN_TYPE_TEXT, string_rawstr(et->name),
            COLUMN_TYPE_TEXT, string_rawstr(et->description),
            COLUMN_TYPE_INT, et->sort,
            COLUMN_TYPE_INT, et->display_in_list,
            COLUMN_TYPE_TEXT, string_rawstr(et->default_value),
            COLUMN_TYPE_INT, et->auto_add_item,
            COLUMN_TYPE_INT, et->id,
            COLUMN_TYPE_END) != 1)
        die("no element_type to update?");
}
void db_update_list_item(Database* db, ListItem* item)
{
    if (exec_query(
            db,
            "update list_item set "
            "name = ?, close = ?, sort = ? where id = ?",
            COLUMN_TYPE_TEXT, string_rawstr(item->name),
            COLUMN_TYPE_INT, item->close,
            COLUMN_TYPE_INT, item->sort,
            COLUMN_TYPE_INT, item->id,
            COLUMN_TYPE_END) != 1)
        die("no list_item to update?");
}
void db_delete_list_item(Database* db, const int id)
{
    exec_query(
            db,
            "delete from list_item "
            "where id = ?",
            COLUMN_TYPE_INT, id,
            COLUMN_TYPE_END);
}
void db_register_list_item(Database* db, ListItem* item)
{
    exec_query(
            db,
            "insert into list_item (id, element_type_id, name, close, sort)"
            "values (NULL, ?, ?, ?, ?)",
            COLUMN_TYPE_INT, item->element_type_id,
            COLUMN_TYPE_TEXT, string_rawstr(item->name),
            COLUMN_TYPE_INT, item->close,
            COLUMN_TYPE_INT, item->sort,
            COLUMN_TYPE_END);
}
int db_register_element_type(Database* db, ElementType* et)
{
    char field_name[DEFAULT_LENGTH];
    char sql[DEFAULT_LENGTH];
    int element_type_id;
    exec_query(
            db,
            "insert into element_type (id, type, ticket_property, reply_property, required, name, description, auto_add_item, display_in_list, sort)"
            "values (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            COLUMN_TYPE_INT, et->type,
            COLUMN_TYPE_INT, et->ticket_property,
            COLUMN_TYPE_INT, et->reply_property,
            COLUMN_TYPE_INT, et->required,
            COLUMN_TYPE_TEXT, string_rawstr(et->name),
            COLUMN_TYPE_TEXT, string_rawstr(et->description),
            COLUMN_TYPE_INT, et->auto_add_item,
            COLUMN_TYPE_INT, et->display_in_list,
            COLUMN_TYPE_INT, et->sort,
            COLUMN_TYPE_END);

    /* columnの追加 */
    element_type_id = sqlite3_last_insert_rowid(db->handle);
    sprintf(field_name, "field%d", element_type_id);
    strcpy(sql, "alter table message add column ");
    strcat(sql, field_name);
    strcat(sql, " text not null default '' ");
    exec_query(db, sql, COLUMN_TYPE_END);
    return element_type_id;
}
void db_delete_element_type(Database* db, const int id)
{
    exec_query(
            db,
            "update element_type set deleted = 1 where id = ?",
            COLUMN_TYPE_INT, id,
            COLUMN_TYPE_END);
}
List* db_get_states_has_not_close(Database* db, List* states)
{
    int r;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;

    sprintf(sql, 
            "select l.id, m.field%d as name, count(t.id) as count "
            "from ticket as t "
            "inner join message as m "
            " on m.id = t.last_message_id "
            "inner join list_item as l "
            " on l.name = m.m.field%d "
            "where l.close = 0 "
            "group by l.id, m.field%d "
            "order by l.sort ", ELEM_ID_STATUS, ELEM_ID_STATUS, ELEM_ID_STATUS);
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        State* s = list_new_element(states);
        s->id = sqlite3_column_int(stmt, 0);
        string_set(s->name, (char*)sqlite3_column_text(stmt, 1));
        s->count = sqlite3_column_int(stmt, 2);
        list_add(states, s);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

ERROR_LABEL(db->handle)
}
List* db_get_states(Database* db, List* states)
{
    int r;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;

    sprintf(sql, 
            "select m.field%d as name, count(t.id) as count "
            "from ticket as t "
            "inner join message as m "
            " on m.id = t.last_message_id "
            "inner join list_item as l "
            " on l.element_type_id = %d and l.name = m.m.field%d "
            "group by m.field%d "
            "order by l.sort ", ELEM_ID_STATUS, ELEM_ID_STATUS, ELEM_ID_STATUS, ELEM_ID_STATUS);
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        State* s = list_new_element(states);
        string_set(s->name, (char*)sqlite3_column_text(stmt, 0));
        s->count = sqlite3_column_int(stmt, 1);
        list_add(states, s);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

ERROR_LABEL(db->handle)
}
List* db_get_statictics_multi(Database* db, List* states, const int element_type_id)
{
    int r;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;

    sprintf(sql, 
            "select l.id, l.name as name, count(t.id) as count "
            "from ticket as t "
            "inner join message as m "
            " on m.id = t.last_message_id "
            "inner join list_item as l "
            " on l.element_type_id = %d and m.m.field%d like '%%' || l.name || '\t%%' "
            "group by l.id "
            "order by l.sort ", element_type_id, element_type_id);
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        State* s = list_new_element(states);
        s->id = sqlite3_column_int(stmt, 0);
        string_set(s->name, (char*)sqlite3_column_text(stmt, 1));
        s->count = sqlite3_column_int(stmt, 2);
        list_add(states, s);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

ERROR_LABEL(db->handle)
}
List* db_get_statictics(Database* db, List* states, const int element_type_id)
{
    int r;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;

    sprintf(sql, 
            "select l.id, l.name as name, count(t.id) as count "
            "from ticket as t "
            "inner join message as m "
            " on m.id = t.last_message_id "
            "inner join list_item as l "
            " on l.element_type_id = %d and l.name = m.m.field%d "
            "group by m.field%d "
            "order by l.sort ", element_type_id, element_type_id, element_type_id);
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        State* s = list_new_element(states);
        s->id = sqlite3_column_int(stmt, 0);
        string_set(s->name, (char*)sqlite3_column_text(stmt, 1));
        s->count = sqlite3_column_int(stmt, 2);
        list_add(states, s);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

ERROR_LABEL(db->handle)
}
ElementFile* db_get_element_file(Database* db, int id, ElementFile* file)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select id, element_type_id, filename, size, mime_type, content "
        "from element_file "
        "where id = ? ";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        int len;
        char* p_src;
        char* p_dist;
        file->id = sqlite3_column_int(stmt, 0);
        file->element_type_id = sqlite3_column_int(stmt, 1);
        string_set(file->name, (char*)sqlite3_column_text(stmt, 2));
        file->size = sqlite3_column_int(stmt, 3);
        string_set(file->mime_type, (char*)sqlite3_column_text(stmt, 4));
        len = sqlite3_column_bytes(stmt, 5);
        p_dist = file->content = xalloc(sizeof(char) * len);
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
ERROR_LABEL(db->handle)
}
List* db_get_newest_information(Database* db, const int limit, List* messages)
{
    int r;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;
    
    sprintf(sql, 
            "select t.id "
            "from ticket as t "
            "inner join message as m on m.id = t.last_message_id "
            "order by m.registerdate desc "
            "limit %d ", limit);

    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        Message* message = list_new_element(messages);
        message->id = sqlite3_column_int(stmt, 0);
        list_add(messages, message);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);
    return messages;

ERROR_LABEL(db->handle)
}
int db_get_element_file_id(Database* db, int message_id, int element_type_id)
{
    return exec_query_scalar_int(
            db,
            "select id from element_file as ef "
            "where ef.message_id = ? and ef.element_type_id = ?",
            COLUMN_TYPE_INT, message_id,
            COLUMN_TYPE_INT, element_type_id,
            COLUMN_TYPE_END);
}
char* db_get_element_file_mime_type(Database* db, const int message_id, const int element_type_id, char* buf)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select mime_type from element_file as ef "
            "where ef.message_id = ? and ef.element_type_id = ?";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, message_id);
    sqlite3_bind_int(stmt, 2, element_type_id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        strcpy(buf, (char*)sqlite3_column_text(stmt, 0));
        break;
    }

    sqlite3_finalize(stmt);

    return buf;
ERROR_LABEL(db->handle)
}
void db_register_wiki(Database* db, Wiki* wiki)
{
    char registerdate[20];
    set_date_string(registerdate);
    exec_query(
            db,
            "insert into wiki (id, name, content, registerdate)"
            "values (NULL, ?, ?, ?)",
            COLUMN_TYPE_TEXT, string_rawstr(wiki->name),
            COLUMN_TYPE_TEXT, wiki->content,
            COLUMN_TYPE_TEXT, registerdate,
            COLUMN_TYPE_END);
}
Wiki* db_get_newest_wiki(Database* db, char* page_name, Wiki* wiki)
{
    int r;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;
    
    sprintf(sql, 
            "select w.id, w.name, w.content "
            "from wiki as w "
            "where name = ? "
            "order by w.registerdate desc "
            "limit 1 ");

    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, page_name, strlen(page_name), NULL);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))) {
        const unsigned char* value;
        wiki->id = sqlite3_column_int(stmt, 0);
        string_set(wiki->name, (char*)sqlite3_column_text(stmt, 1));
        value = sqlite3_column_text(stmt, 2);
        wiki->content = xalloc(sizeof(char) * strlen((char*)value) + 1);
        strcpy(wiki->content, (char*)value);
        break;
    }

    sqlite3_finalize(stmt);
    return wiki;

ERROR_LABEL(db->handle)
}
void db_setting_file_save(Database* db, SettingFile* sf)
{
    if (exec_query(
                db,
                "update setting_file "
                "set "
                " file_name = ?, "
                " size = ?, "
                " mime_type = ?, "
                " content = ? "
                "where name = ?",
                COLUMN_TYPE_TEXT, string_rawstr(sf->file_name),
                COLUMN_TYPE_INT, sf->size,
                COLUMN_TYPE_TEXT, string_rawstr(sf->mime_type),
                COLUMN_TYPE_TEXT, sf->content,
                COLUMN_TYPE_TEXT, string_rawstr(sf->name),
                COLUMN_TYPE_END) != 1)
        die("failed to update setting file.");
}
SettingFile* db_get_setting_file(Database* db, char* name, SettingFile* file)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select file_name, size, mime_type, content "
        "from setting_file "
        "where name = ? ";
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, name, strlen(name), NULL);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        int len;
        char* p_src;
        char* p_dist;
        if (sqlite3_column_text(stmt, 0)) 
            string_set(file->name, (char*)sqlite3_column_text(stmt, 0));
        file->size = sqlite3_column_int(stmt, 1);
        if (sqlite3_column_text(stmt, 2)) 
            string_set(file->mime_type, (char*)sqlite3_column_text(stmt, 2));
        len = sqlite3_column_bytes(stmt, 3);
        p_dist = file->content = xalloc(sizeof(char) * len);
        p_src = (char*)sqlite3_column_blob(stmt, 3);
        while (len--) {
            *p_dist = *p_src;
            p_dist++;
            p_src++;
        }
        break;
    }

    sqlite3_finalize(stmt);

    return file;
ERROR_LABEL(db->handle)
}
void db_update_top_image(Database* db, SettingFile* sf)
{
    if (exec_query(
                db,
                "update setting_file "
                "set "
                " file_name = ?, "
                " size = ?, "
                " mime_type = ?, "
                " content = ? "
                "where name = 'top_image'",
                COLUMN_TYPE_TEXT, string_rawstr(sf->file_name),
                COLUMN_TYPE_INT, sf->size,
                COLUMN_TYPE_TEXT, string_rawstr(sf->mime_type),
                COLUMN_TYPE_BLOB_SETTING_FILE, sf,
                COLUMN_TYPE_END) != 1)
        die("update failed.");
}
List* db_get_burndownchart(Database* db, List* burndowns)
{
    int r;
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;
    time_t timer, now;
    struct tm *date;
    char date_string[24];
    sprintf(sql, 
            "select sum(1), sum(li.close) "
            "from message as m "
            "inner join list_item as li on li.name = m.field%d "
            "where m.id in "
            "  (select max(m.id) from message as m  where m.registerdate < ? group by m.ticket_id)",
            ELEM_ID_STATUS);
    if (sqlite3_prepare(db->handle, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;

    time(&timer);
    now = timer;
    ; /* 30日前 */
    for (timer -= 60 * 60 * 24 * 30; timer <= now; timer += 60 * 60 * 24) {
        int day;
        date = localtime(&timer);
        day = date->tm_mday;
        sprintf(date_string, "%04d-%02d-%02d 23:59:59", date->tm_year + 1900, date->tm_mon + 1, day);
        d("time: %s\n", date_string);

        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, date_string, strlen(date_string), NULL);

        while (SQLITE_ROW == (r = sqlite3_step(stmt))){
            BurndownChartPoint* b = list_new_element(burndowns);
            char d[10];
            b->all = sqlite3_column_int(stmt, 0);
            b->not_closed = b->all - sqlite3_column_int(stmt, 1);
            sprintf(d, "%d", day);
            string_set(b->day, d);
            list_add(burndowns, b);
        }
        if (SQLITE_DONE != r)
            goto error;
    }

    sqlite3_finalize(stmt);
    return burndowns;

ERROR_LABEL(db->handle)
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
