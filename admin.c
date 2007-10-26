#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db.h"
#include "dbutil.h"
#include "util.h"

#define VALUE_LENGTH 1048575
#define MODE_LENGTH 128
#define DEFAULT_LENGTH 1024
#define ADD_ITEM_COUNT 5

/* prototype declares */
void register_actions();
void menu_action();
void project_action();
void project_submit_action();
void env_action();
void env_submit_action();
void items_action();
void items_submit_action();
void style_action();
void display_action();
void update_action();
void new_item_action();
void new_item_submit_action();
void delete_item_action();
void delete_item_submit_action();
void output_header(bt_project*, char*);
void output_footer();
int cgiMain();
void update_elements();

void register_actions()
{
    register_action_actions("menu", menu_action);
    register_action_actions("project", project_action);
    register_action_actions("project_submit", project_submit_action);
    register_action_actions("env", env_action);
    register_action_actions("env_submit", env_submit_action);
    register_action_actions("items", items_action);
    register_action_actions("items_submit", items_submit_action);
/*     register_action_actions("style", style_action); */
    register_action_actions("display", display_action);
    register_action_actions("update", update_action);
    register_action_actions("new_item", new_item_action);
    register_action_actions("new_item_submit", new_item_submit_action);
    register_action_actions("delete_item", delete_item_action);
    register_action_actions("delete_item_submit", delete_item_submit_action);
    register_action_actions("default", menu_action);
}

void output_header(bt_project* project, char* script_name)
{
    cgiHeaderContentType("text/html; charset=utf-8;");
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />"
            "\t<title>Starbug1 管理ツール</title>\n");
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    if (script_name) {
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/prototype.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/%s\"></script>\n", cgiScriptName, script_name);
    }
    o(      "</head>\n"
            "<body>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1 管理ツール\"><a href=\"http://sourceforge.jp/projects/starbug1/\"><img src=\"%s/../img/title.jpg\" alt=\"Starbug1\" /></a></h1>\n"
            "<ul id=\"mainmenu\">\n", cgiScriptName);
    o(      "\t<li><a href=\"%s\">管理ツールメニュー</a></li>\n", cgiScriptName);
    o(      "\t\t<li><a href=\"%s/project\">プロジェクトの設定</a></li>\n", cgiScriptName);
    o(      "\t\t<li><a href=\"%s/env\">環境設定</a></li>\n", cgiScriptName);
    o(      "\t\t<li><a href=\"%s/items\">項目設定</a></li>\n", cgiScriptName);
    o(      "\t\t<li><a href=\"%s/css\">スタイル設定</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/../db/starbug1.db\">バックアップ</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/../index.cgi\">", cgiScriptName);h(project->name); o("トップへ</a></li>\n");
    o(      "</ul>\n"
            "<br clear=\"all\" />\n"
            "<hr />\n");
}
void output_footer()
{
    /* Finish up the page */
    o(      "<hr />\n"
            "<div align=\"right\">\n"
            "<p>\n"
            "\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
            "\t\t<img src=\"http://www.w3.org/Icons/valid-xhtml10\" alt=\"Valid XHTML 1.0 Transitional\" height=\"31\" width=\"88\" />\n"
            "\t</a>\n"
            "</p>\n"
            "<div><address>Powered by cgic.</address></div>\n"
            "<div><address>Copyright smeghead 2007.</address></div>\n"
            "</div>\n"
            "</body>\n</html>\n");
}
int cgiMain() {
    register_actions();
    exec_action();
    return 0;
}
/**
 * 管理メニュー画面をを表示するaction。
 */
void menu_action()
{
    bt_project* project;

    db_init();
    project = db_get_project();
    output_header(project, NULL);

    o("<h2>%s 管理ツール</h2>", project->name);
    o("<div id=\"admin_menu\">\n");
    o("\t<dl>\n");
    o("\t\t<dt><a href=\"%s/project\">プロジェクト設定</a></dt><dd>プロジェクトの基本的な情報の設定です。</dd>\n", cgiScriptName);
    o("\t\t<dt><a href=\"%s/env\">環境設定</a></dt><dd>URLやメール関連の設定です。</dd>\n", cgiScriptName);
    o("\t\t<dt><a href=\"%s/items\">項目設定</a></dt><dd>チケットの項目についての設定です。</dd>\n", cgiScriptName);
    o("\t\t<dt><a href=\"%s/css\">スタイル設定</a></dt><dd>スタイルシートの設定です。</dd>\n", cgiScriptName);
    o("\t</dl>\n");
    o("</div>\n");

    output_footer();
    db_finish();
}
/**
 * プロジェクト設定画面をを表示するaction。
 */
