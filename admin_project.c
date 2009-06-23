#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db_project.h"
#include "db_top.h"
#include "dbutil.h"
#include "alloc.h"
#include "util.h"
#include "wiki.h"

#define ADD_ITEM_COUNT 10

typedef enum _NAVI {
    NAVI_OTHER,
    NAVI_MENU,
    NAVI_PROJECT,
    NAVI_ITEM,
    NAVI_STYLE,
    NAVI_ADMIN_HELP
} NaviType;

/* prototype declares */
void register_actions();
void top_action();
void project_action();
void project_submit_action();
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
void output_header(Project*, char*, char*, NaviType);
void output_footer();
int admin_project_main();
void update_elements();

void register_actions()
{
    REG_ACTION(top);
    REG_ACTION(project);
    REG_ACTION(project_submit);
    REG_ACTION(items);
    REG_ACTION(items_submit);
    REG_ACTION(style);
    REG_ACTION(style_submit);
    REG_ACTION(new_item);
    REG_ACTION(new_item_submit);
    REG_ACTION(delete_item);
    REG_ACTION(delete_item_submit);
    REG_ACTION(admin_help);
}

void output_header(Project* project, char* title, char* script_name, NaviType navi)
{
    cgiHeaderContentType("text/html; charset=utf-8;");
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />");
    o(        "\t<title>管理ツール - "); h(string_rawstr(project->name)); o(" - "); h(title); o("</title>\n");
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../index.%s/%s/setting_file/user.css\" />\n", cgiScriptName, get_ext(cgiScriptName), g_project_name_4_url);
    if (script_name) {
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/prototype.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/%s\"></script>\n", cgiScriptName, script_name);
    }
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://starbug1.sourceforge.jp/\"><img src=\"%s/../index.%s/%s/setting_file/top_image\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName, get_ext(cgiScriptName), g_project_name_4_url);
    o(      "<div id='menu'>\n"
            "<ul>\n");
    o(      "\t<li><a href=\"%s/../index.%s/top/\" title=\"トップページのサブプロジェクト一覧を表示します\">トップページ（サブプロジェクト一覧)</a></li>\n", cgiScriptName, get_ext(cgiScriptName));
    o(      "</ul>\n"
            "<br clear=\"all\" />\n"
            "</div>\n");
    o(      "<ul id=\"projectmenu\">\n");
    o(      "\t<li><a %s href=\"%s/%s/\">管理ツールメニュー</a></li>\n", navi == NAVI_MENU ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o(      "\t\t<li><a %s href=\"%s/%s/project\">サブプロジェクト設定</a></li>\n", navi == NAVI_PROJECT ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o(      "\t\t<li><a %s href=\"%s/%s/items\">項目設定</a></li>\n", navi == NAVI_ITEM ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o(      "\t\t<li><a %s href=\"%s/%s/style\">スタイル設定</a></li>\n", navi == NAVI_STYLE ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o(      "\t\t<li><a %s href=\"%s/%s/admin_help\">ヘルプ</a></li>\n", navi == NAVI_ADMIN_HELP ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o(      "\t<li><a href=\"%s/../index.%s/%s/\">", cgiScriptName, get_ext(cgiScriptName), g_project_name_4_url);h(string_rawstr(project->name)); o("へ</a></li>\n");
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
            "\t\t<img src=\"%s/../img/valid-xhtml10.gif\" alt=\"Valid XHTML 1.0 Transitional\" height=\"31\" width=\"88\" />\n"
            "\t</a>\n"
            "</p>\n"
            "<div><address>Powered by cgic &amp; SQLite3.</address></div>\n"
            "<div><address>Starbug1 version: %s. %s.</address></div>\n"
            "</div>\n"
            "</div>\n"
            "</body>\n</html>\n", cgiScriptName, VERSION, COPYRIGHT);
}
void output_field_information_js() {
    o("<script type=\"text/javascript\" src=\"%s/../js/validate.js\"></script>\n", cgiScriptName); 
}
int admin_project_main() {
    register_actions();
    exec_action();
    free_action_actions();
    return 0;
}
/**
 * 管理メニュー画面をを表示するaction。
 */
void top_action()
{
    Project* project_a = project_new();
    char message[DEFAULT_LENGTH];
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "メニュー", NULL, NAVI_MENU);

    cgiFormStringNoNewlines("message", message, DEFAULT_LENGTH);
    if (strlen(message) > 0) {
        o("<div class=\"complete_message\">"); h(message); o("&nbsp;</div>\n");
    }
    o("<h2>"); h(string_rawstr(project_a->name)); o(" 管理ツール</h2>");
    project_free(project_a);
    o("<div id=\"main_body\">\n");
    o("<h3>管理ツールメニュー</h3>\n");
    o("<div id=\"admin_menu\">\n");
    o("\t<dl>\n");
    o("\t\t<dt><a href=\"%s/%s/project\">プロジェクト設定</a></dt><dd>プロジェクトの基本的な情報の設定です。</dd>\n", cgiScriptName, g_project_name_4_url);
    o("\t\t<dt><a href=\"%s/%s/items\">項目設定</a></dt><dd>チケットの項目についての設定です。</dd>\n", cgiScriptName, g_project_name_4_url);
    o("\t\t<dt><a href=\"%s/%s/style\">スタイル設定</a></dt><dd>スタイルシートの設定です。</dd>\n", cgiScriptName, g_project_name_4_url);
    o("\t</dl>\n");
    o("</div>\n");
    o("<h3>メール送信機能について</h3>\n");
    o("<div class=\"description\">\n");
    o("\t投稿時、返信時のメール送信は、hook機能により実現可能です。hook機能はサーバ上での設定が必要になります。hook機能の設定方法については、以下のサイトのインストールのページを見てください。\n");
    o("\t<ul>\n"
      "\t\t<li><a href=\"http://starbug1.sourceforge.jp/\">http://starbug1.sourceforge.jp/</a></li>\n"
      "\t</ul>\n");
    o("</div>\n");
    o("</div>\n");

    output_footer();
    db_finish(db_a);
}
/**
 * サブプロジェクト設定画面をを表示するaction。
 */
void project_action()
{
    Project* project_a = project_new();
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "サブプロジェクト設定", "management.js", NAVI_PROJECT);

    o("<h2>%s 管理ツール</h2>", string_rawstr(project_a->name));
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/%s/project_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName, g_project_name_4_url);
    o("\t\t<h3>サブプロジェクト設定</h3>\n");
    o("\t\t<table summary=\"project table\">\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>サブプロジェクト名</th>\n");
    o("\t\t\t\t<td><input type=\"text\" name=\"project.name\" value=\"");h(string_rawstr(project_a->name));o("\" maxlength=\"1000\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>画像(ページの一番上に表示される画像です)</th>\n");
    o("\t\t\t\t<td><input type=\"file\" name=\"project.file\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>ファイルアップロードの最大サイズ(Kb)</th>\n");
    o("\t\t\t\t<td><input type=\"text\" name=\"project.upload_max_size\" value=\"%d\" /></td>\n", project_a->upload_max_size);
    o("\t\t\t</tr>\n");
    o("\t\t</table>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o("\t</form>\n");
    o("</div>\n");
    project_free(project_a);
    output_footer();
    db_finish(db_a);
}
void fill_upload_content_setting_file(SettingFile* sf)
{
    int got_count = 0;
    char* buffer;
    char file_name[DEFAULT_LENGTH];
    char b[DEFAULT_LENGTH];
    cgiFilePtr file;
    string_set(sf->name, "top_image");
    cgiFormFileSize("project.file", &sf->size);
    cgiFormFileName("project.file", file_name, DEFAULT_LENGTH);
    string_set(sf->file_name, file_name);
    cgiFormFileContentType("project.file", string_rawstr(sf->mime_type), DEFAULT_LENGTH);
    buffer = sf->content = xalloc(sizeof(char) * sf->size);
    if (cgiFormFileOpen("project.file", &file) != cgiFormSuccess) {
        die("Could not open the file.");
    }

    while (cgiFormFileRead(file, b, sizeof(b), &got_count) == cgiFormSuccess) {
        char* p = b;
        int i;
        for (i = 0; i < got_count; i++) {
            *buffer = *p;
            buffer++;
            p++;
        }
    }
    cgiFormFileClose(file);
}
/**
 * サブプロジェクト設定を更新するaction。
 */
void project_submit_action()
{
    Project* project_a = project_new();
    SettingFile* sf_a = setting_file_new();
    Database* db_a;
    char name[DEFAULT_LENGTH];
    char upload_max_size_str[DEFAULT_LENGTH];
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    db_begin(db_a);
    project_a = db_get_project(db_a, project_a);
    cgiFormStringNoNewlines("project.name", name, DEFAULT_LENGTH);
    string_set(project_a->name, name);
    cgiFormStringNoNewlines("project.upload_max_size", upload_max_size_str, DEFAULT_LENGTH);
    project_a->upload_max_size = atoi(upload_max_size_str);
    db_update_project(db_a, project_a);
    project_free(project_a);

    /* 画像の更新 */
    cgiFormFileSize("project.file", &(sf_a->size));
    d("image size %d.\n", sf_a->size);
    if (sf_a->size > 0) {
        d("try to save top image.\n");
        fill_upload_content_setting_file(sf_a);
        db_update_top_image(db_a, sf_a);
    }
    setting_file_free(sf_a);
    db_commit(db_a);
    db_finish(db_a);
    redirect("", "更新しました");
}
/**
 * 項目設定画面をを表示するaction。
 */
void items_action()
{
    Project* project_a = project_new();
    List* element_types_a;
    Iterator* it;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "項目設定", "management.js", NAVI_ITEM);

    o("<h2>%s 管理ツール</h2>", string_rawstr(project_a->name));
    o("<div id=\"top\">\n");
    project_free(project_a);
    o("<div id=\"setting_form\">\n"
      "\t<form id=\"management_form\" action=\"%s/%s/items_submit\" method=\"post\">\n", cgiScriptName, g_project_name_4_url);
    o("\t\t<h3>項目設定</h3>\n"
      "\t\t<div class=\"description\">チケットID、投稿日時は編集できません。件名、投稿者、状態は、基本属性のため編集できる内容に制限があります。(削除不可、チケット属性、返信専用属性変更不可)</div>\n"
      "\t\t<div><a href=\"%s/%s/new_item\">新規項目の追加</a></div>\n", cgiScriptName, g_project_name_4_url);

    list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
    element_types_a = db_get_element_types_all(db_a, element_types_a);
    output_field_information_js();
    o("\t\t<ul id=\"field_list\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        o("\t\t<li><a href=\"#field%d\">", et->id); hs(et->name); o("</a></li>\n");
    }
    o("\t\t</ul>\n"
      "\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        List* items_a;
        Iterator* it;
        list_alloc(items_a, ListItem, list_item_new, list_item_free);
        items_a = db_get_list_item(db_a, et->id, items_a);
        o("\t\t<a name=\"field%d\"></a>\n", et->id);
        o("\t\t<h4>"); hs(et->name); o("&nbsp;<a href=\"#top\">↑</a></h4>\n");
        o("\t\t<input type=\"hidden\" name=\"field_ids\" value=\"%d\" />\n", et->id);
        o("\t\t<table class=\"item_table\" summary=\"item table\">\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th class=\"required\">項目名<span class=\"required\">※</span></th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input class=\"required\" id=\"field%d.name\" type=\"text\" name=\"field%d.name\" ", et->id, et->id);
        o(                  "value=\"");hs(et->name);o("\" maxlength=\"1000\" />\n"
          "\t\t\t\t\t<div class=\"description\">項目名です。</div>\n"
          "\t\t\t\t\t<div id=\"field%d.name.required\" class=\"error\"></div>\n", et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>項目の説明文</th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input type=\"text\" name=\"field%d.description\" ", et->id);
        o(                  "value=\"");hs(et->description);o("\" maxlength=\"1000\" />\n"
          "\t\t\t\t\t<div class=\"description\">項目の説明文です。投稿時に表示されます。</div>\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>必須項目</th>\n"
          "\t\t\t\t<td>\n");
        o("\t\t\t\t\t<input id=\"field%d.required\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.required\" ", et->id, et->id);
        o(                  "value=\"1\" %s />\n", et->required == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.required\">必須項目とする。</label>\n", et->id);
        o("\t\t\t\t\t<div class=\"description\">入力しないと投稿できない属性とするかどうかです。ただし、チェックはJavaScriptで行なうのみです。</div>\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>チケット属性</th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input id=\"field%d.ticket_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.ticket_property\" ", et->id, et->id);
        o(                  "value=\"1\" %s %s />\n", et->ticket_property == 1 ? "checked=\"checked\"" : "", et->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.ticket_property\">チケット属性とする。</label>\n", et->id);
        o("\t\t\t\t\t<div class=\"description\">\n"
          "\t\t\t\t\t\t返信の属性としてではなくチケット自体の属性として定義するかどうかです。\n"
          "\t\t\t\t\t\t返信時にチケット属性を指定した項目を書き換えた場合、書き換えた内容がチケット最新情報に表示されるようになります。\n"
          "\t\t\t\t\t\tチケット属性を指定していない項目は、チケット最新情報に表示されません。チケット属性を指定していない項目はチケット履歴から参照することになります。\n"
          "\t\t\t\t\t</div>\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>返信専用属性</th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input id=\"field%d.reply_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.reply_property\" ", et->id, et->id);
        o(                  "value=\"1\" %s %s />\n", et->reply_property == 1 ? "checked=\"checked\"" : "", et->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.reply_property\">返信専用属性とする。</label>\n", et->id);
        o("\t\t\t\t\t<div class=\"description\">\n"
          "\t\t\t\t\t\t新規の登録時には、入力項目としないかどうかです。チェックした場合、新規の登録時には、入力項目になりません。\n"
          "\t\t\t\t\t\t返信の時のみ入力できる項目となります。チケットの対応時期などの返信する人でなければわからない項目は、返信専用属性とすることで、新規登録時の不要な入力の手間を減らすことができます。\n"
          "\t\t\t\t\t</div>\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>項目種別</th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input type=\"hidden\" name=\"field%d.element_type_id\" value=\"%d\" />\n", et->id, et->type);
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
                o("\t\t\t<tr>\n"
                  "\t\t\t\t<th>選択要素</th>\n"
                  "\t\t\t\t<td>\n"
                  "\t\t\t\t\t<table class=\"list_item\">\n"
                  "\t\t\t\t\t\t<tr>\n"
                  "\t\t\t\t\t\t\t<th>選択肢名</th>\n"
                  "\t\t\t\t\t\t\t<th class=\"close\">クローズを意味する</th>\n"
                  "\t\t\t\t\t\t\t<th>並び順</th>\n"
                  "\t\t\t\t\t\t\t<th class=\"delete\">削除</th>\n"
                  "\t\t\t\t\t\t</tr>\n");
                foreach (it, items_a) {
                    ListItem* item = it->element;
                    o("\t\t\t\t\t\t<tr>\n"
                      "\t\t\t\t\t\t\t<td>\n"
                      "\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item%d.name\" ", et->id, item->id);
                    o(                       "value=\"");hs(item->name);o("\" maxlength=\"1000\" />\n"
                      "\t\t\t\t\t\t\t</td>\n"
                      "\t\t\t\t\t\t\t<td class=\"center\">\n"
                      "\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.close\" value=\"1\" %s />\n", et->id, item->id, (item->close == 1) ? "checked=\"checked\"" : "");
                    o("\t\t\t\t\t\t\t</td>\n"
                      "\t\t\t\t\t\t\t<td>\n"
                      "\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item%d.sort\" value=\"%d\" maxlength=\"1000\" />\n", et->id, item->id, item->sort);
                    o("\t\t\t\t\t\t\t</td>\n"
                      "\t\t\t\t\t\t\t<td class=\"center\">\n"
                      "\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.delete\" value=\"1\" />\n", et->id, item->id);
                    o("\t\t\t\t\t\t\t</td>\n"
                      "\t\t\t\t\t\t</tr>\n");
                }
                o("\t\t\t\t\t\t<tr>\n"
                  "\t\t\t\t\t\t\t<td>\n"
                  "\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item_new.name0\" value=\"\" maxlength=\"1000\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n"
                  "\t\t\t\t\t\t\t<td class=\"center\">\n"
                  "\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item_new.close0\" value=\"\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n"
                  "\t\t\t\t\t\t\t<td>\n"
                  "\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item_new.sort0\" value=\"\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n"
                  "\t\t\t\t\t\t\t<td class=\"center\">\n"
                  "\t\t\t\t\t\t\t\t<script type=\"text/javascript\">\n"
                  "\t\t\t\t\t\t\t\t\t<!--\n"
                  "\t\t\t\t\t\t\t\t\tdocument.write('<input type=\"button\" value=\"行追加\" id=\"add_new_row.field%d\" class=\"add_new_row\" />');\n"
                  "\t\t\t\t\t\t\t\t\t//-->\n"
                  "\t\t\t\t\t\t\t\t</script>\n"
                  "\t\t\t\t\t\t\t</td>\n"
                  "\t\t\t\t\t\t</tr>\n"
                  "\t\t\t\t\t</table>\n"
                  "\t\t\t\t\t<div class=\"description\">項目種別が選択式の場合の選択肢です。</div>\n"
                  "\t\t\t\t\t<input id=\"field%d.auto_add_item\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.auto_add_item\" ", et->id, et->id, et->id);
                o(                  "value=\"1\" %s />\n", et->auto_add_item == 1 ? "checked=\"checked\"" : "");
                o("\t\t\t\t\t<label for=\"field%d.auto_add_item\">投稿時に、新規項目を指定可能とする。</label>\n", et->id);
                o("\t\t\t\t\t<div class=\"description\">投稿時にテキストボックスを表示し、テキストボックスに入力された場合は、その項目を選択肢に追加する機能を付加するかどうかです。</div>\n"
                  "\t\t\t\t</td>\n"
                  "\t\t\t</tr>\n");
                break;
        }
        o("\t\t\t<tr>\n"
          "\t\t\t\t<th>デフォルト値</th>\n"
          "\t\t\t\t<td>\n");
        if (et->type == ELEM_TYPE_TEXTAREA) {
            o("\t\t\t\t\t<textarea name=\"field%d.default_value\" rows=\"2\" cols=\"10\" >", et->id);hs(et->default_value);o("</textarea>\n");
        } else {
            o("\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.default_value\" value=\"%s\" maxlength=\"1000\" />\n", et->id, string_rawstr(et->default_value));
        }
        o("\t\t\t\t\t<div class=\"description\">投稿画面、返信画面での項目の初期値です。</div>\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>チケット一覧表示</th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input id=\"field%d.display_in_list\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.display_in_list\" ", et->id, et->id);
        o(                  "value=\"1\" %s maxlength=\"1000\" />\n", et->display_in_list == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.display_in_list\">項目をチケット一覧に表示する。</label>\n", et->id);
        o("\t\t\t\t\t<div class=\"description\">項目をチケット一覧で表示するかどうかです。</div>\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>並び順</th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.sort\" value=\"%d\" maxlength=\"5\" />\n", et->id, et->sort);
        o("\t\t\t\t\t<div class=\"description\">チケット一覧、投稿画面、返信画面での項目の並び順です。</div>\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t</table>\n");
        o("\t\t<table style=\"display:none\">\n"
          "\t\t\t<tbody id=\"row_template\">\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field[id].list_item_new.name[no]\" value=\"\" maxlength=\"1000\" />\n"
          "\t\t\t\t</td>\n"
          "\t\t\t\t<td class=\"center\">\n"
          "\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field[id].list_item_new.close[no]\" value=\"\" />\n"
          "\t\t\t\t</td>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field[id].list_item_new.sort[no]\" value=\"\" />\n"
          "\t\t\t\t</td>\n"
          "\t\t\t\t<td class=\"center\">\n"
          "\t\t\t\t\t&nbsp;\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t</tbody>\n"
          "\t\t</table>\n");
        if (et->id > BASIC_ELEMENT_MAX) {
            /* 基本項目は削除できないようにする。 */
            o("\t\t<div class=\"delete_item\"><a href=\"%s/%s/delete_item/%d\">", cgiScriptName, g_project_name_4_url, et->id);o("この項目(");hs(et->name);o(")の削除</a></div>\n");
        }
        list_free(items_a);
        o("\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    }
    list_free(element_types_a);
    o("\t</form>\n"
      "</div>\n"
      "</div>\n");

    output_footer();
    db_finish(db_a);
}
/**
 * 項目設定を更新するaction。
 */
