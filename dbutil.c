#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "util.h"
#include "dbutil.h"

const char* db_name = "db/starbug1.db";
sqlite3 *db = NULL;

void create_tables();

static int fexist(const char *filename)
{
  FILE *fp;
  if ((fp = fopen(filename, "r")) == NULL) return 0;
  fclose(fp); return 1;
}

void db_init()
{
  int exists_db_file = (fexist(db_name) == 1);

  mkdir("db", 0755);
  if (SQLITE_OK != sqlite3_open(db_name, &db)) {
    die("sqlite3 init error.");
  }

  if (!exists_db_file)
    create_tables();
}

void db_finish()
{
  sqlite3_close(db);
}

void db_begin()
{
    exec_query("begin;", COLUMN_TYPE_END);
}
void db_rollback()
{
    exec_query("rollback;", COLUMN_TYPE_END);
}
void db_commit()
{
    exec_query("commit;", COLUMN_TYPE_END);
}
void create_tables()
{
    db_begin();
    exec_query(
            "create table project( "
            " name text, "
            " description text, "
            " home_url text, "
            " smtp_server text, "
            " smtp_port integer, "
            " notify_address text, "
            " admin_address text "
            ");", COLUMN_TYPE_END);
    exec_query(
            "insert into project(name, description, home_url, smtp_server, smtp_port, notify_address, admin_address)"
            "values ('サンプル プロジェクト', 'プロジェクト名と、このプロジェクトの説明は管理ツールで編集してください。', 'http://example.com/', 'localhost', 25, '', '');", COLUMN_TYPE_END);
    exec_query(
            "create table element_type("
            " id integer not null primary key, "
            " type integer, "
            " ticket_property integer, "
            " reply_property integer, "
            " required integer, "
            " element_name text, "
            " description text, "
            " default_value text, "
            " display_in_list integer, "
            " sort integer "
            ");", COLUMN_TYPE_END);
    exec_query(
            "create index index_element_type_0 on element_type (id, type, display_in_list, sort)", COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, element_name, description, default_value, display_in_list, sort) "
            "values (1, ?, 1, 0, 1, '件名', '内容を簡潔に表すような件名を入力してください。', '', 1, 1);", 
            COLUMN_TYPE_INT, ELEM_TYPE_TEXT,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, element_name, description, default_value, display_in_list, sort) "
            "values (2, ?, 0, 0, 1, '投稿者', 'メールアドレスを入力してください。', '', 1, 2);",
            COLUMN_TYPE_INT, ELEM_TYPE_TEXT,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, element_name, description, default_value, display_in_list, sort) "
            "values (3, ?, 1, 0, 1, '状態', '状態を選択してください。', '新規', 1, 3);", 
            COLUMN_TYPE_INT, ELEM_TYPE_LIST_SINGLE,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, element_name, description, default_value, display_in_list, sort) "
            "values (4, ?, 1, 0, 0, 'カテゴリ', 'カテゴリを選択してください。', '', 1, 4);", 
            COLUMN_TYPE_INT, ELEM_TYPE_LIST_MULTI,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, element_name, description, default_value, display_in_list, sort) "
            "values (5, ?, 1, 0, 0, '優先度', '優先度を選択してください。', '', 1, 5);", 
            COLUMN_TYPE_INT, ELEM_TYPE_LIST_SINGLE,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, element_name, description, default_value, display_in_list, sort) "
            "values (6, ?, 1, 0, 1, '詳細', '的確に記述してください。', '', 0, 6);",
            COLUMN_TYPE_INT, ELEM_TYPE_TEXTAREA,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, element_name, description, default_value, display_in_list, sort) "
            "values (7, ?, 1, 0, 0, '再現手順', '問題を再現させるための条件と手順を記述してください。', '', 0, 7);", 
            COLUMN_TYPE_INT, ELEM_TYPE_TEXTAREA,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, element_name, description, default_value, display_in_list, sort) "
            "values (8, ?, 0, 1, 0, 'コメント', 'コメントを記述してください。', '', 0, 8);", 
            COLUMN_TYPE_INT, ELEM_TYPE_TEXTAREA,
            COLUMN_TYPE_END);
    exec_query(
            "create table list_item( "
            " id integer not null primary key, "
            " element_type_id integer, "
            " name text, "
            " close integer, "
            " sort integer "
            ");", COLUMN_TYPE_END);
    exec_query(
            "create index index_list_item_0 on list_item (id, sort)", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (1, 3, '新規', 0, 1);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (2, 3, '受付済', 0, 2);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (3, 3, '修正済', 0, 3);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (4, 3, '保留', 0, 4);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (5, 3, '完了', 1, 5);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (6, 3, '破棄', 1, 6);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (7, 4, '画面', 0, 1);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (8, 4, 'バッチ処理', 0, 2);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (9, 4, 'ドキュメント', 0, 3);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (10, 5, '緊急', 0, 1);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (11, 5, '高', 0, 2);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (12, 5, '中', 0, 3);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (13, 5, '低', 0, 4);", COLUMN_TYPE_END);
    exec_query(
            "create table ticket("
            " id integer not null primary key, "
            " original_message_id integer not null default 0, "
            " last_message_id integer not null default 0, "
            " registerdate text not null, "
            " closed integer not null default 0"
            ");", COLUMN_TYPE_END);
    exec_query(
            "create index index_ticket_0 on ticket (id, last_message_id, last_message_id, closed)", COLUMN_TYPE_END);
    exec_query(
            "create table message("
            " id integer not null primary key, "
            " ticket_id integer not null, "
            " registerdate text, "
            " field1 text not null default '' , "
            " field2 text not null default '' , "
            " field3 text not null default '' , "
            " field4 text not null default '' , "
            " field5 text not null default '' , "
            " field6 text not null default '' , "
            " field7 text not null default '' , "
            " field8 text not null default ''  "
            ");", COLUMN_TYPE_END);
    exec_query(
            "create table element_file("
            " id integer not null primary key, "
            " message_id integer not null, "
            " element_type_id integer not null, "
            " filename text, "
            " size integer, "
            " mime_type text, "
            " content blob "
            ");", COLUMN_TYPE_END);
    db_commit();
}

