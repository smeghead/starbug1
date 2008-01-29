#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db.h"
#include "dbutil.h"
#include "util.h"
#include "css.h"
#include "wiki.h"

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
void style_submit_action();
void display_action();
void new_item_action();
void new_item_submit_action();
void delete_item_action();
void delete_item_submit_action();
void admin_help_action();
void output_header(Project*, char*, char*, int);
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
    register_action_actions("style", style_action);
    register_action_actions("style_submit", style_submit_action);
    register_action_actions("new_item", new_item_action);
    register_action_actions("new_item_submit", new_item_submit_action);
    register_action_actions("delete_item", delete_item_action);
    register_action_actions("delete_item_submit", delete_item_submit_action);
    register_action_actions("admin_help", admin_help_action);
    register_action_actions("top", menu_action);
}

enum NAVI {
    NAVI_OTHER,
    NAVI_MENU,
    NAVI_PROJECT,
    NAVI_ENV,
    NAVI_ITEM,
    NAVI_STYLE,
    NAVI_ADMIN_HELP
};
void output_header(Project* project, char* title, char* script_name, int navi)
{
    cgiHeaderContentType("text/html; charset=utf-8;");
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />"
            "\t<title>%s 管理ツール - %s</title>\n", project->name, title);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/user.css\" />\n", cgiScriptName);
    if (script_name) {
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/prototype.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/%s\"></script>\n", cgiScriptName, script_name);
    }
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"管理ツール\"><a href=\"http://sourceforge.jp/projects/starbug1/\"><img src=\"%s/../img/title.jpg\" alt=\"Starbug1\" /></a></h1>\n"
            "<ul id=\"menu\">\n", cgiScriptName);
    o(      "\t<li><a %s href=\"%s\">管理ツールメニュー</a></li>\n", navi == NAVI_MENU ? "class=\"current\"" : "", cgiScriptName);
    o(      "\t\t<li><a %s href=\"%s/project\">プロジェクトの設定</a></li>\n", navi == NAVI_PROJECT ? "class=\"current\"" : "", cgiScriptName);
    o(      "\t\t<li><a %s href=\"%s/env\">環境設定</a></li>\n", navi == NAVI_ENV ? "class=\"current\"" : "", cgiScriptName);
    o(      "\t\t<li><a %s href=\"%s/items\">項目設定</a></li>\n", navi == NAVI_ITEM ? "class=\"current\"" : "", cgiScriptName);
    o(      "\t\t<li><a %s href=\"%s/style\">スタイル設定</a></li>\n", navi == NAVI_STYLE ? "class=\"current\"" : "", cgiScriptName);
    o(      "\t<li><a href=\"%s/../db/starbug1.db\">バックアップ</a></li>\n", cgiScriptName);
    o(      "\t\t<li><a %s href=\"%s/admin_help\">ヘルプ</a></li>\n", navi == NAVI_ADMIN_HELP ? "class=\"current\"" : "", cgiScriptName);
    o(      "\t<li><a href=\"%s/../index.cgi\">", cgiScriptName);h(project->name); o("トップへ</a></li>\n");
    o(      "</ul>\n"
            "<br clear=\"all\" />\n");
}
void output_footer()
{
    /* Finish up the page */
    o(      "<div id=\"footer\">\n"
            "<hr />\n"
            "<div align=\"right\">\n"
            "<p>\n"
            "\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
            "\t\t<img src=\"http://www.w3.org/Icons/valid-xhtml10\" alt=\"Valid XHTML 1.0 Transitional\" height=\"31\" width=\"88\" />\n"
            "\t</a>\n"
            "</p>\n"
            "<div><address>Powered by cgic.</address></div>\n"
            "<div><address>Starbug1 version: %s. Copyright smeghead 2007 - 2008.</address></div>\n"
            "</div>\n"
            "</div>\n"
            "</body>\n</html>\n", VERSION);
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
    Project* project;
    char message[DEFAULT_LENGTH];

    d("admin\n");
    db_init();
    project = db_get_project();
    output_header(project, "メニュー", NULL, NAVI_MENU);

    cgiFormStringNoNewlines("message", message, DEFAULT_LENGTH);
    if (strlen(message) > 0) {
        o("<div class=\"complete_message\">"); h(message); o("&nbsp;</div>\n");
    }
    o("<h2>%s 管理ツール</h2>", project->name);
    o("<div id=\"admin_menu\">\n");
    o("\t<dl>\n");
    o("\t\t<dt><a href=\"%s/project\">プロジェクト設定</a></dt><dd>プロジェクトの基本的な情報の設定です。</dd>\n", cgiScriptName);
    o("\t\t<dt><a href=\"%s/env\">環境設定</a></dt><dd>URLやメール関連の設定です。</dd>\n", cgiScriptName);
    o("\t\t<dt><a href=\"%s/items\">項目設定</a></dt><dd>チケットの項目についての設定です。</dd>\n", cgiScriptName);
    o("\t\t<dt><a href=\"%s/style\">スタイル設定</a></dt><dd>スタイルシートの設定です。</dd>\n", cgiScriptName);
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
    Project* project;

    db_init();
    project = db_get_project();
    output_header(project, "プロジェクト設定", "management.js", NAVI_PROJECT);

    o("<h2>%s 管理ツール</h2>", project->name);
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/project_submit\" method=\"post\">\n", cgiScriptName);
    o("\t\t<h3>プロジェクト設定</h3>\n");
    o("\t\t<table summary=\"project table\">\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>プロジェクト名</th>\n");
    o("\t\t\t\t<td><input type=\"text\" name=\"project.name\" value=\"");h(project->name);o("\" maxlength=\"1023\" /></td>\n");
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
    Project* project;

    db_init();
    db_begin();
    project = db_get_project();
    cgiFormStringNoNewlines("project.name", project->name, DEFAULT_LENGTH);
    db_update_project(project);

    project = db_get_project();
    db_commit();
    db_finish();
    redirect("", "更新しました");
}
/**
 * 環境設定の設定画面をを表示するaction。
 */
