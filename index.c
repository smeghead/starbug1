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
#include "mail.h"

/* prototype declares */
void register_actions();
void list_action();
void search_actoin();
void register_submit_action();
void register_action();
void ticket_action();
void help_action();
void edit_top_action();
void edit_top_submit_action();
void download_action();
void rss_action();
void default_action();
void output_header(Project*, char*, char*);
void output_footer();
int cgiMain();
void output_form_element(List* element, ElementType* e_type);
void output_form_element_4_condition(char*, ElementType*);
int get_mode();
static int contains(char* const, const char*);

enum MODE {
    MODE_INVALID,
    MODE_REGISTER,
    MODE_REPLY
};

int get_mode()
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
    register_action_actions("register_submit", register_submit_action);
    register_action_actions("ticket", ticket_action);
    register_action_actions("reply", ticket_action);
    register_action_actions("help", help_action);
    register_action_actions("edit_top", edit_top_action);
    register_action_actions("edit_top_submit", edit_top_submit_action);
    register_action_actions("download", download_action);
    register_action_actions("rss", rss_action);
    register_action_actions("default", default_action);
}

void output_header(Project* project, char* title, char* script_name)
{
    o("Pragma: no-cache\r\n");
    o("Cache-Control: no-cache\t\n");
    cgiHeaderContentType("text/html; charset=utf-8;");
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />"
            "\t<title>Starbug1 %s - %s</title>\n", project->name, title);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/user.css\" />\n", cgiScriptName);
    if (script_name) {
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/prototype.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/%s\"></script>\n", cgiScriptName, script_name);
    }
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://sourceforge.jp/projects/starbug1/\"><img src=\"%s/../img/title.jpg\" alt=\"Starbug1\" /></a></h1>\n"
            "<ul id=\"mainmenu\">\n", cgiScriptName);
    o(      "\t<li><a href=\"%s\">トップ</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/list\">状態別チケット一覧</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/register\">チケット登録</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/search\">チケット検索</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/rss\"><img src=\"%s/../img/rss.gif\" alt=\"rss\" /></a></li>\n", cgiScriptName, cgiScriptName);
    o(      "\t<li><a href=\"%s/help\">ヘルプ</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/../admin.cgi\">管理ツール</a></li>\n", cgiScriptName);
    o(      "</ul>\n"
            "<br clear=\"all\" />\n"
            "<hr />\n");
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
            "<div><address>Copyright smeghead 2007 - 2008.</address></div>\n"
            "</div>\n"
            "</div>\n"
            "</body>\n</html>\n");
}
int cgiMain() {
    register_actions();
    exec_action();
    return 0;
}
void output_navigater(SearchResult* result, char* query_string)
{
    int i;
    if (result->hit_count < LIST_PER_PAGE) return;
    o("<div class=\"navigater\">\n");
    if (result->page > 0)
        o("<a href=\"%s/search?p=%d&amp;%s\">&lt;&lt;</a>\n", cgiScriptName, result->page - 1, query_string);
    for (i = 0; i * LIST_PER_PAGE < result->hit_count; i++) {
        if (i == result->page)
            o("%d\n", i + 1);
        else
            o("<a href=\"%s/search?p=%d&amp;%s\">%d</a>\n", cgiScriptName, i, query_string, i + 1);
    }
    if (result->page * LIST_PER_PAGE < result->hit_count - LIST_PER_PAGE)
        o("<a href=\"%s/search?p=%d&amp;%s\">&gt;&gt;</a>\n", cgiScriptName, result->page + 1, query_string);
    o("</div>\n");
}
char* format_query_string(char* buffer)
{
    char **array, **arrayStep;
    if (cgiFormEntries(&array) != cgiFormSuccess) {
        return "";
    }
    strcpy(buffer, "");
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
            strcat(buffer, name);
            strcat(buffer, "=");
            strcat(buffer, encodedvalue);
            strcat(buffer, "&amp;");
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
    o(      "\t\t<th>ID</th>\n");
    foreach (it, element_types) {
        ElementType* et = it->element;
        o("\t\t<th>\n");
        h(et->name);
        o("\t\t</th>\n");
    }
    o("\t\t<th>投稿日時</th>\n");
    o("\t\t<th>最終更新日時</th>\n");
    o("\t\t<th>放置日数</th>\n");
    o("\t</tr>\n");
}
void output_ticket_table_header(List* element_types, char* query_string)
{
    Iterator* it;
    char* reverse = strstr(cgiQueryString, "rsort=");

    o(      "\t<tr>\n");
    o(      "\t\t<th><a href=\"%s/search?%ssort=-1&amp;%s\">ID</a></th>\n", cgiScriptName, reverse ? "" : "r", query_string);
    foreach (it, element_types) {
        ElementType* et = it->element;
        o("\t\t<th>\n");
        o("\t\t\t<a href=\"%s/search?%ssort=%d&amp;%s\">", cgiScriptName, reverse ? "" : "r", et->id, query_string);
        h(et->name);
        o("</a>\n");
        o("\t\t</th>\n");
    }
    o("\t\t<th><a href=\"%s/search?%ssort=-2&amp;%s\">投稿日時</a></th>\n", cgiScriptName, reverse ? "" : "r", query_string);
    o("\t\t<th><a href=\"%s/search?%ssort=-3&amp;%s\">最終更新日時</a></th>\n", cgiScriptName, reverse ? "" : "r", query_string);
    o("\t\t<th><a href=\"%s/search?%ssort=-3&amp;%s\">放置日数</a></th>\n", cgiScriptName, reverse ? "" : "r", query_string);
    o("\t</tr>\n");
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
        o("\t<tr>\n");
        o("\t\t<td class=\"field%d-%d\"><a href=\"%s/ticket/%s\">%s</a></td>\n", 
                ELEM_ID_ID, 
                get_element_lid_by_id(elements_a, ELEM_ID_ID), 
                cgiScriptName, 
                get_element_value_by_id(elements_a, ELEM_ID_ID), 
                get_element_value_by_id(elements_a, ELEM_ID_ID));
        foreach (it, element_types) {
            ElementType* et = it->element;
            o("\t\t<td class=\"field%d-%d\">", et->id, get_element_lid_by_id(elements_a, et->id));
            if (et->id == ELEM_ID_TITLE)
                o("<a href=\"%s/ticket/%d\">", cgiScriptName, message->id);
            if (et->id == ELEM_ID_SENDER)
                hmail(get_element_value_by_id(elements_a, ELEM_ID_ORG_SENDER)); /* 最初の投稿者を表示する。 */
            else
                h(get_element_value_by_id(elements_a, et->id));
            if (et->id == ELEM_ID_TITLE)
                o("</a>");
            o("&nbsp;</td>\n");
        }
        o("\t\t<td>"); h(get_element_value_by_id(elements_a, ELEM_ID_REGISTERDATE)); o("&nbsp;</td>\n");
        o("\t\t<td>"); h(get_element_value_by_id(elements_a, ELEM_ID_LASTREGISTERDATE)); o("&nbsp;</td>\n");
        o("\t\t<td>"); h(get_element_value_by_id(elements_a, ELEM_ID_LASTREGISTERDATE_PASSED)); o("&nbsp;</td>\n");
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
void output_ticket_table(SearchResult* result, List* element_types, char* query_string)
{
    o("<table summary=\"ticket list\">\n");
    output_ticket_table_header(element_types, query_string);
    output_ticket_table_body(result, element_types);
    o("</table>\n");
}
/**
 * 一覧を表示するaction。
 */
void list_action()
{
    SearchResult* result;
    List* element_types_a;
    List* conditions_a = NULL;
    Project* project;
    List* states_a;
    Iterator* it;
    char message[DEFAULT_LENGTH];
    List* messages_a;

    db_init();
    project = db_get_project();
    output_header(project, "状態別チケット一覧", "list.js");
    cgiFormStringNoNewlines("message", message, DEFAULT_LENGTH);
    if (strlen(message) > 0) {
        o("<div class=\"complete_message\">"); h(message); o("&nbsp;</div>\n");
    }
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_4_list(element_types_a);
    o("<h2>"); h(project->name); o(" - 状態別チケット一覧</h2>\n");
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    /* stateの表示 */
    o("<div id=\"state_index\">\n");
    o("\t<ul>\n");
    foreach (it, states_a) {
        State* s = it->element;
        o("\t\t<li>\n");
        o("\t\t\t<a href=\"%s/search?field%d=", cgiScriptName, ELEM_ID_STATUS); u(s->name); o("\">");
        h(s->name);
        o("</a>");
        o("(%d)", s->count);
        o("\t\t</li>\n");
    }
    list_free(states_a);
    o("\t\t<li>\n");
    o("\t\t\t<form action=\"%s/search\" method=\"get\">\n", cgiScriptName);
    o("\t\t\t\t<input type=\"text\" class=\"number\" name=\"id\" />\n");
    o("\t\t\t\t<input type=\"submit\" class=\"button\" value=\"ID指定で表示\" />\n");
    o("\t\t\t</form>\n");
    o("\t\t</li>\n");
    o("\t</ul>\n");
    o("</div>\n");
    o("<br clear=\"all\" />\n");
    fflush(cgiOut);
    o("<div id=\"ticket_list\">\n");

    o("<h3>状態別チケット一覧</h3>\n");
    o("<div class=\"message\">未クローズの状態毎にチケットを表示しています。</div>\n");
    o("<div id=\"state_index_anchor\">\n");
    o("\t<ul>\n");
    list_alloc(states_a, State);
    states_a = db_get_states_has_not_close(states_a);
    foreach (it, states_a) {
        State* s = it->element;
        o("\t\t<li>\n");
        o("\t\t\t<a href=\"#");
        h(s->name);
        o("\">");
        h(s->name);
        o("</a>");
        o("\t\t</li>\n");
    }
    o("\t</ul>\n");
    o("</div>\n");
    o("<br clear=\"all\" />\n");
    foreach (it, states_a) {
        State* s = it->element;

        /* 検索 */
        list_alloc(conditions_a, Condition);
        list_alloc(messages_a, Message);
        result = db_get_tickets_by_status(s->name, messages_a);
        list_free(conditions_a);

        o("<a name=\""); h(s->name); o("\"></a>\n");
        o("<div>\n");
        o("<h4 class=\"status\">");h(s->name);o("&nbsp;(%d件)&nbsp;<a href=\"#top\">↑</a></h4>\n", s->count);
        if (result->hit_count == LIST_PER_PAGE) {
            o("\t\t<div class=\"description\">最初の%d件のみを表示しています。<a href=\"%s/search?field%d=", 
                    result->hit_count, 
                    cgiScriptName,
                    ELEM_ID_STATUS);
            u(s->name);
            o("\">状態が%sである全てのチケットを表示する</a></div>\n", s->name);
        }
        output_ticket_table_status_index(result, element_types_a);
        list_free(result->messages);
        free(result);
        o("</div>\n");
        fflush(cgiOut);
    }
    list_free(states_a);
    list_free(element_types_a);
    o("</div>\n");
    output_footer();
    db_finish();
}
/**
 * 検索画面を表示するaction。
 */
void search_actoin()
{
    SearchResult* result;
    List* element_types_a;
    List* conditions_a = NULL;
    Condition* sort = NULL;
    Project* project;
    List* states_a;
    Iterator* it;
    char sortstr[DEFAULT_LENGTH];
    char id[DEFAULT_LENGTH];
    char q[DEFAULT_LENGTH];
    char p[DEFAULT_LENGTH];
    List* messages_a;

    cgiFormStringNoNewlines("id", id, DEFAULT_LENGTH);
    if (strlen(id) > 0) {
        char uri[DEFAULT_LENGTH];
        sprintf(uri, "/ticket/%s", id);
        redirect(uri, NULL);
    }
    
    db_init();
    project = db_get_project();
    output_header(project, "チケット検索", "list.js");
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_4_list(element_types_a);
    o("<h2>"); h(project->name); o(" - チケット検索</h2>\n");
    /* 検索 */
    list_alloc(conditions_a, Condition);
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        Condition* c;

        sprintf(name, "field%d", et->id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        if (strlen(value) == 0) continue;
        c = list_new_element(conditions_a);
        c->element_type_id = et->id;
        strcpy(c->value, value);
        list_add(conditions_a, c);
    }
    cgiFormStringNoNewlines("q", q, DEFAULT_LENGTH);
    cgiFormStringNoNewlines("sort", sortstr, DEFAULT_LENGTH);
    if (strlen(sortstr) > 0) {
        sort = (Condition*)xalloc(sizeof(Condition));
        sort->element_type_id = atoi(sortstr);
    } else {
        cgiFormStringNoNewlines("rsort", sortstr, DEFAULT_LENGTH);
        if (strlen(sortstr) > 0) {
            sort = (Condition*)xalloc(sizeof(Condition));
            sort->element_type_id = atoi(sortstr);
            strcpy(sort->value, "reverse");
        }
    }
    cgiFormStringNoNewlines("p", p, DEFAULT_LENGTH);
    list_alloc(messages_a, Message);
    result = db_search_tickets(conditions_a, q, sort, atoi(p), messages_a);
    free(conditions_a);
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    /* stateの表示 */
    o("<div id=\"state_index\">\n");
    o("\t<ul>\n");
    foreach (it, states_a) {
        State* s = it->element;
        o("\t\t<li>\n");
        o("\t\t\t<a href=\"%s/search?field%d=", cgiScriptName, ELEM_ID_STATUS); u(s->name); o("\">");
        h(s->name);
        o("</a>");
        o("(%d)", s->count);
        o("\t\t</li>\n");
    }
    list_free(states_a);
    o("\t\t<li>\n");
    o("\t\t\t<form action=\"%s/search\" method=\"get\">\n", cgiScriptName);
    o("\t\t\t\t<input type=\"text\" class=\"number\" name=\"id\" />\n");
    o("\t\t\t\t<input type=\"submit\" class=\"button\" value=\"ID指定で表示\" />\n");
    o("\t\t\t</form>\n");
    o("\t\t</li>\n");
    o("\t</ul>\n");
    o("<br clear=\"all\" />\n");
    o("</div>\n");
    o("<div id=\"condition_form\">\n");
    o("<h3>検索条件</h3>\n");
    o("<form action=\"%s/search\" method=\"get\">\n", cgiScriptName);
    o(      "<table summary=\"condition table\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        sprintf(name, "field%d", et->id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);

        o("<tr>\n");
        o("\t<th>"); h(et->name); o("</th>\n");
        o("\t<td>\n"); 
        output_form_element_4_condition(value, et);
        o("\t</td>\n");
        o("</tr>\n");
    }
    o("<tr>\n");
    o("\t<th>キーワード検索</th>\n");
    o("\t<td>\n"); 
    o("\t\t<input type=\"text\" name=\"q\" value=\""); v(q); o("\" />\n");
    o("\t\t<div id=\"message\">履歴も含めて全ての項目から検索を行ないます。</div>\n");
    o("\t</td>\n");
    o("</tr>\n");
    o("</table>\n");
    o("<input class=\"button\" type=\"submit\" value=\"検索\" />");
    o("</form>\n");
    o("</div>\n");
    fflush(cgiOut);
    o("<div id=\"ticket_list\">\n");
    o("<h3>検索結果</h3>\n");

    if (result->messages->size) {
        char query_string_buffer[DEFAULT_LENGTH];
        char* query_string;

        query_string = format_query_string(query_string_buffer);
        o(      "<div class=\"description\">");
        o(      "%d件ヒットしました。\n", result->hit_count);
        o(      "</div>\n");
        output_navigater(result, query_string);
        output_ticket_table(result, element_types_a, query_string);
        output_navigater(result, query_string);
    }
    o("</div>\n");
    output_footer();
    db_finish();
    list_free(result->messages);
    list_free(element_types_a);
    free(result);
}
/**
 * form要素を表示する。
 */
void output_form_element_4_condition(char* value, ElementType* e_type)
{
    char id[DEFAULT_LENGTH];
    List* items_a;
    Iterator* it;

    sprintf(id, "%d", e_type->id);
    switch (e_type->type) {
        case ELEM_TYPE_TEXT:
        case ELEM_TYPE_TEXTAREA:
        case ELEM_TYPE_CHECKBOX:
            o("<input type=\"text\" class=\"element\" id=\"field");
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
            items_a = db_get_list_item(e_type->id, items_a);

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
    }
}
/**
 * form要素を表示する。
 */
void output_form_element(List* elements, ElementType* e_type)
{
    char* value = "";
    List* items_a;
    Iterator* it;
    int list_count = 0;

    if (elements != NULL) {
        value = get_element_value(elements, e_type);
    } else {
        if (e_type->id == ELEM_ID_SENDER) {
            char* user_name = getenv("REMOTE_USER");
            char sender[DEFAULT_LENGTH];
            cgiCookieString("starbug1_sender", sender, DEFAULT_LENGTH);
            if (strlen(sender))
                /* 投稿者のフィールドは、cookieから値が取得できれば、その値を表示する。 */
                value = sender;
            else if (user_name)
                /* 投稿者のフィールドは、basic認証が行なわれていればそのユーザ名を表示する。 */
                value = user_name;
            else 
                value = e_type->default_value;
        } else {
            value = e_type->default_value;
        }
    }
    switch (e_type->type) {
        case ELEM_TYPE_TEXT:
            o("<input type=\"text\" class=\"element\" id=\"field%d\" name=\"field%d\" value=\"",
                    e_type->id, e_type->id);
            v(value);
            o("\" />\n");
            break;
        case ELEM_TYPE_TEXTAREA:
            o("<textarea id=\"field%d\" name=\"field%d\" rows=\"3\" cols=\"10\">",
                    e_type->id, e_type->id);
            v(value);
            o("</textarea>\n");
            break;
        case ELEM_TYPE_CHECKBOX:
            o("<input type=\"checkbox\" id=\"field%d\" class=\"checkbox\" name=\"field%d\" value=\"",
                    e_type->id, e_type->id); 
            v(e_type->name);
            o("\" %s />",
                    (strlen(value) || strcmp(value, "0") == 0) ? "checked=\"checked\"" : "");
            o("<label for=\"field%d\">", e_type->id); h(e_type->name); o("</lable>\n");
            break;
        case ELEM_TYPE_LIST_SINGLE:
            o("<select id=\"field%d\" name=\"field%d\">\n",
                    e_type->id, e_type->id);
            o("<option value=\"\">&nbsp;</option>\n");
            list_alloc(items_a, ListItem);
            items_a = db_get_list_item(e_type->id, items_a);
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
            items_a = db_get_list_item(e_type->id, items_a);
            /* リストの要素数をカウントする */
            foreach (it, items_a) list_count++;
            o("<select size=\"%d\" id=\"field%d", list_count + 1, e_type->id);
            o("\" name=\"field%d\" multiple=\"multiple\">\n", e_type->id);

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
                    e_type->id, e_type->id);
            break;
    }
    switch (e_type->type) {
        case ELEM_TYPE_LIST_SINGLE:
        case ELEM_TYPE_LIST_MULTI:
            if (e_type->auto_add_item) {
                /* 新規項目を設定可能である場合、テキストボックスを表示する。 */
                o("<input type=\"text\" class=\"element_new_item\" id=\"field%d\" name=\"field%d.new_item\" />\n",
                    e_type->id, e_type->id);
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
/**
 * 登録画面を表示するaction。
 */
void register_action()
{
    Project* project;
    char sender[DEFAULT_LENGTH];
    cgiCookieString("starbug1_sender", sender, DEFAULT_LENGTH);

    db_init();
    project = db_get_project();
    output_header(project, "チケット登録", "register.js");
    o(      "<h2>"); h(project->name);o(" - チケット登録</h2>\n"
            "<div id=\"input_form\">\n"
            "<h3>チケット登録</h3>\n"
            "<div class=\"message\">新規チケットを登録する場合は、以下のフォームを記入し登録ボタンをクリックしてください。</div>\n"
            "<div class=\"message\">※必須項目の入力チェックは、javascriptで行なっています。</div>\n");
    o(      "<form id=\"register_form\" name=\"register_form\" action=\"%s/register_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName);
    o(      "<table summary=\"input infomation\">\n");
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
            output_form_element(NULL, et);
            o("\t\t\t<div class=\"description\">");h(et->description);o("&nbsp;</div>\n");
            o("\t\t</td>\n");
            o("\t</tr>\n");
        }
        list_free(element_types_a);
    }
    o(      "</table>\n");
    o(      "<input class=\"button\" type=\"submit\" name=\"register\" value=\"登録\" />\n"
            "<input id=\"save2cookie\" type=\"checkbox\" name=\"save2cookie\" class=\"checkbox\" value=\"1\" %s />\n"
            "<label for=\"save2cookie\">投稿者を保存する。(cookie使用)</label>\n"
            "</form>\n"
            "</div>\n", strlen(sender) ? "checked" : "");
    db_finish();
    output_footer();
}
/**
 * チケット詳細画面を表示するaction。
 */
void ticket_action()
{
    char path_info[DEFAULT_LENGTH];
    char* ticket_id;
    List* elements_a = NULL;
    List* last_elements = NULL;
    List* element_types_a;
    Iterator* it;
    int iid, *message_ids_a, i;
    Project* project;
    char sender[DEFAULT_LENGTH];
    cgiCookieString("starbug1_sender", sender, DEFAULT_LENGTH);

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
    if (!elements_a) {
        redirect("/list", "存在しないIDが指定されました。");
        return;
    }
    project = db_get_project();
    output_header(project, "チケット詳細", "reply.js");

    message_ids_a = db_get_message_ids(iid);
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(element_types_a);
    o("<h2 id=\"subject\">"); h(project->name); o(" - ID:%5d ", iid);
    h(get_element_value_by_id(elements_a, ELEM_ID_TITLE));
    o(" &nbsp;</h2>\n");
    o(      "<div id=\"ticket_newest\">\n"
            "<h3>チケット最新情報</h3>\n"
            "<table summary=\"newest table\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        char* value = get_element_value(elements_a, et);
        if (et->ticket_property == 0) continue;
        o("\t<tr>\n");
        o("\t\t<th>");
        h(et->name);
        o("&nbsp;</th>\n");
        o("\t\t<td>");
        switch (et->id) {
            case ELEM_ID_SENDER:
                hmail(value);
                break;
            default:
                if (et->type == ELEM_TYPE_UPLOADFILE) {
                    if (strlen(value)) {
                        o("<a href=\"%s/download/%d/", cgiScriptName, -1 /* TODO get_element_id(elements_a, et)*/); u(value); o("\" target=\"_blank\">");h(value); o("</a>\n");
                    }
                } else {
                    hm(value);
                }
        }
        o("&nbsp;</td>\n");
        o("\t</tr>\n");
    }
    o("\t<tr>\n");
    o("\t\t<th>全添付ファイル</th>\n");
    o("\t\t\t<td>\n");
    for (i = 0; message_ids_a[i] != 0; i++) {
        List* attachment_elements_a;
        list_alloc(attachment_elements_a, Element);
        attachment_elements_a = db_get_elements(message_ids_a[i], attachment_elements_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            if (et->type == ELEM_TYPE_UPLOADFILE) {
                char* attachment_file_name = get_element_value(attachment_elements_a, et);
                if (strlen(attachment_file_name) == 0) continue;
                o("\t\t<div>\n");
                o("<a href=\"%s/download/%d/", 
                        cgiScriptName, 
                        db_get_element_file_id(message_ids_a[i], et->id)); 
                u(attachment_file_name); o("\" target=\"_blank\">");h(attachment_file_name); o("</a>\n");
                o("\t\t&nbsp;</div>\n");
            }
        }
        list_free(attachment_elements_a);
    }
    o("\t\t\t</td>\n");
    o("\t</tr>\n");
    o(      "</table>\n"
            "</div>");
    o(      "<div class=\"description\"><a href=\"#reply\">返信する</a></div>\n");
    o(      "<div id=\"ticket_history\">\n"
            "<h3>チケット履歴</h3>\n");
    list_free(elements_a);
    /* 履歴の表示 */
    for (i = 0; message_ids_a[i] != 0; i++) {
        List* previous = last_elements;
        list_alloc(elements_a, Element);
        last_elements = elements_a = db_get_elements(message_ids_a[i], elements_a);

        o(      "<table summary=\"reply table\">\n");
        o(      "\t<tr>\n"
                "\t\t<td colspan=\"2\" class=\"title\">投稿: %d ", i + 1); o("["); h(get_element_value_by_id(elements_a, ELEM_ID_LASTREGISTERDATE)); o("]</td>\n"
                "\t</tr>\n");
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            char* value = get_element_value(elements_a, et);
            char* last_value = get_element_value(previous, et);

            /* チケット属性で、直前の値と同じ項目は表示しない。 */
            if (et->ticket_property == 1 && strcmp(value, last_value) == 0)
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
                            o("<a href=\"%s/download/%d/", 
                                    cgiScriptName, 
                                    db_get_element_file_id(message_ids_a[i], et->id)); 
                            u(value); o("\" target=\"_blank\">");h(value); o("</a>\n");
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
    free(message_ids_a);
    o(  "</div>\n");
    /* フォームの表示 */
    o(      "<a name=\"reply\"></a>\n");
    o(      "<div id=\"input_form\">\n"
            "<h3>チケット返信</h3>\n");
    o(      "<form id=\"reply_form\" name=\"reply_form\" action=\"%s/register_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName);
    o(      "<input type=\"hidden\" name=\"ticket_id\" value=\"%s\" />\n", ticket_id);
    o(      "<div class=\"message\">返信を行なう場合は、以下のフォームに内容を記入して返信ボタンをクリックしてください。</div>\n"
            "<div class=\"message\">※必須項目の入力チェックは、javascriptで行なっています。</div>\n"
            "<table summary=\"input table\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
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
        if (last_elements != NULL) {
            if (et->ticket_property)
                output_form_element(last_elements, et);
            else
                output_form_element(NULL, et);
/*                 last_elements = last_elements->next; */
        } else {
            output_form_element(NULL, et);
        }
        o("\t\t<div class=\"description\">");h(et->description);o("&nbsp;</div>\n");
        o("\t</td>\n");
        o("\t</tr>\n");
    }
    o(      "</table>\n"
            "<input class=\"button\" type=\"submit\" name=\"reply\" value=\"返信\" />&nbsp;&nbsp;&nbsp;\n"
            "<input id=\"save2cookie\" type=\"checkbox\" name=\"save2cookie\" class=\"checkbox\" value=\"1\" %s />\n"
            "<label for=\"save2cookie\">投稿者を保存する。(cookie使用)</label>\n"
            "</form>\n"
            "</div>\n", strlen(sender) ? "checked" : "");
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
    free(item_a);
}
/**
 * 登録するaction。
 * 登録モード、編集モード、削除モードがある。
 */
void register_submit_action()
{
    Project* project;
    List* element_types_a;
    Iterator* it;
    List* elements_a = NULL;
    Message* ticket_a;
    char ticket_id[DEFAULT_LENGTH];
    int mode = get_mode();
    int mail_result;
    char** multi;
    char save2cookie[2];

    cgiFormStringNoNewlines("save2cookie", save2cookie, 2);
    if (mode == MODE_INVALID)
        die("reqired invalid mode.");
    ticket_a = (Message*)xalloc(sizeof(Message));
    db_init();
    db_begin();
    project = db_get_project();
    list_alloc(element_types_a, ElementType);
    element_types_a = db_get_element_types_all(element_types_a);
    cgiFormStringNoNewlines("ticket_id", ticket_id, DEFAULT_LENGTH);
    if (mode == MODE_REGISTER)
        ticket_a->id = -1;
    else
        ticket_a->id = atoi(ticket_id);
    list_alloc(elements_a, Element);
    if (mode == MODE_REGISTER || mode == MODE_REPLY) {
        char* value_a = (char*)xalloc(sizeof(char) * VALUE_LENGTH); /* 1M */
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
                    cgiFormStringNoNewlines(name, value_a, VALUE_LENGTH);
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, value_a);
                    break;
                case ELEM_TYPE_TEXTAREA:
                    cgiFormString(name, value_a, VALUE_LENGTH);
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, value_a);
                    break;
                case ELEM_TYPE_CHECKBOX:
                    cgiFormString(name, value_a, VALUE_LENGTH);
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, value_a);
                    break;
                case ELEM_TYPE_LIST_SINGLE:
                    /* 新規選択肢 */
                    cgiFormString(name_new_item, value_a, VALUE_LENGTH);
                    if (strlen(value_a)) {
                        e->str_val = (char*)xalloc(sizeof(char) * strlen(value_a) + 1);
                        strcpy(e->str_val, value_a);
                        /* 新しく選択肢を追加 */
                        register_list_item(et->id, value_a);
                    } else {
                        cgiFormString(name, value_a, VALUE_LENGTH);
                        e->str_val = (char*)xalloc(sizeof(char) * strlen(value_a) + 1);
                        strcpy(e->str_val, value_a);
                    }
                    break;
                case ELEM_TYPE_LIST_MULTI:
                    /* 新規選択肢 */
                    cgiFormString(name_new_item, value_a, VALUE_LENGTH);
                    if (strlen(value_a)) {
                        e->str_val = (char*)xalloc(sizeof(char) * strlen(value_a) + 1);
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
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, value_a);
                    cgiStringArrayFree(multi);
                    break;
                case ELEM_TYPE_UPLOADFILE:
                    if (get_upload_size(et->id) > MAX_FILE_SIZE * 1024) {
                        goto file_size_error;
                    }
                    cgiFormFileName(name, value_a, VALUE_LENGTH);
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value_a) + 1);
                    strcpy(e->str_val, get_filename_without_path(value_a));
                    if (strlen(e->str_val)) {
                        e->is_file = 1;
                    }
                    break;
            }
            if (e->element_type_id == ELEM_ID_SENDER) {
                if (strcmp(save2cookie, "1") == 0) {
                    cgiHeaderCookieSetString("starbug1_sender", e->str_val, 86400 * 30, cgiScriptName, cgiServerName);
                } else {
                    cgiHeaderCookieSetString("starbug1_sender", "", 0, cgiScriptName, cgiServerName);
                }
            }
            list_add(elements_a, e);
        }
        free(value_a);
        ticket_a->elements = elements_a;
        ticket_a->id = db_register_ticket(ticket_a);
    }
    /* mail */
    mail_result = mail_send(project, ticket_a, elements_a, element_types_a);
    list_free(element_types_a);
    free_element_list(elements_a);
    free(ticket_a);
    if (mail_result != 0 && mail_result != MAIL_GAVE_UP) {
        die("mail send error.");
    }
    db_commit();
    db_finish();

    if (mode == MODE_REGISTER)
        redirect("/list", "登録しました。");
    else if (mode == MODE_REPLY)
        redirect("/list", "返信しました。");
    return;

