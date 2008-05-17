#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sqlite3.h>
#include "data.h"
#include "db.h"
#include "util.h"
#include "dbutil.h"
#include "simple_string.h"


extern const char* db_name;
extern sqlite3 *db;

void create_columns_exp(List*, char*, char*);

static List* db_get_element_types(int all, List* element_types)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    if (all) {
        sql = "select id, type, ticket_property, reply_property, required, name, description, display_in_list, sort, default_value, auto_add_item from element_type where deleted = 0 order by sort";
    } else {
        sql = "select id, type, ticket_property, reply_property, required, name, description, display_in_list, sort, default_value, auto_add_item from element_type where deleted = 0 and display_in_list = 1 order by sort";
    }
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        unsigned const char* value;
        ElementType* et = list_new_element(element_types);

        et->id = sqlite3_column_int(stmt, 0);
        et->type = sqlite3_column_int(stmt, 1);
        et->ticket_property = sqlite3_column_int(stmt, 2);
        et->reply_property = sqlite3_column_int(stmt, 3);
        et->required = sqlite3_column_int(stmt, 4);
        strcpy(et->name, sqlite3_column_text(stmt, 5));
        strcpy(et->description, sqlite3_column_text(stmt, 6));
        et->display_in_list = sqlite3_column_int(stmt, 7);
        et->sort = sqlite3_column_int(stmt, 8);
        value = sqlite3_column_text(stmt, 9);
        if (value != NULL) {
            strcpy(et->default_value, value);
        }
        et->auto_add_item = sqlite3_column_int(stmt, 10);
        list_add(element_types, et);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return element_types;

ERROR_LABEL
}
List* db_get_element_types_4_list(List* elements)
{
    return db_get_element_types(0, elements);
}
List* db_get_element_types_all(List* elements)
{
    return db_get_element_types(1, elements);
}
ElementType* db_get_element_type(int id, ElementType* e)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort "
        "from element_type "
        "where id = ? "
        " and deleted = 0 "
        "order by sort";
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        const unsigned char* value;
        e->id = sqlite3_column_int(stmt, 0);
        e->type = sqlite3_column_int(stmt, 1);
        e->ticket_property = sqlite3_column_int(stmt, 2);
        e->reply_property = sqlite3_column_int(stmt, 3);
        e->required = sqlite3_column_int(stmt, 4);
        strcpy(e->name, sqlite3_column_text(stmt, 5));
        strcpy(e->description, sqlite3_column_text(stmt, 6));
        e->auto_add_item = sqlite3_column_int(stmt, 7);
        value = sqlite3_column_text(stmt, 8);
        if (value != NULL)
            strcpy(e->default_value, value);
        e->display_in_list = sqlite3_column_int(stmt, 9);
        e->sort = sqlite3_column_int(stmt, 10);
        break;
    }

    sqlite3_finalize(stmt);

    return e;
ERROR_LABEL
}

List* db_get_list_item(const int element_type, List* items)
{
    int r;
    const char *sql = "select id, name, close, sort from list_item where element_type_id = ? order by sort";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, element_type);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        ListItem* item = list_new_element(items);
        item->id = sqlite3_column_int(stmt, 0);
        strcpy(item->name, sqlite3_column_text(stmt, 1));
        item->close = sqlite3_column_int(stmt, 2);
        item->sort = sqlite3_column_int(stmt, 3);
        list_add(items, item);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return items;

ERROR_LABEL
}