void env_action()
{
    Project* project;

    db_init();
    project = db_get_project();
    output_header(project, "環境設定", "management.js", NAVI_ENV);

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
    o("\t\t<div class=\"description\">サーバ名、通知先アドレス、管理者アドレスが入力されていない場合、メール通知機能は機能しません。</div>\n");
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
    Project* project;
    char smtp_port[DEFAULT_LENGTH];

    db_init();
    db_begin();
    project = db_get_project();
    cgiFormStringNoNewlines("project.home_url", project->home_url, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.smtp_server", project->smtp_server, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.smtp_port", smtp_port, DEFAULT_LENGTH);
    project->smtp_port = atoi(smtp_port);
    cgiFormStringNoNewlines("project.notify_address", project->notify_address, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("project.admin_address", project->admin_address, DEFAULT_LENGTH);

    db_update_project(project);

    project = db_get_project();
    db_commit();
    db_finish();
    redirect("", "更新しました");
}
/**
 * 項目設定画面をを表示するaction。
 */
void items_action()
{
    Project* project;
    List* element_types_a;
    Iterator* it;

    db_init();
    project = db_get_project();
    output_header(project, "項目設定", "management.js", NAVI_ITEM);

    o("<h2>%s 管理ツール</h2>", project->name);
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/items_submit\" method=\"post\">\n", cgiScriptName);
    o("\t\t<h3>項目設定</h3>\n");
    o("\t\t<div class=\"description\">チケットID、投稿日時は編集できません。件名、投稿者、状態は、基本属性のため編集に制限があります。(削除不可、チケット属性、返信専用属性変更不可)</div>\n");
    o("\t\t<div><a href=\"%s/new_item\">新規項目の追加</a></div>\n", cgiScriptName);

    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(element_types_a);
    o("\t\t<ul id=\"field_list\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        o("\t\t<li><a href=\"#field%d\">", et->id); h(et->name); o("</a></li>\n");
    }
    o("\t\t</ul>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        List* items_a;
        Iterator* it;
        list_alloc(items_a, ListItem);
        items_a = db_get_list_item(et->id, items_a);
        o("\t\t<hr />\n");
        o("\t\t<a name=\"field%d\"></a>\n", et->id);
        o("\t\t<h4>"); h(et->name); o("</h4>\n");
        o("\t\t<div class=\"item_navigation\"><a href=\"#top\">このページのトップへ</a></div>\n");
        o("\t\t<input type=\"hidden\" name=\"field_ids\" value=\"%d\" />\n", et->id);
        o("\t\t<table class=\"item_table\" summary=\"item table\">\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目名</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"text\" name=\"field%d.name\" ", et->id);
        o(                  "value=\"");h(et->name);o("\" />\n");
        o("\t\t\t\t\t<div class=\"description\">項目名です。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目の説明文</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"text\" name=\"field%d.description\" ", et->id);
        o(                  "value=\"");h(et->description);o("\" />\n");
        o("\t\t\t\t\t<div class=\"description\">項目の説明文です。投稿時に表示されます。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>必須項目</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.required\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.required\" ", et->id, et->id);
        o(                  "value=\"1\" %s />\n", et->required == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.required\">必須項目とする。</label>\n", et->id);
        o("\t\t\t\t\t<div class=\"description\">入力しないと投稿できない属性とするかどうかです。ただし、チェックはJavaScriptで行なうのみです。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>チケット属性</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.ticket_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.ticket_property\" ", et->id, et->id);
        o(                  "value=\"1\" %s %s />\n", et->ticket_property == 1 ? "checked=\"checked\"" : "", et->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.ticket_property\">チケット属性とする。</label>\n", et->id);
        o("\t\t\t\t\t<div class=\"description\">\n");
        o("\t\t\t\t\t\t返信の属性としてではなくチケット自体の属性として定義するかどうかです。\n");
        o("\t\t\t\t\t\t返信時にチケット属性を指定した項目を書き換えた場合、書き換えた内容がチケット最新情報に表示されるようになります。\n");
        o("\t\t\t\t\t\tチケット属性を指定していない項目は、チケット最新情報に表示されません。チケット属性を指定していない項目はチケット履歴から参照することになります。\n");
        o("\t\t\t\t\t</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>返信専用属性</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.reply_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.reply_property\" ", et->id, et->id);
        o(                  "value=\"1\" %s %s />\n", et->reply_property == 1 ? "checked=\"checked\"" : "", et->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.reply_property\">返信専用属性とする。</label>\n", et->id);
        o("\t\t\t\t\t<div class=\"description\">\n");
        o("\t\t\t\t\t\t新規の登録時には、入力項目としないかどうかです。チェックした場合、新規の登録時には、入力項目になりません。\n");
        o("\t\t\t\t\t\t返信の時のみ入力できる項目となります。チケットの対応時期などの返信する人でなければわからない項目は、返信専用属性とすることで、新規登録時の不要な入力の手間を減らすことができます。\n");
        o("\t\t\t\t\t</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>項目種別</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input type=\"hidden\" name=\"field%d.element_type_id\" value=\"%d\" />\n", et->id, et->type);
        switch (et->type) {
            case ELEM_TYPE_TEXT:
                o("1行テキスト(input[type=text])");
                break;
            case ELEM_TYPE_TEXTAREA:
                o("複数行テキスト(textarea)");
                break;
            case ELEM_TYPE_CHECKBOX:
                o("真偽値(input[type=checkbox])");
                break;
            case ELEM_TYPE_LIST_SINGLE:
                o("選択リスト(select)");
                break;
            case ELEM_TYPE_LIST_MULTI:
                o("複数選択可能リスト(select[multiple=multiple])");
                break;
            case ELEM_TYPE_UPLOADFILE:
                o("ファイル(input[type=file])");
                break;
            case ELEM_TYPE_DATE:
                o("日付(input[type=text])");
                break;
        }
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        switch (et->type) {
            case ELEM_TYPE_LIST_SINGLE:
            case ELEM_TYPE_LIST_MULTI:
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
                foreach (it, items_a) {
                    ListItem* item = it->element;
                    o("\t\t\t\t\t\t<tr>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item%d.name\" ", et->id, item->id);
                    o(                       "value=\"");h(item->name);o("\" />\n");
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.close\" value=\"1\" %s />\n", et->id, item->id, (item->close == 1) ? "checked=\"checked\"" : "");
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item%d.sort\" value=\"%d\" />\n", et->id, item->id, item->sort);
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t\t<td>\n");
                    o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.delete\" value=\"1\" />\n", et->id, item->id);
                    o("\t\t\t\t\t\t\t</td>\n");
                    o("\t\t\t\t\t\t</tr>\n");
                }
                o("\t\t\t\t\t\t<tr>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item_new.name\" value=\"\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item_new.close\" value=\"\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item_new.sort\" value=\"\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t&nbsp;\n");
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t</tr>\n");
                o("\t\t\t\t\t</table>\n");
                o("\t\t\t\t\t<div class=\"description\">項目種別が選択式の場合の選択肢です。</div>\n");
                o("\t\t\t\t\t<input id=\"field%d.auto_add_item\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.auto_add_item\" ", et->id, et->id);
                o(                  "value=\"1\" %s />\n", et->auto_add_item == 1 ? "checked=\"checked\"" : "");
                o("\t\t\t\t\t<label for=\"field%d.auto_add_item\">投稿時に、新規項目を指定可能とする。</label>\n", et->id);
                o("\t\t\t\t\t<div class=\"description\">投稿時にテキストボックスを表示し、テキストボックスに入力された場合は、その項目を選択肢に追加する機能を付加するかどうかです。</div>\n");
                o("\t\t\t\t</td>\n");
                o("\t\t\t</tr>\n");
                break;
        }
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>デフォルト値</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.default_value\" value=\"%s\" maxlength=\"1000\" />\n", et->id, et->default_value);
        o("\t\t\t\t\t<div class=\"description\">投稿画面、返信画面での項目の初期値です。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>チケット一覧表示</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.display_in_list\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.display_in_list\" ", et->id, et->id);
        o(                  "value=\"1\" %s />\n", et->display_in_list == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.display_in_list\">項目をチケット一覧に表示する。</label>\n", et->id);
        o("\t\t\t\t\t<div class=\"description\">項目をチケット一覧で表示するかどうかです。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t\t<tr>\n");
        o("\t\t\t\t<th>並び順</th>\n");
        o("\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.sort\" value=\"%d\" maxlength=\"5\" />\n", et->id, et->sort);
        o("\t\t\t\t\t<div class=\"description\">チケット一覧、投稿画面、返信画面での項目の並び順です。</div>\n");
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        o("\t\t</table>\n");
        if (et->id > BASIC_ELEMENT_MAX) {
            /* 基本項目は削除できないようにする。 */
            o("\t\t<div><a href=\"%s/delete_item/%d\">", cgiScriptName, et->id);o("この項目(");h(et->name);o(")の削除</a></div>\n");
        }
        list_free(items_a);
    }
    list_free(element_types_a);
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
    Project* project;

    db_init();
    db_begin();
    project = db_get_project();
    update_elements();
    db_commit();
    db_finish();
    redirect("", "更新しました");
}

