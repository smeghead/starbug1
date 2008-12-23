#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db_project.h"
#include "db_top.h"
#include "dbutil.h"
#include "list.h"
#include "util.h"
#include "wiki.h"
#include "hook.h"
#include "csv.h"
#include "simple_string.h"

typedef enum _NAVI {
    NAVI_OTHER,
    NAVI_TOP,
    NAVI_LIST,
    NAVI_REGISTER,
    NAVI_REGISTER_AT_ONCE,
    NAVI_SEARCH,
    NAVI_RSS,
    NAVI_HELP,
    NAVI_STATISTICS,
    NAVI_MANAGEMENT
} NaviType;
typedef enum _MODE {
    MODE_INVALID,
    MODE_REGISTER,
    MODE_REPLY
} ModeType;

/* prototype declares */
void register_actions();
void list_action();
void search_action();
void register_at_once_action();
void register_at_once_confirm_action();
void register_at_once_submit_action();
void register_submit_action();
void register_action();
void ticket_action();
void statistics_action();
void help_action();
void edit_top_action();
void edit_top_submit_action();
void download_action();
void report_csv_download_action();
void rss_action();
void top_action();
void setting_file_action();
void output_header(Project*, char*, char*, const NaviType);
void output_footer();
int public_main();
void output_form_element(Database*, List*, ElementType*);
void output_form_element_4_condition(Database*, ElementType*, List* conditions);
ModeType get_mode();
void output_calendar_js();

#define COOKIE_SENDER "starbug1_sender"
#define COOKIE_SAVE_CONDITION "starbug1_save_condition"
#define COOKIE_CONDITION_FORMAT "starbug1_field%d"
#define COOKIE_CONDITION_KEYWORD "starbug1_keyword"
ModeType get_mode()
{
    char mode[MODE_LENGTH];
    cgiFormStringNoNewlines("register", mode, MODE_LENGTH);
    if (strlen(mode) > 0) return MODE_REGISTER;
    cgiFormStringNoNewlines("reply", mode, MODE_LENGTH);
    if (strlen(mode) > 0) return MODE_REPLY;
    return MODE_INVALID;
}
void register_actions()
{
    REG_ACTION(list);
    REG_ACTION(search);
    REG_ACTION(register);
    REG_ACTION(register_at_once);
    REG_ACTION(register_at_once_confirm);
    REG_ACTION(register_at_once_submit);
    REG_ACTION(register_submit);
    REG_ACTION(ticket);
    REG_ACTION(statistics);
    REG_ACTION(help);
    REG_ACTION(edit_top);
    REG_ACTION(edit_top_submit);
    REG_ACTION(download);
    REG_ACTION(rss);
    REG_ACTION(report_csv_download);
    REG_ACTION(top);
    REG_ACTION(setting_file);
}