void set_date_string(char* buf)
{
    struct timeval tv;
    struct tm *tp;

    gettimeofday(&tv, NULL);
    tp = localtime(&tv.tv_sec);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
            tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
            tp->tm_hour, tp->tm_min, tp->tm_sec);
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
int db_register_ticket(Message* ticket)
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
    foreach (it, elements) {
        Element* e = it->element;
        int c = exec_query_scalar_int("select close from list_item "
                "where list_item.element_type_id = ? and list_item.name = ?",
                COLUMN_TYPE_INT, e->element_type_id,
                COLUMN_TYPE_TEXT, e->str_val,
                COLUMN_TYPE_END);
        if (c != INVALID_INT && c != 0) {
            closed = true;
            break;
        }
    }
    /* クローズ状態に変更されていた場合は、closedに1を設定する。 */
    if (exec_query("update ticket set closed = ? where id = ?",
            COLUMN_TYPE_INT, closed,
            COLUMN_TYPE_INT, ticket->id,
            COLUMN_TYPE_END) != 1)
        die("no ticket to update?");

    elements = ticket->elements;
    create_message_insert_sql(elements, sql);

    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    i = 1;
    sqlite3_bind_int(stmt, i++, ticket->id);
    sqlite3_bind_text(stmt, i++, registerdate, strlen(registerdate), NULL);
    foreach (it, elements) {
        Element* e = it->element;
        sqlite3_bind_text(stmt, i++, e->str_val, strlen(e->str_val), NULL);
    }
    if (exec_and_wait_4_done(stmt) != SQLITE_RETURN_OK) goto error;
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
                        "insert into element_file("
                        " id, message_id, element_type_id, filename, size, mime_type, content"
                        ") values (NULL, ?, ?, ?, ?, ?, ?) ",
                    COLUMN_TYPE_INT, message_id,
                    COLUMN_TYPE_INT, e->element_type_id,
                    COLUMN_TYPE_TEXT, fname,
                    COLUMN_TYPE_INT, size,
                    COLUMN_TYPE_TEXT, mime_type,
                    COLUMN_TYPE_BLOB, content_a,
                    COLUMN_TYPE_END) == 0)
                die("insert failed.");
            element_file_free(content_a);
        }
    }
    return ticket->id;

ERROR_LABEL
}