void project_action()
{
    bt_project* project;

    db_init();
    project = db_get_project();
    output_header(project, "management.js");

    o("<h2>%s 管理ツール</h2>", project->name);
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/project_submit\" method=\"post\">\n", cgiScriptName);
    o("\t\t<h3>プロジェクト設定</h3>\n");
    o("\t\t<table summary=\"project table\">\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>プロジェクト名</th>\n");
    o("\t\t\t\t<td><input type=\"text\" name=\"project.name\" value=\"");h(project->name);o("\" maxlength=\"1023\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>説明</th>\n");
    o("\t\t\t\t<td><input type=\"text\" name=\"project.description\" value=\"");h(project->description);o("\" maxlength=\"1023\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t</table>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o("\t</form>\n");
    o("</div>\n");
    output_footer();
    db_finish();
}
/**
 * プロジェクト設定を更新するaction。
 */
void project_submit_action()
{
    bt_project* project;

    db_init();
    db_begin();
    project = db_get_project();
    cgiFormStringNoNewlines("project.name", project->name, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.description", project->description, DEFAULT_LENGTH);
    db_update_project(project);

    project = db_get_project();
    db_commit();
    db_finish();
    output_header(project, NULL);
    o(      "<h2>処理完了</h2>\n"
            "<div id=\"complete_message\">\n"
            "<h3>おつかれさまでした。</h3>\n");
        o("<div class=\"message\">更新しました</div>\n");
    o(      "</div>\n");
    output_footer();
}
/**
 * 環境設定の設定画面をを表示するaction。
 */
void env_action()
{
    bt_project* project;

    db_init();
    project = db_get_project();
    output_header(project, "management.js");

    o("<h2>%s 管理ツール</h2>", project->name);
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/env_submit\" method=\"post\">\n", cgiScriptName);
    o("\t\t<h3>環境設定</h3>\n");
    o("\t\t<table summary=\"project table\">\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>home_url</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.home_url\" value=\"");h(project->home_url);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">各ページに表示されるナビゲータの、\"ホーム\" アンカーのリンク先を指定します。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>host_name</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.host_name\" value=\"");h(project->host_name);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">Starbug1を運用するホスト名です。 メール送信時に、通知メール内で BTS の URL として使用されます。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>smtp_server</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.smtp_server\" value=\"");h(project->smtp_server);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">メール送信時に使用するSMTPサーバのホスト名です。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>smtp_port</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.smtp_port\" value=\"%d\" maxlength=\"1023\" />", project->smtp_port);
    o("\t\t\t\t\t<div class=\"description\">メール送信時に使用するSMTPサーバのポート番号です。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>通知先アドレス</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.notify_address\" value=\"");h(project->notify_address);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">投稿があったときに通知を行なうメールアドレスです。メーリングリストなどを指定してください。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>管理者アドレス</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.admin_address\" value=\"");h(project->admin_address);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">メール通知のfromに指定されるメールアドレスです。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t</table>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o("\t</form>\n");
    o("</div>\n");
    output_footer();
    db_finish();
}
/**
 * 環境設定を更新するaction。
 */