file_size_error:
    db_rollback();
    db_finish();
    output_header(project, "エラー", NULL);
    o("<h1>エラー発生</h1>\n");
    o("<div class=\"message\">ファイルサイズが大きすぎます。%dkbより大きいファイルは登録できません。ブラウザの戻るボタンで戻ってください。</div>\n", MAX_FILE_SIZE);
    output_footer();
}
/**
 * デフォルトのaction。
 */
void default_action()
{
    Project* project;
    List* tickets_a;
    List* states_a;
    Iterator* it;

    db_init();
    project = db_get_project();
    output_header(project, "トップページ", NULL);
    list_alloc(states_a, State);
    states_a = db_get_states(states_a);
    o(      "<div id=\"info\">\n");
    /* 最新情報の表示 */
    o(      "<div id=\"top_newest\">\n");
    o(      "<h4>最新情報</h4>\n");
    o(      "\t<ul>\n");
    list_alloc(tickets_a, Message);
    tickets_a = db_get_newest_information(10, tickets_a);
    if (tickets_a != NULL) {
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
    }
    list_free(tickets_a);
    o(      "\t</ul>\n");
    o(      "</div>\n");
    /* stateの表示 */
    o(      "<div id=\"top_state_index\">\n");
    o(      "<h4>状態別件数</h4>\n");
    o(      "\t<ul>\n");
    foreach (it, states_a) {
        State* s = it->element;
        o("\t\t<li>\n");
        o("\t\t\t<a href=\"%s/search?field%d=", cgiScriptName, ELEM_ID_STATUS); u(s->name); o("\">");
        h(s->name);
        o("\t\t\t</a>\n");
        o("(%d)", s->count);
        o("\t\t</li>\n");
    }
    list_free(states_a);
    o(      "\t</ul>\n");
    o(      "</div>\n");
    o(      "</div>\n");
    db_finish();
    o(      "<div id=\"main\">\n");
    o(      "<h2>");h(project->name);o("&nbsp;</h2>\n");
    o(      "<div id=\"main_body\">\n"
            "<div id=\"description\">");h(project->description);o("</div>\n");
    o(      "<div class=\"top_edit\"><a href=\"%s/edit_top\">トップページの編集</a></div>\n", cgiScriptName);
    wiki_out("wiki/top.wiki");
    o(      "</div>\n");
    o(      "</div>\n");
    output_footer();
}