static String* create_columns_like_exp(List* element_types, char* table_name, List* keywords, String* buf)
{
    Iterator* it_keyword;
    Iterator* it;
    foreach (it_keyword, keywords) {
        foreach (it, element_types) {
            char column_name[DEFAULT_LENGTH];
            ElementType* et = it->element;
            if (string_len(buf))
                string_append(buf, "or ");
            sprintf(column_name, "%s.field%d like '%%' || ? || '%%' ", table_name, et->id);
            string_append(buf, column_name);
        }
    }
    return buf;
}
static List* parse_keywords(List* keywords, char* query)
{
    char* hit;
    char* p = query;
    if (strlen(p) == 0) {
        return keywords;
    }
    while (1) {
        hit = strchr(p, ' ');
        if (hit == NULL) {
            char* word = xalloc(sizeof(char) * strlen(p) + 1);
            strcpy(word, p);
            if (strlen(word))
                list_add(keywords, word);
            break;
        } else {
            int len = hit - p + 1;
            char* word = xalloc(len);
            strncpy(word, p, len - 1);
            p = hit + 1;
            if (strlen(word))
                list_add(keywords, word);
        }
    }
    return keywords;
}
static String* get_search_sql_string(List* conditions, Condition* sort, List* keywords, String* sql_string)
{
    int i = 0;
    Iterator* it;

    string_appendf(sql_string,
            "select "
            " t.id as id, m.field%d as status "
            "from ticket as t "
            "inner join message as m on m.id = t.last_message_id "
            "inner join message as org_m on org_m.id = t.original_message_id ", ELEM_ID_STATUS);

    
    if (keywords->size > 0)
        string_append(sql_string, "inner join message as m_all on m_all.ticket_id = t.id ");

    if (valid_condition_size(conditions) || keywords->size > 0)
        string_append(sql_string, "where ");
    if (valid_condition_size(conditions)) {
        foreach (it, conditions) {
            Condition* cond = it->element;
            char val[DEFAULT_LENGTH];
            if (cond->element_type_id < 0) continue;
            if (strlen(cond->value) == 0) continue;
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
            if (strlen(cond->value) == 0) continue;
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
            }
            string_append(sql_string, val);
        }
    }
    if (keywords->size > 0) {
        String* columns_a = string_new(0);
        List* element_types_a;
        list_alloc(element_types_a, ElementType);
        element_types_a = db_get_element_types_all(element_types_a);
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
        sprintf(sort_type, "%s", strstr(sort->value, "reverse") ? "desc" : "asc");
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

    d("sql: %s\n", string_rawstr(sql_string));
    return sql_string;
}
int set_conditions(sqlite3_stmt* stmt, List* conditions, List* keywords)
{
    int n = 1;
    Iterator* it;
    foreach (it, conditions) {
        Condition* cond = it->element;
        if (strlen(cond->value) == 0) continue;
        sqlite3_bind_text(stmt, n++, cond->value, strlen(cond->value), NULL);
    }
    if (keywords->size > 0) {
        List* element_types_a;
        Iterator* it_keyword;
        Iterator* it;
        list_alloc(element_types_a, ElementType);
        element_types_a = db_get_element_types_all(element_types_a);
        foreach (it_keyword, keywords) {
            char* word = it_keyword->element;
            foreach (it, element_types_a) {
                sqlite3_bind_text(stmt, n++, word, strlen(word), NULL);
            }
        }
        list_free(element_types_a);
    }
    return n;
}
SearchResult* db_get_tickets_by_status(const char* status, List* messages, SearchResult* result)
{
    int r, n, hit_count = 0;
    String* sql_a = string_new(0);
    List* conditions;
    Condition* cond_status;
    sqlite3_stmt *stmt = NULL;
    List* keywords_a;

    list_alloc(keywords_a, char);
    list_alloc(conditions, Condition);
    cond_status = list_new_element(conditions);
    cond_status->element_type_id = ELEM_ID_STATUS;
    strcpy(cond_status->value, status);
    list_add(conditions, cond_status);
    sql_a = get_search_sql_string(conditions, NULL, keywords_a, sql_a);
    string_append(sql_a, " limit ? ");
    d("sql_a: %s\n", string_rawstr(sql_a));
    result->messages = messages;

    if (sqlite3_prepare(db, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    n = set_conditions(stmt, conditions, keywords_a);
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
    string_free(sql_a);
    list_free(keywords_a);

    list_free(conditions);
    result->hit_count = hit_count;
    return result;
ERROR_LABEL
}
SearchResult* db_search_tickets(List* conditions, char* q, Condition* sorts, const int page, SearchResult* result)
{
    int r, n;
    String* sql_a = string_new(0);
    sqlite3_stmt *stmt = NULL;
    List* keywords_a;

    list_alloc(keywords_a, char);
    keywords_a = parse_keywords(keywords_a, q);
    sql_a = get_search_sql_string(conditions, sorts, keywords_a, sql_a);

    string_append(sql_a, " limit ? offset ? ");
    if (sqlite3_prepare(db, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    n = set_conditions(stmt, conditions, keywords_a);
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
    /* hit件数を取得する。 */
    {
        String* s = string_new(0);
        string_append(s, "select count(res.id), res.status from (");
        s = get_search_sql_string(conditions, sorts, keywords_a, s);
        string_append(s, ") as res left join element_type as et on et.id = res.status group by status");
        if (sqlite3_prepare(db, string_rawstr(s), string_len(s), &stmt, NULL) == SQLITE_ERROR) goto error;
        sqlite3_reset(stmt);
        n = set_conditions(stmt, conditions, keywords_a);
        while (SQLITE_ROW == (r = sqlite3_step(stmt))){
            State* s = list_new_element(result->states);
            result->hit_count += s->count = sqlite3_column_int(stmt, 0);
            strcpy(s->name, sqlite3_column_text(stmt, 1));
            list_add(result->states, s);
        }
        string_free(s);
        if (SQLITE_DONE != r)
            goto error;
    }

    sqlite3_finalize(stmt);
    list_free(keywords_a);

    result->page = page;
    return result;
ERROR_LABEL
}
SearchResult* db_search_tickets_4_report(List* conditions, char* q, Condition* sorts, List* messages, SearchResult* result)
{
    int r, n;
    String* sql_a = string_new(0);
    sqlite3_stmt *stmt = NULL;
    List* keywords_a;

    list_alloc(keywords_a, char);
    keywords_a = parse_keywords(keywords_a, q);
    result->messages = messages;
    sql_a = get_search_sql_string(conditions, sorts, keywords_a, sql_a);

    if (sqlite3_prepare(db, string_rawstr(sql_a), string_len(sql_a), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    n = set_conditions(stmt, conditions, keywords_a);

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
ERROR_LABEL
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
        e->str_val = xalloc(sizeof(char) * strlen(str_val) + 1);
        strcpy(e->str_val, str_val);
    }
}
List* db_get_last_elements_4_list(const int ticket_id, List* elements)
{
    char sql[DEFAULT_LENGTH] = "";
    char sql_suf[DEFAULT_LENGTH] = "";
    sqlite3_stmt *stmt = NULL;
    int r;
    char columns[DEFAULT_LENGTH] = "";
    List* element_types_a = NULL;
    Iterator* it;

    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_4_list(element_types_a);
    create_columns_exp(element_types_a, "last_m", columns);
    sprintf(sql, "select t.id, org_m.field%d ", ELEM_ID_SENDER);
    strcat(sql, columns);
    sprintf(sql_suf, 
            "  , substr(t.registerdate, 0, 16), substr(last_m.registerdate, 0, 16),  "
            "  julianday(current_date) - julianday(date(last_m.registerdate)) as passed_date "
            "from ticket as t "
            "inner join message as last_m on last_m.id = t.last_message_id "
            "inner join message as org_m on org_m.id = t.original_message_id "
            "where t.id = ?");
    strcat(sql, sql_suf);
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
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
ERROR_LABEL
}
List* db_get_last_elements(const int ticket_id, List* elements)
{
    char sql[DEFAULT_LENGTH];
    sqlite3_stmt *stmt = NULL;
    int r;
    List* element_types_a = NULL;
    char columns[DEFAULT_LENGTH] = "";

    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(element_types_a);
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
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
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
ERROR_LABEL
}
List* db_get_elements(int message_id, List* elements)
{
    char sql[DEFAULT_LENGTH] = "";
    sqlite3_stmt *stmt = NULL;
    int r;
    char columns[DEFAULT_LENGTH] = "";
    List* element_types_a = NULL;

    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(element_types_a);
    create_columns_exp(element_types_a, "m", columns);

    strcpy(sql, "select m.registerdate");
    strcat(sql, columns);
    strcat(sql,
            "from message as m "
            "where m.id = ? ");
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
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

ERROR_LABEL
}
int* db_get_message_ids_a(const int ticket_id)
{
    int* message_ids = NULL;
    int r, i = 0;
    const char *sql = "select count(*) from message as m where m.ticket_id = ?";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, ticket_id);

    if (SQLITE_ROW != (r = sqlite3_step(stmt))) {
        goto error;
    }
    message_ids = xalloc(sizeof(int) * (sqlite3_column_int(stmt, 0) + 1));
    sqlite3_finalize(stmt);

    sql = "select id from message as m where m.ticket_id = ? order by m.registerdate";
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
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

ERROR_LABEL
}

Project* db_get_project(Project* project)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select name, home_url from project";
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        strcpy(project->name, sqlite3_column_text(stmt, 0));
        strcpy(project->home_url, sqlite3_column_text(stmt, 1));
        break;
    }

    sqlite3_finalize(stmt);

    return project;
ERROR_LABEL
}
void db_update_project(Project* project)
{
    if (exec_query(
            "update project set "
            "name = ?, home_url = ? ",
            COLUMN_TYPE_TEXT, project->name,
            COLUMN_TYPE_TEXT, project->home_url,
            COLUMN_TYPE_END) != 1)
        die("no project to update? or too many?");
}
void db_update_element_type(ElementType* et)
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
            COLUMN_TYPE_TEXT, et->name,
            COLUMN_TYPE_TEXT, et->description,
            COLUMN_TYPE_INT, et->sort,
            COLUMN_TYPE_INT, et->display_in_list,
            COLUMN_TYPE_TEXT, et->default_value,
            COLUMN_TYPE_INT, et->auto_add_item,
            COLUMN_TYPE_INT, et->id,
            COLUMN_TYPE_END) != 1)
        die("no element_type to update?");
}
void db_update_list_item(ListItem* item)
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
void db_delete_list_item(const int id)
{
    exec_query(
            "delete from list_item "
            "where id = ?",
            COLUMN_TYPE_INT, id,
            COLUMN_TYPE_END);
}
void db_register_list_item(ListItem* item)
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
int db_register_element_type(ElementType* et)
{
    char field_name[DEFAULT_LENGTH];
    char sql[DEFAULT_LENGTH];
    int element_type_id;
    exec_query(
            "insert into element_type (id, type, ticket_property, reply_property, required, name, description, auto_add_item, display_in_list, sort)"
            "values (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            COLUMN_TYPE_INT, et->type,
            COLUMN_TYPE_INT, et->ticket_property,
            COLUMN_TYPE_INT, et->reply_property,
            COLUMN_TYPE_INT, et->required,
            COLUMN_TYPE_TEXT, et->name,
            COLUMN_TYPE_TEXT, et->description,
            COLUMN_TYPE_INT, et->auto_add_item,
            COLUMN_TYPE_INT, et->display_in_list,
            COLUMN_TYPE_INT, et->sort,
            COLUMN_TYPE_END);

    /* columnの追加 */
    element_type_id = sqlite3_last_insert_rowid(db);
    sprintf(field_name, "field%d", element_type_id);
    strcpy(sql, "alter table message add column ");
    strcat(sql, field_name);
    strcat(sql, " text not null default '' ");
    exec_query(sql, COLUMN_TYPE_END);
    return element_type_id;
}
void db_delete_element_type(const int id)
{
    exec_query(
            "update element_type set deleted = 1 where id = ?",
            COLUMN_TYPE_INT, id,
            COLUMN_TYPE_END);
}
List* db_get_states_has_not_close(List* states)
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
            " on l.name = m.m.field%d "
            "where l.close = 0 "
            "group by m.field%d "
            "order by l.sort ", ELEM_ID_STATUS, ELEM_ID_STATUS, ELEM_ID_STATUS);
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        State* s = list_new_element(states);
        strcpy(s->name, sqlite3_column_text(stmt, 0));
        s->count = sqlite3_column_int(stmt, 1);
        list_add(states, s);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