/**
 * 戻り値がないSQLの実行をラップします。
 */
int exec_query(const char* sql, ...)
{
    int i, loop = 0;
    va_list ap;
    int type;
    sqlite3_stmt *stmt = NULL;

    d("sql: %s\n", sql);
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    va_start(ap,sql);
    for(i = 0; (type = va_arg(ap, int)) != COLUMN_TYPE_END; i++){
        if (type == COLUMN_TYPE_INT) {
            int value = va_arg(ap, int);
            sqlite3_bind_int(stmt, i + 1, value);
        } else if (type == COLUMN_TYPE_TEXT) {
            char* value = va_arg(ap, char*);
            if (value == NULL) goto error;
            sqlite3_bind_text(stmt, i + 1, value, strlen(value), NULL);
        } else if (type == COLUMN_TYPE_BLOB) {
            ElementFile* content = va_arg(ap, ElementFile*);
            if (content == NULL) goto error;
            sqlite3_bind_blob(stmt, i + 1, content->blob, content->size, NULL);
        }
    }
    va_end(ap);

    // stmtのSQLを実行
    while (SQLITE_DONE != sqlite3_step(stmt)){
        if (loop++ > 1000)
            goto error;
    }
    // stmt を開放
    sqlite3_finalize(stmt);

    return sqlite3_changes(db);

ERROR_LABEL
}
/**
 * 戻り値が単一のint型の数値であるSQLの実行をラップします。
 * 注意:結果が無い場合は、INVALID_INT(-99999)を返却する。
 */
int exec_query_scalar_int(const char* sql, ...)
{
    int i, int_value = 0, r;
    va_list ap;
    int type;
    sqlite3_stmt *stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);

    va_start(ap,sql);
    for(i = 0; (type = va_arg(ap, int)) != COLUMN_TYPE_END; i++){
        if (type == COLUMN_TYPE_INT) {
            int value = va_arg(ap, int);
            d("type: %d = %d\n", type, value);
            sqlite3_bind_int(stmt, i + 1, value);
        } else if (type == COLUMN_TYPE_TEXT) {
            char* value = va_arg(ap, char*);
            d("type(text): %d = %s\n", type, value);
            if (value == NULL) goto error;
            sqlite3_bind_text(stmt, i + 1, value, strlen(value), NULL);
        }
    }
    va_end(ap);
    r = sqlite3_step(stmt);
    if (r == SQLITE_ROW){
        int_value = sqlite3_column_int(stmt, 0);
    } else if (r == SQLITE_DONE) {
        int_value = INVALID_INT;
    } else {
        goto error;
    }
    sqlite3_finalize(stmt);

    return int_value;

ERROR_LABEL
}
/* vim: set ts=4 sw=4 sts=4 expandtab: */
