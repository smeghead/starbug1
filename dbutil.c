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
            " home_url text, "
            " smtp_server text, "
            " smtp_port integer, "
            " notify_address text, "
            " admin_address text "
            ");", COLUMN_TYPE_END);
    exec_query(
            "insert into project(name, home_url, smtp_server, smtp_port, notify_address, admin_address)"
            "values ('サンプル プロジェクト', 'http://example.com/', 'localhost', 25, '', '');", COLUMN_TYPE_END);
    exec_query(
            "create table element_type("
            " id integer not null primary key, "
            " type integer, "
            " ticket_property integer not null default 0, "
            " reply_property integer not null default 0, "
            " required integer not null default 0, "
            " name text, "
            " description text, "
            " default_value text, "
            " auto_add_item integer not null default 0, "
            " display_in_list integer not null default 0, "
            " sort integer "
            ");", COLUMN_TYPE_END);
    exec_query(
            "create index index_element_type_0 on element_type (id, type, display_in_list, sort)", COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort) "
            "values (1, ?, 1, 0, 1, '件名', '内容を簡潔に表すような件名を入力してください。', 0, '', 1, 1);", 
            COLUMN_TYPE_INT, ELEM_TYPE_TEXT,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort) "
            "values (2, ?, 0, 0, 1, '投稿者', '投稿者を入力してください。', 0, '', 1, 2);",
            COLUMN_TYPE_INT, ELEM_TYPE_TEXT,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort) "
            "values (3, ?, 1, 0, 1, '状態', '状態を選択してください。', 0, '新規', 1, 3);", 
            COLUMN_TYPE_INT, ELEM_TYPE_LIST_SINGLE,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort) "
            "values (4, ?, 1, 0, 0, 'カテゴリ', 'カテゴリを選択してください。', 1, '', 1, 4);", 
            COLUMN_TYPE_INT, ELEM_TYPE_LIST_MULTI,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort) "
            "values (5, ?, 1, 0, 0, '優先度', '優先度を選択してください。', 0, '', 1, 5);", 
            COLUMN_TYPE_INT, ELEM_TYPE_LIST_SINGLE,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort) "
            "values (6, ?, 1, 0, 1, '詳細', '的確に記述してください。', 0, '', 0, 6);",
            COLUMN_TYPE_INT, ELEM_TYPE_TEXTAREA,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort) "
            "values (7, ?, 1, 0, 0, '再現手順', '問題を再現させるための条件と手順を記述してください。', 0, '', 0, 7);", 
            COLUMN_TYPE_INT, ELEM_TYPE_TEXTAREA,
            COLUMN_TYPE_END);
    exec_query(
            "insert into element_type(id, type, ticket_property, reply_property, required, name, description, auto_add_item, default_value, display_in_list, sort) "
            "values (8, ?, 0, 1, 0, 'コメント', 'コメントを記述してください。', 0, '', 0, 8);", 
            COLUMN_TYPE_INT, ELEM_TYPE_TEXTAREA,
            COLUMN_TYPE_END);
    exec_query(
            "create table list_item( "
            " id integer not null primary key, "
            " element_type_id integer not null default 0, "
            " name text, "
            " close integer not null default 0, "
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
    exec_query(
            "create table wiki( "
            " id integer not null primary key, "
            " name text, "
            " content text, "
            " registerdate text "
            ");", COLUMN_TYPE_END);
    exec_query(
            "insert into wiki(id, name, content, registerdate) values (NULL, 'top', '"
            "**編集可能領域\n"
            "自由に編集できます。右側の「トップページの編集」のリンクから編集してください。色々な用途に使用してください。\n"
            "-お知らせ\n"
            "-Starbug1の使い方についての注意事項など\n"
            "', current_timestamp);", COLUMN_TYPE_END);
    exec_query(
            "insert into wiki(id, name, content, registerdate) values (NULL, 'help', '"
            "**逆引きヘルプ\n"
            "-チケットを新規に作成するには？\n"
            "画面上部のメニューの「チケット登録」をクリックしチケット登録画面に移動します。\n"
            "フォームの各フィールドに値を入力して「登録」ボタンを押すことで新規にチケットを作成することができます。\n"
            "-チケットを検索するには？\n"
            "画面上部のメニューの「チケット検索」をクリックしチケット検索画面に移動します。\n"
            "検索条件を入力して「検索」ボタンを押すことで検索を行なうことができます。\n"
            "-IDを指定してチケットの情報を見るには？\n"
            "画面上部のメニューの「チケット登録」をクリックしチケット登録画面に移動します。\n"
            "「IDを指定で表示」ボタンの左のテキストボックスに、チケットIDを入力し、「IDを指定で表示」ボタンを押すことで、チケット情報\n"
            "また、同様の「IDを指定で表示」ボタンは、トップ、状態別チケット画面にもあります。\n"
            "-チケットの詳細情報を見るには？\n"
            "画面上部のメニューの「状態別チケット」または「チケット検索」からチケット情報の一覧を表示させることができます。\n"
            "チケット情報の一覧から、詳細情報を見たいチケットのIDか件名のリンクをクリックすることでチケット詳細画面に移動します。\n"
            "チケット詳細画面では、チケットの最新情報、チケットの履歴を見ることができます。また、チケットに返信することもできます。\n"
            "-チケットに対して返信を行なうには？\n"
            "画面上部のメニューの「状態別チケット」または「チケット検索」からチケット情報の一覧を表示させることができます。\n"
            "チケット情報の一覧から、詳細情報を見たいチケットのIDか件名のリンクをクリックすることでチケット詳細画面に移動します。\n"
            "チケット詳細画面の下にチケット返信のフォームがありますので、必要な項目を入力して返信ボタンを押してください。\n"
            "**管理ツール\n"
            "管理ツールに遷移します。\n"
            "管理ツールでは以下を編集できます。\n"
            "-プロジェクト設定\n"
            "プロジェクトの基本的な情報の設定です。\n"
            "-環境設定\n"
            "URLやメール関連の設定です。\n"
            "-項目設定\n"
            "チケットの項目についての設定です。\n"
            "-スタイル設定\n"
            "スタイルシートの設定です。\n"
            "', current_timestamp);", COLUMN_TYPE_END);
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
