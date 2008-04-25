#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db.h"
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
void search_actoin();
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
void output_header(Project*, char*, char*, NaviType);
void output_footer();
int cgiMain();
void output_form_element(List*, ElementType*);
void output_form_element_4_condition(ElementType*);
ModeType get_mode();
static int contains(char* const, const char*);
void output_calendar_js();

#define COOKIE_SENDER "starbug1_sender"
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
    register_action_actions("list", list_action);
    register_action_actions("search", search_actoin);
    register_action_actions("register", register_action);
    register_action_actions("register_at_once", register_at_once_action);
    register_action_actions("register_at_once_confirm", register_at_once_confirm_action);
    register_action_actions("register_at_once_submit", register_at_once_submit_action);
    register_action_actions("register_submit", register_submit_action);
    register_action_actions("ticket", ticket_action);
    register_action_actions("statistics", statistics_action);
    register_action_actions("reply", ticket_action);
    register_action_actions("help", help_action);
    register_action_actions("edit_top", edit_top_action);
    register_action_actions("edit_top_submit", edit_top_submit_action);
    register_action_actions("download", download_action);
    register_action_actions("rss", rss_action);
    register_action_actions("report_csv_download", report_csv_download_action);
    register_action_actions("top", top_action);
}

void output_header(Project* project, char* title, char* script_name, NaviType navi)
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
    o(        "\t<title>"); h(project->name); o(" - %s</title>\n", title);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/user.css\" />\n", cgiScriptName);
    if (script_name) {
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/prototype.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/%s\"></script>\n", cgiScriptName, script_name);
    }
    o(      "\t<link rel=\"alternate\" type=\"application/rss+xml\" title=\"RSS\" href=\"%s/rss\">\n", string_rawstr(base_url_a));
    string_free(base_url_a);
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://starbug1.sourceforge.jp/\"><img src=\"%s/../img/title.jpg\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName);
    o("<div>\n"
      "<ul id='menu'>\n");
    o("\t<li><a href='%s' title=\"ホームへ移動します\">ホーム</a></li>\n", project->home_url);
    o("\t<li><a %s href='%s' title=\"トップページへ移動します\">トップ</a></li>\n", navi == NAVI_TOP ? "class=\"current\"" : "", cgiScriptName);
    o("\t<li><a %s href='%s/list' title=\"状態別のチケット一覧を参照します\">状態別チケット一覧</a></li>\n", navi == NAVI_LIST ? "class=\"current\"" : "", cgiScriptName);
    o("\t<li><a %s href='%s/register' title=\"新規にチケットを登録します\">チケット登録</a></li>\n", navi == NAVI_REGISTER ? "class=\"current\"" : "", cgiScriptName);
    o("\t<li><a %s href='%s/search' title=\"チケットを検索します\">チケット検索</a></li>\n", navi == NAVI_SEARCH ? "class=\"current\"" : "", cgiScriptName);
    o("\t<li><a %s href='%s/statistics' title=\"統計情報を表示します\">統計情報</a></li>\n", navi == NAVI_STATISTICS ? "class=\"current\"" : "", cgiScriptName);
    o("\t<li><a %s href='%s/register_at_once' title=\"複数新規にチケットを登録します\">チケット一括登録</a></li>\n", navi == NAVI_REGISTER_AT_ONCE ? "class=\"current\"" : "", cgiScriptName);
    o("\t<li><a %s href='%s/rss' title=\"RSS Feed\">RSS Feed</a></li>\n", navi == NAVI_RSS ? "class=\"current\"" : "", cgiScriptName);
    o("\t<li><a %s href='%s/help' title=\"ヘルプを参照します\">ヘルプ</a></li>\n", navi == NAVI_HELP ? "class=\"current\"" : "", cgiScriptName);
    o("\t<li><a %s href='%s/../admin.cgi' title=\"各種設定を行ないます\">管理ツール</a></li>\n", navi == NAVI_MANAGEMENT ? "class=\"current\"" : "", cgiScriptName);
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
void output_navigater(SearchResult* result, char* query_string)
{
    int i;
    if (result->hit_count < LIST_COUNT_PER_SEARCH_PAGE) return;
    o("<div class=\"navigater\">\n");
    if (result->page > 0)
        o("<a href=\"%s/search?p=%d&amp;%s\">&lt;&lt;</a>\n", cgiScriptName, result->page - 1, query_string);
    for (i = 0; i * LIST_COUNT_PER_SEARCH_PAGE < result->hit_count; i++) {
        if (i < result->page - 5 || i > result->page + 5)
            continue;
        if (i == result->page)
            o("%d\n", i + 1);
        else
            o("<a href=\"%s/search?p=%d&amp;%s\">%d</a>\n", cgiScriptName, i, query_string, i + 1);
    }
    if (result->page * LIST_COUNT_PER_SEARCH_PAGE < result->hit_count - LIST_COUNT_PER_SEARCH_PAGE)
        o("<a href=\"%s/search?p=%d&amp;%s\">&gt;&gt;</a>\n", cgiScriptName, result->page + 1, query_string);
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
            url_encode(value, encodedvalue, DEFAULT_LENGTH);
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
            url_encode(value, encodedvalue, DEFAULT_LENGTH);
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
    o(      "\t\t<th class=\"id\"><a href=\"%s/search?%ssort=-1&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">ID</a></th>\n", cgiScriptName, reverse ? "" : "r", string_rawstr(query_string_a));
    foreach (it, element_types) {
        ElementType* et = it->element;
        o("\t\t<th class=\"field%d\">\n", et->id);
        o("\t\t\t<a href=\"%s/search?%ssort=%d&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">", cgiScriptName, reverse ? "" : "r", et->id, string_rawstr(query_string_a));
        h(et->name);
        o("</a>\n");
        o("\t\t</th>\n");
    }
    o("\t\t<th class=\"registerdate\"><a href=\"%s/search?%ssort=-2&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">投稿日時</a></th>\n", cgiScriptName, reverse ? "" : "r", string_rawstr(query_string_a));
    o("\t\t<th class=\"lastregisterdate\"><a href=\"%s/search?%ssort=-3&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">最終更新日時</a></th>\n", cgiScriptName, reverse ? "" : "r", string_rawstr(query_string_a));
    o("\t\t<th class=\"leftdate\"><a href=\"%s/search?%ssort=-3&amp;%s#result\" title=\"押す度に昇順、降順で並べ替えを行ないます。\">放置日数</a></th>\n", cgiScriptName, reverse ? "" : "r", string_rawstr(query_string_a));
    o("\t</tr>\n");
    string_free(query_string_a);
}
void output_ticket_table_body(SearchResult* result, List* element_types)
{
    Iterator* it;
    Iterator* it_msg;

    foreach (it_msg, result->messages) {
        Message* message = it_msg->element;
        List* elements_a;
        list_alloc(elements_a, Element);
        elements_a = db_get_last_elements_4_list(message->id, elements_a);
        o("\t<tr>\n"
          "\t\t<td class=\"id field%d-\"><a href=\"%s/ticket/%s\">%s</a></td>\n", 
                ELEM_ID_ID, 
                cgiScriptName, 
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
                o("<a href=\"%s/ticket/%d\">", cgiScriptName, message->id);
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
    }
}
void output_ticket_table_status_index(SearchResult* result, List* element_types)
{
    o("<table summary=\"ticket list\">\n");
    output_ticket_table_header_no_link(element_types);
    output_ticket_table_body(result, element_types);
    o("</table>\n");
}
void output_ticket_table(SearchResult* result, List* element_types)
{
    o("<table summary=\"ticket list\">\n");
    output_ticket_table_header(element_types);
    output_ticket_table_body(result, element_types);
    o("</table>\n");
}
void output_states(List* states)
{
    Iterator* it;
    /* stateの表示 */
    o("<div id=\"state_index\">\n");
    o("\t<ul>\n");
    foreach (it, states) {
        State* s = it->element;
        o("\t\t<li>\n");
        o("\t\t\t<a href=\"%s/search?field%d=", cgiScriptName, ELEM_ID_STATUS);
        u(s->name);
        o("\" title=\"状態を条件にして検索を行ないます。\">");
        h(s->name);
        o("</a>");
        o("(%d)\n", s->count);
        o("\t\t</li>\n");
    }
    o("\t\t<li>\n");
    o("\t\t\t<form action=\"%s/search\" method=\"get\">\n", cgiScriptName);
    o("\t\t\t\t<input type=\"text\" class=\"number\" name=\"id\" title=\"入力したIDのチケットを表示します。\" maxlength=\"1000\" />\n"
      "\t\t\t\t<input type=\"submit\" class=\"button\" value=\"ID指定で表示\" />\n"
      "\t\t\t</form>\n"
      "\t\t</li>\n"
      "\t</ul>\n"
      "\t<br clear=\"all\" />\n"
      "</div>\n");
/*       "<br clear=\"all\" />\n"); */
}
/**
 * 一覧を表示するaction。
 */
void list_action()
{
    List* element_types_a;
    List* conditions_a = NULL;
    Project* project_a = xalloc(sizeof(Project));
    List* states_a;
    Iterator* it;
    List* messages_a;
    char** multi;

    db_init();
    project_a = db_get_project(project_a);
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
    element_types_a = db_get_element_types_4_list(element_types_a);
    o("<h2>"); h(project_a->name); o(" - 状態別チケット一覧</h2>\n");
    xfree(project_a);
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    output_states(states_a);
    list_free(states_a);
    fflush(cgiOut);
    o("<div id=\"ticket_list\">\n"
      "<h3>状態別チケット一覧</h3>\n"
      "<div class=\"description\">未クローズの状態毎にチケットを表示しています。\n");
    list_alloc(states_a, State);
    states_a = db_get_states_has_not_close(states_a);
    foreach (it, states_a) {
        State* s = it->element;
        o("\t\t\t<a href=\"#");
        h(s->name);
        o("\" title=\"ページ内へのリンク\">");
        h(s->name);
        o("</a>");
    }
    o("</div>\n"
      "<br clear=\"all\" />\n");
    foreach (it, states_a) {
        State* s = it->element;
        SearchResult* result_a = xalloc(sizeof(SearchResult));

        /* 検索 */
        list_alloc(conditions_a, Condition);
        list_alloc(messages_a, Message);
        result_a = db_get_tickets_by_status(s->name, messages_a, result_a);
        list_free(conditions_a);

        o("<a name=\""); h(s->name); o("\"></a>\n");
        o("<div>\n");
        o("<h4 class=\"status\">");h(s->name);o("&nbsp;(%d件)&nbsp;<a href=\"#top\">↑</a></h4>\n", s->count);
        if (result_a->hit_count == LIST_COUNT_PER_LIST_PAGE) {
            o("\t\t<div class=\"infomation\">新しい%d件のみを表示しています。<a href=\"%s/search?field%d=", 
                    result_a->hit_count, 
                    cgiScriptName,
                    ELEM_ID_STATUS);
            u(s->name);
            o("\">状態が"); h(s->name); o("である全てのチケットを表示する</a></div>\n");
        }
        output_ticket_table_status_index(result_a, element_types_a);
        if (result_a->hit_count == LIST_COUNT_PER_LIST_PAGE) {
            o("\t\t<div class=\"infomation\">続きがあります。<a href=\"%s/search?field%d=", 
                    cgiScriptName,
                    ELEM_ID_STATUS);
            u(s->name);
            o("\">状態が"); h(s->name); o("である全てのチケットを表示する</a></div>\n");
        }
        list_free(result_a->messages);
        xfree(result_a);
        o("</div>\n");
        fflush(cgiOut);
    }
    list_free(states_a);
    list_free(element_types_a);
    o("</div>\n");
    output_footer();
    db_finish();
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
List* create_conditions(List* conditions, List* element_types)
{
    char name[DEFAULT_LENGTH];
    char value[DEFAULT_LENGTH];
    Iterator* it;
    Condition* c;
    foreach (it, element_types) {
        ElementType* et = it->element;
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];

        switch (et->type) {
            case ELEM_TYPE_DATE:
                /* 日付 from */
                sprintf(name, "field%d_from", et->id);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                if (strlen(value) > 0) {
                    c = list_new_element(conditions);
                    c->element_type_id = et->id;
                    c->condition_type = CONDITION_TYPE_DATE_FROM;
                    strcpy(c->value, value);
                    list_add(conditions, c);
                }
                /* 日付 to */
                sprintf(name, "field%d_to", et->id);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                if (strlen(value) > 0) {
                    c = list_new_element(conditions);
                    c->element_type_id = et->id;
                    c->condition_type = CONDITION_TYPE_DATE_TO;
                    strcpy(c->value, value);
                    list_add(conditions, c);
                }
                break;
            default:
                sprintf(name, "field%d", et->id);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                if (strlen(value) == 0) continue;
                c = list_new_element(conditions);
                c->element_type_id = et->id;
                c->condition_type = CONDITION_TYPE_NORMAL;
                strcpy(c->value, value);
                list_add(conditions, c);
        }
    }
    /* 登録日時 */
    sprintf(name, "registerdate.from");
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    if (strlen(value)) {
        c = list_new_element(conditions);
        c->element_type_id = ELEM_ID_REGISTERDATE;
        c->condition_type = CONDITION_TYPE_DATE_FROM;
        strcpy(c->value, value);
        list_add(conditions, c);
    }
    sprintf(name, "registerdate.to");
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    if (strlen(value)) {
        c = list_new_element(conditions);
        c->element_type_id = ELEM_ID_REGISTERDATE;
        c->condition_type = CONDITION_TYPE_DATE_TO;
        strcpy(c->value, value);
        list_add(conditions, c);
    }
    /* 更新日時 */
    sprintf(name, "lastregisterdate.from");
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    if (strlen(value)) {
        c = list_new_element(conditions);
        c->element_type_id = ELEM_ID_LASTREGISTERDATE;
        c->condition_type = CONDITION_TYPE_DATE_FROM;
        strcpy(c->value, value);
        list_add(conditions, c);
    }
    sprintf(name, "lastregisterdate.to");
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
    if (strlen(value)) {
        c = list_new_element(conditions);
        c->element_type_id = ELEM_ID_LASTREGISTERDATE;
        c->condition_type = CONDITION_TYPE_DATE_TO;
        strcpy(c->value, value);
        list_add(conditions, c);
    }
    return conditions;
}
/**
 * 検索画面を表示するaction。
 */
void search_actoin()
{
    SearchResult* result_a = xalloc(sizeof(SearchResult));
    List* element_types_a;
    List* conditions_a = NULL;
    Condition* sort_a = NULL;
    Project* project_a = xalloc(sizeof(Project));
    List* states_a;
    Iterator* it;
    char id[DEFAULT_LENGTH];
    char q[DEFAULT_LENGTH];
    char p[DEFAULT_LENGTH];
    char registerdate_from[DEFAULT_LENGTH];
    char registerdate_to[DEFAULT_LENGTH];
    char updatedate_from[DEFAULT_LENGTH];
    char updatedate_to[DEFAULT_LENGTH];
    List* messages_a;
    int col_index;

    cgiFormStringNoNewlines("id", id, DEFAULT_LENGTH);
    if (strlen(id) > 0) {
        char uri[DEFAULT_LENGTH];
        sprintf(uri, "/ticket/%s", id);
        redirect(uri, NULL);
    }
    
    db_init();
    project_a = db_get_project(project_a);
    output_header(project_a, "チケット検索", "calendar.js", NAVI_SEARCH);
    output_calendar_js();
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_4_list(element_types_a);
    o("<h2>"); h(project_a->name); o(" - チケット検索</h2>\n");
    xfree(project_a);
    /* 検索 */
    list_alloc(conditions_a, Condition);
    conditions_a = create_conditions(conditions_a, element_types_a);
    cgiFormStringNoNewlines("q", q, DEFAULT_LENGTH);
    sort_a = xalloc(sizeof(Condition));
    sort_a = create_sort_condition(sort_a);
    cgiFormStringNoNewlines("p", p, DEFAULT_LENGTH);
    list_alloc(messages_a, Message);
    result_a = db_search_tickets(conditions_a, q, sort_a, atoi(p), messages_a, result_a);
    xfree(conditions_a);
    xfree(sort_a);
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    output_states(states_a);
    list_free(states_a);
    o("<div id=\"condition_form\">\n"
      "<h3>検索条件</h3>\n"
      "<div class=\"description\">検索条件を入力して検索ボタンを押してください。</div>\n");
    o("<form action=\"%s/search\" method=\"get\">\n", cgiScriptName);
    o(      "<table summary=\"condition table\">\n");
    cgiFormStringNoNewlines("registerdate.from", registerdate_from, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("registerdate.to", registerdate_to, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("lastregisterdate.from", updatedate_from, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("lastregisterdate.to", updatedate_to, DEFAULT_LENGTH);
    o("<tr>\n"
      "\t<th>キーワード検索</th>\n"
      "\t<td>\n"
      "\t\t<input type=\"text\" class=\"conditionelement\" name=\"q\" value=\""); v(q); o("\" maxlength=\"1000\" />\n"
      "\t\t<div class=\"description\">履歴も含めて全ての項目から検索を行ないます。</div>\n"
      "\t</td>\n"
      "\t<th>投稿日時</th>\n"
      "\t<td>\n"
      "\t\t<span>\n"
      "\t\t<input type=\"text\" class=\"calendar\" name=\"registerdate.from\" value=\""); v(registerdate_from); o("\" maxlength=\"20\" />\n"
      "\t\t</span>\n"
      "〜\n"
      "\t\t<span>\n"
      "\t\t<input type=\"text\" class=\"calendar\" name=\"registerdate.to\" value=\""); v(registerdate_to); o("\" maxlength=\"20\" />\n"
      "\t\t</span>\n"
      "\t\t<div class=\"description\">yyyy-mm-dd形式で入力してください。</div>\n"
      "\t</td>\n"
      "\t<th>更新日時</th>\n"
      "\t<td>\n"
      "\t\t<span>\n"
      "\t\t<input type=\"text\" class=\"calendar\" name=\"lastregisterdate.from\" value=\""); v(updatedate_from); o("\" maxlength=\"20\" />\n"
      "\t\t</span>\n"
      "〜\n"
      "\t\t<span>\n"
      "\t\t<input type=\"text\" class=\"calendar\" name=\"lastregisterdate.to\" value=\""); v(updatedate_to); o("\" maxlength=\"20\" />\n"
      "\t\t</span>\n"
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
        output_form_element_4_condition(et);
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
    o("</table>\n"
      "<input class=\"button\" type=\"submit\" value=\"検索\" />"
      "</form>\n"
      "</div>\n");
    fflush(cgiOut);
    o("<div id=\"ticket_list\">\n"
      "<a name=\"result\"></a>\n"
      "<h3>検索結果</h3>\n");

    if (result_a->messages->size) {
        String* query_string_a = string_new(0);

        query_string_a = format_query_string_without_page(query_string_a);
        o(      "<div class=\"infomation\">");
        o(      "%d件ヒットしました。\n", result_a->hit_count);
        o(      "<a href=\"%s/report_csv_download?%s\" target=\"_blank\">検索結果をCSVでダウンロードする。</a>\n",
                cgiScriptName,
                string_rawstr(query_string_a));
        o(      "</div>\n");
        output_navigater(result_a, string_rawstr(query_string_a));
        output_ticket_table(result_a, element_types_a);
        output_navigater(result_a, string_rawstr(query_string_a));
        string_free(query_string_a);
    }
    o("</div>\n");
    output_footer();
    db_finish();
    list_free(result_a->messages);
    list_free(element_types_a);
    xfree(result_a);
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
void output_ticket_information_4_csv_report(SearchResult* result, List* element_types)
{
    Iterator* it;
    Iterator* it_msg;

    foreach (it_msg, result->messages) {
        Message* message = it_msg->element;
        List* elements_a;
        char id_str[DEFAULT_LENGTH];
        sprintf(id_str, "%d", message->id);
        list_alloc(elements_a, Element);
        elements_a = db_get_last_elements(message->id, elements_a);
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
    }
}
/**
 * CSVレポートをダウンロードするaction。
 */
void report_csv_download_action()
{
    SearchResult* result_a = xalloc(sizeof(SearchResult));
    List* element_types_a;
    List* conditions_a = NULL;
    Condition* sort_a = NULL;
    Project* project_a = xalloc(sizeof(Project));
    List* states_a;
    char q[DEFAULT_LENGTH];
    List* messages_a;

    db_init();
    project_a = db_get_project(project_a);
    list_alloc(element_types_a, ElementType);

    element_types_a = db_get_element_types_all(element_types_a);
    /* 検索 */
    list_alloc(conditions_a, Condition);
    conditions_a = create_conditions(conditions_a, element_types_a);
    cgiFormStringNoNewlines("q", q, DEFAULT_LENGTH);
    sort_a = xalloc(sizeof(Condition));
    create_sort_condition(sort_a);
    list_alloc(messages_a, Message);
    result_a = db_search_tickets_4_report(conditions_a, q, sort_a, messages_a, result_a);
    xfree(conditions_a);
    xfree(sort_a);
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);

    o("Content-Disposition: attachment; filename=\"report.csv\"\r\n");
    cgiHeaderContentType("text/x-csv; charset=Windows-31J;");

    csv_field(project_a->name); o("\r\n");
    xfree(project_a);
    output_ticket_information_4_csv_report_header(element_types_a);
    output_ticket_information_4_csv_report(result_a, element_types_a);
    db_finish();
    list_free(result_a->messages);
    list_free(element_types_a);
    xfree(result_a);
}
/**
 * form要素を表示する。
 */
void output_form_element_4_condition(ElementType* et)
{
    char id[DEFAULT_LENGTH];
    List* items_a;
    Iterator* it;
    char name[DEFAULT_LENGTH];
    char value[DEFAULT_LENGTH];
    sprintf(name, "field%d", et->id);
    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);

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
            o("\" />\n");
            break;
        case ELEM_TYPE_LIST_SINGLE:
        case ELEM_TYPE_LIST_MULTI:
            o("<select class=\"element\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\">\n");
            list_alloc(items_a, ListItem);
            items_a = db_get_list_item(et->id, items_a);

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
            sprintf(name, "field%d_from", et->id);
            cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
            o("<span>\n");
            o("<input type=\"text\" class=\"calendar\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("_from\" value=\"");
            v(value);
            o("\" maxlength=\"10\" />\n");
            o("</span>\n");
            o("〜\n");
            sprintf(name, "field%d_to", et->id);
            cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
            o("<span>\n");
            o("<input type=\"text\" class=\"calendar\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("_to\" value=\"");
            v(value);
            o("\" maxlength=\"10\" />\n");
            o("</span>\n");
            break;
    }
}
/**
 * form要素を表示する。
 */
void output_form_element(List* elements, ElementType* et)
{
    char* value = "";
    List* items_a;
    Iterator* it;
    int list_count = 0;

    if (elements != NULL) {
        value = get_element_value(elements, et);
    } else {
        if (et->id == ELEM_ID_SENDER) {
            char sender[DEFAULT_LENGTH];
            cgiCookieString(COOKIE_SENDER, sender, DEFAULT_LENGTH);
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
            o("\" maxlength=\"1000\" />\n");
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
            items_a = db_get_list_item(et->id, items_a);
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
            items_a = db_get_list_item(et->id, items_a);
            /* リストの要素数をカウントする */
            foreach (it, items_a) list_count++;
            o("<select size=\"%d\" id=\"field%d", list_count + 1, et->id);
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
            o("\" maxlength=\"10\"/>\n"
              "</span>\n"
              "<div class=\"description\">yyyy-mm-dd形式で入力してください。</div>\n");
            break;
    }
    switch (et->type) {
        case ELEM_TYPE_LIST_SINGLE:
        case ELEM_TYPE_LIST_MULTI:
            if (et->auto_add_item) {
                /* 新規項目を設定可能である場合、テキストボックスを表示する。 */
                o("<input type=\"text\" class=\"element_new_item\" id=\"field%d.new_item\" name=\"field%d.new_item\" maxlength=\"1000\" />\n",
                    et->id, et->id);
                o("選択肢を追加する場合はテキストボックスに入力してください。\n");
            }
            break;
    }
}
static int contains(char* const value, const char* name)
{
    char* p = value;
    do {
        if (*p == '\t') p++;
        if (strncmp(p, name, strlen(name)) == 0)
            return 1;
    } while ((p = strstr(p, "\t")) != NULL);
    return 0;
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
    Project* project_a = xalloc(sizeof(Project));
    List* states_a;
    char sender[DEFAULT_LENGTH];
    cgiCookieString(COOKIE_SENDER, sender, DEFAULT_LENGTH);

    db_init();
    project_a = db_get_project(project_a);
    output_header(project_a, "チケット登録", "register.js", NAVI_REGISTER);
    output_calendar_js();
    o(      "<h2>"); h(project_a->name);o(" - チケット登録</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    output_states(states_a);
    list_free(states_a);
    o(      "<div id=\"input_form\">\n"
            "<h3>チケット登録</h3>\n"
            "<div class=\"description\">新規チケットを登録する場合は、以下のフォームを記入し登録ボタンを押してください。</div>\n"
            "<noscript><div class=\"description\">※必須項目の入力チェックは、javascriptで行なっています。</div></noscript>\n");
    o(      "<form id=\"register_form\" name=\"register_form\" action=\"%s/register_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName);
    o(      "<table summary=\"input infomation\">\n");
    xfree(project_a);
    {
        List* element_types_a;
        Iterator* it;
        list_alloc(element_types_a, ElementType);
        element_types_a = db_get_element_types_all(element_types_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            /* 返信専用属性は表示しない。 */
            if (et->reply_property == 1) continue;
            o("\t<tr>\n");
            o("\t\t<th %s>", et->required ? "class=\"required\"" : "");
            h(et->name);
            if (et->required) {
                o("<span class=\"required\">※</span>");
            }
            o("</th><td>\n");
            if (et->required)
                o("\t\t\t<div id=\"field%d.required\" class=\"error\"></div>\n", et->id);
            if (et->type == ELEM_TYPE_DATE)
                o("\t\t<div id=\"field%d.datefield\" class=\"error\"></div>\n", et->id);
            output_form_element(NULL, et);
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
            "</div>\n", strlen(sender) ? "checked" : "");
    o(      "<div class=\"description\">\n"
            "\t<ul>\n"
            "\t\t<li>複数行テキスト項目の内容では、#1 のように書くと、ID が 1 のチケットのチケット詳細ページへのリンクとなります。</li>\n"
            "\t\t<li>複数行テキスト項目の内容では、行頭が >| から始まる行から、行頭が |< から始まる行までは、整形済みブロックになります。</li>\n"
            "\t</ul>\n"
            "</div>\n");
    db_finish();
    output_footer();
}
/**
 * チケット詳細画面を表示するaction。
 */
void ticket_action()
{
    String* title_a = string_new(0);
    char path_info[DEFAULT_LENGTH];
    char* ticket_id;
    List* elements_a = NULL;
    List* last_elements = NULL;
    List* element_types_a;
    Iterator* it;
    int iid, *message_ids_a, i;
    Project* project_a = xalloc(sizeof(Project));
    char sender[DEFAULT_LENGTH];
    cgiCookieString(COOKIE_SENDER, sender, DEFAULT_LENGTH);

    strcpy(path_info, cgiPathInfo);
    ticket_id = strchr(path_info + 1, '/');
    if (ticket_id) ticket_id++;
    iid = atoi(ticket_id);
    if (!iid) {
        redirect("/list", "存在しないIDが指定されました。");
            return;
    }
    db_init();
    list_alloc(elements_a, Element);
    elements_a = db_get_last_elements(iid, elements_a);
    if (elements_a->size == 0) {
        redirect("/list", "存在しないIDが指定されました。");
        return;
    }
    string_appendf(title_a, "#%d %s", iid, get_element_value_by_id(elements_a, ELEM_ID_TITLE));
    project_a = db_get_project(project_a);
    output_header(project_a, string_rawstr(title_a), "reply.js", NAVI_OTHER);
    output_calendar_js();

    message_ids_a = db_get_message_ids_a(iid);
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(element_types_a);
    o("<h2 id=\"subject\">"); h(project_a->name); o(" - ");
    xfree(project_a);
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
                        o("<a href=\"%s/download/%d/", 
                                cgiScriptName, 
                                db_get_element_file_id(atoi(id_str), et->id)); 
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
        attachment_elements_a = db_get_elements(message_ids_a[i], attachment_elements_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            if (et->type == ELEM_TYPE_UPLOADFILE) {
                char* attachment_file_name = get_element_value(attachment_elements_a, et);
                if (strlen(attachment_file_name) == 0) continue;
                o("\t\t<span>\n");
                o("<a href=\"%s/download/%d/", 
                        cgiScriptName, 
                        db_get_element_file_id(message_ids_a[i], et->id)); 
                u(attachment_file_name); o("\" target=\"_blank\">");h(attachment_file_name); o("</a>\n");
                o("\t\t&nbsp;</span>\n");
            }
        }
        list_free(attachment_elements_a);
    }
    o("\t\t\t</td>\n"
      "\t</tr>\n"
            "</table>\n"
            "</div>"
            "<div class=\"infomation\"><a href=\"#reply\">返信する</a></div>\n"
            "<div id=\"ticket_history\">\n"
            "<h3>チケット履歴</h3>\n"
            "<div class=\"description\">チケットの履歴情報です。</div>\n");
    list_free(elements_a);
    /* 履歴の表示 */
    for (i = 0; message_ids_a[i] != 0; i++) {
        List* previous = last_elements;
        list_alloc(elements_a, Element);
        last_elements = elements_a = db_get_elements(message_ids_a[i], elements_a);
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
                            int file_id = db_get_element_file_id(message_ids_a[i], et->id);
                            o("<a href=\"%s/download/%d/", cgiScriptName, file_id); 
                            u(value); o("\" target=\"_blank\">");h(value); o("</a>\n");
                            mime_type = db_get_element_file_mime_type(message_ids_a[i], et->id, buf);
                            if (strstr(mime_type, "image") != NULL) {
                                o("<div>\n");
                                o("<img class=\"attachment_image\" src=\"%s/download/%d\" alt=\"attachment file\" />\n",
                                        cgiScriptName, file_id);
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
        o("</table>\n");
    }
    xfree(message_ids_a);
    o(  "</div>\n");
    /* フォームの表示 */
    o(      "<a name=\"reply\"></a>\n"
            "<div id=\"input_form\">\n"
            "<h3>チケット返信</h3>\n"
            "<form id=\"reply_form\" name=\"reply_form\" action=\"%s/register_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName);
    o(      "<input type=\"hidden\" name=\"ticket_id\" value=\"%s\" />\n", ticket_id);
    o(      "<div class=\"description\">返信を行なう場合は、以下のフォームに内容を記入して返信ボタンを押してください。</div>\n"
            "<noscript><div class=\"description\">※必須項目の入力チェックは、javascriptで行なっています。</div></noscript>\n");
    o(      "<h4>チケット情報の更新</h4>\n"
            "<div class=\"description\">チケット情報(チケットの状態など)を更新する必要がある場合は、以下の情報を変更してください。</div>\n"
            "<table summary=\"input table\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        if (et->ticket_property == 0) continue;
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
        if (last_elements != NULL) {
            if (et->ticket_property)
                output_form_element(last_elements, et);
            else
                output_form_element(NULL, et);
        } else {
            output_form_element(NULL, et);
        }
        o("\t\t<div class=\"description\">");h(et->description);o("&nbsp;</div>\n");
        o("\t</td>\n");
        o("\t</tr>\n");
    }
    o(      "</table>\n");
    o(      "<h4>返信情報の追加</h4>\n"
            "<div class=\"description\">返信情報を記入してください。</div>\n"
            "<table summary=\"input table\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        if (et->ticket_property == 1) continue;
        o("\t<tr>\n");
        o("\t\t<th %s>", et->required ? "class=\"required\"" : "");
        if (et->ticket_property)
            o("&nbsp;<span class=\"ticket_property\" title=\"チケット属性\">");
        h(et->name);
        if (et->required)
            o("<span class=\"required\">※</span>");
        if (et->ticket_property)
            o("</span>");
        o("</th>\n\t\t<td>");
        if (et->required)
            o("\t\t<div id=\"field%d.required\" class=\"error\"></div>\n", et->id);
        if (et->type == ELEM_TYPE_DATE)
            o("\t\t<div id=\"field%d.datefield\" class=\"error\"></div>\n", et->id);
        if (last_elements != NULL) {
            if (et->ticket_property)
                output_form_element(last_elements, et);
            else
                output_form_element(NULL, et);
        } else {
            output_form_element(NULL, et);
        }
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
            "\t\t<li>複数行テキスト項目の内容では、行頭が >| から始まる行から、行頭が |< から始まる行までは、整形済みブロックになります。</li>\n"
            "\t</ul>\n"
            "</div>\n");
    output_field_information_js(element_types_a);
    db_finish();
    output_footer();
    list_free(element_types_a);
}
void register_list_item(int id, char* name)
{
    ListItem* item_a = xalloc(sizeof(ListItem));
    item_a->element_type_id = id;
    strcpy(item_a->name, name);
    item_a->close = 0;
    item_a->sort = 0;
    db_register_list_item(item_a);
    xfree(item_a);
}
/**
 * 登録するaction。
 * 登録モード、編集モード、削除モードがある。
 */
void register_submit_action()
{
    Project* project_a = xalloc(sizeof(Project));
    List* element_types_a;
    Iterator* it;
    List* elements_a = NULL;
    Message* ticket_a;
    char ticket_id[DEFAULT_LENGTH];
    ModeType mode = get_mode();
    char** multi;
    char save2cookie[2];
    char* complete_message = NULL;
    HOOK* hook = NULL;

    cgiFormStringNoNewlines("save2cookie", save2cookie, 2);
    if (mode == MODE_INVALID)
        die("requested invalid mode.");
    ticket_a = xalloc(sizeof(Message));
    db_init();
    project_a = db_get_project(project_a);
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(element_types_a);
    cgiFormStringNoNewlines("ticket_id", ticket_id, DEFAULT_LENGTH);
    if (mode == MODE_REGISTER)
        ticket_a->id = -1;
    else
        ticket_a->id = atoi(ticket_id);
    list_alloc(elements_a, Element);
    if (mode == MODE_REGISTER || mode == MODE_REPLY) {
        char* value_a = xalloc(sizeof(char) * VALUE_LENGTH); /* 1M */
        /* register, reply */
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            Element* e = list_new_element(elements_a);
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
                    e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, value_a);
                    break;
                case ELEM_TYPE_TEXTAREA:
                    cgiFormString(name, value_a, VALUE_LENGTH);
                    e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, value_a);
                    break;
                case ELEM_TYPE_CHECKBOX:
                    cgiFormString(name, value_a, VALUE_LENGTH);
                    e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, value_a);
                    break;
                case ELEM_TYPE_LIST_SINGLE:
                    /* 新規選択肢 */
                    cgiFormString(name_new_item, value_a, VALUE_LENGTH);
                    if (strlen(value_a)) {
                        e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
                        strcpy(e->str_val, value_a);
                        /* 新しく選択肢を追加 */
                        register_list_item(et->id, value_a);
                    } else {
                        cgiFormString(name, value_a, VALUE_LENGTH);
                        e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
                        strcpy(e->str_val, value_a);
                    }
                    break;
                case ELEM_TYPE_LIST_MULTI:
                    /* 新規選択肢 */
                    cgiFormString(name_new_item, value_a, VALUE_LENGTH);
                    if (strlen(value_a)) {
                        e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
                        strcpy(e->str_val, value_a);
                        /* 新しく選択肢を追加 */
                        register_list_item(et->id, value_a);
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
                    e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, value_a);
                    cgiStringArrayFree(multi);
                    break;
                case ELEM_TYPE_UPLOADFILE:
                    if (get_upload_size(et->id) > MAX_FILE_SIZE * 1024) {
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
                    cgiHeaderCookieSetString(COOKIE_SENDER, e->str_val, 86400 * 30, "/", cgiServerName);
                } else {
                    cgiHeaderCookieSetString(COOKIE_SENDER, "", 0, "/", cgiServerName);
                }
            }
            list_add(elements_a, e);
        }
        xfree(value_a);
        ticket_a->elements = elements_a;
        db_begin();
        ticket_a->id = db_register_ticket(ticket_a);
        db_commit();
        /* hook */
        hook = init_hook(HOOK_MODE_REGISTERED);
        hook = exec_hook(hook, project_a, ticket_a, elements_a, element_types_a);
        if (mode == MODE_REGISTER)
            complete_message = "登録しました。";
        else if (mode == MODE_REPLY)
            complete_message = "返信しました。";
        xfree(project_a);
        list_free(element_types_a);
        free_element_list(elements_a);
        xfree(ticket_a);
    }
    db_finish();

    redirect_with_hook_messages("/list", complete_message, hook->results);
    if (hook) clean_hook(hook);
    return;

file_size_error:
    db_rollback();
    db_finish();
    output_header(project_a, "エラー", NULL, NAVI_OTHER);
    o("<h1>エラー発生</h1>\n"
      "<div class=\"message\">ファイルサイズが大きすぎます。%dkbより大きいファイルは登録できません。ブラウザの戻るボタンで戻ってください。</div>\n", MAX_FILE_SIZE);
    output_footer();
    xfree(project_a);
}
/**
 * 一括登録画面を表示するaction。
 */
void register_at_once_action()
{
    Project* project_a = xalloc(sizeof(Project));
    List* states_a;
    char sender[DEFAULT_LENGTH];
    cgiCookieString(COOKIE_SENDER, sender, DEFAULT_LENGTH);

    db_init();
    project_a = db_get_project(project_a);
    output_header(project_a, "チケット一括登録", "register_at_once.js", NAVI_REGISTER_AT_ONCE);
    output_calendar_js();
    o(      "<h2>"); h(project_a->name);o(" - チケット一括登録</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    output_states(states_a);
    list_free(states_a);
    o(      "<div id=\"input_form\">\n"
            "<h3>チケット一括登録</h3>\n"
            "<div class=\"description\">新規チケットを一括登録する場合は、以下のフォームを記入し登録ボタンを押してください。</div>\n"
            "<noscript><div class=\"description\">※必須項目の入力チェックは、javascriptで行なっています。</div></noscript>\n");
    o(      "<form id=\"register_form\" name=\"register_form\" action=\"%s/register_at_once_confirm\" method=\"post\">\n", cgiScriptName);
    xfree(project_a);
    {
        /* 一括用、CSV形式フィールド */
        o("<table summary=\"input infomation\">\n"
          "\t<tr>\n"
          "\t\t<th class=\"required\">CSV"
          "<span class=\"required\">※</span>"
          "</th><td>\n"
          "\t\t\t<div id=\"field.csvdata.required\" class=\"error\"></div>\n"
          "\t\t\t<textarea name=\"csvdata\" id=\"csvdata\" row=\"5\" col=\"5\" wrap=\"off\"></textarea>\n"
          "\t\t\t<div class=\"description\">登録したいデータをCSV形式で貼り付けてください。&nbsp;</div>\n"
          "\t\t</td>\n"
          "\t</tr>\n"
          "</table>\n");
    }
    o(      "<input class=\"button\" type=\"submit\" name=\"register\" value=\"登録\" />\n"
            "</form>\n"
            "</div>\n");
    db_finish();
    output_footer();
}
/**
 * 一括登録確認画面を表示するaction。
 */
void register_at_once_confirm_action()
{
    int i;
    int row = 0;
    Project* project_a = xalloc(sizeof(Project));
    List* states_a;
    Csv* csv_a;
    char sender[DEFAULT_LENGTH];
    char* content_a = xalloc(sizeof(char) * VALUE_LENGTH);
    cgiCookieString(COOKIE_SENDER, sender, DEFAULT_LENGTH);

    db_init();
    d("start\n");
    cgiFormString("csvdata", content_a, VALUE_LENGTH);
    d("content_a: %s\n", content_a);
    csv_a = csv_new(content_a);



    project_a = db_get_project(project_a);
    output_header(project_a, "チケット一括登録確認", "register_at_once.js", NAVI_REGISTER_AT_ONCE);
    output_calendar_js();
    o(      "<h2>"); h(project_a->name);o(" - チケット一括登録確認</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    output_states(states_a);
    list_free(states_a);
    o(      "<div id=\"input_form\">\n"
            "<h3>チケット一括登録確認</h3>\n"
            "<div class=\"description\">各カラムの項目を選択して、登録ボタンを押してください。</div>\n"
            "<noscript><div class=\"description\">※必須項目の入力チェックは、javascriptで行なっています。</div></noscript>\n");
    o(      "<form id=\"register_form\" name=\"register_form\" action=\"%s/register_at_once_submit\" method=\"post\">\n", cgiScriptName);
    o(      "<table summary=\"input infomation\">\n");
    xfree(project_a);
    {
        int line_count = 0, fields_count = 0;
        List* element_types_a;
        Iterator* it;
        Iterator* it_line;
        list_alloc(element_types_a, ElementType);
        element_types_a = db_get_element_types_all(element_types_a);
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
            if (et->type == ELEM_TYPE_DATE)
                o("\t\t<div id=\"field%d.datefield\" class=\"error\"></div>\n", et->id);
            output_form_element(NULL, et);
            o("\t\t\t<div class=\"description\">");h(et->description);o("&nbsp;</div>\n");
            o("\t\t</td>\n");
            o("\t</tr>\n");
        }
        o("</table>\n");
        o("<input type=\"hidden\" name=\"fields_count\" value=\"%d\" />\n", csv_a->field_count);
        o(      "<table id=\"register_at_once_confirm\">\n"
                "\t<tr>\n");
        o(  "\t\t<th>&nbsp;</th>\n");
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
            d("csv field size %d\n", line->fields->size);
            o("\t<tr>\n");
            o("\t\t<th>%d", (row++) + 1);
            o("</th>\n");
            fields_count = 0;
            foreach (it_fields, line->fields) {
                CsvField* field = it_fields->element;
                d("csv col %s\n", string_rawstr(field->data));
                o("\t\t<td>\n");
                o("\t\t\t<textarea name=\"csvfield%d.%d\" row=\"5\" col=\"5\">", line_count, fields_count); h(string_rawstr(field->data)); o("</textarea>\n");
                o("\t\t</td>\n");
                fields_count++;
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
            "</form>\n"
            "</div>\n", strlen(sender) ? "checked" : "");
    db_finish();
    output_footer();
}
void register_at_once_submit_action()
{
    int row_count = 0, col_count = 0;
    Iterator* it;
    char save2cookie[2];
    char* value_a;
    char senderfield[DEFAULT_LENGTH];
    char sender[DEFAULT_LENGTH];
    char fields_count_str[5];
    List* field_ids_a;
    int i = 0;
    int fields_count;

    cgiFormStringNoNewlines("save2cookie", save2cookie, 2);
    sprintf(senderfield, "field%d", ELEM_ID_SENDER);
    cgiFormStringNoNewlines(senderfield, sender, DEFAULT_LENGTH);
    if (strcmp(save2cookie, "1") == 0) {
        d("set cookie: %s, %s, %s\n", sender, cgiScriptName, cgiServerName);
        cgiHeaderCookieSetString(COOKIE_SENDER, sender, 86400 * 30, "/", cgiServerName);
    } else {
        cgiHeaderCookieSetString(COOKIE_SENDER, "", 0, "/", cgiServerName);
    }

    cgiFormStringNoNewlines("fields_count", fields_count_str, 5);
    fields_count = atoi(fields_count_str);
    d("1 fields_count %d\n", fields_count);
    /* 項目一覧を取得する。 */
    list_alloc(field_ids_a, int);
    for (i = 0; i < fields_count; i++) {
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        int* field_id;
        sprintf(name, "col_field%d", i);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        d("1 %s %d %s\n", name, i, value);
        field_id = list_new_element(field_ids_a);
        if (strlen(value) != 0) {
            *field_id = atoi(value);
        } else {
            *field_id = -1;
        }
        list_add(field_ids_a, field_id);
        d("1 add field\n");
    }
    d("2\n");
    db_init();
    db_begin();
    while (1) {
        Message* ticket_a = xalloc(sizeof(Message));
        List* elements_a = NULL;
        ticket_a->id = -1;
        list_alloc(elements_a, Element);
        value_a = xalloc(sizeof(char) * VALUE_LENGTH); /* 1M */
        /* register */
        col_count = -1;
        foreach (it, field_ids_a) {
            int* field_id = it->element;
            Element* e;
            col_count++;
            if (*field_id == -1) continue;
            e = list_new_element(elements_a);
            char name[DEFAULT_LENGTH] = "";
    d("3 field_id: %d\n", *field_id);
            sprintf(name, "csvfield%d.%d", row_count, col_count);
            strcpy(value_a, "");
            e->element_type_id = *field_id;
            e->is_file = 0;
            cgiFormString(name, value_a, VALUE_LENGTH);
            if (strlen(value_a) == 0) continue; /* 値が無ければelementを追加しない */
            e->str_val = xalloc(sizeof(char) * strlen(value_a) + 1);
            strcpy(e->str_val, value_a);
    d("3 value: %s\n", value_a);
            list_add(elements_a, e);
        }
    d("4\n");
        if (elements_a->size == 0) break; /* elementが無い状態だったら、登録処理終了。 */
        xfree(value_a);
        ticket_a->elements = elements_a;
        ticket_a->id = db_register_ticket(ticket_a);
        free_element_list(elements_a);
        xfree(ticket_a);
        row_count++;
    }
    d("5\n");
    db_commit();
    db_finish();

    redirect("/list", "登録しました。");
    list_free(field_ids_a);
    return;

}
/**
 * デフォルトのaction。
 */
void top_action()
{
    Project* project_a = xalloc(sizeof(Project));
    List* tickets_a;
    List* states_a;
    Iterator* it;

    db_init();
    project_a = db_get_project(project_a);
    output_header(project_a, "トップページ", NULL, NAVI_TOP);
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    o(      "<div id=\"info\">\n");
    /* 最新情報の表示 */
    o(      "<div id=\"top_newest\">\n"
            "<h4>最新情報</h4>\n"
            "\t<ul>\n");
    list_alloc(tickets_a, Message);
    tickets_a = db_get_newest_information(10, tickets_a);
    if (tickets_a->size) {
        foreach (it, tickets_a) {
            Message* ticket = it->element;
            List* elements_a;
            list_alloc(elements_a, Element);
            elements_a = db_get_last_elements_4_list(ticket->id, elements_a);
            o("\t\t<li>\n");
            o("\t\t\t<a href=\"%s/ticket/%d", cgiScriptName, ticket->id); o("\">");
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
            o("\t\t\t<a href=\"%s/search?field%d=", cgiScriptName, ELEM_ID_STATUS); u(s->name); o("\">");
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
            "\t<form action=\"%s/search\" method=\"get\">\n", cgiScriptName);
    o(      "\t\t<input type=\"text\" class=\"number\" name=\"id\" title=\"入力したIDのチケットを表示します。\" maxlength=\"1000\" />\n"
            "\t\t<input type=\"submit\" class=\"button\" value=\"表示\" />\n"
            "\t</form>\n"
            "</div>\n"
            "</div>\n"
            "<div id=\"main\">\n"
            "<h2>");h(project_a->name);o("&nbsp;</h2>\n");
    xfree(project_a);
    o(      "<div id=\"main_body\">\n"
            "<div class=\"top_edit\"><a href=\"%s/edit_top\">トップページの編集</a></div>\n", cgiScriptName);
    wiki_out("top");
    o(      "</div>\n"
            "</div>\n");
    db_finish();
    output_footer();
}

void rss_action()
{
    List* tickets_a;
    Iterator* it;
    Project* project_a = xalloc(sizeof(Project));

    db_init();
    project_a = db_get_project(project_a);
    cgiHeaderContentType("text/xml; charset=utf-8;");
    o(      "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
            "<rdf:RDF \n"
            "\t\txmlns=\"http://purl.org/rss/1.0/\"\n"
            "\t\txmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" \n"
            "\t\txml:lang=\"ja\">\n"
            "\n"
            "\t<channel rdf:about=\"");h(project_a->home_url);o("%s/rss\">\n", cgiScriptName);
    o(      "\t\t<title>");h(project_a->name); o("</title>\n"
            "\t\t<link>");h(project_a->home_url);o("/bt/</link>\n");
    o(      "\t\t<description>");h(project_a->name);o("</description>\n"
            "\t\t<items>\n"
            "\t\t\t<rdf:Seq>\n");
    o(      "\t\t\t\t<rdf:li rdf:resource=\"");h(cgiServerName);o("%s/list\"/>\n", cgiScriptName);
    o(      "\t\t\t\t<rdf:li rdf:resource=\"");h(cgiServerName);o("%s/register\"/>\n", cgiScriptName);
    o(      "\t\t\t</rdf:Seq>\n"
            "\t\t</items>\n"
            "\t</channel>\n");

    list_alloc(tickets_a, Message);
    tickets_a = db_get_newest_information(10, tickets_a);
    if (tickets_a != NULL) {
        foreach (it, tickets_a) {
            Message* ticket = it->element;
            List* elements_a;
            Iterator* it;
            list_alloc(elements_a, Element);
            elements_a = db_get_last_elements_4_list(ticket->id, elements_a);
            o(      "\t<item rdf:about=\"");h(project_a->home_url);o("%s/ticket/%d\">\n", cgiScriptName, ticket->id);
            o(      "\t\t<title>ID:%5d ", ticket->id);
            h(get_element_value_by_id(elements_a, ELEM_ID_TITLE));
            o(      "</title>\n");
            o(      "\t\t<link>");h(project_a->home_url);o("%s/ticket/%d</link>\n", cgiScriptName, ticket->id);
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
    xfree(project_a);
    list_free(tickets_a);
    o(      "</rdf:RDF>\n");
    db_finish();
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
    Project* project_a = xalloc(sizeof(Project));
    List* element_types_a;
    Iterator* it;
    List* states_a;

    db_init();
    project_a = db_get_project(project_a);
    output_header(project_a, "統計情報", "graph.js", NAVI_STATISTICS);
    output_graph_js();
    o(      "<h2>");h(project_a->name);o("</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    output_states(states_a);
    list_free(states_a);
    o(      "<div id=\"top\">\n"
            "<h3>統計情報</h3>\n"
            "\t<div class=\"description\">統計情報を表示します。</div>\n");
    xfree(project_a);
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(element_types_a);
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        List* items_a;
        List* all_items_a;
        Iterator* it_item;

        list_alloc(items_a, State);
        list_alloc(all_items_a, ListItem);
        switch (et->type) {
            case ELEM_TYPE_LIST_SINGLE:
                items_a = db_get_statictics(items_a, et->id);
                goto got_item;
            case ELEM_TYPE_LIST_MULTI:
                items_a = db_get_statictics_multi(items_a, et->id);
got_item:
                all_items_a = db_get_list_item(et->id, all_items_a);
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
                    o("\t\t[\""); h(s->name); o("\t(%d)\", %d]", count, count);
                    if (iterator_next(it_item)) o(",");
                    o("\n");
                }
                o(      "\t\t];\n"
                        "\t\t// -->\n"
                        "\t\t</script>\n"
                  "<canvas width=\"400\" height=\"300\" id=\"graph_%d\"></canvas>\n", et->id);
                o("</div>\n");
                break;
        }
        list_free(items_a);
        list_free(all_items_a);
    }
    list_free(element_types_a);
    o(      "</div>\n");
    output_footer();
    db_finish();
}
void help_action()
{
    Project* project_a = xalloc(sizeof(Project));
    db_init();
    project_a = db_get_project(project_a);
    output_header(project_a, "ヘルプ", NULL, NAVI_HELP);
    o(      "<h2>");h(project_a->name);o("</h2>\n"
            "<div id=\"top\">\n");
    wiki_out("help");
    o(      "</div>\n");
    xfree(project_a);
    db_finish();
    output_footer();
}
void edit_top_action()
{
    Project* project_a = xalloc(sizeof(Project));
    db_init();
    project_a = db_get_project(project_a);
    output_header(project_a, "トップページの編集", "edit_top.js", NAVI_OTHER);
    xfree(project_a);
    o(      "<h2>トップページの編集</h2>\n"
            "<div id=\"top\">\n"
            "<h3>トップページの編集</h3>\n"
            "<div id=\"description\">簡易wikiの文法でトップページのコンテンツの編集を行ない、更新ボタンを押してください。</div>\n"
            "<form id=\"edit_top_form\" action=\"%s/edit_top_submit\" method=\"post\">\n", cgiScriptName);
    o(      "<textarea name=\"edit_top\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    wiki_content_out("top");
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
    db_finish();
    output_footer();
}
void edit_top_submit_action()
{
    char* value_a = xalloc(sizeof(char) * VALUE_LENGTH);

    cgiFormString("edit_top", value_a, VALUE_LENGTH);
    db_init();
    wiki_save("top", value_a);
    db_finish();
    xfree(value_a);

    redirect("", NULL);
}
void download_action()
{
    ElementFile* file_a = xalloc(sizeof(ElementFile));
    char path_info[DEFAULT_LENGTH];
    char* element_id_str;
    int element_file_id;
    int i;
    char* p;

    strcpy(path_info, cgiPathInfo);
    element_id_str = strchr(path_info + 1, '/');
    if (element_id_str) element_id_str++;
    element_file_id = atoi(element_id_str);

    db_init();
    file_a = db_get_element_file(element_file_id, file_a);
    if (!file_a) goto error;
    o("Content-Type: %s\r\n", file_a->mime_type);
    o("Content-Length: %d\r\n", file_a->size);
    o("Content-Disposition: attachment;\r\n");
    o("\r\n");

    p = file_a->blob;
    for (i = 0; i < file_a->size; i++) {
        fputc(*p, cgiOut);
        p++;
    }
    db_finish();
    xfree(file_a->blob);
    xfree(file_a);
    return;

error:
    cgiHeaderContentType("text/plain; charset=utf-8;");
    o("error: ファイルがありません。");
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