void rss_action()
{
    List* tickets_a;
    Iterator* it;
    Project* project;

    db_init();
    project = db_get_project();
    cgiHeaderContentType("text/xml; charset=utf-8;");
    o(      "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
            "<rdf:RDF \n"
            "\t\txmlns=\"http://purl.org/rss/1.0/\"\n"
            "\t\txmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" \n"
            "\t\txml:lang=\"ja\">\n"
            "\n"
            "\t<channel rdf:about=\"");h(project->home_url);o("%s/rss\">\n", cgiScriptName);
    o(      "\t\t<title>");h(project->name); o(" - Starbug1</title>\n"
            "\t\t<link>");h(project->home_url);o("/bt/</link>\n");
    o(      "\t\t<description>");h(project->description);o("</description>\n"
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
            o(      "\t<item rdf:about=\"");h(project->home_url);o("%s/ticket/%d\">\n", cgiScriptName, ticket->id);
            o(      "\t\t<title>ID:%5d ", ticket->id);
            h(get_element_value_by_id(elements_a, ELEM_ID_TITLE));
            o(      "</title>\n");
            o(      "\t\t<link>");h(project->home_url);o("%s/ticket/%d</link>\n", cgiScriptName, ticket->id);
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
    list_free(tickets_a);
    o(      "</rdf:RDF>\n");
    db_finish();
}
void help_action()
{
    Project* project;
    db_init();
    project = db_get_project();
    output_header(project, "ヘルプ", NULL);
    db_finish();
    o(      "<h2>");h(project->name);o(" - Starbug1</h2>\n"
            "<div id=\"top\">\n");
    wiki_out("wiki/help.wiki");
    o(      "</div>\n");
    output_footer();
}
void edit_top_action()
{
    Project* project;
    db_init();
    project = db_get_project();
    output_header(project, "トップページの編集", "edit_top.js");
    db_finish();
    o(      "<h2>トップページの編集</h2>\n"
            "<div id=\"top\">\n"
            "<h3>トップページの編集</h3>\n"
            "<div id=\"message\">簡易wikiの文法でトップページのコンテンツの編集を行ない、更新ボタンをクリックしてください。</div>\n"
            "<form id=\"edit_top_form\" action=\"%s/edit_top_submit\" method=\"post\">\n", cgiScriptName);
    o(      "<textarea name=\"edit_top\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    wiki_content_out("wiki/top.wiki");
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"更新\" />\n"
            "</form>");
    o(      "<div>\n");
    o(      "<h3>超簡易wikiのサポートする文法</h3>\n");
    o(      "<ul>\n");
    o(      "<li>行頭に*を記述した行は、大見出しになります。</li>\n");
    o(      "<li>行頭に**を記述した行は、見出しになります。</li>\n");
    o(      "<li>行頭に-を記述した行は、箇条書きになります。</li>\n");
    o(      "<li>行頭に----を記述した行は、区切り線になります。</li>\n");
    o(      "</ul>\n");
    o(      "</div>\n");
    o(      "</div>\n");
    output_footer();
}
void edit_top_submit_action()
{
    char value[VALUE_LENGTH];

    cgiFormString("edit_top", value, VALUE_LENGTH);
    wiki_save("wiki/top.wiki", value);

    redirect("", NULL);
}
void download_action()
{
    ElementFile* file_a;
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
    file_a = db_get_element_file(element_file_id);
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
    free(file_a->blob);
    free(file_a);
    return;

error:
    cgiHeaderContentType("text/plain; charset=utf-8;");
    o("error: ファイルがありません。");
}
/* vim: set ts=4 sw=4 sts=4 expandtab: */