void update_elements()
{
    char** ids;
    int i = -1;
    if ((cgiFormStringMultiple("field_ids", &ids)) == cgiFormNotFound) {
        die("cannot find field_ids.");
    }
    while (ids[++i]) {
        char* id = ids[i];
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        List* items_a;
        Iterator* it;
        ElementType* et_a = xalloc(sizeof(ElementType));

        et_a = db_get_element_type(atoi(id), et_a);

        d("while id: %s\n", id);
        d("while\n");
        sprintf(name, "field%s.name", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        strcpy(et_a->name, value);

        sprintf(name, "field%s.description", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        strcpy(et_a->description, value);

        sprintf(name, "field%s.ticket_property", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->ticket_property = atoi(value);

        sprintf(name, "field%s.reply_property", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->reply_property = atoi(value);

        sprintf(name, "field%s.required", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->required = atoi(value);

        sprintf(name, "field%s.display_in_list", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->display_in_list = atoi(value);

        sprintf(name, "field%s.sort", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->sort = atoi(value);

        sprintf(name, "field%s.default_value", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        strcpy(et_a->default_value, value);

        sprintf(name, "field%s.auto_add_item", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        d("auto_add_item: %s\n", value);
        et_a->auto_add_item = atoi(value);
        d("auto_add_item: %d\n", et_a->auto_add_item);

        db_update_element_type(et_a);

        /* list_item */
        switch (et_a->type) {
            case ELEM_TYPE_LIST_SINGLE:
            case ELEM_TYPE_LIST_MULTI:
                /* 選択要素のあるelementだけ、list_itemの更新を行なう。 */
                list_alloc(items_a, ListItem);
                items_a = db_get_list_item(et_a->id, items_a);
                foreach (it, items_a) {
                    ListItem* item = it->element;
                    strcpy(name, "");
                    strcpy(value, "");
                    sprintf(name, "field%d.list_item%d.name", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    strcpy(item->name, value);
                    sprintf(name, "field%d.list_item%d.close", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->close = atoi(value);
                    sprintf(name, "field%d.list_item%d.sort", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->sort = atoi(value);
                    sprintf(name, "field%d.list_item%d.delete", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    if (atoi(value) == 1)
                        db_delete_list_item(item->id);
                    else
                        db_update_list_item(item);
                }
                list_free(items_a);
                strcpy(name, "");
                strcpy(value, "");
                sprintf(name, "field%d.list_item_new.name", et_a->id);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                if (strlen(value) > 0) {
                    ListItem* item_a = xalloc(sizeof(ListItem));
                    item_a->element_type_id = et_a->id;
                    strcpy(item_a->name, value);
                    sprintf(name, "field%d.list_item_new.close", et_a->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item_a->close = atoi(value);
                    sprintf(name, "field%d.list_item_new.sort", et_a->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item_a->sort = atoi(value);
                    db_register_list_item(item_a);
                    xfree(item_a);
                }
                break;
        }

        xfree(et_a);
    }
    cgiStringArrayFree(ids);
}
void new_item_action()
{
    Project* project;
    int i;

    db_init();
    project = db_get_project();
    output_header(project, "新規項目登録", "new_item.js", NAVI_OTHER);

    o("<h2>%s 管理ツール</h2>", project->name);
    o(      "<div id=\"new_item\">\n"
            "<h3>項目の追加</h3>\n"
            "<div class=\"description\">チケットに新しい項目を追加します。追加する項目についての情報を入力し、追加ボタンを押してください。</div>\n"
            "<form id=\"new_item_form\" action=\"%s/new_item_submit\" method=\"post\">\n"
            "<table class=\"item_table\" summary=\"new item table\">\n", cgiScriptName);
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
    o("\t\t\t\t\t<div class=\"description\">\n");
    o("\t\t\t\t\t\t返信の属性としてではなくチケット自体の属性として定義するかどうかです。\n");
    o("\t\t\t\t\t\t返信時にチケット属性を指定した項目を書き換えた場合、書き換えた内容がチケット最新情報に表示されるようになります。\n");
    o("\t\t\t\t\t\tチケット属性を指定していない項目は、チケット最新情報に表示されません。チケット属性を指定していない項目はチケット履歴から参照することになります。\n");
    o("\t\t\t\t\t</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>返信専用属性</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.reply_property\" class=\"checkbox\" type=\"checkbox\" name=\"field.reply_property\" ");
    o(                  "value=\"1\" />\n");
    o("\t\t\t\t\t<label for=\"field.reply_property\">返信専用属性とする。</label>\n");
    o("\t\t\t\t\t<div class=\"description\">\n");
    o("\t\t\t\t\t\t新規の登録時には、入力項目としないかどうかです。チェックした場合、新規の登録時には、入力項目になりません。\n");
    o("\t\t\t\t\t\t返信の時のみ入力できる項目となります。チケットの対応時期などの返信する人でなければわからない項目は、返信専用属性とすることで、新規登録時の不要な入力の手間を減らすことができます。\n");
    o("\t\t\t\t\t</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>項目種別</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" type=\"radio\" name=\"field.type\" ", ELEM_TYPE_TEXT);
    o(                  "value=\"%d\" checked=\"checked\" />\n", ELEM_TYPE_TEXT);
    o("\t\t\t\t\t<label for=\"field.type%d\">1行テキスト(input[type=text])</label><br />\n", ELEM_TYPE_TEXT);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_TEXTAREA);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_TEXTAREA);
    o("\t\t\t\t\t<label for=\"field.type%d\">複数行テキスト(textarea)</label><br />\n", ELEM_TYPE_TEXTAREA);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_CHECKBOX);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_CHECKBOX);
    o("\t\t\t\t\t<label for=\"field.type%d\">真偽値(input[type=checkbox])</label><br />\n", ELEM_TYPE_CHECKBOX);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_LIST_SINGLE);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_LIST_SINGLE);
    o("\t\t\t\t\t<label for=\"field.type%d\">選択リスト(select)</label><br />\n", ELEM_TYPE_LIST_SINGLE);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_LIST_MULTI);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_LIST_MULTI);
    o("\t\t\t\t\t<label for=\"field.type%d\">複数選択可能リスト(select[multiple=multiple])</label><br />\n", ELEM_TYPE_LIST_MULTI);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_UPLOADFILE);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_UPLOADFILE);
    o("\t\t\t\t\t<label for=\"field.type%d\">ファイル(input[type=file])</label><br />\n", ELEM_TYPE_UPLOADFILE);
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_DATE);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_DATE);
    o("\t\t\t\t\t<label for=\"field.type%d\">日付(input[type=text])</label><br />\n", ELEM_TYPE_DATE);
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
            o("\t\t\t\t\t<input id=\"field.auto_add_item\" class=\"checkbox\" type=\"checkbox\" name=\"field.auto_add_item\" ");
            o(                  "value=\"1\" />\n");
            o("\t\t\t\t\t<label for=\"field.auto_add_item\">投稿時に、新規項目を指定可能とする。</label>\n");
            o("\t\t\t\t\t<div class=\"description\">投稿時にテキストボックスを表示し、テキストボックスに入力された場合は、その項目を選択肢に追加する機能を付加するかどうかです。</div>\n");
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
    ElementType* et_a = xalloc(sizeof(ElementType));
    Project* project;
    char value[DEFAULT_LENGTH];
    int i, e_type_id;

    db_init();
    db_begin();
    project = db_get_project();

    cgiFormStringNoNewlines("field.name", et_a->name, DEFAULT_LENGTH);

    cgiFormStringNoNewlines("field.description", et_a->description, DEFAULT_LENGTH);

    cgiFormStringNoNewlines("field.type", value, DEFAULT_LENGTH);
    et_a->type = atoi(value);

    cgiFormStringNoNewlines("field.ticket_property", value, DEFAULT_LENGTH);
    et_a->ticket_property = atoi(value);

    cgiFormStringNoNewlines("field.reply_property", value, DEFAULT_LENGTH);
    et_a->reply_property = atoi(value);

    cgiFormStringNoNewlines("field.display_in_list", value, DEFAULT_LENGTH);
    et_a->display_in_list = atoi(value);
    cgiFormStringNoNewlines("field.auto_add_item", value, DEFAULT_LENGTH);
    et_a->auto_add_item = atoi(value);
    cgiFormStringNoNewlines("field.sort", value, DEFAULT_LENGTH);
    et_a->sort = atoi(value);
    e_type_id = db_register_element_type(et_a);
    switch (et_a->type) {
        case ELEM_TYPE_LIST_SINGLE:
        case ELEM_TYPE_LIST_MULTI:
            for (i = 0; i < ADD_ITEM_COUNT; i++) {
                char name[DEFAULT_LENGTH];
                ListItem* item_a = xalloc(sizeof(ListItem));
                item_a->element_type_id = e_type_id;
                sprintf(name, "field.list_item_new%d.name", i);
                cgiFormStringNoNewlines(name, item_a->name, DEFAULT_LENGTH);
                if (strlen(item_a->name) > 0) {
                    sprintf(name, "field.list_item_new%d.close", i);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item_a->close = atoi(value);
                    sprintf(name, "field.list_item_new%d.sort", i);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item_a->sort = atoi(value);
                    db_register_list_item(item_a);
                }
                xfree(item_a);
            }
            break;
    }
    xfree(et_a);
    db_commit();
    db_finish();
    redirect("", "追加しました");
}
void delete_item_action()
{
    char path_info[DEFAULT_LENGTH];
    char* e_type_id;
    int iid;
    Project* project;
    ElementType* et_a = xalloc(sizeof(ElementType));

    strcpy(path_info, cgiPathInfo);
    e_type_id = strchr(path_info + 1, '/');
    if (e_type_id) e_type_id++;
    iid = atoi(e_type_id);
    db_init();
    project = db_get_project();
    output_header(project, "項目削除", "delete_item.js", NAVI_OTHER);

    et_a = db_get_element_type(iid, et_a);
    o("<h2>%s 管理ツール</h2>", project->name);
    o(      "<div id=\"delete_item/%d\">\n", iid);
    o(      "<h3>項目(");h(et_a->name);o(")の削除</h3>\n"
            "<form id=\"delete_item_form\" action=\"%s/delete_item_submit/%d\" method=\"post\">\n"
            "<div class=\"infomation\"><strong>削除すると元には戻せません。"
            "登録されているチケットの項目についても参照できなくなります。</strong></div>"
            "<div class=\"infomation\">削除してよければ、削除ボタンを押してください。</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"削除\" />\n"
            "</form>\n", cgiScriptName, iid);
    o(      "</div>\n");
    db_finish();
    output_footer();
    xfree(et_a);
}
void delete_item_submit_action()
{
    char path_info[DEFAULT_LENGTH];
    char* e_type_id;
    int iid;

    strcpy(path_info, cgiPathInfo);
    e_type_id = strchr(path_info + 1, '/');
    if (e_type_id) e_type_id++;
    iid = atoi(e_type_id);
    db_init();
    db_begin();

    db_delete_element_type(iid);
    db_commit();
    redirect("", "削除しました");
    db_finish();

}
void style_action()
{
    Project* project;
    db_init();
    project = db_get_project();
    output_header(project, "スタイル設定", "style.js", NAVI_STYLE);
    o(      "<h2>スタイル編集</h2>\n"
            "<div id=\"top\">\n"
            "<h3>スタイルシートの編集</h3>\n"
            "<div id=\"description\">スタイルシートの編集を行ない、更新ボタンを押してください。</div>\n"
            "<form id=\"edit_css_form\" action=\"%s/style_submit\" method=\"post\">\n", cgiScriptName);
    o(      "<textarea name=\"edit_css\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    css_content_out("css/user.css");
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"更新\" />\n"
            "</form>");
    {
        List* element_types_a;
        Iterator* it;
        o(      "<div class=\"description\">以下は、一覧の背景設定用のサンプルです。</div>\n");
        o(      "<pre>\n");
        list_alloc(element_types_a, ElementType);
        element_types_a = db_get_element_types_all(element_types_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            if (et->type == ELEM_TYPE_LIST_SINGLE && et->display_in_list) {
                List* items_a;
                Iterator* it_item;
                list_alloc(items_a, ListItem);
                items_a = db_get_list_item(et->id, items_a);
                o(  "/* ================================ */ \n"
                    "/* チケット一覧の"); h(et->name); o("の背景色設定     */\n");
                o(  "/* ================================ */ \n");
                foreach (it_item, items_a) {
                    ListItem* item = it_item->element;
                    o("/* "); h(item->name); o(" */\n");
                    o("#ticket_list td.field%d-", et->id);
                    css_field(item->name);
                    o(" {\n");
                    o(" background-color: lightyellow !important;\n");
                    o("}\n");
                }
            }
        }
        list_free(element_types_a);
        o(      "</pre>\n");
    }
    o(      "</div>\n");
    db_finish();
    output_footer();
}
void style_submit_action()
{
    char* value_a = xalloc(sizeof(char) * VALUE_LENGTH);

    cgiFormString("edit_css", value_a, VALUE_LENGTH);
    css_save("css/user.css", value_a);

    redirect("", "更新しました。");
    xfree(value_a);
}
void admin_help_action()
{
    Project* project;
    db_init();
    project = db_get_project();
    output_header(project, "ヘルプ", NULL, NAVI_ADMIN_HELP);
    o(      "<h2>");h(project->name);o("</h2>\n"
            "<div id=\"top\">\n");
    wiki_out("adminhelp");
    o(      "</div>\n");
    db_finish();
    output_footer();
}
/* vim: set ts=4 sw=4 sts=4 expandtab: */