void items_submit_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    db_begin(db_a);
    update_elements();
    db_commit(db_a);
    db_finish(db_a);
    redirect("", "更新しました");
}

void update_elements(Database* db)
{
    char** ids;
    int i = -1;

    if ((cgiFormStringMultiple("field_ids", &ids)) == cgiFormNotFound) {
        die("cannot find field_ids.");
    }
    while (ids[++i]) {
        int j;
        char* id = ids[i];
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        List* items_a;
        Iterator* it;
        ElementType* et_a = element_type_new();

        et_a = db_get_element_type(db, atoi(id), et_a);

        sprintf(name, "field%s.name", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        string_set(et_a->name, value);

        sprintf(name, "field%s.description", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        string_set(et_a->description, value);

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
        cgiFormString(name, value, DEFAULT_LENGTH);
        string_set(et_a->default_value, value);

        sprintf(name, "field%s.auto_add_item", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->auto_add_item = atoi(value);

        db_update_element_type(db, et_a);

        /* list_item */
        switch (et_a->type) {
            case ELEM_TYPE_LIST_SINGLE:
            case ELEM_TYPE_LIST_MULTI:
                /* 選択要素のあるelementだけ、list_itemの更新を行なう。 */
                list_alloc(items_a, ListItem, list_item_new, list_item_free);
                items_a = db_get_list_item(db, et_a->id, items_a);
                foreach (it, items_a) {
                    ListItem* item = it->element;
                    strcpy(name, "");
                    strcpy(value, "");
                    sprintf(name, "field%d.list_item%d.name", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    string_set(item->name, value);
                    sprintf(name, "field%d.list_item%d.close", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->close = atoi(value);
                    sprintf(name, "field%d.list_item%d.sort", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->sort = atoi(value);
                    sprintf(name, "field%d.list_item%d.delete", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    if (atoi(value) == 1)
                        db_delete_list_item(db, item->id);
                    else
                        db_update_list_item(db, item);
                }
                list_free(items_a);
                //新しい項目の追加
                for (j = 0; j < 100; j++) {
                    strcpy(name, "");
                    strcpy(value, "");
                    sprintf(name, "field%d.list_item_new.name%d", et_a->id, j);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    if (strlen(value) > 0) {
                        ListItem* item_a = list_item_new();
                        item_a->element_type_id = et_a->id;
                        d("%s\n", value);
                        string_set(item_a->name, value);
                        sprintf(name, "field%d.list_item_new.close%d", et_a->id, j);
                        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                        item_a->close = atoi(value);
                        sprintf(name, "field%d.list_item_new.sort%d", et_a->id, j);
                        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                        item_a->sort = atoi(value);
                        db_register_list_item(db, item_a);
                        xfree(item_a);
                    } else {
                        //名称が取得できない場合は、追加終了
                        break;
                    }
                }
                break;
        }

        element_type_free(et_a);
    }
    cgiStringArrayFree(ids);
}
void new_item_action()
{
    Project* project_a = project_new();
    int i;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "新規項目登録", "new_item.js", NAVI_OTHER);

    o("<h2>%s 管理ツール</h2>", string_rawstr(project_a->name));
    project_free(project_a);
    o(      "<div id=\"new_item\">\n"
            "<h3>項目の追加</h3>\n"
            "<div class=\"description\">チケットに新しい項目を追加します。追加する項目についての情報を入力し、追加ボタンを押してください。</div>\n"
            "<form id=\"new_item_form\" action=\"%s/%s/new_item_submit\" method=\"post\">\n"
            "<table class=\"item_table\" summary=\"new item table\">\n", cgiScriptName, g_project_name_4_url);
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th class=\"required\">項目名<span class=\"required\">※</span></th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.name\" type=\"text\" name=\"field.name\" value=\"\" maxlength=\"1000\" />\n");
    o("\t\t\t\t\t<div class=\"description\">項目名です。</div>\n");
    o("\t\t\t\t\t<div id=\"field.name.required\" class=\"error\"></div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>項目の説明文</th>\n");
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"field.description\" value=\"\" maxlength=\"1000\" />\n");
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
                o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field.list_item_new%d.name\" value=\"\" maxlength=\"1000\" />\n", i);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field.list_item_new%d.close\" value=\"\" />\n", i);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field.list_item_new%d.sort\" value=\"\" maxlength=\"5\" />\n", i);
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
    db_finish(db_a);
    output_footer();
}
void new_item_submit_action()
{
    ElementType* et_a = element_type_new();
    char value[DEFAULT_LENGTH];
    int i, e_type_id;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    db_begin(db_a);

    cgiFormStringNoNewlines("field.name", value, DEFAULT_LENGTH);
    string_set(et_a->name, value);

    cgiFormStringNoNewlines("field.description", value, DEFAULT_LENGTH);
    string_set(et_a->description, value);

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
    e_type_id = db_register_element_type(db_a, et_a);
    switch (et_a->type) {
        case ELEM_TYPE_LIST_SINGLE:
        case ELEM_TYPE_LIST_MULTI:
            for (i = 0; i < ADD_ITEM_COUNT; i++) {
                char name[DEFAULT_LENGTH];
                ListItem* item_a = list_item_new();
                item_a->element_type_id = e_type_id;
                sprintf(name, "field.list_item_new%d.name", i);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                string_set(item_a->name, value);
                if (string_len(item_a->name) > 0) {
                    sprintf(name, "field.list_item_new%d.close", i);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item_a->close = atoi(value);
                    sprintf(name, "field.list_item_new%d.sort", i);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item_a->sort = atoi(value);
                    db_register_list_item(db_a, item_a);
                }
                xfree(item_a);
            }
            break;
    }
    element_type_free(et_a);
    db_commit(db_a);
    db_finish(db_a);
    redirect("", "追加しました");
}
void delete_item_action()
{
    char* e_type_id;
    int iid;
    Project* project_a = project_new();
    ElementType* et_a = element_type_new();
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    d("g_path_info: %s\n", g_path_info);
    e_type_id = g_path_info;
    iid = atoi(e_type_id);
    d("iid: %d\n", iid);
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "項目削除", "delete_item.js", NAVI_OTHER);

    et_a = db_get_element_type(db_a, iid, et_a);
    o("<h2>%s 管理ツール</h2>", string_rawstr(project_a->name));
    project_free(project_a);
    o(      "<div id=\"delete_item/%d\">\n", iid);
    o(      "<h3>項目(");hs(et_a->name);o(")の削除</h3>\n"
            "<form id=\"delete_item_form\" action=\"%s/%s/delete_item_submit/%d\" method=\"post\">\n"
            "<div class=\"infomation\"><strong>削除すると元には戻せません。"
            "登録されているチケットの項目についても参照できなくなります。</strong></div>"
            "<div class=\"infomation\">削除してよければ、削除ボタンを押してください。</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"削除\" />\n"
            "</form>\n", cgiScriptName, g_project_name_4_url, iid);
    o(      "</div>\n");
    db_finish(db_a);
    output_footer();
    element_type_free(et_a);
}
void delete_item_submit_action()
{
    char* e_type_id;
    int iid;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    /* postの時以外は処理しない。 */
    if (strcmp(cgiRequestMethod, "POST") != 0) {
        die("不正なリクエストです。");
    }
    d("g_path_info: %s\n", g_path_info);
    e_type_id = g_path_info;
    iid = atoi(e_type_id);
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    db_begin(db_a);

    db_delete_element_type(db_a, iid);
    db_commit(db_a);
    redirect("", "削除しました");
    db_finish(db_a);

}
void style_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    Project* project_a = project_new();
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "スタイル設定", "style.js", NAVI_STYLE);
    o(      "<h2>%s 管理ツール</h2>", string_rawstr(project_a->name));
    project_free(project_a);
    o(      "<div id=\"top\">\n"
            "<h3>スタイルシートの編集</h3>\n"
            "<div id=\"description\">スタイルシートの編集を行ない、更新ボタンを押してください。</div>\n"
            "<form id=\"edit_css_form\" action=\"%s/%s/style_submit\" method=\"post\">\n", cgiScriptName, g_project_name_4_url);
    o(      "<textarea name=\"edit_css\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    {
        /* user.css の出力 */
        SettingFile* file_a = setting_file_new();
        file_a = db_get_setting_file(db_a, "user.css", file_a);
        fwrite(file_a->content, sizeof(char), file_a->size, cgiOut);
        setting_file_free(file_a);
    }
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"更新\" />\n"
            "</form>");
    {
        List* element_types_a;
        Iterator* it;
        o(      "<div class=\"description\">以下は、一覧の背景設定用のサンプルです。</div>\n");
        o(      "<pre>\n");
        list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
        element_types_a = db_get_element_types_all(db_a, element_types_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            if (et->type == ELEM_TYPE_LIST_SINGLE && et->display_in_list) {
                List* items_a;
                Iterator* it_item;
                list_alloc(items_a, ListItem, list_item_new, list_item_free);
                items_a = db_get_list_item(db_a, et->id, items_a);
                o(  "/* ================================ */ \n"
                    "/* チケット一覧の"); hs(et->name); o("の背景色設定     */\n");
                o(  "/* ================================ */ \n");
                foreach (it_item, items_a) {
                    ListItem* item = it_item->element;
                    o("/* "); hs(item->name); o(" */\n");
                    o("#ticket_list td.field%d-%d", et->id, item->id);
                    o(" {\n");
                    o(" background-color: lightyellow !important;\n");
                    o("}\n");
                }
                list_free(items_a);
            }
        }
        list_free(element_types_a);
        o(      "</pre>\n");
    }
    o(      "</div>\n");
    db_finish(db_a);
    output_footer();
}
void style_submit_action()
{
    SettingFile* sf_a = setting_file_new();
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    string_set(sf_a->name, "user.css");
    string_set(sf_a->file_name, "");
    sf_a->content = xalloc(sizeof(char) * VALUE_LENGTH);

    cgiFormString("edit_css", sf_a->content, VALUE_LENGTH);
    sf_a->size = strlen(sf_a->content);
    string_set(sf_a->mime_type, "text/css");
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    db_setting_file_save(db_a, sf_a);
    db_finish(db_a);

    redirect("", "更新しました。");
    setting_file_free(sf_a);
}
void admin_help_action()
{
    Project* project_a = project_new();
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "ヘルプ", NULL, NAVI_ADMIN_HELP);
    o(      "<h2>");h(string_rawstr(project_a->name));o("</h2>\n"
            "<div id=\"top\">\n");
    wiki_out(db_a, "adminhelp");
    o(      "</div>\n");
    project_free(project_a);
    db_finish(db_a);
    output_footer();
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
