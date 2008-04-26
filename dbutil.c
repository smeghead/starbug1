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
            "values ('BTS', 'http://example.com/');", COLUMN_TYPE_END);
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
            "*機能説明\n"
            "各ページの説明です。\n"
            "**トップ\n"
            "ナビゲーションメニューから移動できます。\n"
            "***簡易ページ編集機能\n"
            "トップページには、編集可能な領域があります。簡易wikiページとして使用できます。\n"
            "プロジェクトの説明や、Starbug1の運用方法について記述を簡単に行なうことができます。\n"
            "\n"
            "****編集方法\n"
            "プロジェクト名が表示されている下の「トップページの編集」というリンクを押し、トップページ編集のページが表示します。\n"
            "テキストエリアのトップページの内容を編集し「更新」ボタンを押すことで、編集内容を保存できます。\n"
            "トップページは、Wiki風の文法をサポートしていますので、見出しや箇条書きや区切り線を含めることが可能です。詳細は、ページ下部の説明を参照してください。 \n"
            "****最新情報\n"
            "最近更新されたチケットの件名を10件分表示します。\n"
            "この内容は、RSS Feedで配信している情報と同等のものとなっています。\n"
            "チケットの件名のリンクを押すと、該当のチケット詳細ページを表示します。\n"
            "****状態別件数\n"
            "チケットの状態毎に件数を表示します。\n"
            "状態名のリンクを押すと、検索ページで該当の状態を検索条件で検索したのと同じページを表示します。\n"
            "****ID検索\n"
            "チケットIDを指定してチケット詳細ページに移動することができます。\n"
            "チケットIDを入力して「表示」ボタンを押すと、チケット詳細ページを表示します。\n"
            "**状態別チケット\n"
            "ナビゲーションメニューから移動できます。\n"
            "***チケットナビゲーション部(ページ上部)\n"
            "状態別件数とID検索の機能があります。\n"
            "***状態別チケット一覧\n"
            "未クローズの状態毎に、チケット一覧を表示します。\n"
            "各状態に表示されるチケットの最大件数は、30件です。ある状態のチケットが30件以上ある場合には、「新しい30件のみを表示しています。」というメッセージが表示されます。加えてある状態で検索した結果の検索ページへのリンクも表示されます。\n"
            "****一覧に表示される項目\n"
            "一覧に必ず表示される項目は、ID、投稿日時、最終更新日、放置日数です。その他に表示される項目は、管理ページ(項目設定)で一覧に表示するように設定された項目になります。一覧に表示するかしないかは、自由に設定可能です。 \n"
            "****放置日数\n"
            "放置日数というのは、最終更新日時が、現在日時より何日前かを表示します。これにより、ある状態で止まってしまっているチケットが可視化できます。\n"
            "****チケット詳細ページへの移動\n"
            "一覧のIDまたは件名のリンクを押すとチケット詳細を参照することができます。 \n"
            "**チケット登録\n"
            "ナビゲーションメニューから移動できます。\n"
            "***チケット登録フォーム\n"
            "チケットを新規に登録するページです。\n"
            "フォームの各項目を入力し、登録ボタンを押すことで、新規にチケットを登録できます。\n"
            "****入力チェック\n"
            "管理ページ(項目設定)で必須項目として設定されている項目については、登録ボタンを押した時に入力チェックが行なわれます。 (JavaScriptでチェックを行なっていますが、JavaScriptが使用できないブラウザでも投稿自体は可能です。JavaScriptが使用できない場合、入力チェックが行なわれなくなります。)\n"
            "****投稿者の値の保存\n"
            "登録ボタンの右の「投稿者を保存する。」のチェックを付けた状態で投稿を行なうと、(投稿者に入力した値を、次回投稿時や返信時まで保存しておくことができます。(この機能は、cookieを使用していますので、ブラウザでcookieを無効にしている場合は、保存されません。) \n"
            "**チケット検索\n"
            "ナビゲーションメニューから移動できます。\n"
            "***チケットナビゲーション部(画面上部)\n"
            "状態別件数とID検索の機能があります。\n"
            "***検索条件フォーム\n"
            "検索条件を指定するためのフォームを表示します。\n"
            "検索条件で指定できる項目は、2種類あります。\n"
            "****管理ページ(項目設定)でチケット一覧に表示するように設定されている項目(複数)\n"
            "チケットの最新状態の項目の値が検索対象になります。\n"
            "****キーワード検索\n"
            "チケットの履歴も含めた全ての項目が検索対象になります。\n"
            "***検索結果\n"
            "検索条件フォームで指定された条件で検索した結果を表示します。\n"
            "****ページャ\n"
            "1ページに表示するチケットの最大件数は、30件です。検索結果が30件以上ある場合には、<< 1 2 3 >> というページャが表示されます。各リンクを押すことで別のページに移動できます。\n"
            "****一覧に表示される項目\n"
            "一覧に必ず表示される項目は、ID、投稿日時、最終更新日、放置日数です。\n"
            "その他に表示される項目は、管理ページ(項目設定)で一覧に表示するように設定された項目になります。\n"
            "一覧に表示するかしないかは、自由に設定可能です。\n"
            "****放置日数\n"
            "放置日数というのは、最終更新日時が、現在日時より何日前かを表示します。これにより、ある状態で止まってしまっているチケットが可視化できます。\n"
            "****検索結果の並べ替え\n"
            "検索結果の表のヘッダ部分のリンクをクリックすることで検索結果の並べ替えを行なうことができます。 ヘッダ部分を押す度に、昇順降順を切り替えます。\n"
            "****チケット詳細ページへの移動\n"
            "一覧のIDまたは件名のリンクを押すとチケット詳細ページへ移動することができます。\n"
            "****レポート(CSV形式)\n"
            "検索結果上部の「検索結果をCSVでダウンロードする。」のリンクを押すことで、検索結果の最新情報をCSV形式でダウンロードすることができます。\n"
            "ダウンロードしたファイルを元に、エクセル形式に変換すれば、簡単に顧客提出用や開発会議用の資料を作成することができます。 \n"
            "**チケット詳細\n"
            "状態別チケットページやチケット検索ページのチケット一覧中のIDと件名のリンクを押すとチケット詳細ページに移動します。\n"
            "***チケット最新情報\n"
            "チケットの最新情報です。最新チケットのチケット属性の付いている属性と全添付ファイルを表示します。\n"
            "***チケット履歴\n"
            "チケットの履歴情報です。新規投稿されてから、全ての返信にていての変更履歴を表示します。\n"
            "***チケット登録フォーム\n"
            "チケットを新規に登録するページです。\n"
            "フォームの各項目を入力し、登録ボタンを押すことで、新規にチケットを登録できます。\n"
            "****入力チェック\n"
            "管理ページ(項目設定)で必須項目として設定されている項目については、登録ボタンを押した時に入力チェックが行なわれます。 (JavaScriptでチェックを行なっていますが、JavaScriptが使用できないブラウザでも投稿自体は可能です。JavaScriptが使用できない場合、入力チェックが行なわれなくなります。)\n"
            "****投稿者の値の保存\n"
            "登録ボタンの右の「投稿者を保存する。」のチェックを付けた状態で投稿を行なうと、(投稿者に入力した値を、次回投稿時や返信時まで保存しておくことができます。(この機能は、cookieを使用していますので、ブラウザでcookieを無効にしている場合は、保存されません。) \n"
            "**統計情報\n"
            "ナビゲーションメニューから移動できます。\n"
            "登録されているチケットの統計情報を表示します。\n"
            "チケットの項目の内、項目種別が、単一選択肢の項目と複数選択肢の項目について、値の分布を円グラフで表示します。\n"
            "ただし、JavaScriptが有効になっていない場合は、項目の値とその値が選択されているチケット数をグラフではなく文字で表示します。 \n"
            "**RSS\n"
            "最新情報をRSS feedを配信します。\n"
            "最近更新のあった10件についての情報を配信します。\n"
            "**チケット一括登録\n"
            "複数のチケットを一括登録します。\n"
            "タスク管理に利用する場合など、登録するチケットが多数あり予めわかっている場合に一括でチケットを登録できます。\n"
            "**ヘルプ\n"
            "ナビゲーションメニューから移動できます。\n"
            "ヘルプを表示します。\n"
            "**管理ツール\n"
            "管理ツールメニューへ移動します。\n"
            "\n"
            "*逆引きヘルプ\n"
            "**最新のチケットの情報をCSV形式でダウンロードしたい\n"
            "チケット検索で検索した結果を「検索結果をCSVでダウンロードする。」のリンクからダウンロードできます。\n"
            "**ある状態で進捗のないチケットを調べたい\n"
            "チケット検索ページか状態別チケットページで、最終更新日時から現在までの日数を放置日数として表示しています。\n"
            "放置日数を確認してください。\n"
            "**チケットを新規に作成するには？\n"
            "ナビゲーションメニューの「チケット登録」をクリックしチケット登録ページに移動します。 フォームの各フィールドに値を入力して「登録」ボタンを押すことで新規にチケットを作成することができます。 \n"
            "**チケットを検索するには？\n"
            "ナビゲーションメニューの「チケット検索」をクリックしチケット検索ページに移動します。 検索条件を入力して「検索」ボタンを押すことで検索を行なうことができます。 \n"
            "**IDを指定してチケットの情報を見るには？\n"
            "ナビゲーションメニューの「チケット登録」をクリックしチケット登録ページに移動します。「IDを指定で表示」ボタンの左のテキストボックスに、チケットIDを入力し、「IDを指定で表示」ボタンを押すことで、チケット詳細ページに移動します。 また、同様の「IDを指定で表示」ボタンは、トップページ、状態別チケットページにもあります。 \n"
            "**チケットの詳細情報を見るには？\n"
            "ナビゲーションの「状態別チケット」または「チケット検索」からチケット情報の一覧を表示させることができます。チケット情報の一覧から、詳細情報を見たいチケットのIDか件名のリンクをクリックすることでチケット詳細ページに移動します。チケット詳細ページでは、チケットの最新情報、チケットの履歴を見ることができます。また、チケットに返信することもできます。  \n"
            "**チケットに対して返信を行なうには？\n"
            "ナビゲーションメニューの「状態別チケット」または「チケット検索」からチケット情報の一覧を表示させることができます。チケット情報の一覧から、詳細情報を見たいチケットのIDか件名のリンクをクリックすることでチケット詳細ページに移動します。チケット詳細ページの下にチケット返信のフォームがありますので、必要な項目を入力して返信ボタンを押してください。  \n"
            "**他チケットにリンクを貼るには？\n"
            "複数行テキスト項目の値には、他のチケットの詳細ページへのリンクを貼ることができます。#1 のように、書くと、チケットIDが 1 のチケットへのリンクになります。  \n"
            "**ソースコードなどを貼るには？\n"
            "複数行テキスト項目の値には、整形済ブロック用の記法が用意されています。ソースコードなどを貼る場合に、インデントを崩さずに表示することができます。行頭が >| から始まる行から、行頭が |< から始まる行までを整形済ブロックとして表示します。  \n"
            "', current_timestamp);", COLUMN_TYPE_END);
    exec_query(
            "insert into wiki(id, name, content, registerdate) values (NULL, 'adminhelp', '"
            "*管理ユーザ用説明\n"
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
            "*仕様\n"
            "**項目に設定可能な設定値\n"
            "***項目名\n"
            "***項目の説明文\n"
            "***返信専用属性\n"
            "***項目種別\n"
            "-1行テキスト\n"
            "-複数行テキスト\n"
            "-真偽値\n"
            "-選択リスト\n"
            "-複数選択可能リスト\n"
            "-ファイル\n"
            "-日付\n"
            "***選択要素\n"
            "-項目種別が選択リストか複数選択可能リストの場合の選択肢の内容\n"
            "-自動的に増える選択肢\n"
            "***デフォルト値\n"
            "***チケット一覧表示\n"
            "***並び順\n"
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
    for(i = 0; (type = va_arg(ap, int)) != COLUMN_TYPE_END; i++){
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