void output_header(Project* project, char* title, char* script_name, const NaviType navi)
{

    String* base_url_a = string_new(0);
    base_url_a = get_base_url(base_url_a);
    o("Pragma: no-cache\r\n");
    o("Cache-Control: no-cache\t\n");
    cgiHeaderContentType("text/html; charset=utf-8;");
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />\n");
    o(        "\t<title>"); h(project->name); o(" - "); h(title); o("</title>\n");
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/%s/setting_file/user.css\" />\n", cgiScriptName, g_project_name_4_url);
    if (script_name) {
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/prototype.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/%s\"></script>\n", cgiScriptName, script_name);
    }
    o(      "\t<link rel=\"alternate\" type=\"application/rss+xml\" title=\"RSS\" href=\"%s/rss\" />\n", string_rawstr(base_url_a));
    string_free(base_url_a);
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://starbug1.sourceforge.jp/\"><img src=\"%s/%s/setting_file/top_image\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName, g_project_name_4_url);
    o("<div id='menu'>\n"
      "<ul>\n");
    o("\t<li><a href='%s/%s/rss' title=\"RSS Feed\"><img src=\"%s/../img/rss.png\" alt=\"rss\" /></a></li>\n", cgiScriptName, g_project_name_4_url, cgiScriptName);
    o("\t<li><a href='%s/top/' title=\"トップページのサブプロジェクト一覧を表示します\">トップページ(サブプロジェクト一覧)</a></li>\n", cgiScriptName);
    o("</ul>\n"
      "<br clear='all' />\n"
      "</div>\n");
    o("<div>\n"
      "<ul id='projectmenu'>\n");
    o("\t<li><a %s href='%s/%s/' title=\"サブプロジェクトトップへ移動します\">サブプロジェクトトップ</a></li>\n", navi == NAVI_TOP ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o("\t<li><a %s href='%s/%s/list' title=\"状態別のチケット一覧を参照します\">状態別チケット一覧</a></li>\n", navi == NAVI_LIST ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o("\t<li><a %s href='%s/%s/register' title=\"新規にチケットを登録します\">チケット登録</a></li>\n", navi == NAVI_REGISTER ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o("\t<li><a %s href='%s/%s/search' title=\"チケットを検索します\">チケット検索</a></li>\n", navi == NAVI_SEARCH ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o("\t<li><a %s href='%s/%s/statistics' title=\"統計情報を表示します\">統計情報</a></li>\n", navi == NAVI_STATISTICS ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o("\t<li><a %s href='%s/%s/register_at_once' title=\"複数新規にチケットを登録します\">チケット一括登録</a></li>\n", navi == NAVI_REGISTER_AT_ONCE ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o("\t<li><a %s href='%s/%s/help' title=\"ヘルプを参照します\">ヘルプ</a></li>\n", navi == NAVI_HELP ? "class=\"current\"" : "", cgiScriptName, g_project_name_4_url);
    o("\t<li><a %s href='%s/../admin.%s/%s' title=\"各種設定を行ないます\">サブプロジェクトの管理</a></li>\n", navi == NAVI_MANAGEMENT ? "class=\"current\"" : "", cgiScriptName, get_ext(cgiScriptName), g_project_name_4_url);
    o("</ul>\n"
      "<br clear='all' />\n"
      "</div>\n");
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
            "<div><address>Starbug1 version: %s. Copyright smeghead 2007 - 2008.</address></div>\n"
            "</div>\n"
            "</div>\n"
            "</body>\n</html>\n", cgiScriptName, VERSION);
}
bool is_enabled_project(char* project_name)
{
    Database* db_a;
    ProjectInfo* project_info_a = project_info_new();
    int id = 0;
    db_a = db_init("db/1.db");
    project_info_a = db_top_get_project_info(db_a, project_info_a, project_name);
    id = project_info_a->id;
    project_info_free(project_info_a);
    db_finish(db_a);
    d("id: %d\n", id);
    return id ? 1 : 0;
}
int index_project_main() {
    register_actions();
    if (is_enabled_project(g_project_name) == 0) {
        page_not_found();
        return 0;
    }
    exec_action();
    free_action_actions();
    return 0;
}
void output_navigater(SearchResult* result, char* query_string)
{
    int i;
    if (result->hit_count < LIST_COUNT_PER_SEARCH_PAGE) return;
    o("<div class=\"navigater\">\n");
    if (result->page > 0)
        o("<a href=\"%s/%s/search?p=%d&amp;%s\">&lt;&lt;</a>\n", cgiScriptName, g_project_name_4_url, result->page - 1, query_string);
    for (i = 0; i * LIST_COUNT_PER_SEARCH_PAGE < result->hit_count; i++) {
        if (i < result->page - 5 || i > result->page + 5)
            continue;
        if (i == result->page)
            o("%d\n", i + 1);
        else
            o("<a href=\"%s/%s/search?p=%d&amp;%s\">%d</a>\n", cgiScriptName, g_project_name_4_url, i, query_string, i + 1);
    }
    if (result->page * LIST_COUNT_PER_SEARCH_PAGE < result->hit_count - LIST_COUNT_PER_SEARCH_PAGE)
        o("<a href=\"%s/%s/search?p=%d&amp;%s\">&gt;&gt;</a>\n", cgiScriptName, g_project_name_4_url, result->page + 1, query_string);
    o("</div>\n");
}
String* format_query_string_without_page(String* buffer)
{
    char **array, **arrayStep;
    if (cgiFormEntries(&array) != cgiFormSuccess) {
        return buffer;
    }
    arrayStep = array;
    while (*arrayStep) {
        if (strcmp(*arrayStep, "p") != 0) {
            char name[DEFAULT_LENGTH] = "";
            char value[DEFAULT_LENGTH] = "";
            char encodedvalue[DEFAULT_LENGTH] = "";
            strcpy(name, *arrayStep);
            cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
            url_encode((unsigned char*)value, (unsigned char*)encodedvalue, DEFAULT_LENGTH);
            string_append(buffer, name);
            string_append(buffer, "=");
            string_append(buffer, encodedvalue);
            string_append(buffer, "&amp;");
        }
        arrayStep++;
    }
    cgiStringArrayFree(array);

    return buffer;
}
String* format_query_string_without_sort_and_page(String* buffer)
{
    char **array, **arrayStep;
    if (cgiFormEntries(&array) != cgiFormSuccess) {
        return buffer;
    }
    arrayStep = array;
    while (*arrayStep) {
        if (strcmp(*arrayStep, "rsort") != 0 &&
                strcmp(*arrayStep, "sort") != 0 &&
                strcmp(*arrayStep, "p") != 0) {
            char name[DEFAULT_LENGTH] = "";
            char value[DEFAULT_LENGTH] = "";
            char encodedvalue[DEFAULT_LENGTH] = "";
            strcpy(name, *arrayStep);
            cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
            url_encode((unsigned char*)value, (unsigned char*)encodedvalue, DEFAULT_LENGTH);
            string_append(buffer, name);
            string_append(buffer, "=");
            string_append(buffer, encodedvalue);
            string_append(buffer, "&amp;");
        }
        arrayStep++;
    }
    cgiStringArrayFree(array);

    return buffer;
}
void output_ticket_table_header_no_link(List* element_types)
{
    Iterator* it;

    o(      "\t<tr>\n");
    o(      "\t\t<th class=\"id\">ID</th>\n");
    foreach (it, element_types) {
        ElementType* et = it->element;
        o("\t\t<th class=\"field%d\">", et->id);
        h(et->name);
        o(    "</th>\n");
    }
    o("\t\t<th class=\"registerdate\">投稿日時</th>\n"
      "\t\t<th class=\"lastregisterdate\">最終更新日時</th>\n"
      "\t\t<th class=\"leftdate\">放置日数</th>\n"
      "\t</tr>\n");
}
void output_ticket_table_header(List* element_types)
{
    Iterator* it;
    char* reverse = strstr(cgiQueryString, "rsort=");
    String* query_string_a = string_new(0);
    query_string_a = format_query_string_without_sort_and_page(query_string_a);

    o(      "\t<tr>\n");
    o(      "\t\t<th class=\"id\"><a href=\"%s/%s/search?%ssort=-1&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">ID</a></th>\n", cgiScriptName, g_project_name_4_url, reverse ? "" : "r", string_rawstr(query_string_a));
    foreach (it, element_types) {
        ElementType* et = it->element;
        o("\t\t<th class=\"field%d\">\n", et->id);
        o("\t\t\t<a href=\"%s/%s/search?%ssort=%d&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">", cgiScriptName, g_project_name_4_url, reverse ? "" : "r", et->id, string_rawstr(query_string_a));
        h(et->name);
        o("</a>\n");
        o("\t\t</th>\n");
    }
    o("\t\t<th class=\"registerdate\"><a href=\"%s/%s/search?%ssort=-2&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">投稿日時</a></th>\n", cgiScriptName, g_project_name_4_url, reverse ? "" : "r", string_rawstr(query_string_a));
    o("\t\t<th class=\"lastregisterdate\"><a href=\"%s/%s/search?%ssort=-3&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">最終更新日時</a></th>\n", cgiScriptName, g_project_name_4_url, reverse ? "" : "r", string_rawstr(query_string_a));
    o("\t\t<th class=\"leftdate\"><a href=\"%s/%s/search?%ssort=-3&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">放置日数</a></th>\n", cgiScriptName, g_project_name_4_url, reverse ? "" : "r", string_rawstr(query_string_a));
    o("\t</tr>\n");
    string_free(query_string_a);
}
void output_ticket_table_body(Database* db, SearchResult* result, List* element_types)
{
    Iterator* it;
    Iterator* it_msg;

    foreach (it_msg, result->messages) {
        Message* message = it_msg->element;
        List* elements_a;
        list_alloc(elements_a, Element);
        elements_a = db_get_last_elements_4_list(db, message->id, elements_a);
        o("\t<tr>\n"
          "\t\t<td class=\"id field%d-\"><a href=\"%s/%s/ticket/%s\">%s</a></td>\n", 
                ELEM_ID_ID, 
                cgiScriptName, 
                g_project_name_4_url,
                get_element_value_by_id(elements_a, ELEM_ID_ID), 
                get_element_value_by_id(elements_a, ELEM_ID_ID));
        foreach (it, element_types) {
            ElementType* et = it->element;
            char* val = get_element_value_by_id(elements_a, et->id);
            o("\t\t<td class=\"field%d field%d-", et->id, et->id); 
            if (et->type == ELEM_TYPE_LIST_SINGLE)
                css_field(val);
            o("\">");
            if (et->id == ELEM_ID_TITLE)
                o("<a href=\"%s/%s/ticket/%d\">", cgiScriptName, g_project_name_4_url, message->id);
            if (et->id == ELEM_ID_SENDER)
                hmail(get_element_value_by_id(elements_a, ELEM_ID_ORG_SENDER)); /* 最初の投稿者を表示する。 */
            else
                h(val);
            if (et->id == ELEM_ID_TITLE)
                o("</a>");
            o("&nbsp;</td>\n");
        }
        o("\t\t<td class=\"registerdate\">"); h(get_element_value_by_id(elements_a, ELEM_ID_REGISTERDATE)); o("&nbsp;</td>\n");
        o("\t\t<td class=\"lastregisterdate\">"); h(get_element_value_by_id(elements_a, ELEM_ID_LASTREGISTERDATE)); o("&nbsp;</td>\n");
        o("\t\t<td class=\"passed\">"); h(get_element_value_by_id(elements_a, ELEM_ID_LASTREGISTERDATE_PASSED)); o("&nbsp;</td>\n");
        o("\t</tr>\n");
        free_element_list(elements_a);
    }
}
void output_ticket_table_status_index(Database* db, SearchResult* result, List* element_types)
{
    o("<table summary=\"ticket list\">\n");
    output_ticket_table_header_no_link(element_types);
    output_ticket_table_body(db, result, element_types);
    o("</table>\n");
}
void output_ticket_table(Database* db, SearchResult* result, List* element_types)
{
    o("<table summary=\"ticket list\">\n");
    output_ticket_table_header(element_types);
    output_ticket_table_body(db, result, element_types);
    o("</table>\n");
}
void output_states(List* states, bool with_new_ticket_link)
{
    Iterator* it;
    /* stateの表示 */
    o("<div id=\"state_index\">\n");
    o("\t<ul>\n");
    foreach (it, states) {
        State* s = it->element;
        o("\t\t<li>\n");
        o("\t\t\t<a href=\"%s/%s/search?field%d=", cgiScriptName, g_project_name_4_url, ELEM_ID_STATUS);
        u(s->name);
        o("\" title=\"状態を条件にして検索を行ないます。\">");
        h(s->name);
        o("</a>");
        o("(%d)\n", s->count);
        o("\t\t</li>\n");
    }
    o("\t\t<li>\n");
    o("\t\t\t<form action=\"%s/%s/search\" method=\"get\">\n", cgiScriptName, g_project_name_4_url);
    o("\t\t\t\t<input type=\"text\" class=\"number\" name=\"id\" title=\"入力したIDのチケットを表示します。\" maxlength=\"%d\" value=\"\" />\n", NUM_LENGTH - 1);
    o("\t\t\t\t<input type=\"submit\" class=\"button\" value=\"ID指定で表示\" />\n"
      "\t\t\t</form>\n"
      "\t\t</li>\n");
    if (with_new_ticket_link) {
        o("\t\t<li>\n"
          "\t\t\t<a id=\"new_ticket_link\" href=\"%s/%s/register\">新しいチケットを登録する</a>\n", cgiScriptName, g_project_name_4_url);
        o("\t\t</li>\n");
    }
    o("\t</ul>\n"
      "\t<br clear=\"all\" />\n"
      "</div>\n");
/*     o("<br class=\"clear\" clear=\"all\" />\n"); */
}

/**
 * 一覧を表示するaction。
 */
void list_action()
{
    List* element_types_a;
    List* conditions_a = NULL;
    Project* project_a = project_new();
    List* states_a;
    Iterator* it;
    char** multi;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "状態別チケット一覧", NULL, NAVI_LIST);
    /* メッセージの取得 */
    if ((cgiFormStringMultiple("message", &multi)) != cgiFormNotFound) {
        int i = 0;
        o("<div class=\"complete_message\">");
        while (multi[i]) {
            if (strstr(multi[i], "[ERROR]") != NULL) {
                o("<div class=\"error\">");
            } else {
                o("<div>");
            }
            h(multi[i]);
            o("</div>");
            i++;
        }
        o("</div>\n");
    }
    cgiStringArrayFree(multi);
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_4_list(db_a, element_types_a);
    o("<h2>"); h(project_a->name); o(" - 状態別チケット一覧</h2>\n");
    project_free(project_a);
    list_alloc(states_a, State);
    states_a = db_get_states(db_a, states_a);
    output_states(states_a, true);
    list_free(states_a);
    fflush(cgiOut);
    o("<div id=\"ticket_list\">\n"
      "<h3>状態別チケット一覧</h3>\n"
      "<div class=\"description\">未クローズの状態毎にチケットを表示しています。\n");
    list_alloc(states_a, State);
    states_a = db_get_states_has_not_close(db_a, states_a);
    foreach (it, states_a) {
        State* s = it->element;
        o("\t\t\t<a href=\"#state%d""\" title=\"ページ内へのリンク\">", s->id);
        h(s->name);
        o("</a>&nbsp;\n");
    }
    o("</div>\n"
      "<br clear=\"all\" />\n");
    foreach (it, states_a) {
        State* s = it->element;
        SearchResult* result_a = search_result_new();

        /* 検索 */
        list_alloc(conditions_a, Condition);
        result_a = db_get_tickets_by_status(db_a, s->name, result_a);
        list_free(conditions_a);

        o("<a id=\"state%d\" name=\"state%d\"></a>\n", s->id, s->id);
        o("<div>\n");
        o("<h4 class=\"status\">");h(s->name);o("&nbsp;(%d件)&nbsp;<a href=\"#top\">↑</a></h4>\n", s->count);
        if (result_a->hit_count == LIST_COUNT_PER_LIST_PAGE) {
            o("\t\t<div class=\"infomation\">新しい%d件のみを表示しています。<a href=\"%s/%s/search?field%d=", 
                    result_a->hit_count, 
                    cgiScriptName,
                    g_project_name_4_url,
                    ELEM_ID_STATUS);
            u(s->name);
            o("\">状態が"); h(s->name); o("である全てのチケットを表示する</a></div>\n");
        }
        output_ticket_table_status_index(db_a, result_a, element_types_a);
        if (result_a->hit_count == LIST_COUNT_PER_LIST_PAGE) {
            o("\t\t<div class=\"infomation\">続きがあります。<a href=\"%s/%s/search?field%d=", 
                    cgiScriptName,
                    g_project_name_4_url,
                    ELEM_ID_STATUS);
            u(s->name);
            o("\">状態が"); h(s->name); o("である全てのチケットを表示する</a></div>\n");
        }
        search_result_free(result_a);
        o("</div>\n");
        fflush(cgiOut);
    }
    list_free(states_a);
    list_free(element_types_a);
    o("</div>\n");
    output_footer();
    db_finish(db_a);
}
Condition* create_sort_condition(Condition* sort)
{
    char sortstr[DEFAULT_LENGTH];
    cgiFormStringNoNewlines("sort", sortstr, DEFAULT_LENGTH);
    if (strlen(sortstr) > 0) {
        sort->element_type_id = atoi(sortstr);
    } else {
        cgiFormStringNoNewlines("rsort", sortstr, DEFAULT_LENGTH);
        if (strlen(sortstr) > 0) {
            sort->element_type_id = atoi(sortstr);
            strcpy(sort->value, "reverse");
        }
    }
    return sort;
}
List* create_conditions(List* conditions, List* element_types, bool cookie_restore)
{
    char name[DEFAULT_LENGTH];
    char value[DEFAULT_LENGTH];
    char cookie_name[DEFAULT_LENGTH];
    char cookie_value[DEFAULT_LENGTH];
    Iterator* it;
    Condition* c;
    foreach (it, element_types) {
        ElementType* et = it->element;

        switch (et->type) {
            case ELEM_TYPE_DATE:
                /* 日付 from */
                sprintf(name, "field%d_from", et->id);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                sprintf(cookie_name, COOKIE_CONDITION_FORMAT ".from", et->id);
                get_cookie_string(cookie_name, cookie_value);
                c = list_new_element(conditions);
                set_condition_values(c, et->id, CONDITION_TYPE_DATE_FROM, value, cookie_value, cookie_restore);
                list_add(conditions, c);
                /* 日付 to */
                sprintf(name, "field%d_to", et->id);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                sprintf(cookie_name, COOKIE_CONDITION_FORMAT ".to", et->id);
                get_cookie_string(cookie_name, cookie_value);
                c = list_new_element(conditions);
                set_condition_values(c, et->id, CONDITION_TYPE_DATE_TO, value, cookie_value, cookie_restore);
                list_add(conditions, c);
                break;
            default:
                sprintf(name, "field%d", et->id);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                sprintf(cookie_name, COOKIE_CONDITION_FORMAT, et->id);
                get_cookie_string(cookie_name, cookie_value);
                c = list_new_element(conditions);
                set_condition_values(c, et->id, CONDITION_TYPE_NORMAL, value, cookie_value, cookie_restore);
                list_add(conditions, c);
        }
    }
    /* 登録日時 */
    sprintf(name, "registerdate.from");
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    sprintf(cookie_name, COOKIE_CONDITION_FORMAT ".from", ELEM_ID_REGISTERDATE);
    get_cookie_string(cookie_name, cookie_value);
    c = list_new_element(conditions);
    set_condition_values(c, ELEM_ID_REGISTERDATE, CONDITION_TYPE_DATE_FROM, value, cookie_value, cookie_restore);
    list_add(conditions, c);
    sprintf(name, "registerdate.to");
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    sprintf(cookie_name, COOKIE_CONDITION_FORMAT ".to", ELEM_ID_REGISTERDATE);
    get_cookie_string(cookie_name, cookie_value);
    c = list_new_element(conditions);
    set_condition_values(c, ELEM_ID_REGISTERDATE, CONDITION_TYPE_DATE_TO, value, cookie_value, cookie_restore);
    list_add(conditions, c);
    /* 更新日時 */
    sprintf(name, "lastregisterdate.from");
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    sprintf(cookie_name, COOKIE_CONDITION_FORMAT ".from", ELEM_ID_LASTREGISTERDATE);
    get_cookie_string(cookie_name, cookie_value);
    c = list_new_element(conditions);
    set_condition_values(c, ELEM_ID_LASTREGISTERDATE, CONDITION_TYPE_DATE_FROM, value, cookie_value, cookie_restore);
    list_add(conditions, c);
    sprintf(name, "lastregisterdate.to");
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    sprintf(cookie_name, COOKIE_CONDITION_FORMAT ".to", ELEM_ID_LASTREGISTERDATE);
    get_cookie_string(cookie_name, cookie_value);
    c = list_new_element(conditions);
    set_condition_values(c, ELEM_ID_LASTREGISTERDATE, CONDITION_TYPE_DATE_TO, value, cookie_value, cookie_restore);
    list_add(conditions, c);
    return conditions;
}
static void save_condition2cookie(List* conditions, char* q, bool save)
{
    Iterator* it;
    foreach (it, conditions) {
        Condition* c = it->element;
        char cookie_key[DEFAULT_LENGTH];
        if (c->element_type_id > 0) {
            switch (c->condition_type) {
                case CONDITION_TYPE_DATE_TO:
                case CONDITION_TYPE_DATE_FROM:
                    sprintf(cookie_key, COOKIE_CONDITION_FORMAT ".%s",
                            c->element_type_id,
                            c->condition_type == CONDITION_TYPE_DATE_FROM ? "from" : "to");
                    break;
                default:
                    sprintf(cookie_key, COOKIE_CONDITION_FORMAT, c->element_type_id);
            }
        } else {
            switch (c->element_type_id) {
                case ELEM_ID_REGISTERDATE:
                    sprintf(cookie_key, COOKIE_CONDITION_FORMAT ".%s",
                            ELEM_ID_REGISTERDATE,
                            c->condition_type == CONDITION_TYPE_DATE_FROM ? "from" : "to");
                    break;
                case ELEM_ID_LASTREGISTERDATE:
                    sprintf(cookie_key, COOKIE_CONDITION_FORMAT ".%s",
                            ELEM_ID_LASTREGISTERDATE,
                            c->condition_type == CONDITION_TYPE_DATE_FROM ? "from" : "to");
                    break;
                default:
                    die("It will never reach!");
            }
        }
        if (save) {
            d("set cookie: %d, %s\n", c->element_type_id, get_condition_valid_value(c));
            set_cookie(cookie_key, get_condition_valid_value(c));
        } else {
            clear_cookie(cookie_key);
        }
    }
    if (save) {
        d("set cookie: keyword, %s\n", q);
        set_cookie(COOKIE_CONDITION_KEYWORD, q);
        set_cookie(COOKIE_SAVE_CONDITION, "1");
        d("set cookie: keywordend , %s\n", q);
    } else {
        clear_cookie(COOKIE_CONDITION_KEYWORD);
        clear_cookie(COOKIE_SAVE_CONDITION);
    }
}
/**
 * 検索画面を表示するaction。
 */
void search_action()
{
    SearchResult* result_a = search_result_new();
    List* element_types_a;
    List* conditions_a = NULL;
    Condition* sort_a = NULL;
    Project* project_a = project_new();
    List* states_a;
    Iterator* it;
    char id[NUM_LENGTH];
    char q[DEFAULT_LENGTH];
    char p[NUM_LENGTH];
    char save_condition[NUM_LENGTH];
    char cookie_save_condition[DEFAULT_LENGTH];
    int col_index;
    bool condition_will_save, condition_restore;
    char search_button[DEFAULT_LENGTH];
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    cgiFormStringNoNewlines("id", id, NUM_LENGTH);
    if (strlen(id) > 0) {
        char uri[DEFAULT_LENGTH];
        sprintf(uri, "/ticket/%s", id);
        redirect(uri, NULL);
    }
    
    /* 検索条件のcookie保存 */
    cgiFormStringNoNewlines("search_button", search_button, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("save_condition", save_condition, NUM_LENGTH);
    get_cookie_string(COOKIE_SAVE_CONDITION, cookie_save_condition);
    condition_will_save = (strcmp(save_condition, "1") == 0 ||
            (strlen(search_button) == 0 && strcmp(cookie_save_condition, "1") == 0))
        ? true : false;
    condition_restore = (strlen(search_button) == 0 && strcmp(cookie_save_condition, "1") == 0)
        ? true : false;
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_4_list(db_a, element_types_a);
    /* 検索 */
    list_alloc(conditions_a, Condition);
    conditions_a = create_conditions(conditions_a, element_types_a, condition_restore);
    cgiFormStringNoNewlines("q", q, DEFAULT_LENGTH);
    if (strlen(q) == 0 && condition_restore) {
        /* 検索条件を保存する状況で、queryStringが指定されていなかったら、cookieから復元する。 */
        get_cookie_string(COOKIE_CONDITION_KEYWORD, q);
    }
    sort_a = condition_new();
    sort_a = create_sort_condition(sort_a);
    cgiFormStringNoNewlines("p", p, NUM_LENGTH);
    result_a = db_search_tickets(db_a, conditions_a, q, sort_a, atoi(p), result_a);
    /* 検索条件を保存のチェックボックスがチェックされている場合は、保存。それ意外はクリアする。 */
    save_condition2cookie(conditions_a, q, condition_will_save);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "チケット検索", "calendar.js", NAVI_SEARCH);
    output_calendar_js();
    o("<h2>"); h(project_a->name); o(" - チケット検索</h2>\n");
    project_free(project_a);
    condition_free(sort_a);
    list_alloc(states_a, State);
    states_a = db_get_states(db_a, states_a);
    output_states(states_a, true);
    list_free(states_a);
    o("<div id=\"condition_form\">\n"
      "<h3>検索条件</h3>\n"
      "<div class=\"description\">検索条件を入力して検索ボタンを押してください。</div>\n");
    o("<form action=\"%s/%s/search\" method=\"get\">\n", cgiScriptName, g_project_name_4_url);
    o(      "<table summary=\"condition table\">\n");
    o("<tr>\n"
      "\t<th>キーワード検索</th>\n"
      "\t<td>\n"
      "\t\t<input type=\"text\" class=\"conditionelement\" name=\"q\" value=\""); v(q); o("\" maxlength=\"%d\" />\n", DEFAULT_LENGTH - 1);
    o("\t\t<div class=\"description\">履歴も含めて全ての項目から検索を行ないます。</div>\n"
      "\t</td>\n"
      "\t<th>投稿日時</th>\n"
      "\t<td>\n"
      "\t\t<span>\n"
      "\t\t<input type=\"text\" class=\"calendar\" name=\"registerdate.from\" value=\""); v(get_condition_value(conditions_a, ELEM_ID_REGISTERDATE, CONDITION_TYPE_DATE_FROM)); o("\" maxlength=\"%d\" />\n", DATE_LENGTH - 1);
    o("\t\t</span>\n"
      "〜\n"
      "\t\t<span>\n"
      "\t\t<input type=\"text\" class=\"calendar\" name=\"registerdate.to\" value=\""); v(get_condition_value(conditions_a, ELEM_ID_REGISTERDATE, CONDITION_TYPE_DATE_TO)); o("\" maxlength=\"%d\" />\n", DATE_LENGTH - 1);
    o("\t\t</span>\n"
      "\t\t<div class=\"description\">yyyy-mm-dd形式で入力してください。</div>\n"
      "\t</td>\n"
      "\t<th>更新日時</th>\n"
      "\t<td>\n"
      "\t\t<span>\n"
      "\t\t<input type=\"text\" class=\"calendar\" name=\"lastregisterdate.from\" value=\""); v(get_condition_value(conditions_a, ELEM_ID_LASTREGISTERDATE, CONDITION_TYPE_DATE_FROM)); o("\" maxlength=\"%d\" />\n", DATE_LENGTH - 1);
    o("\t\t</span>\n"
      "〜\n"
      "\t\t<span>\n"
      "\t\t<input type=\"text\" class=\"calendar\" name=\"lastregisterdate.to\" value=\""); v(get_condition_value(conditions_a, ELEM_ID_LASTREGISTERDATE, CONDITION_TYPE_DATE_TO)); o("\" maxlength=\"%d\" />\n", DATE_LENGTH - 1);
    o("\t\t</span>\n"
      "\t\t<div class=\"description\">yyyy-mm-dd形式で入力してください。</div>\n"
      "\t</td>\n"
      "</tr>\n");
    col_index = 1;
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        sprintf(name, "field%d", et->id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);

        if (col_index == 1)
            o("<tr>\n");
        o("\t<th>"); h(et->name); o("</th>\n");
        o("\t<td>\n"); 
        output_form_element_4_condition(db_a, et, conditions_a);
        o("\t</td>\n");
        if (!iterator_next(it)) {
            /* 空いたセルの調整 */
            int i = col_index;
            for (; i < 3; i++) {
                o("\t<th>&nbsp;</th>\n");
                o("\t<td>&nbsp;</td>\n");
            }
        }
        if (col_index == 3 || !iterator_next(it))
            o("</tr>\n");
        col_index = col_index++ == 3 ? 1 : col_index;
    }
    list_free(conditions_a);
    o("</table>\n"
      "<input name=\"search_button\" class=\"button\" type=\"submit\" value=\"検索\" />"
      "<input id=\"save_condition\" type=\"checkbox\" name=\"save_condition\" class=\"checkbox\" value=\"1\" %s />\n"
      "<label for=\"save_condition\">検索条件を保存する。(cookie使用)</label>\n",
      condition_will_save ? "checked=\"checked\"" : "");
    o("</form>\n"
      "</div>\n");
    fflush(cgiOut);
    o("<div id=\"ticket_list\">\n"
      "<a name=\"result\"></a>\n"
      "<h3>検索結果</h3>\n");

    if (result_a->messages->size) {
        String* query_string_a = string_new(0);
        Iterator* it;

        query_string_a = format_query_string_without_page(query_string_a);
        o(      "<div class=\"infomation\">");
        o(      "%d件ヒットしました。\n", result_a->hit_count);
        o(      "[&nbsp;");
        foreach (it, result_a->states) {
            State* s = it->element;
            h(s->name);
            o("(%d)&nbsp;", s->count);
        }
        o(      "]&nbsp;");
        o(      "<a href=\"%s/%s/report_csv_download?%s\" target=\"_blank\">検索結果をCSVでダウンロードする。</a>\n",
                cgiScriptName,
                g_project_name_4_url,
                string_rawstr(query_string_a));
        o(      "</div>\n");
        output_navigater(result_a, string_rawstr(query_string_a));
        output_ticket_table(db_a, result_a, element_types_a);
        output_navigater(result_a, string_rawstr(query_string_a));
        string_free(query_string_a);
    }
    o("</div>\n");
    output_footer();
    db_finish(db_a);
    list_free(element_types_a);
    search_result_free(result_a);
}
void output_ticket_information_4_csv_report_header(List* element_types)
{
    Iterator* it;

    csv_field("ID"); 
    o(",");
    foreach (it, element_types) {
        ElementType* et = it->element;
        if (!et->ticket_property) continue;
        csv_field(et->name); o(",");
    }
    csv_field("投稿日時"); o(",");
    csv_field("最終更新日時"); o(",");
    csv_field("放置日数"); o("\r\n");
}
void output_ticket_information_4_csv_report(Database* db, SearchResult* result, List* element_types)
{
    Iterator* it;
    Iterator* it_msg;

    foreach (it_msg, result->messages) {
        Message* message = it_msg->element;
        List* elements_a;
        char id_str[NUM_LENGTH];
        sprintf(id_str, "%d", message->id);
        list_alloc(elements_a, Element);
        elements_a = db_get_last_elements(db, message->id, elements_a);
        csv_field(id_str); o(",");
        foreach (it, element_types) {
            ElementType* et = it->element;
            if (!et->ticket_property) continue;
            csv_field(get_element_value_by_id(elements_a, et->id)); o(",");
        }
        csv_field(get_element_value_by_id(elements_a, ELEM_ID_REGISTERDATE)); o(",");
        csv_field(get_element_value_by_id(elements_a, ELEM_ID_LASTREGISTERDATE)); o(",");
        csv_field(get_element_value_by_id(elements_a, ELEM_ID_LASTREGISTERDATE_PASSED));
        o("\r\n");
        free_element_list(elements_a);
    }
}
/**
 * CSVレポートをダウンロードするaction。
 */
void report_csv_download_action()
{
    SearchResult* result_a = search_result_new();
    List* element_types_a;
    List* conditions_a = NULL;
    Condition* sort_a = NULL;
    Project* project_a = project_new();
    char q[DEFAULT_LENGTH];
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    list_alloc(element_types_a, ElementType);

    element_types_a = db_get_element_types_all(db_a, element_types_a);
    /* 検索 */
    list_alloc(conditions_a, Condition);
    conditions_a = create_conditions(conditions_a, element_types_a, false);
    cgiFormStringNoNewlines("q", q, DEFAULT_LENGTH);
    sort_a = condition_new();
    create_sort_condition(sort_a);
    result_a = db_search_tickets_4_report(db_a, conditions_a, q, sort_a, result_a);
    list_free(conditions_a);
    condition_free(sort_a);

    o("Content-Disposition: attachment; filename=\"report.csv\"\r\n");
    cgiHeaderContentType("text/x-csv; charset=Windows-31J;");

    csv_field(project_a->name); o("\r\n");
    project_free(project_a);
    output_ticket_information_4_csv_report_header(element_types_a);
    output_ticket_information_4_csv_report(db_a, result_a, element_types_a);
    db_finish(db_a);
    list_free(element_types_a);
    search_result_free(result_a);
}
/**
 * form要素を表示する。
 */
void output_form_element_4_condition(Database* db, ElementType* et, List* conditions)
{
    char id[NUM_LENGTH];
    List* items_a;
    Iterator* it;
    char name[DEFAULT_LENGTH];
    char value[DEFAULT_LENGTH];
    sprintf(name, "field%d", et->id);
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    if (strlen(value) == 0) {
        /* 条件が存在しない場合、cookieから復元する。 */
        foreach (it, conditions) {
            Condition* c = it->element;
            if (c->element_type_id != et->id) continue;
            strcpy(value, get_condition_valid_value(c));
        }
    }

    sprintf(id, "%d", et->id);
    switch (et->type) {
        case ELEM_TYPE_TEXT:
        case ELEM_TYPE_TEXTAREA:
        case ELEM_TYPE_CHECKBOX:
            o("<input type=\"text\" class=\"conditionelement\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\" value=\"");
            v(value);
            o("\" maxlength=\"%d\" />\n", DEFAULT_LENGTH - 1);
            break;
        case ELEM_TYPE_LIST_SINGLE:
        case ELEM_TYPE_LIST_MULTI:
            o("<select class=\"element\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\">\n");
            list_alloc(items_a, ListItem);
            items_a = db_get_list_item(db, et->id, items_a);

            o("<option value=\"\">&nbsp;</option>");
            foreach (it, items_a) {
                ListItem* item = it->element;
                o("<option value=\"");
                v(item->name);
                if (!strcmp(value, item->name))
                    o("\" selected=\"selected\">");
                else
                    o("\">");
                h(item->name);
                o("</option>\n");
            }
            o("</select>\n");
            list_free(items_a);
            break;
        case ELEM_TYPE_DATE:
            o("<span>\n");
            o("<input type=\"text\" class=\"calendar\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("_from\" value=\"");
            v(get_condition_value(conditions, et->id, CONDITION_TYPE_DATE_FROM));
            o("\" maxlength=\"%d\" />\n", DATE_LENGTH - 1);
            o("</span>\n");
            o("〜\n");
            o("<span>\n");
            o("<input type=\"text\" class=\"calendar\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("_to\" value=\"");
            v(get_condition_value(conditions, et->id, CONDITION_TYPE_DATE_TO));
            o("\" maxlength=\"%d\" />\n", DATE_LENGTH -1);
            o("</span>\n");
            break;
    }
}
/**
 * form要素を表示する。
 */
void output_form_element(Database* db, List* elements, ElementType* et)
{
    char* value = "";
    List* items_a;
    Iterator* it;

    if (elements != NULL) {
        value = get_element_value(elements, et);
    } else {
        if (et->id == ELEM_ID_SENDER) {
            char sender[DEFAULT_LENGTH];
            get_cookie_string(COOKIE_SENDER, sender);
            if (strlen(sender))
                /* 投稿者のフィールドは、cookieから値が取得できれば、その値を表示する。 */
                value = sender;
            else 
                value = et->default_value;
        } else {
            value = et->default_value;
        }
    }
    switch (et->type) {
        case ELEM_TYPE_TEXT:
            o("<input type=\"text\" class=\"element\" id=\"field%d\" name=\"field%d\" value=\"",
                    et->id, et->id);
            v(value);
            o("\" maxlength=\"%d\" />\n", DEFAULT_LENGTH - 1);
            break;
        case ELEM_TYPE_TEXTAREA:
            o("<textarea id=\"field%d\" name=\"field%d\" rows=\"3\" cols=\"10\">",
                    et->id, et->id);
            h(value);
            o("</textarea>\n");
            break;
        case ELEM_TYPE_CHECKBOX:
            o("<input type=\"checkbox\" id=\"field%d\" class=\"checkbox\" name=\"field%d\" value=\"",
                    et->id, et->id); 
            v(et->name);
            o("\" %s />",
                    (strlen(value) || strcmp(value, "0") == 0) ? "checked=\"checked\"" : "");
            o("<label for=\"field%d\">", et->id); h(et->name); o("</label>\n");
            break;
        case ELEM_TYPE_LIST_SINGLE:
            o("<select id=\"field%d\" name=\"field%d\">\n",
                    et->id, et->id);
            o("<option value=\"\">&nbsp;</option>\n");
            list_alloc(items_a, ListItem);
            items_a = db_get_list_item(db, et->id, items_a);
            foreach (it, items_a) {
                ListItem* item = it->element;
                o("<option value=\"");
                v(item->name);
                if (!strcmp(value, item->name))
                    o("\" selected=\"selected\">");
                else
                    o("\">");
                h(item->name);
                o("</option>\n");
            }
            list_free(items_a);
            o("</select>\n");

            break;
        case ELEM_TYPE_LIST_MULTI:
            list_alloc(items_a, ListItem);
            items_a = db_get_list_item(db, et->id, items_a);
            o("<select size=\"%d\" id=\"field%d", items_a->size + 1, et->id);
            o("\" name=\"field%d\" multiple=\"multiple\">\n", et->id);

            o("<option value=\"\">&nbsp;</option>\n");
            foreach (it, items_a) {
                ListItem* item = it->element;
                o("<option value=\"");
                v(item->name);
                if (contains(value, item->name))
                    o("\" selected=\"selected\">");
                else
                    o("\">");
                h(item->name);
                o("</option>\n");
            }
            list_free(items_a);
            o("</select>\n");

            break;
        case ELEM_TYPE_UPLOADFILE:
            o("<input type=\"file\" class=\"element\" id=\"field%d\" name=\"field%d\" />\n",
                    et->id, et->id);
            o("<div class=\"description\">ファイルサイズは、%dKb以下になるようにしてください。</div>\n", MAX_FILE_SIZE);
            break;
        case ELEM_TYPE_DATE:
            o("<span>\n");
            o("<input type=\"text\" class=\"calendar\" id=\"field%d\" name=\"field%d\" value=\"\n",
                    et->id, et->id);
            v(value);
            o("\" maxlength=\"%d\"/>\n"
              "</span>\n"
              "<div class=\"description\">yyyy-mm-dd形式で入力してください。</div>\n", DATE_LENGTH - 1);
            break;
    }
    switch (et->type) {
        case ELEM_TYPE_LIST_SINGLE:
        case ELEM_TYPE_LIST_MULTI:
            if (et->auto_add_item) {
                /* 新規項目を設定可能である場合、テキストボックスを表示する。 */
                o("<input type=\"text\" class=\"element_new_item\" id=\"field%d.new_item\" name=\"field%d.new_item\" maxlength=\"%d\" />\n",
                    et->id, et->id, DEFAULT_LENGTH -1 );
                o("選択肢を追加する場合はテキストボックスに入力してください。\n");
            }
            break;
    }
}
void output_calendar_js() {
    o("<script type=\"text/javascript\" src=\"%s/../js/calendar.js\"></script>\n", cgiScriptName); 
}
void output_graph_js() {
    o("<!--[if IE]><script type=\"text/javascript\" src=\"%s/../js/excanvas-compressed.js\"></script><![endif]-->\n", cgiScriptName);
    o("<script type=\"text/javascript\" src=\"%s/../js/circle.js\"></script>\n", cgiScriptName); 
}
void output_field_information_js(List* element_types) {
    Iterator* it;
    o("<script type=\"text/javascript\" src=\"%s/../js/validate.js\"></script>\n", cgiScriptName); 
    o("<script type=\"text/javascript\">\n"
      "\t<!--\n"
      "\tvar required_field_indexs = [\n");
    foreach (it, element_types) {
        ElementType* et = it->element;
        if (et->required) {
            o("\t%d,\n", et->id);
        }
    }
    o("\t-1];\n");
    o("\tvar date_field_indexs = [\n");
    foreach (it, element_types) {
        ElementType* et = it->element;
        if (et->type == ELEM_TYPE_DATE) {
            o("\t%d,\n", et->id);
        }
    }
    o("\t-1];\n");
    o("\t// -->\n");
    o("</script>\n");
}
/**
 * 登録画面を表示するaction。
 */
void register_action()
{
    Project* project_a = project_new();
    List* states_a;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    char sender[DEFAULT_LENGTH];
    get_cookie_string(COOKIE_SENDER, sender);

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "チケット登録", "register.js", NAVI_REGISTER);
    output_calendar_js();
    o(      "<h2>"); h(project_a->name);o(" - チケット登録</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(db_a, states_a);
    output_states(states_a, false);
    list_free(states_a);
    o(      "<div id=\"input_form\">\n"
            "<h3>チケット登録</h3>\n"
            "<div class=\"description\">新規チケットを登録する場合は、以下のフォームを記入し登録ボタンを押してください。</div>\n"
            "<noscript><div class=\"description\">※必須項目の入力チェックは、javascriptで行なっています。</div></noscript>\n");
    o(      "<form id=\"register_form\" name=\"register_form\" action=\"%s/%s/register_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName, g_project_name_4_url);
    o(      "<table summary=\"input infomation\">\n");
    project_free(project_a);
    {
        List* element_types_a;
        Iterator* it;
        list_alloc(element_types_a, ElementType);
        element_types_a = db_get_element_types_all(db_a, element_types_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            char class_name[32] = "";
            /* 返信専用属性は表示しない。 */
            if (et->reply_property == 1) continue;
            o("\t<tr>\n");
            if (et->required)
                strcat(class_name, "required");
            if (et->ticket_property)
                strcat(class_name, " ticket_property");
            if (class_name[0] != '\0') {
                char class_attr[64];
                sprintf(class_attr, "class=\"%s\"", class_name);
                o("\t\t<th %s>", class_attr);
            } else {
                o("\t\t<th>");
            }
            h(et->name);
            if (et->required) {
                o("<span class=\"required\">※</span>");
            }
            o("</th><td>\n");
            if (et->required)
                o("\t\t\t<div id=\"field%d.required\" class=\"error\"></div>\n", et->id);
            if (et->type == ELEM_TYPE_DATE)
                o("\t\t<div id=\"field%d.datefield\" class=\"error\"></div>\n", et->id);
            output_form_element(db_a, NULL, et);
            o("\t\t\t<div class=\"description\">");h(et->description);o("&nbsp;</div>\n");
            o("\t\t</td>\n");
            o("\t</tr>\n");
        }
        o("</table>\n");
        output_field_information_js(element_types_a);
        list_free(element_types_a);
    }
    o(      "<input class=\"button\" type=\"submit\" name=\"register\" value=\"登録\" />\n"
            "<input id=\"save2cookie\" type=\"checkbox\" name=\"save2cookie\" class=\"checkbox\" value=\"1\" %s />\n"
            "<label for=\"save2cookie\">投稿者を保存する。(cookie使用)</label>\n"
            "</form>\n"
            "</div>\n", strlen(sender) ? "checked=\"checked\"" : "");
    o(      "<div class=\"description\">\n"
            "\t<ul>\n"
            "\t\t<li>複数行テキスト項目の内容では、#1 のように書くと、ID が 1 のチケットのチケット詳細ページへのリンクとなります。</li>\n"
            "\t\t<li>複数行テキスト項目の内容では、#bts:1 のように書くと、プロジェクトIDがbtsであるプロジェクトの、ID が 1 のチケットのチケット詳細ページへのリンクとなります。</li>\n"
            "\t\t<li>複数行テキスト項目の内容では、行頭が &gt;| から始まる行から、行頭が |&lt; から始まる行までは、整形済みブロックになります。</li>\n"
            "\t</ul>\n"
            "</div>\n");
    db_finish(db_a);
    output_footer();
}
/**
 * チケット詳細画面を表示するaction。
 */
void ticket_action()
{
    String* title_a = string_new(0);
    char ticket_id[DEFAULT_LENGTH];
    List* elements_a = NULL;
    List* last_elements = NULL;
    List* element_types_a;
    Iterator* it;
    int iid, *message_ids_a, i;
    Project* project_a = project_new();
    Database* db_a;
    char sender[DEFAULT_LENGTH];
    char buffer[DEFAULT_LENGTH];
    get_cookie_string(COOKIE_SENDER, sender);

    strcpy(ticket_id, g_path_info);
    iid = atoi(ticket_id);
    if (!iid) {
        redirect("/list", "存在しないIDが指定されました。");
            return;
    }
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    list_alloc(elements_a, Element);
    elements_a = db_get_last_elements(db_a, iid, elements_a);
    if (elements_a->size == 0) {
        redirect("/list", "存在しないIDが指定されました。");
        return;
    }
    string_appendf(title_a, "#%d %s", iid, get_element_value_by_id(elements_a, ELEM_ID_TITLE));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, string_rawstr(title_a), "reply.js", NAVI_OTHER);
    output_calendar_js();

    message_ids_a = db_get_message_ids_a(db_a, iid);
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(db_a, element_types_a);
    o("<h2 id=\"subject\">"); h(project_a->name); o(" - ");
    project_free(project_a);
    h(string_rawstr(title_a));
    string_free(title_a);
    o(" &nbsp;</h2>\n");
    o(      "<div id=\"ticket_newest\">\n"
            "<h3>チケット最新情報</h3>\n"
            "<div class=\"description\">チケットの最新情報です。最新チケットのチケット属性の付いている属性と全添付ファイルを表示しています。</div>\n"
            "<table summary=\"newest table\">\n"
            "\t<tr>\n"
            "\t\t<th>ID</th>\n"
            "\t\t<td>%d</td>\n", iid);
    o(      "\t</tr>\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        char* value = get_element_value(elements_a, et);
        if (et->ticket_property == 0) continue;
        o("\t<tr>\n"
          "\t\t<th>\n");
        h(et->name);
        o("&nbsp;</th>\n"
          "\t\t<td>\n");
        switch (et->id) {
            case ELEM_ID_SENDER:
                hmail(value);
                break;
            default:
                if (et->type == ELEM_TYPE_UPLOADFILE) {
                    if (strlen(value)) {
                        char* id_str = get_element_value_by_id(elements_a, ELEM_ID_ID);
                        o("<a href=\"%s/%s/download/%d/", 
                                cgiScriptName, 
                                g_project_name_4_url,
                                db_get_element_file_id(db_a, atoi(id_str), et->id)); 
                        u(value);
                        o("\" target=\"_blank\">");h(value); o("</a>\n");
                    }
                } else {
                    hm(value);
                }
        }
        o("&nbsp;</td>\n"
          "\t</tr>\n");
    }
    o("\t<tr>\n"
      "\t\t<th>全添付ファイル</th>\n"
      "\t\t\t<td>\n");
    for (i = 0; message_ids_a[i] != 0; i++) {
        List* attachment_elements_a;
        list_alloc(attachment_elements_a, Element);
        attachment_elements_a = db_get_elements(db_a, message_ids_a[i], attachment_elements_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            if (et->type == ELEM_TYPE_UPLOADFILE) {
                char* attachment_file_name = get_element_value(attachment_elements_a, et);
                if (strlen(attachment_file_name) == 0) continue;
                o("\t\t<span>\n");
                o("<a href=\"%s/%s/download/%d/", 
                        cgiScriptName, 
                        g_project_name_4_url,
                        db_get_element_file_id(db_a, message_ids_a[i], et->id)); 
                u(attachment_file_name); o("\" target=\"_blank\">");h(attachment_file_name); o("</a>\n");
                o("\t\t&nbsp;</span>\n");
            }
        }
        free_element_list(attachment_elements_a);
    }
    o("\t\t\t</td>\n"
      "\t</tr>\n"
            "</table>\n"
            "</div>"
            "<div class=\"infomation\"><a href=\"#reply\">返信する</a></div>\n"
            "<div id=\"ticket_history\">\n"
            "<h3>チケット履歴</h3>\n"
            "<div class=\"description\">チケットの履歴情報です。</div>\n");
    free_element_list(elements_a);
    /* 履歴の表示 */
    for (i = 0; message_ids_a[i] != 0; i++) {
        List* previous = last_elements;
        list_alloc(elements_a, Element);
        last_elements = elements_a = db_get_elements(db_a, message_ids_a[i], elements_a);
        o(      "\t\t<h4 class=\"title\">%d: ", i + 1);
        h(get_element_value_by_id(elements_a, ELEM_ID_SENDER));
        o("&nbsp;<span class=\"date\">("); h(get_element_value_by_id(elements_a, ELEM_ID_LASTREGISTERDATE)); o(")</span>&nbsp;");
        o(      "\t</h4>\n");
        o(      "<table summary=\"reply table\">\n");
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            char* value = get_element_value(elements_a, et);
            char* last_value = get_element_value(previous, et);

            /* 
             * 投稿者は表示しない。 */
            if (et->id == ELEM_ID_SENDER)
                continue;
            /* チケット属性で、直前の値と同じ項目は表示しない。 */
            if (et->ticket_property == 1 && strcmp(value, last_value) == 0)
                continue;
            /* チケット属性でなくて、空白の項目は表示しない。 */
            if (et->ticket_property == 0 && strlen(value) == 0)
                continue;
            o(      "\t<tr>\n"
                    "\t\t<th>");
            h(et->name);
            o(      "&nbsp;</th>\n"
                    "\t\t<td>");
            switch (et->id) {
                case ELEM_ID_SENDER:
                    hmail(value);
                    break;
                default:
                    if (et->type == ELEM_TYPE_UPLOADFILE) {
                        if (strlen(value)) {
                            char buf[DEFAULT_LENGTH];
                            char* mime_type;
                            int file_id = db_get_element_file_id(db_a, message_ids_a[i], et->id);
                            o("<a href=\"%s/%s/download/%d/", cgiScriptName, g_project_name_4_url, file_id); 
                            u(value); o("\" target=\"_blank\">");h(value); o("</a>\n");
                            mime_type = db_get_element_file_mime_type(db_a, message_ids_a[i], et->id, buf);
                            if (strstr(mime_type, "image") != NULL) {
                                o("<div>\n");
                                o("<img class=\"attachment_image\" src=\"%s/%s/download/%d\" alt=\"attachment file\" />\n",
                                        cgiScriptName, g_project_name_4_url, file_id);
                                o("</div>\n");
                            }
                        }
                    } else {
                        hm(value);
                    }
            }
            o(      "&nbsp;</td>\n"
                    "\t</tr>\n");
        }
        if (previous)   /* 不要になった1つ前の要素をfreeする。最後の要素は、last_elementsとして返信フォーム表示後に、freeする */
            free_element_list(previous);
        o("</table>\n");
    }
    xfree(message_ids_a);
    o(  "</div>\n");
    /* フォームの表示 */
    o(      "<a name=\"reply\"></a>\n"
            "<div id=\"input_form\">\n"
            "<h3>チケット返信</h3>\n"
            "<form id=\"reply_form\" name=\"reply_form\" action=\"%s/%s/register_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName, g_project_name_4_url);
    o(      "<input type=\"hidden\" name=\"ticket_id\" value=\"%s\" />\n", ticket_id);
    o(      "<div class=\"description\">返信を行なう場合は、以下のフォームに内容を記入して返信ボタンを押してください。</div>\n"
            "<noscript><div class=\"description\">※必須項目の入力チェックは、javascriptで行なっています。</div></noscript>\n");
    o(      "<h4>チケット情報の更新</h4>\n"
            "<div class=\"description\">チケット情報(チケットの状態など)を更新する必要がある場合は、以下の情報を変更してください。</div>\n"
            "<table class=\"reply_table\" summary=\"input table\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        if (et->ticket_property == 0) continue;
        o("\t<tr>\n");
        o("\t\t<th %s>", et->required ? "class=\"required ticket_property\"" : "class=\"ticket_property\"");
        h(et->name);
        if (et->required)
            o("<span class=\"required\">※</span>");
        o("</th>\n\t\t<td>");
        if (et->required)
            o("\t\t<div id=\"field%d.required\" class=\"error\"></div>\n", et->id);
        if (et->type == ELEM_TYPE_DATE)
            o("\t\t<div id=\"field%d.datefield\" class=\"error\"></div>\n", et->id);
        output_form_element(db_a, last_elements, et);
        o("\t\t<div class=\"description\">");h(et->description);o("&nbsp;</div>\n");
        o("\t</td>\n");
        o("\t</tr>\n");
    }
    o(      "</table>\n");
    free_element_list(last_elements);
    o(      "<h4>返信情報の追加</h4>\n"
            "<div class=\"description\">返信情報を記入してください。</div>\n"
            "<table summary=\"input table\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        if (et->ticket_property == 1) continue;
        o("\t<tr>\n");
        o("\t\t<th %s>", et->required ? "class=\"required\"" : "");
        h(et->name);
        if (et->required)
            o("<span class=\"required\">※</span>");
        o("</th>\n\t\t<td>");
        if (et->required)
            o("\t\t<div id=\"field%d.required\" class=\"error\"></div>\n", et->id);
        if (et->type == ELEM_TYPE_DATE)
            o("\t\t<div id=\"field%d.datefield\" class=\"error\"></div>\n", et->id);
        output_form_element(db_a, NULL, et);
        o("\t\t<div class=\"description\">");h(et->description);o("&nbsp;</div>\n");
        o("\t</td>\n");
        o("\t</tr>\n");
    }
    o(      "</table>\n");
    o(      "<input class=\"button\" type=\"submit\" name=\"reply\" value=\"返信\" />&nbsp;&nbsp;&nbsp;\n"
            "<input id=\"save2cookie\" type=\"checkbox\" name=\"save2cookie\" class=\"checkbox\" value=\"1\" %s />\n"
            "<label for=\"save2cookie\">投稿者を保存する。(cookie使用)</label>\n"
            "</form>\n"
            "</div>\n", strlen(sender) ? "checked=\"checked\"" : "");
    o(      "<div class=\"description\">\n"
            "\t<ul>\n"
            "\t\t<li>複数行テキスト項目の内容では、#1 のように書くと、ID が 1 のチケットのチケット詳細ページへのリンクとなります。</li>\n"
            "\t\t<li>複数行テキスト項目の内容では、#bts:1 のように書くと、プロジェクトIDがbtsであるプロジェクトの、ID が 1 のチケットのチケット詳細ページへのリンクとなります。</li>\n"
            "\t\t<li>複数行テキスト項目の内容では、行頭が &gt;| から始まる行から、行頭が |&lt; から始まる行までは、整形済みブロックになります。</li>\n"
            "\t</ul>\n"
            "</div>\n");
    output_field_information_js(element_types_a);
    db_finish(db_a);
    output_footer();
    list_free(element_types_a);
}
static void register_list_item(Database* db, int id, char* name)
{
    ListItem* item_a = list_item_new();
    item_a->element_type_id = id;
    strcpy(item_a->name, name);
    item_a->close = 0;
    item_a->sort = 0;
    db_register_list_item(db, item_a);
    list_item_free(item_a);
}
/**
 * 登録するaction。
 * 登録モード、編集モードがある。
 */
void register_submit_action()
{
    Project* project_a = project_new();
    List* element_types_a;
    Iterator* it;
    Message* ticket_a;
    char ticket_id[NUM_LENGTH];
    ModeType mode = get_mode();
    char** multi;
    char save2cookie[2];
    char* complete_message = NULL;
    HOOK* hook = NULL;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    cgiFormStringNoNewlines("save2cookie", save2cookie, 2);
    if (mode == MODE_INVALID)
        die("requested invalid mode.");
    ticket_a = message_new();
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(db_a, element_types_a);
    cgiFormStringNoNewlines("ticket_id", ticket_id, NUM_LENGTH);
    if (mode == MODE_REGISTER)
        ticket_a->id = -1;
    else
        ticket_a->id = atoi(ticket_id);
    {
        char* value_a = xalloc(sizeof(char) * VALUE_LENGTH); /* 1M */
        /* register, reply */
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            Element* e = list_new_element(ticket_a->elements);
            char name_new_item[DEFAULT_LENGTH] = "";
            char name[DEFAULT_LENGTH] = "";
            sprintf(name_new_item, "field%d.new_item", et->id);
            sprintf(name, "field%d", et->id);
            strcpy(value_a, "");

            e->element_type_id = et->id;
            e->is_file = 0;
            switch (et->type) {
                case ELEM_TYPE_TEXT:
                case ELEM_TYPE_DATE:
                    cgiFormStringNoNewlines(name, value_a, VALUE_LENGTH);
                    set_element_value(e, value_a);
                    break;
                case ELEM_TYPE_TEXTAREA:
                    cgiFormString(name, value_a, VALUE_LENGTH);
                    set_element_value(e, value_a);
                    break;
                case ELEM_TYPE_CHECKBOX:
                    cgiFormString(name, value_a, VALUE_LENGTH);
                    set_element_value(e, value_a);
                    break;
                case ELEM_TYPE_LIST_SINGLE:
                    /* 新規選択肢 */
                    cgiFormString(name_new_item, value_a, VALUE_LENGTH);
                    if (strlen(value_a)) {
                        set_element_value(e, value_a);
                        /* 新しく選択肢を追加 */
                        register_list_item(db_a, et->id, value_a);
                    } else {
                        cgiFormString(name, value_a, VALUE_LENGTH);
                        set_element_value(e, value_a);
                    }
                    break;
                case ELEM_TYPE_LIST_MULTI:
                    /* 新規選択肢 */
                    cgiFormString(name_new_item, value_a, VALUE_LENGTH);
                    if (strlen(value_a)) {
                        /* 新しく選択肢を追加 */
                        register_list_item(db_a, et->id, value_a);
                        strcat(value_a, "\t");
                    }
                    if ((cgiFormStringMultiple(name, &multi)) != cgiFormNotFound) {
                        int i = 0;
                        int len = 0;
                        while (multi[i]) {
                            len += strlen(multi[i]) + 1;
                            /* VALUE_LENGTH を超えない範囲で連結していく。
                             * 通常超えないはず */
                            if (len < VALUE_LENGTH) {
                                strcat(value_a, multi[i]);
                                strcat(value_a, "\t");
                            }
                            i++;
                        }
                    }
                    set_element_value(e, value_a);
                    cgiStringArrayFree(multi);
                    break;
                case ELEM_TYPE_UPLOADFILE:
                    if (get_upload_size(et->id) > (MAX_FILE_SIZE * 1024)) {
                        goto file_size_error;
                    }
                    cgiFormFileName(name, value_a, VALUE_LENGTH);
                    e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, get_filename_without_path(value_a));
                    if (strlen(e->str_val)) {
                        e->is_file = 1;
                    }
                    break;
            }
            if (e->element_type_id == ELEM_ID_SENDER) {
                if (strcmp(save2cookie, "1") == 0) {
                    d("set cookie: %s, %s, %s\n", e->str_val, cgiScriptName, cgiServerName);
                    set_cookie(COOKIE_SENDER, e->str_val);
                } else {
                    clear_cookie(COOKIE_SENDER);
                }
            }
            list_add(ticket_a->elements, e);
        }
        xfree(value_a);
        db_begin(db_a);
        ticket_a->id = db_register_ticket(db_a, ticket_a);
        d("commit\n");
        db_commit(db_a);
        d("commited\n");
        /* hook */
        hook = init_hook(HOOK_MODE_REGISTERED);
        d("init_hook\n");
        hook = exec_hook(hook, project_a, ticket_a, ticket_a->elements, element_types_a);
        d("exec_hook\n");
        if (mode == MODE_REGISTER)
            complete_message = "登録しました。";
        else if (mode == MODE_REPLY)
            complete_message = "返信しました。";
        d("finish\n");
        project_free(project_a);
        list_free(element_types_a);
    }
        d("finish\n");
    db_finish(db_a);
        d("finished\n");
    message_free(ticket_a);

    redirect_with_hook_messages("/list", complete_message, hook->results);
    if (hook) clean_hook(hook);
        d("end\n");
    return;

file_size_error:
    db_finish(db_a);
    output_header(project_a, "エラー", NULL, NAVI_OTHER);
    o("<h1>エラー発生</h1>\n"
      "<div class=\"message\">ファイルサイズが大きすぎます。%dkbより大きいファイルは登録できません。ブラウザの戻るボタンで戻ってください。</div>\n", MAX_FILE_SIZE);
    output_footer();
    project_free(project_a);
}
/**
 * 一括登録画面を表示するaction。
 */
void register_at_once_action()
{
    Project* project_a = project_new();
    List* states_a;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    char sender[DEFAULT_LENGTH];
    get_cookie_string(COOKIE_SENDER, sender);

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "チケット一括登録", "register_at_once.js", NAVI_REGISTER_AT_ONCE);
    output_calendar_js();
    o(      "<h2>"); h(project_a->name);o(" - チケット一括登録</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(db_a, states_a);
    output_states(states_a, true);
    list_free(states_a);
    o(      "<div id=\"input_form\">\n"
            "<h3>チケット一括登録</h3>\n"
            "<div class=\"description\">\n"
            "新規チケットを一括登録する場合は、以下のフォームにチケット情報をCSV形式で記入し解析ボタンを押してください。登録の手順の詳細は以下のとおりです。\n"
            "\t<ul>\n"
            "\t\t<li>チケット一括登録ページ(このページ)で、登録したい複数のチケットの情報をCSV形式で貼り付けて、解析ボタンを押す。</li>\n"
            "\t\t<li>チケット一括登録確認ページで、解析結果が表示されるので、各フィールドに対応する項目を選択し、登録ボタンを押す。</li>\n"
            "\t\t<li>登録完了。</li>\n"
            "\t</ul>\n"
            "</div>\n"
            "<noscript><div class=\"description\">※必須項目の入力チェックは、javascriptで行なっています。</div></noscript>\n");
    o(      "<form id=\"register_form\" name=\"register_form\" action=\"%s/%s/register_at_once_confirm\" method=\"post\">\n", cgiScriptName, g_project_name_4_url);
    project_free(project_a);
    {
        /* 一括用、CSV形式フィールド */
        o("<table summary=\"input infomation\">\n"
          "\t<tr>\n"
          "\t\t<th class=\"required\">CSV"
          "<span class=\"required\">※</span>"
          "</th><td>\n"
          "\t\t\t<div id=\"csvdata.required\" class=\"error\"></div>\n"
          "\t\t\t<textarea name=\"csvdata\" id=\"csvdata\" rows=\"5\" cols=\"5\"></textarea>\n"
          "\t\t\t<div class=\"description\">登録したいデータをCSV形式で貼り付けてください。次の画面で取り込むフィールドを設定可能です。&nbsp;</div>\n"
          "\t\t</td>\n"
          "\t</tr>\n"
          "</table>\n");
    }
    o(      "<input class=\"button\" type=\"submit\" name=\"register\" value=\"解析\" />\n"
            "</form>\n"
            "</div>\n");
    db_finish(db_a);
    output_footer();
}
/**
 * 一括登録確認画面を表示するaction。
 */
void register_at_once_confirm_action()
{
    int i;
    int row = 0;
    Project* project_a = project_new();
    List* states_a;
    Csv* csv_a;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    char sender[DEFAULT_LENGTH];
    char* content_a = xalloc(sizeof(char) * VALUE_LENGTH);
    get_cookie_string(COOKIE_SENDER, sender);

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    cgiFormString("csvdata", content_a, VALUE_LENGTH);
    csv_a = csv_new(content_a);
    xfree(content_a);

    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "チケット一括登録確認", "register_at_once_submit.js", NAVI_REGISTER_AT_ONCE);
    output_calendar_js();
    o(      "<h2>"); h(project_a->name);o(" - チケット一括登録確認</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(db_a, states_a);
    output_states(states_a, true);
    list_free(states_a);
    o(      "<div id=\"input_form\">\n"
            "<h3>チケット一括登録確認</h3>\n"
            "<div class=\"description\">各桁の登録対象コンボボックスの選択値を、その桁のデータの登録対象となる項目を選択して、登録ボタンを押してください。</div>\n"
            "<noscript><div class=\"description\">※必須項目の入力チェックは、javascriptで行なっています。</div></noscript>\n");
    o(      "<form id=\"register_form\" name=\"register_form\" action=\"%s/%s/register_at_once_submit\" method=\"post\">\n", cgiScriptName, g_project_name_4_url);
    o(      "<table summary=\"input infomation\">\n");
    project_free(project_a);
    {
        int line_count = 0;
        List* element_types_a;
        Iterator* it;
        Iterator* it_line;
        list_alloc(element_types_a, ElementType);
        element_types_a = db_get_element_types_all(db_a, element_types_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            /* 返信専用属性は表示しない。 */
            if (et->reply_property == 1) continue;
            /* 投稿者だけを表示する。 */
            if (et->id != ELEM_ID_SENDER) continue;
            o("\t<tr>\n");
            o("\t\t<th %s>", et->required ? "class=\"required\"" : "");
            h(et->name);
            if (et->required) {
                o("<span class=\"required\">※</span>");
            }
            o("</th><td>\n");
            if (et->required)
                o("\t\t\t<div id=\"field%d.required\" class=\"error\"></div>\n", et->id);
            output_form_element(db_a, NULL, et);
            o("\t\t\t<div class=\"description\">");h(et->description);o("&nbsp;</div>\n");
            o("\t\t</td>\n");
            o("\t</tr>\n");
        }
        o("</table>\n");
        o("<input type=\"hidden\" name=\"fields_count\" value=\"%d\" />\n", csv_a->field_count);
        o(      "<table id=\"register_at_once_confirm\">\n"
                "\t<tr>\n");
        o(  "\t\t<th>登録<br />対象</th>\n");
        for (i = 0; i < csv_a->field_count; i++) {
            o(  "\t\t<th>\n");
            o(  "\t\t\t<select name=\"col_field%d\">\n", i);
            o(  "\t\t\t\t<option value=\"\">無視</option>\n");
            foreach (it, element_types_a) {
                ElementType* et = it->element;
                if (et->id == ELEM_ID_SENDER) continue;
                o("\t\t\t\t<option value=\"%d\">\n", et->id);
                o("\t\t\t\t\t"); h(et->name); o("\n");
                o("\t\t\t\t</option>\n");
            }
            o(  "\t\t\t</select>\n");
            o(  "\t\t</th>\n");
        }
        o(      "\t</tr>\n");
        /* データ */
        foreach (it_line, csv_a->lines) {
            CsvLine* line = it_line->element;
            Iterator* it_fields;
            int fields_count = 0, i;
            o("\t<tr>\n");
            o("\t\t<th>%d", (row++) + 1);
            o("</th>\n");
            foreach (it_fields, line->fields) {
                CsvField* field = it_fields->element;
                o("\t\t<td>\n");
                o("\t\t\t<textarea name=\"csvfield%d.%d\" row=\"5\" col=\"5\">", line_count, fields_count); h(string_rawstr(field->data)); o("</textarea>\n");
                o("\t\t</td>\n");
                fields_count++;
            }
            for (i = csv_a->field_count; i > fields_count; i--) {
                o("\t\t<td>&nbsp;</td>\n");
            }
            o("\t</tr>\n");
            line_count++;
        }
        o("</table>\n");
        list_free(element_types_a);
    }
    o(      "<input class=\"button\" type=\"submit\" name=\"register\" value=\"登録\" />\n"
            "<input id=\"save2cookie\" type=\"checkbox\" name=\"save2cookie\" class=\"checkbox\" value=\"1\" %s />\n"
            "<label for=\"save2cookie\">投稿者を保存する。(cookie使用)</label>\n"
            "</form>\n", strlen(sender) ? "checked=\"checked\"" : "");
    o(      "<div class=\"description\">\n"
            "\t<ul>\n"
            "\t\t<li>登録対象コンボボックスの選択値を、「無視」のまま登録すると、登録対象から省かれます。</li>\n"
            "\t\t<li>登録する各値は、入力チェックが行なわれませんので、登録前に確認してください。</li>\n"
            "\t</ul>\n"
            "</div>\n");
    o(      "</div>\n");
    db_finish(db_a);
    csv_free(csv_a);
    output_footer();
}
void register_at_once_submit_action()
{
    int row_count = 0, col_count = 0;
    Iterator* it;
    char save2cookie[2];
    char senderfield[DEFAULT_LENGTH];
    char sender[DEFAULT_LENGTH];
    char fields_count_str[5];
    List* field_ids_a;
    int i = 0;
    int fields_count;
    int registered_tickets_count = 0;
    List* element_types_a; 
    Iterator* it_et;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    cgiFormStringNoNewlines("save2cookie", save2cookie, 2);
    sprintf(senderfield, "field%d", ELEM_ID_SENDER);
    cgiFormStringNoNewlines(senderfield, sender, DEFAULT_LENGTH);
    if (strcmp(save2cookie, "1") == 0) {
        d("set cookie: %s, %s, %s\n", sender, cgiScriptName, cgiServerName);
        set_cookie(COOKIE_SENDER, sender);
    } else {
        clear_cookie(COOKIE_SENDER);
    }

    cgiFormStringNoNewlines("fields_count", fields_count_str, 5);
    fields_count = atoi(fields_count_str);
    /* 項目一覧を取得する。 */
    list_alloc(field_ids_a, int);
    for (i = 0; i < fields_count; i++) {
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        int* field_id;
        sprintf(name, "col_field%d", i);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        field_id = list_new_element(field_ids_a);
        if (strlen(value) != 0) {
            *field_id = atoi(value);
        } else {
            *field_id = -1;
        }
        list_add(field_ids_a, field_id);
    }
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    db_begin(db_a);
    while (1) {
        Message* ticket_a = message_new();
        ticket_a->id = -1;
        /* register */
        col_count = -1;
        foreach (it, field_ids_a) {
            int* field_id = it->element;
            char* value_a;
            Element* e;
            value_a = xalloc(sizeof(char) * VALUE_LENGTH); /* 1M */
            col_count++;
            if (*field_id == -1) {
                xfree(value_a);
                continue;
            }
            e = list_new_element(ticket_a->elements);
            char name[DEFAULT_LENGTH] = "";
            sprintf(name, "csvfield%d.%d", row_count, col_count);
            strcpy(value_a, "");
            e->element_type_id = *field_id;
            e->is_file = 0;
            cgiFormString(name, value_a, VALUE_LENGTH);
            if (strlen(value_a) == 0) {
                /* 値が無ければelementを追加しない */
                xfree(value_a);
                xfree(e);
                continue;
            }
            set_element_value(e, value_a);
            list_add(ticket_a->elements, e);
            if (ticket_a->elements->size == 1) {
                /* 1回だけ投稿者を追加する。 */
                Element* sender_element = list_new_element(ticket_a->elements);
                sender_element->element_type_id = ELEM_ID_SENDER;
                set_element_value(sender_element, sender);
                list_add(ticket_a->elements, sender_element);
            }
            xfree(value_a);
        }
        if (ticket_a->elements->size == 0) {
            /* elementが無い状態だったら、登録処理終了。 */
            message_free(ticket_a);
            break;
        }
        /* 指定されていない項目は、デフォルト値を登録する。 */
        list_alloc(element_types_a, ElementType);
        element_types_a = db_get_element_types_all(db_a, element_types_a);
        foreach (it_et, element_types_a) {
            ElementType* et = it_et->element;
            Element* element = NULL;
            if (strlen(et->default_value) == 0)
                continue;
            foreach (it, ticket_a->elements) {
                Element* e = it->element;
                if (et->id == e->element_type_id) {
                    element = e;
                    break;
                }
            }
            if (element == NULL) {
                Element* e_added = list_new_element(ticket_a->elements);
                e_added->element_type_id = et->id;
                set_element_value(e_added, et->default_value);
                list_add(ticket_a->elements, e_added);
            }
        }
        list_free(element_types_a);
        ticket_a->id = db_register_ticket(db_a, ticket_a);
        registered_tickets_count++;
        message_free(ticket_a);
        row_count++;
    }
    db_commit(db_a);
    db_finish(db_a);

    if (registered_tickets_count) {
        char message[DEFAULT_LENGTH];
        sprintf(message, "%d件登録しました。", registered_tickets_count);
        redirect("/list", message);
    } else {
        redirect("/list", "登録されたチケットはありませんでした。戻るボタンで戻って確認してください。");
    }
    list_free(field_ids_a);
    return;

}
/**
 * デフォルトのaction。
 */
void top_action()
{
    Project* project_a = project_new();
    List* tickets_a;
    List* states_a;
    Iterator* it;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "プロジェクトトップ", NULL, NAVI_TOP);
    list_alloc(states_a, State);
    states_a = db_get_states(db_a, states_a);
    o(      "<div id=\"info\">\n");
    /* 最新情報の表示 */
    o(      "<div id=\"top_newest\">\n"
            "<h4>最新情報</h4>\n"
            "\t<ul>\n");
    list_alloc(tickets_a, Message);
    tickets_a = db_get_newest_information(db_a, 10, tickets_a);
    if (tickets_a->size) {
        foreach (it, tickets_a) {
            Message* ticket = it->element;
            List* elements_a;
            list_alloc(elements_a, Element);
            elements_a = db_get_last_elements_4_list(db_a, ticket->id, elements_a);
            o("\t\t<li>\n");
            o("\t\t\t<a href=\"%s/%s/ticket/%d", cgiScriptName, g_project_name_4_url, ticket->id); o("\">");
            h(get_element_value_by_id(elements_a, ELEM_ID_TITLE));
            o(      "</a>\n");
            o("\t\t</li>\n");
            free_element_list(elements_a);
        }
    } else {
        o("<li>最新情報がありません。</li>\n");
    }
    list_free(tickets_a);
    o(      "\t</ul>\n"
            "</div>\n");
    /* stateの表示 */
    o(      "<div id=\"top_state_index\">\n"
            "<h4>状態別件数</h4>\n"
            "\t<ul>\n");
    if (states_a->size) {
        foreach (it, states_a) {
            State* s = it->element;
            o("\t\t<li>\n");
            o("\t\t\t<a href=\"%s/%s/search?field%d=", cgiScriptName, g_project_name_4_url, ELEM_ID_STATUS); u(s->name); o("\">");
            h(s->name);
            o("\t\t\t</a>\n");
            o("(%d)", s->count);
            o("\t\t</li>\n");
        }
    } else {
        o("<li>チケット情報がありません。</li>\n");
    }
    list_free(states_a);
    o(      "\t</ul>\n"
            "</div>\n");
    /* ID検索フォームの表示 */
    o(      "<div id=\"top_id_search\">\n"
            "<h4>ID検索</h4>\n"
            "\t<form action=\"%s/%s/search\" method=\"get\">\n", cgiScriptName, g_project_name_4_url);
    o(      "\t\t<input type=\"text\" class=\"number\" name=\"id\" title=\"入力したIDのチケットを表示します。\" maxlength=\"%d\" />\n", DEFAULT_LENGTH - 1);
    o(      "\t\t<input type=\"submit\" class=\"button\" value=\"表示\" />\n"
            "\t</form>\n"
            "</div>\n"
            "</div>\n"
            "<div id=\"main\">\n"
            "<h2>");h(project_a->name);o("&nbsp;</h2>\n");
    project_free(project_a);
    o(      "<div id=\"main_body\">\n"
            "<div class=\"top_edit\">\n"
            "<a id=\"new_ticket_link\" href=\"%s/%s/register\">新しいチケットを登録する</a>&nbsp;\n", cgiScriptName, g_project_name_4_url);
    o(      "<a href=\"%s/%s/edit_top\">ページの編集</a>\n", cgiScriptName, g_project_name_4_url);
    o(      "</div>\n");
    wiki_out(db_a, "top");
    o(      "</div>\n"
            "</div>\n");
    db_finish(db_a);
    output_footer();
}

void rss_action()
{
    List* tickets_a;
    Iterator* it;
    Project* project_a = project_new();
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    cgiHeaderContentType("text/xml; charset=utf-8;");
    o(      "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
            "<rdf:RDF \n"
            "\t\txmlns=\"http://purl.org/rss/1.0/\"\n"
            "\t\txmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" \n"
            "\t\txml:lang=\"ja\">\n"
            "\n"
            "\t<channel rdf:about=\"");h(project_a->home_url);o("%s/%s/rss\">\n", cgiScriptName, g_project_name_4_url);
    o(      "\t\t<title>");h(project_a->name); o("</title>\n"
            "\t\t<link>");h(project_a->home_url);o("/bt/</link>\n");
    o(      "\t\t<description>");h(project_a->name);o("</description>\n"
            "\t\t<items>\n"
            "\t\t\t<rdf:Seq>\n");
    o(      "\t\t\t\t<rdf:li rdf:resource=\"");h(cgiServerName);o("%s/%s/list\"/>\n", cgiScriptName, g_project_name_4_url);
    o(      "\t\t\t\t<rdf:li rdf:resource=\"");h(cgiServerName);o("%s/%s/register\"/>\n", cgiScriptName, g_project_name_4_url);
    o(      "\t\t\t</rdf:Seq>\n"
            "\t\t</items>\n"
            "\t</channel>\n");

    list_alloc(tickets_a, Message);
    tickets_a = db_get_newest_information(db_a, 10, tickets_a);
    if (tickets_a != NULL) {
        foreach (it, tickets_a) {
            Message* ticket = it->element;
            List* elements_a;
            Iterator* it;
            list_alloc(elements_a, Element);
            elements_a = db_get_last_elements_4_list(db_a, ticket->id, elements_a);
            o(      "\t<item rdf:about=\"");h(project_a->home_url);o("%s/%s/ticket/%d\">\n", cgiScriptName, g_project_name_4_url, ticket->id);
            o(      "\t\t<title>ID:%5d ", ticket->id);
            h(get_element_value_by_id(elements_a, ELEM_ID_TITLE));
            o(      "</title>\n");
            o(      "\t\t<link>");h(project_a->home_url);o("%s/%s/ticket/%d</link>\n", cgiScriptName, g_project_name_4_url, ticket->id);
            o(      "\t\t<description><![CDATA[\n");
            o(      "投稿者: ");
            hmail(get_element_value_by_id(elements_a, ELEM_ID_ORG_SENDER));
            o("\n");
            o(      "投稿日: ");
            h(get_element_value_by_id(elements_a, ELEM_ID_REGISTERDATE));
            o("\n");
            foreach (it, elements_a) {
                Element* e = it->element;
                h(e->str_val);
                o("\n");
            }
            free_element_list(elements_a);
            o(      "]]></description>\n"
                    "\t</item>\n");
        }
    }
    project_free(project_a);
    list_free(tickets_a);
    o(      "</rdf:RDF>\n");
    db_finish(db_a);
}
State* get_statictics(int element_type_id, List* states)
{
    Iterator* it;
    foreach (it, states) {
        State* s = it->element;
        if (element_type_id == s->id)
            return s;
    }
    return NULL;
}
void statistics_action()
{
    Project* project_a = project_new();
    List* element_types_a;
    Iterator* it;
    List* states_a;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "統計情報", "graph.js", NAVI_STATISTICS);
    output_graph_js();
    o(      "<h2>");h(project_a->name);o("</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(db_a, states_a);
    output_states(states_a, true);
    list_free(states_a);
    o(      "<div id=\"top\">\n"
            "<h3>統計情報</h3>\n"
            "\t<div class=\"description\">統計情報を表示します。</div>\n");
    project_free(project_a);
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(db_a, element_types_a);
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        List* items_a;
        List* all_items_a;
        Iterator* it_item;

        list_alloc(items_a, State);
        list_alloc(all_items_a, ListItem);
        switch (et->type) {
            case ELEM_TYPE_LIST_SINGLE:
                items_a = db_get_statictics(db_a, items_a, et->id);
                goto got_item;
            case ELEM_TYPE_LIST_MULTI:
                items_a = db_get_statictics_multi(db_a, items_a, et->id);
got_item:
                all_items_a = db_get_list_item(db_a, et->id, all_items_a);
                o(      "\t<h4 class=\"item\">");
                h(et->name);
                o(      "\t</h4>"
                  "<div class=\"graph\">\n"
                  "<noscript>JavaScriptでグラフを表示しています。JavaScriptが有効なブラウザで見てください。\n"
                        "\t<ul>\n");
                foreach (it_item, all_items_a) {
                    ListItem* item = it_item->element;
                    State* s = get_statictics(item->id, items_a);
                    o(      "\t\t<li>");
                    h(item->name);
                    o(      "(%d)", s == NULL ? 0 : s->count);
                    o(      "\t\t</li>\n");
                }
                o(      "\t</ul>\n"
                        "</noscript>\n"
                        "\t\t<script type=\"text/javascript\">\n"
                        "\t\t<!--\n"
                        "\t\tvar graph_%d = [\n", et->id);
                foreach (it_item, items_a) {
                    State* s = it_item->element;
                    int count = s == NULL ? 0 : s->count;
                    o("\t\t[\""); o(s->name); o("\t(%d)\", %d]", count, count); /* html5ライブラリ側でエスケープしてるので、s->nameはエクケープしない */
                    if (iterator_next(it_item)) o(",");
                    o("\n");
                }
                o(      "\t\t];\n"
                        "\t\tdocument.write('<canvas id=\"graph_%d\"></canvas>');\n"
                        "\t\t// -->\n"
                        "\t\t</script>\n", et->id);
                o("</div>\n");
                break;
        }
        list_free(items_a);
        list_free(all_items_a);
    }
    list_free(element_types_a);
    o(      "</div>\n");
    output_footer();
    db_finish(db_a);
}
void help_action()
{
    Project* project_a = project_new();
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "ヘルプ", NULL, NAVI_HELP);
    o(      "<h2>");h(project_a->name);o("</h2>\n"
            "<div id=\"top\">\n");
    wiki_out(db_a, "help");
    o(      "</div>\n");
    project_free(project_a);
    db_finish(db_a);
    output_footer();
}
void edit_top_action()
{
    Project* project_a = project_new();
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "トップページの編集", "edit_top.js", NAVI_OTHER);
    project_free(project_a);
    o(      "<h2>トップページの編集</h2>\n"
            "<div id=\"top\">\n"
            "<h3>トップページの編集</h3>\n"
            "<div id=\"description\">簡易wikiの文法でトップページのコンテンツの編集を行ない、更新ボタンを押してください。</div>\n"
            "<form id=\"edit_top_form\" action=\"%s/%s/edit_top_submit\" method=\"post\">\n", cgiScriptName, g_project_name_4_url);
    o(      "<textarea name=\"edit_top\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    wiki_content_out(db_a, "top");
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"更新\" />\n"
            "</form>"
            "<div>\n"
            "<h3>簡易wikiのサポートする文法</h3>\n"
            "<ul>\n"
            "<li>行頭に*を記述した行は、大見出しになります。</li>\n"
            "<li>行頭に**を記述した行は、中見出しになります。</li>\n"
            "<li>行頭に***を記述した行は、小見出しになります。</li>\n"
            "<li>行頭に****を記述した行は、極小見出しになります。</li>\n"
            "<li>行頭に-を記述した行は、箇条書きになります。</li>\n"
            "<li>行頭に----を記述した行は、区切り線になります。</li>\n"
            "<li>行頭が >| から始まる行から、行頭が |< から始まる行までは、整形済みブロックになります。</li>\n"
            "</ul>\n"
            "<h5>例</h5>\n"
            "<pre>\n"
            "*編集可能領域\n"
            "自由に編集できます。\n"
            "右側の「トップページの編集」のリンクから編集してください。\n"
            "色々な用途に使用してください。\n"
            "-お知らせ\n"
            "-Starbug1の使い方についての注意事項など\n"
            "\n"
            ">|\n"
            "void displayWidgets (Iterable<Widget> widgets) {\n"
            "  for (Widget w : widgets) {\n"
            "    w.display();\n"
            "  }\n"
            "}\n"
            "|<\n"
            "</pre>\n"
            "</div>\n"
            "</div>\n");
    db_finish(db_a);
    output_footer();
}
void edit_top_submit_action()
{
    char* value_a = xalloc(sizeof(char) * VALUE_LENGTH);
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    cgiFormString("edit_top", value_a, VALUE_LENGTH);
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    wiki_save(db_a, "top", value_a);
    db_finish(db_a);
    xfree(value_a);

    redirect("", NULL);
}
void download_action()
{
    ElementFile* file_a = element_file_new();
    char element_id_str[DEFAULT_LENGTH];
    int element_file_id;
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    strcpy(element_id_str, g_path_info);
    element_file_id = atoi(element_id_str);

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    file_a = db_get_element_file(db_a, element_file_id, file_a);
    if (!file_a) goto error;
    o("Content-Type: %s\r\n", file_a->mime_type);
    o("Content-Length: %d\r\n", file_a->size);
    o("Content-Disposition: attachment;\r\n");
    o("\r\n");

    fwrite(file_a->content, sizeof(char), file_a->size, cgiOut);
    db_finish(db_a);
    element_file_free(file_a);
    return;

error:
    cgiHeaderContentType("text/plain; charset=utf-8;");
    o("error: ファイルがありません。");
}
void setting_file_action()
{
    SettingFile* file_a = setting_file_new();
    char name[DEFAULT_LENGTH];
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    strcpy(name, g_path_info);

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    file_a = db_get_setting_file(db_a, name, file_a);
    if (!file_a) goto error;
    o("Content-Type: %s\r\n", file_a->mime_type);
    o("Content-Length: %d\r\n", file_a->size);
    o("Cache-Control: max-age=%d\r\n", 60 * 24 * 1);
    o("\r\n");

    fwrite(file_a->content, sizeof(char), file_a->size, cgiOut);
    db_finish(db_a);
    setting_file_free(file_a);
    return;

error:
    cgiHeaderContentType("text/plain; charset=utf-8;");
    o("error: ファイルがありません。");
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