ERROR_LABEL
}
List* db_get_states(List* states)
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
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        State* s = list_new_element(states);
        strcpy(s->name, sqlite3_column_text(stmt, 0));
        s->count = sqlite3_column_int(stmt, 1);
        list_add(states, s);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

ERROR_LABEL
}
List* db_get_statictics_multi(List* states, const int element_type_id)
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
            "group by m.field%d "
            "order by l.sort ", element_type_id, element_type_id, element_type_id);
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        State* s = list_new_element(states);
        s->id = sqlite3_column_int(stmt, 0);
        strcpy(s->name, sqlite3_column_text(stmt, 1));
        s->count = sqlite3_column_int(stmt, 2);
        list_add(states, s);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

ERROR_LABEL
}
List* db_get_statictics(List* states, const int element_type_id)
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
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        State* s = list_new_element(states);
        s->id = sqlite3_column_int(stmt, 0);
        strcpy(s->name, sqlite3_column_text(stmt, 1));
        s->count = sqlite3_column_int(stmt, 2);
        list_add(states, s);
    }
    if (SQLITE_DONE != r)
        goto error;

    sqlite3_finalize(stmt);

    return states;

ERROR_LABEL
}
ElementFile* db_get_element_file(int id, ElementFile* file)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select id, element_type_id, filename, size, mime_type, content "
        "from element_file "
        "where id = ? ";
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        int len;
        char* p_src;
        char* p_dist;
        file->id = sqlite3_column_int(stmt, 0);
        file->element_type_id = sqlite3_column_int(stmt, 1);
        strcpy(file->name, sqlite3_column_text(stmt, 2));
        file->size = sqlite3_column_int(stmt, 3);
        strcpy(file->mime_type, sqlite3_column_text(stmt, 4));
        len = sqlite3_column_bytes(stmt, 5);
        p_dist = file->blob = xalloc(sizeof(char) * len);
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
ERROR_LABEL
}
List* db_get_newest_information(const int limit, List* messages)
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

    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
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