void env_submit_action()
{
    bt_project* project;
    char smtp_port[DEFAULT_LENGTH];

    db_init();
    db_begin();
    project = db_get_project();
    cgiFormStringNoNewlines("project.home_url", project->home_url, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.host_name", project->host_name, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.smtp_server", project->smtp_server, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.smtp_port", smtp_port, DEFAULT_LENGTH);
    project->smtp_port = atoi(smtp_port);
    cgiFormStringNoNewlines("project.notify_address", project->notify_address, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.admin_address", project->admin_address, DEFAULT_LENGTH);

    db_update_project(project);

    project = db_get_project();
    db_commit();
    db_finish();
    output_header(project, NULL);
    o(      "<h2>処理完了</h2>\n"
            "<div id=\"complete_message\">\n"
            "<h3>おつかれさまでした。</h3>\n");
        o("<div class=\"message\">更新しました</div>\n");
    o(      "</div>\n");
    output_footer();
}
/**
 * 項目設定画面をを表示するaction。
 */
void items_action()
{
    bt_project* project;
    bt_element_type* element_types;
    bt_element_type* e_types;

    db_init();
    project = db_get_project();
    output_header(project, "management.js");

    o("<h2>%s 管理ツール</h2>", project->name);
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/items_submit\" method=\"post\">\n", cgiScriptName);
    o("\t\t<h3>項目設定</h3>\n");
    o("\t\t<div class=\"message\">チケットID、投稿日時は編集できません。件名、投稿者、状態は、基本属性のため編集に制限があります。(削除不可、チケット属性、返信専用属性変更不可)</div>\n");
    o("\t\t<div class=\"message\"><a href=\"%s/new_item\">新規項目の追加</a></div>\n", cgiScriptName);

    e_types = element_types = db_get_element_types(1);
    o("\t\t<ul id=\"field_list\">\n");
    for (; e_types != NULL; e_types = e_types->next) {
        o("\t\t<li><a href=\"#field%d\">", e_types->id); h(e_types->name); o("</a></li>\n");
    }
    o("\t\t</ul>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    e_types = element_types;
    for (; e_types != NULL; e_types = e_types->next) {
        bt_list_item* items = db_get_list_item(e_types->id);
        o("\t\t<hr />\n");
        o("\t\t<a name=\"field%d\" />\n", e_types->id);
        o("\t\t<h4>"); h(e_types->name); o("</h4>\n");
        o("\t\t<input type=\"hidden\" name=\"field_ids\" value=\"%d\" />\n", e_types->id);
        o("\t\t<table summary=\"project table\">\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目名</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"text\" name=\"field%d.name\" ", e_types->id);
        o(                  "value=\"");h(e_types->name);o("\" />\n");
        o("\t\t\t\t\t<div class=\"description\">項目名です。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目の説明文</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"text\" name=\"field%d.description\" ", e_types->id);
        o(                  "value=\"");h(e_types->description);o("\" />\n");
        o("\t\t\t\t\t<div class=\"description\">項目の説明文です。投稿時に表示されます。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>必須項目</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.required\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.required\" ", e_types->id, e_types->id);
        o(                  "value=\"1\" %s />\n", e_types->required == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.required\">必須項目とする。</label>\n", e_types->id);
        o("\t\t\t\t\t<div class=\"description\">入力しないと投稿できない属性とするかどうかです。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>チケット属性</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.ticket_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.ticket_property\" ", e_types->id, e_types->id);
        o(                  "value=\"1\" %s %s />\n", e_types->ticket_property == 1 ? "checked=\"checked\"" : "", e_types->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.ticket_property\">チケット属性とする。</label>\n", e_types->id);
        o("\t\t\t\t\t<div class=\"description\">返信の属性としてではなくチケット自体の属性として定義するかどうかです。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>返信専用属性</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.reply_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.reply_property\" ", e_types->id, e_types->id);
        o(                  "value=\"1\" %s %s />\n", e_types->reply_property == 1 ? "checked=\"checked\"" : "", e_types->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.reply_property\">返信専用属性とする。</label>\n", e_types->id);
        o("\t\t\t\t\t<div class=\"description\">新規の登録時には、入力項目としないかどうかです。チェックした場合、新規の登録時には、入力項目になりません。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目種別</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"hidden\" name=\"field%d.element_type_id\" value=\"%d\" />\n", e_types->id, e_types->type);
        switch (e_types->type) {
            case ELEM_TEXT:
                o("1行テキスト(input[type=text])");
                break;
            case ELEM_TEXTAREA:
                o("複数行テキスト(textarea)");
                break;
            case ELEM_CHECKBOX:
                o("真偽値(input[type=checkbox])");
                break;
            case ELEM_RADIO:
                o("ラジオボタン(input[type=radio])");
                break;
            case ELEM_LIST_SINGLE:
                o("選択リスト(select)");
                break;
            case ELEM_LIST_MULTI:
                o("複数選択可能リスト(select[multiple=multiple])");
            case ELEM_UPLOADFILE:
                o("ファイル(input[type=file])");
                break;
        }
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        switch (e_types->type) {
            case ELEM_LIST_SINGLE:
            case ELEM_LIST_MULTI:
                o("\t\t\t<tr>\n");
                o("\t\t\t\t<th>選択要素</th>\n");
                o("\t\t\t\t<td>\n");
                o("\t\t\t\t\t<table class=\"list_item\">\n");
                o("\t\t\t\t\t\t<tr>\n");
                o("\t\t\t\t\t\t\t<th>選択肢名</th>\n");
                o("\t\t\t\t\t\t\t<th class=\"close\">クローズを意味する</th>\n");
                o("\t\t\t\t\t\t\t<th>並び順</th>\n");
                o("\t\t\t\t\t\t\t<th class=\"delete\">削除</th>\n");
                o("\t\t\t\t\t\t</tr>\n");
                for (; items != NULL; items = items->next) {
                    o("\t\t\t\t\t\t<tr>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item%d.name\" ", e_types->id, items->id);
                    o(                       "value=\"");h(items->name);o("\" />\n");
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.close\" value=\"1\" %s />\n", e_types->id, items->id, (items->close == 1) ? "checked=\"checked\"" : "");
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item%d.sort\" value=\"%d\" />\n", e_types->id, items->id, items->sort);
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.delete\" value=\"1\" />\n", e_types->id, items->id);
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t</tr>\n");
                }
                o("\t\t\t\t\t\t<tr>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item_new.name\" value=\"\" />\n", e_types->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item_new.close\" value=\"\" />\n", e_types->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item_new.sort\" value=\"\" />\n", e_types->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t&nbsp;\n");
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t</tr>\n");
                o("\t\t\t\t\t</table>\n");
                o("\t\t\t\t\t<div class=\"description\">項目種別が選択式の場合の選択肢です。</div>\n");
                o("\t\t\t\t</td>\n");
                o("\t\t\t</tr>\n");
                break;
        }
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>チケット一覧表示</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.display_in_list\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.display_in_list\" ", e_types->id, e_types->id);
        o(                  "value=\"1\" %s />\n", e_types->display_in_list == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.display_in_list\">項目をチケット一覧に表示する。</label>\n", e_types->id);
        o("\t\t\t\t\t<div class=\"description\">項目をチケット一覧で表示するかどうかです。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>並び順</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.sort\" value=\"%d\" maxlength=\"5\" />\n", e_types->id, e_types->sort);
        o("\t\t\t\t\t<div class=\"description\">チケット一覧、投稿画面、返信画面での項目の並び順です。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t</table>\n");
        if (e_types->id > BASIC_ELEMENT_MAX) {
            /* 基本項目は削除できないようにする。 */
            o("\t\t<div class=\"message\"><a href=\"%s/delete_item/%d\">", cgiScriptName, e_types->id);o("この項目(");h(e_types->name);o(")の削除</a></div>\n");
        }
    }
    o("\t\t<hr />\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o("\t</form>\n");
    o("</div>\n");

    output_footer();
    db_finish();
}
/**
 * 項目設定を更新するaction。
 */
