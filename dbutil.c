#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "util.h"
#include "dbutil.h"

const char* db_name = "db/starbug1.db";
sqlite3 *db = NULL;

void create_tables();

int exec_and_wait_4_done(sqlite3_stmt* stmt)
{
    int loop = 0, ret;

    while (SQLITE_DONE != (ret = sqlite3_step(stmt))) {
        if (loop++ > 100000) {
            d("sql update error. database may be locked.\n");
            return SQLITE_RETURN_ERROR;
        }
    }
    return SQLITE_RETURN_OK;
}

static int fexist(const char *filename)
{
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) return 0;
    fclose(fp); return 1;
}

void db_init()
{
    bool exists_db_file = (fexist(db_name) == 1);

    mkdir("db", 0755);
    if (SQLITE_OK != sqlite3_open(db_name, &db)) {
        die("sqlite3 init error.");
    }

    if (!exists_db_file)
        create_tables();
}

void db_finish()
{
    int ret;
    ret = sqlite3_close(db);
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
            " home_url text "
            ");", COLUMN_TYPE_END);
    exec_query(
            "insert into project(name, home_url)"
            "values ('BTS', '');", COLUMN_TYPE_END);
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
            " sort integer, "
            " deleted integer not null default 0 "
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
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (6, 3, '対応せず', 1, 6);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (7, 3, '仕様通り', 1, 7);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (8, 4, '画面', 0, 1);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (9, 4, 'バッチ処理', 0, 2);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (10, 4, 'ドキュメント', 0, 3);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (11, 5, '緊急', 0, 1);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (12, 5, '高', 0, 2);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (13, 5, '中', 0, 3);", COLUMN_TYPE_END);
    exec_query("insert into list_item(id, element_type_id, name, close, sort) values (14, 5, '低', 0, 4);", COLUMN_TYPE_END);
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
            "*編集可能領域\n"
            "自由に編集できます。右側の「トップページの編集」のリンクから編集してください。色々な用途に使用してください。\n"
            "-お知らせ\n"
            "-Starbug1の使い方についての注意事項など\n"
            "', current_timestamp);", COLUMN_TYPE_END);
    exec_query(
            "insert into wiki(id, name, content, registerdate) values (NULL, 'help', '"
            "*逆引きヘルプ\n"
            "**チケットを更新する方法は？\n"
            "チケットに対して更新を行なうアクションは、チケットの新規登録と、チケットに対する返信のみです。 \n"

            "**チケットを新規に作成するには？\n"
            "ナビゲーションメニューの「チケット登録」をクリックしチケット登録ページに移動します。 フォームの各フィールドに値を入力して「登録」ボタンを押すことで新規にチケットを作成することができます。 \n"

            "**チケットの詳細情報を見るには？\n"
            "ナビゲーションの「状態別チケット一覧」または「チケット検索」からチケット情報の一覧を表示させることができます。チケット情報の一覧から、詳細情報を見たいチケットのIDか件名のリンクをクリックすることでチケット詳細ページに移動します。チケット詳細ページでは、チケットの最新情報、チケットの履歴を見ることができます。また、チケットに返信することもできます。  \n"

            "**チケットを検索するには？\n"
            "ナビゲーションメニューの「チケット検索」をクリックしチケット検索ページに移動します。 検索条件を入力して「検索」ボタンを押すことで検索を行なうことができます。 \n"

            "**IDを指定してチケットの情報を見るには？\n"
            "ナビゲーションメニューの「チケット登録」をクリックしチケット登録ページに移動します。「IDを指定で表示」ボタンの左のテキストボックスに、チケットIDを入力し、「IDを指定で表示」ボタンを押すことで、チケット詳細ページに移動します。 また、同様の「IDを指定で表示」ボタンは、トップページ、状態別チケット一覧ページにもあります。 \n"

            "**チケットの項目をカスタマイズするには？\n"
            "ナビゲーションメニューの「管理ツール」をクリックし管理ツールを開き、メニューの「項目設定」をクリックし、項目設定ページに移動します。\n"
            "各項目の追加、削除、編集を行なうことができます。\n"

            "**チケットの項目に添付資料(ファイル)を追加するには？\n"
            "ナビゲーションメニューの「管理ツール」をクリックし管理ツールを開き、メニューの「項目設定」をクリックし、項目設定ページに移動します。\n"
            "「新規項目を追加」リンクをクリックし、項目種別でファイルを選択して項目を追加してください。\n"

            "**ある状態で進捗のないチケットを調べたい\n"
            "チケット検索ページか状態別チケット一覧ページで、最終更新日時から現在までの日数を放置日数として表示しています。\n"
            "放置日数を確認してください。\n"

            "**投稿時、返信時にメーリングリストなどにメールを送信するには？\n"
            "投稿時、返信時のメール送信は、hook機能により実現可能です。hook機能はサーバ上での設定が必要になります。hook機能の設定方法については、以下のサイトのインストールのページを見てください。\n"
            "-http://starbug1.sourceforge.jp/\n"

            "**チケットの項目をカスタマイズしたい\n"
            "管理ツールの項目設定ページで、全ての項目のカスタマイズを行なうことができます。\n"
            "件名、投稿者、状態は基本属性であるため、カスタマイズできる内容に制限があります。(削除不可、チケット属性、返信専用属性変更不可)\n"

            "**複数のチケットを一括で登録したい\n"
            "一括で登録したいチケットの情報を、CSV形式で準備することができれば、一括で登録することができます。プロジェクト管理ツールなどで出力したCSV形式のタスク一覧を利用することができます。\n"
            "ナビゲーションメニューの「チケット一括登録」をクリックしチケット一括登録ページに移動します。\n"
            "チケット一括登録ページで、登録したい複数のチケットの情報をCSV形式で貼り付けて、解析ボタンを押します。 \n"
            "チケット一括登録確認ページで、解析結果が表示されるので、各フィールドに対応する項目を選択し、登録ボタンを押します。\n"

            "**最新のチケットの情報をCSV形式でダウンロードしたい\n"
            "チケット検索で検索した結果を「検索結果をCSVでダウンロードする。」のリンクからダウンロードできます。\n"

            "**チケットに対して返信を行なうには？\n"
            "ナビゲーションメニューの「状態別チケット一覧」または「チケット検索」からチケット情報の一覧を表示させることができます。チケット情報の一覧から、詳細情報を見たいチケットのIDか件名のリンクをクリックすることでチケット詳細ページに移動します。チケット詳細ページの下にチケット返信のフォームがありますので、必要な項目を入力して返信ボタンを押してください。  \n"

            "**他チケットにリンクを貼るには？\n"
            "複数行テキスト項目の値には、他のチケットの詳細ページへのリンクを貼ることができます。#1 のように、書くと、チケットIDが 1 のチケットへのリンクになります。  \n"

            "**ソースコードなどを貼るには？\n"
            "複数行テキスト項目の値には、整形済ブロック用の記法が用意されています。ソースコードなどを貼る場合に、インデントを崩さずに表示することができます。行頭が >| から始まる行から、行頭が |< から始まる行までを整形済ブロックとして表示します。  \n"

            "**チケット一覧の状態などのセルの背景色などを変更したい\n"
            "管理ツールのスタイル設定から、状態などの単一選択肢の項目の一覧表示時のスタイルシートをカスタマイズすることが可能です。カスタマイズするためには、CSSの知識が必要です。\n"

            "**ページのスタイルを自由に設定したい\n"
            "管理ツールのスタイル設定から、スタイルシートをカスタマイズすることが可能です。カスタマイズするためには、CSSの知識が必要です。\n"

            "**データのバックアップをしたい\n"
            "バックアップの方法については、以下のサイトのインストールのページを見てください。\n"
            "-http://starbug1.sourceforge.jp/\n"

            "**設定情報をエクスポートしたい\n"
            "Starbug1を複数立ち上げる際などに、1から設定しなくて済むように、運用中のStarbug1から設定情報をエクスポートすることができます。設定情報のエクスポートの方法については、以下のサイトのインストールのページを見てください。\n"
            "-http://starbug1.sourceforge.jp/\n"

            "', current_timestamp);", COLUMN_TYPE_END);
    exec_query(
            "insert into wiki(id, name, content, registerdate) values (NULL, 'adminhelp', '"
            "*管理ユーザ用説明\n"
            "各ページについての説明です。\n"

            "**メニュー\n"
            "管理ツールの各設定ページへのリンクです。\n"

            "**プロジェクト設定\n"
            "プロジェクト情報の設定をするページです。\n"
            "各ページのヘッダ部分などに表示されるプロジェクト名を設定することができます。\n"

            "**環境設定\n"
            "環境に関する情報を設定するページです。\n"
            "一般ユーザ向けページのナビゲーションメニューの「ホーム」のリンク先を設定するととができます。\n"

            "**項目設定\n"
            "チケットの項目を設定するページです。\n"
            "チケットの項目を追加、修正、削除することができます。\n"
            "***項目の追加\n"
            "「新規項目の追加」リンクを押すことで、項目の追加ページに移動します。\n"
            "各項目についての説明は、各項目の下に表示されています。\n"
            "項目の項目種別は、追加するときにしか選択できません。\n"
            "***項目の修正\n"
            "各項目の設定内容を変更することができます。\n"
            "各項目についての説明は、各項目の下に表示されています。\n"
            "項目の情報についての設定後、「更新」ボタンを押すことで、項目の設定を修正できます。\n"
            "項目の項目種別については、修正時に変更することはできません。 \n"
            "***項目の削除\n"
            "項目の削除を行なうことができます。\n"
            "「この項目(カテゴリ)の削除」などのリンクを押すと削除確認ページに移動します。\n"
            "削除確認ページで、削除ボタンを押すと項目が削除されます。\n"
            "ただし、基本項目(件名、投稿者、状態)については、削除できません。 \n"

            "**スタイル設定\n"
            "各画面の見た目を変更することができます。\n"
            "チケット一覧での状態などの項目の背景色は、ここで設定を行なってください。\n"
            "状態などの選択肢の値を変更した場合(\"新規\" から \"new\"に変更した場合)は、再度このページで背景色の設定を行なってください。\n"
            "その際、ページの下部に、チケット一覧の背景設定用のサンプルが表示されますので参考にしてください。 \n"

            "', current_timestamp);", COLUMN_TYPE_END);
    db_commit();
}

/**
 * 戻り値がないSQLの実行をラップします。
 */
int exec_query(const char* sql, ...)
{
    int i;
    va_list ap;
    int type;
    sqlite3_stmt *stmt = NULL;

    d("sql: %s\n", sql);
    if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) == SQLITE_ERROR) goto error;
    sqlite3_reset(stmt);

    va_start(ap, sql);
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
    if (exec_and_wait_4_done(stmt) != SQLITE_RETURN_OK) goto error;
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
    sqlite3_stmt* stmt = NULL;

    sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_reset(stmt);

    va_start(ap,sql);
    for (i = 0; (type = va_arg(ap, int)) != COLUMN_TYPE_END; i++) {
        if (type == COLUMN_TYPE_INT) {
            int value = va_arg(ap, int);
            sqlite3_bind_int(stmt, i + 1, value);
        } else if (type == COLUMN_TYPE_TEXT) {
            char* value = va_arg(ap, char*);
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
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