ERROR_LABEL
}
int db_get_element_file_id(int message_id, int element_type_id)
{
    return exec_query_scalar_int("select id from element_file as ef "
            "where ef.message_id = ? and ef.element_type_id = ?",
            COLUMN_TYPE_INT, message_id,
            COLUMN_TYPE_INT, element_type_id,
            COLUMN_TYPE_END);
}
char* db_get_element_file_mime_type(const int message_id, const int element_type_id, char* buf)
{
    int r;
    const char *sql;
    sqlite3_stmt *stmt = NULL;

    sql = "select mime_type from element_file as ef "
            "where ef.message_id = ? and ef.element_type_id = ?";
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, message_id);
    sqlite3_bind_int(stmt, 2, element_type_id);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))){
        strcpy(buf, sqlite3_column_text(stmt, 0));
        break;
    }

    sqlite3_finalize(stmt);

    return buf;
ERROR_LABEL
}
void db_register_wiki(Wiki* wiki)
{
    char registerdate[20];
    set_date_string(registerdate);
    exec_query(
            "insert into wiki (id, name, content, registerdate)"
            "values (NULL, ?, ?, ?)",
            COLUMN_TYPE_TEXT, wiki->name,
            COLUMN_TYPE_TEXT, wiki->content,
            COLUMN_TYPE_TEXT, registerdate,
            COLUMN_TYPE_END);
}
Wiki* db_get_newest_wiki(char* page_name, Wiki* wiki)
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

    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, page_name, strlen(page_name), NULL);

    while (SQLITE_ROW == (r = sqlite3_step(stmt))) {
        const unsigned char* value;
        wiki->id = sqlite3_column_int(stmt, 0);
        strcpy(wiki->name, sqlite3_column_text(stmt, 1));
        value = sqlite3_column_text(stmt, 2);
        wiki->content = xalloc(sizeof(char) * strlen(value) + 1);
        strcpy(wiki->content, value);
        break;
    }

    sqlite3_finalize(stmt);
    return wiki;

ERROR_LABEL
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