void items_submit_action()
{
    bt_project* project;

    db_init();
    db_begin();
    project = db_get_project();
    update_elements();
    db_commit();
    db_finish();
    output_header(project, NULL);
    o(      "<h2>処理完了</h2>\n"
            "<div id=\"complete_message\">\n"
            "<h3>おつかれさまでした。</h3>\n");
        o("<div class=\"message\">更新しました</div>\n");
    o(      "</div>\n");
    output_footer();
}
/**
 * 管理画面をを表示するaction。
 */
void display_action()
{
    bt_project* project;
    bt_element_type* element_types;
    bt_element_type* e_types;

    db_init();
    project = db_get_project();
    output_header(project, "management.js");

    o("<h2>%s 管理ツール</h2>", project->name);
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/update\" method=\"post\">\n", cgiScriptName);
    o("\t\t<h3>プロジェクトの設定</h3>\n");
    o("\t\t<table summary=\"project table\">\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>プロジェクト名</th>\n");
    o("\t\t\t\t<td><input type=\"text\" name=\"project.name\" value=\"");h(project->name);o("\" maxlength=\"1023\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>説明</th>\n");
    o("\t\t\t\t<td><input type=\"text\" name=\"project.description\" value=\"");h(project->description);o("\" maxlength=\"1023\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t</table>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o("\t\t<h3>環境設定</h3>\n");
    o("\t\t<table summary=\"project table\">\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>home_url</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.home_url\" value=\"");h(project->home_url);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">各ページに表示されるナビゲータの、\"ホーム\" アンカーのリンク先を指定します。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>host_name</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.host_name\" value=\"");h(project->host_name);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">Starbug1を運用するホスト名です。 メール送信時に、通知メール内で BTS の URL として使用されます。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>smtp_server</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.smtp_server\" value=\"");h(project->smtp_server);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">メール送信時に使用するSMTPサーバのホスト名です。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>smtp_port</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.smtp_port\" value=\"%d\" maxlength=\"1023\" />", project->smtp_port);
    o("\t\t\t\t\t<div class=\"description\">メール送信時に使用するSMTPサーバのポート番号です。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>通知先アドレス</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.notify_address\" value=\"");h(project->notify_address);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">投稿があったときに通知を行なうメールアドレスです。メーリングリストなどを指定してください。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>管理者アドレス</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"project.admin_address\" value=\"");h(project->admin_address);o("\" maxlength=\"1023\" />\n");
    o("\t\t\t\t\t<div class=\"description\">メール通知のfromに指定されるメールアドレスです。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t</table>\n");
    o("\t\t<div class=\"description\">smtp_server、smtp_port、通知先アドレス、管理者アドレスが設定されている場合のみ、投稿時に通知先アドレスへのメール送信を行ないます。</div>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o("\t\t<h3>項目設定</h3>\n");
    o("\t\t<div class=\"message\">チケットID、投稿日時は編集できません。件名、投稿者、状態は、基本属性のため編集に制限があります。(削除不可、チケット属性、返信専用属性変更不可)</div>\n");
    o("\t\t<div class=\"message\"><a href=\"%s/new_item\">新規項目の追加</a></div>\n", cgiScriptName);

    e_types = element_types = db_get_element_types(1);
    o("\t\t<ul id=\"field_list\">\n");
    for (; e_types != NULL; e_types = e_types->next) {
        o("\t\t<li><a href=\"#field%d\">", e_types->id); h(e_types->name); o("</a></li>\n");
    }
    o("\t\t</ul>\n");
    e_types = element_types;
    for (; e_types != NULL; e_types = e_types->next) {
        bt_list_item* items = db_get_list_item(e_types->id);
        o("\t\t<hr />\n");
        o("\t\t<a name=\"field%d\" />\n", e_types->id);
        o("\t\t<h4>"); h(e_types->name); o("</h4>\n");
        o("\t\t<input type=\"hidden\" name=\"field_ids\" value=\"%d\" />\n", e_types->id);
        o("\t\t<table summary=\"project table\">\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目名</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"text\" name=\"field%d.name\" ", e_types->id);
        o(                  "value=\"");h(e_types->name);o("\" />\n");
        o("\t\t\t\t\t<div class=\"description\">項目名です。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目の説明文</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"text\" name=\"field%d.description\" ", e_types->id);
        o(                  "value=\"");h(e_types->description);o("\" />\n");
        o("\t\t\t\t\t<div class=\"description\">項目の説明文です。投稿時に表示されます。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>必須項目</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.required\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.required\" ", e_types->id, e_types->id);
        o(                  "value=\"1\" %s />\n", e_types->required == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.required\">必須項目とする。</label>\n", e_types->id);
        o("\t\t\t\t\t<div class=\"description\">入力しないと投稿できない属性とするかどうかです。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>チケット属性</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.ticket_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.ticket_property\" ", e_types->id, e_types->id);
        o(                  "value=\"1\" %s %s />\n", e_types->ticket_property == 1 ? "checked=\"checked\"" : "", e_types->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.ticket_property\">チケット属性とする。</label>\n", e_types->id);
        o("\t\t\t\t\t<div class=\"description\">返信の属性としてではなくチケット自体の属性として定義するかどうかです。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>返信専用属性</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.reply_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.reply_property\" ", e_types->id, e_types->id);
        o(                  "value=\"1\" %s %s />\n", e_types->reply_property == 1 ? "checked=\"checked\"" : "", e_types->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.reply_property\">返信専用属性とする。</label>\n", e_types->id);
        o("\t\t\t\t\t<div class=\"description\">新規の登録時には、入力項目としないかどうかです。チェックした場合、新規の登録時には、入力項目になりません。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目種別</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"hidden\" name=\"field%d.element_type_id\" value=\"%d\" />\n", e_types->id, e_types->type);
        switch (e_types->type) {
            case ELEM_TEXT:
                o("1行テキスト(input[type=text])");
                break;
            case ELEM_TEXTAREA:
                o("複数行テキスト(textarea)");
                break;
            case ELEM_CHECKBOX:
                o("真偽値(input[type=checkbox])");
                break;
            case ELEM_RADIO:
                o("ラジオボタン(input[type=radio])");
                break;
            case ELEM_LIST_SINGLE:
                o("選択リスト(select)");
                break;
            case ELEM_LIST_MULTI:
                o("複数選択可能リスト(select[multiple=multiple])");
            case ELEM_UPLOADFILE:
                o("ファイル(input[type=file])");
                break;
        }
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        switch (e_types->type) {
            case ELEM_LIST_SINGLE:
            case ELEM_LIST_MULTI:
                o("\t\t\t<tr>\n");
                o("\t\t\t\t<th>選択要素</th>\n");
                o("\t\t\t\t<td>\n");
                o("\t\t\t\t\t<table class=\"list_item\">\n");
                o("\t\t\t\t\t\t<tr>\n");
                o("\t\t\t\t\t\t\t<th>選択肢名</th>\n");
                o("\t\t\t\t\t\t\t<th class=\"close\">クローズを意味する</th>\n");
                o("\t\t\t\t\t\t\t<th>並び順</th>\n");
                o("\t\t\t\t\t\t\t<th class=\"delete\">削除</th>\n");
                o("\t\t\t\t\t\t</tr>\n");
                for (; items != NULL; items = items->next) {
                    o("\t\t\t\t\t\t<tr>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item%d.name\" ", e_types->id, items->id);
                    o(                       "value=\"");h(items->name);o("\" />\n");
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.close\" value=\"1\" %s />\n", e_types->id, items->id, (items->close == 1) ? "checked=\"checked\"" : "");
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item%d.sort\" value=\"%d\" />\n", e_types->id, items->id, items->sort);
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.delete\" value=\"1\" />\n", e_types->id, items->id);
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t</tr>\n");
                }
                o("\t\t\t\t\t\t<tr>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item_new.name\" value=\"\" />\n", e_types->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item_new.close\" value=\"\" />\n", e_types->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item_new.sort\" value=\"\" />\n", e_types->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t&nbsp;\n");
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t</tr>\n");
                o("\t\t\t\t\t</table>\n");
                o("\t\t\t\t\t<div class=\"description\">項目種別が選択式の場合の選択肢です。</div>\n");
                o("\t\t\t\t</td>\n");
                o("\t\t\t</tr>\n");
                break;
        }
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>チケット一覧表示</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.display_in_list\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.display_in_list\" ", e_types->id, e_types->id);
        o(                  "value=\"1\" %s />\n", e_types->display_in_list == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.display_in_list\">項目をチケット一覧に表示する。</label>\n", e_types->id);
        o("\t\t\t\t\t<div class=\"description\">項目をチケット一覧で表示するかどうかです。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>並び順</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.sort\" value=\"%d\" maxlength=\"5\" />\n", e_types->id, e_types->sort);
        o("\t\t\t\t\t<div class=\"description\">チケット一覧、投稿画面、返信画面での項目の並び順です。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t</table>\n");
        if (e_types->id > BASIC_ELEMENT_MAX) {
            /* 基本項目は削除できないようにする。 */
            o("\t\t<div class=\"message\"><a href=\"%s/delete_item/%d\">", cgiScriptName, e_types->id);o("この項目(");h(e_types->name);o(")の削除</a></div>\n");
        }
    }
    o("\t\t<hr />\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o("\t</form>\n");
    o("</div>\n");





    output_footer();
    db_finish();
}
/**
 * 更新するaction。
 */
void update_action()
{
    bt_project* project;
    char smtp_port[DEFAULT_LENGTH];

    db_init();
    db_begin();
    project = db_get_project();
    cgiFormStringNoNewlines("project.name", project->name, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.description", project->description, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.home_url", project->home_url, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.host_name", project->host_name, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.smtp_server", project->smtp_server, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.smtp_port", smtp_port, DEFAULT_LENGTH);
    project->smtp_port = atoi(smtp_port);
    cgiFormStringNoNewlines("project.notify_address", project->notify_address, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.admin_address", project->admin_address, DEFAULT_LENGTH);

    db_update_project(project);

    project = db_get_project();
    update_elements();
    db_commit();
    db_finish();
    output_header(project, NULL);
    o(      "<h2>処理完了</h2>\n"
            "<div id=\"complete_message\">\n"
            "<h3>おつかれさまでした。</h3>\n");
        o("<div class=\"message\">更新しました</div>\n");
    o(      "</div>\n");
    output_footer();
}

void update_elements()
{
    char** ids;
    int i = -1;
    bt_element_type* e_type = NULL;
    if ((cgiFormStringMultiple("field_ids", &ids)) == cgiFormNotFound) {
        die("cannot find field_ids.");
    }
    while (ids[++i]) {
        char* id = ids[i];
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        bt_list_item* items = NULL;

        e_type = db_get_element_type(atoi(id));
/*         e_type = (bt_element_type*)xalloc(sizeof(bt_element_type)); */
/*         e_type->id = atoi(id); */

        sprintf(name, "field%s.name", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        strcpy(e_type->name, value);

        sprintf(name, "field%s.description", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        strcpy(e_type->description, value);

        sprintf(name, "field%s.ticket_property", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        e_type->ticket_property = atoi(value);

        sprintf(name, "field%s.reply_property", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        e_type->reply_property = atoi(value);

        sprintf(name, "field%s.required", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        e_type->required = atoi(value);

        sprintf(name, "field%s.display_in_list", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        e_type->display_in_list = atoi(value);

        sprintf(name, "field%s.sort", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        e_type->sort = atoi(value);

        db_update_element_type(e_type);

        /* list_item */
        switch (e_type->type) {
            case ELEM_LIST_SINGLE:
            case ELEM_LIST_MULTI:
                /* 選択要素のあるelementだけ、list_itemの更新を行なう。 */
                items = db_get_list_item(e_type->id);
                for (; items != NULL; items = items->next) {
                    strcpy(name, "");
                    strcpy(value, "");
                    sprintf(name, "field%d.list_item%d.name", e_type->id, items->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    strcpy(items->name, value);
                    sprintf(name, "field%d.list_item%d.close", e_type->id, items->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    items->close = atoi(value);
                    sprintf(name, "field%d.list_item%d.sort", e_type->id, items->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    items->sort = atoi(value);
                    sprintf(name, "field%d.list_item%d.delete", e_type->id, items->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    if (atoi(value) == 1)
                        db_delete_list_item(items->id);
                    else
                        db_update_list_item(items);
                }
                strcpy(name, "");
                strcpy(value, "");
                sprintf(name, "field%d.list_item_new.name", e_type->id);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                if (strlen(value) > 0) {
                    bt_list_item* item = (bt_list_item*)xalloc(sizeof(bt_list_item));
                    item->element_type_id = e_type->id;
                    strcpy(item->name, value);
                    sprintf(name, "field%d.list_item_new.close", e_type->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->close = atoi(value);
                    sprintf(name, "field%d.list_item_new.sort", e_type->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->sort = atoi(value);
                    db_register_list_item(item);
                }
                break;
        }

        free(e_type);
    }
    cgiStringArrayFree(ids);
}
void new_item_action()
{
    bt_project* project;
    int i;

    db_init();
    project = db_get_project();
    output_header(project, "new_item.js");

    o("<h2>%s 管理ツール</h2>", project->name);
    o(      "<div id=\"new_item\">\n"
            "<h3>項目の追加</h3>\n"
            "<form id=\"new_item_form\" action=\"%s/new_item_submit\" method=\"post\">\n"
            "<table summary=\"new item table\">\n", cgiScriptName);
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>項目名</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"field.name\" value=\"\" />\n");
    o("\t\t\t\t\t<div class=\"description\">項目名です。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>項目の説明文</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"field.description\" value=\"\" />\n");
    o("\t\t\t\t\t<div class=\"description\">項目の説明文です。投稿時に表示されます。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>チケット属性</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.ticket_property\" class=\"checkbox\" type=\"checkbox\" name=\"field.ticket_property\" ");
    o(                  "value=\"1\" />\n");
    o("\t\t\t\t\t<label for=\"field.ticket_property\">チケット属性とする。</label>\n");
    o("\t\t\t\t\t<div class=\"description\">返信の属性としてではなくチケット自体の属性として定義するかどうかです。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>返信専用属性</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.reply_property\" class=\"checkbox\" type=\"checkbox\" name=\"field.reply_property\" ");
    o(                  "value=\"1\" />\n");
    o("\t\t\t\t\t<label for=\"field.reply_property\">返信専用属性とする。</label>\n");
    o("\t\t\t\t\t<div class=\"description\">新規の登録時には、入力項目としないかどうかです。チェックした場合、新規の登録時には、入力項目になりません。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>項目種別</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" type=\"radio\" name=\"field.type\" ", ELEM_TEXT);
    o(                  "value=\"%d\" checked=\"checked\" />\n", ELEM_TEXT);
    o("\t\t\t\t\t<label for=\"field.type%d\" class=\"description\">1行テキスト(input[type=text])</label><br />\n", ELEM_TEXT);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TEXTAREA);
    o(                  "value=\"%d\" />\n", ELEM_TEXTAREA);
    o("\t\t\t\t\t<label for=\"field.type%d\" class=\"description\">複数行テキスト(textarea)</label><br />\n", ELEM_TEXTAREA);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_CHECKBOX);
    o(                  "value=\"%d\" disabled=\"disabled\" />\n", ELEM_CHECKBOX);
    o("\t\t\t\t\t<label for=\"field.type%d\" class=\"description\">真偽値(input[type=checkbox])</label><br />\n", ELEM_CHECKBOX);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_RADIO);
    o(                  "value=\"%d\" disabled=\"disabled\" />\n", ELEM_RADIO);
    o("\t\t\t\t\t<label for=\"field.type%d\" class=\"description\">ラジオボタン(input[type=radio])</label><br />\n", ELEM_RADIO);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_LIST_SINGLE);
    o(                  "value=\"%d\" />\n", ELEM_LIST_SINGLE);
    o("\t\t\t\t\t<label for=\"field.type%d\" class=\"description\">選択リスト(select)</label><br />\n", ELEM_LIST_SINGLE);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_LIST_MULTI);
    o(                  "value=\"%d\" />\n", ELEM_LIST_MULTI);
    o("\t\t\t\t\t<label for=\"field.type%d\" class=\"description\">複数選択可能リスト(select[multiple=multiple])</label><br />\n", ELEM_LIST_MULTI);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_UPLOADFILE);
    o(                  "value=\"%d\" />\n", ELEM_UPLOADFILE);
    o("\t\t\t\t\t<label for=\"field.type%d\" class=\"description\">ファイル(input[type=file])</label><br />\n", ELEM_UPLOADFILE);
    o("\t\t\t\t\t<div class=\"description\">項目を入力する時の入力形式です。項目種別は、追加後に変更することができません。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
            o("\t\t\t<tr>\n");
            o("\t\t\t\t<th>選択要素</th>\n");
            o("\t\t\t\t<td>\n");
            o("\t\t\t\t\t<table class=\"list_item\">\n");
            o("\t\t\t\t\t\t<tr>\n");
            o("\t\t\t\t\t\t\t<th>選択肢名</th>\n");
            o("\t\t\t\t\t\t\t<th class=\"close\">クローズを意味する</th>\n");
            o("\t\t\t\t\t\t\t<th>並び順</th>\n");
            o("\t\t\t\t\t\t</tr>\n");
            for (i = 0; i < ADD_ITEM_COUNT; i++) {
                o("\t\t\t\t\t\t<tr>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field.list_item_new%d.name\" value=\"\" />\n", i);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field.list_item_new%d.close\" value=\"\" />\n", i);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field.list_item_new%d.sort\" value=\"\" />\n", i);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t</tr>\n");
            }
            o("\t\t\t\t\t</table>\n");
            o("\t\t\t\t\t<div class=\"description\">項目種別が選択式の場合の選択肢です。</div>\n");
            o("\t\t\t\t</td>\n");
            o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>チケット一覧表示</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field.display_in_list\" ");
    o(                  "value=\"1\" />\n");
    o("\t\t\t\t\t<div class=\"description\">項目をチケット一覧で表示するかどうかです。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>並び順</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field.sort\" value=\"\" maxlength=\"5\" />\n");
    o("\t\t\t\t\t<div class=\"description\">チケット一覧、投稿画面、返信画面での項目の並び順です。</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t</table>\n");
    o("\t\t\t<input class=\"button\" type=\"submit\" value=\"追加\" />\n");
    o("\t\t\t</form>\n");
    o(      "</div>\n");
    db_finish();
    output_footer();
}
void new_item_submit_action()
{
    bt_element_type* e_type = (bt_element_type*)xalloc(sizeof(bt_element_type));
    bt_project* project;
    char value[DEFAULT_LENGTH];
    int i, e_type_id;

    db_init();
    db_begin();
    project = db_get_project();

    cgiFormStringNoNewlines("field.name", e_type->name, DEFAULT_LENGTH);

    cgiFormStringNoNewlines("field.description", e_type->description, DEFAULT_LENGTH);

    cgiFormStringNoNewlines("field.type", value, DEFAULT_LENGTH);
    e_type->type = atoi(value);

    cgiFormStringNoNewlines("field.ticket_property", value, DEFAULT_LENGTH);
    e_type->ticket_property = atoi(value);

    cgiFormStringNoNewlines("field.reply_property", value, DEFAULT_LENGTH);
    e_type->reply_property = atoi(value);

    cgiFormStringNoNewlines("field.display_in_list", value, DEFAULT_LENGTH);
    e_type->display_in_list = atoi(value);
    cgiFormStringNoNewlines("field.sort", value, DEFAULT_LENGTH);
    e_type->sort = atoi(value);
    e_type_id = db_register_element_type(e_type);
    switch (e_type->type) {
        case ELEM_LIST_SINGLE:
        case ELEM_LIST_MULTI:
            for (i = 0; i < ADD_ITEM_COUNT; i++) {
                char name[DEFAULT_LENGTH];
                bt_list_item* item = (bt_list_item*)xalloc(sizeof(bt_list_item));
                item->element_type_id = e_type_id;
                sprintf(name, "field.list_item_new%d.name", i);
                cgiFormStringNoNewlines(name, item->name, DEFAULT_LENGTH);
                if (strlen(item->name) > 0) {
                    sprintf(name, "field.list_item_new%d.close", i);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->close = atoi(value);
                    sprintf(name, "field.list_item_new%d.sort", i);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->sort = atoi(value);
                    db_register_list_item(item);
                }
            }
            break;
    }
    output_header(project, NULL);
    o("<h2>%s 管理ツール</h2>", project->name);
    o(      "<h2>処理完了</h2>\n"
            "<div id=\"complete_message\">\n"
            "<h3>おつかれさまでした。</h3>\n");
        o("<div class=\"message\">追加しました</div>\n");
    o(      "</div>\n");

    
    db_commit();
    db_finish();
    output_footer();
}
void delete_item_action()
{
    char path_info[DEFAULT_LENGTH];
    char* e_type_id;
    int iid;
    bt_project* project;
    bt_element_type* e_type;

    strcpy(path_info, cgiPathInfo);
    e_type_id = strchr(path_info + 1, '/');
    if (e_type_id) e_type_id++;
    iid = atoi(e_type_id);
    db_init();
    project = db_get_project();
    output_header(project, "delete_item.js");

    e_type = db_get_element_type(iid);
    o("<h2>%s 管理ツール</h2>", project->name);
    o(      "<div id=\"delete_item/%d\">\n", iid);
    o(      "<h3>項目(");h(e_type->name);o(")の削除</h3>\n"
            "<form id=\"delete_item_form\" action=\"%s/delete_item_submit/%d\" method=\"post\">\n"
            "<div class=\"message\"><strong>削除すると元には戻せません。"
            "登録されているチケットの項目についても参照できなくなります。</strong></div>"
            "<div class=\"message\">削除してよければ、削除ボタンをクリックしてください。</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"削除\" />\n"
            "</form>\n", cgiScriptName, iid);
    o(      "</div>\n");
    db_finish();
    output_footer();
}
void delete_item_submit_action()
{
    char path_info[DEFAULT_LENGTH];
    char* e_type_id;
    int iid;
    bt_project* project;

    strcpy(path_info, cgiPathInfo);
    e_type_id = strchr(path_info + 1, '/');
    if (e_type_id) e_type_id++;
    iid = atoi(e_type_id);
    db_init();
    project = db_get_project();
    output_header(project, NULL);

    db_delete_element_type(iid);
    o("<h2>%s 管理ツール</h2>", project->name);
    o(      "<h2>処理完了</h2>\n"
            "<div id=\"complete_message\">\n"
            "<h3>おつかれさまでした。</h3>\n");
        o("<div class=\"message\">削除しました</div>\n");
    db_finish();
    output_footer();
}
