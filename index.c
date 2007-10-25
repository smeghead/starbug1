#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db.h"
#include "dbutil.h"
#include "util.h"
#include "wiki.h"
#include "mail.h"


/* prototype declares */
void register_actions();
void list_action();
void register_submit_action();
void register_action();
void reply_action();
void help_action();
void edit_top_action();
void edit_top_submit_action();
void download_action();
void rss_action();
void default_action();
void output_header(bt_project* project, char* script_name);
void output_footer();
int cgiMain();
void output_form_element(bt_element* element, bt_element_type* e_type);
void output_form_element_4_condition(char*, bt_element_type*);
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
    register_action_actions("register", register_action);
    register_action_actions("register_submit", register_submit_action);
    register_action_actions("reply", reply_action);
    register_action_actions("help", help_action);
    register_action_actions("edit_top", edit_top_action);
    register_action_actions("edit_top_submit", edit_top_submit_action);
    register_action_actions("download", download_action);
    register_action_actions("rss", rss_action);
    register_action_actions("default", default_action);
}

void output_header(bt_project* project, char* script_name)
{
    cgiHeaderContentType("text/html; charset=utf-8;");
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />"
            "\t<title>Starbug1</title>\n");
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    if (script_name) {
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/prototype.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/%s\"></script>\n", cgiScriptName, script_name);
    }
    o(      "</head>\n"
            "<body>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://sourceforge.jp/projects/starbug1/\"><img src=\"%s/../img/title.jpg\" alt=\"Starbug1\" /></a></h1>\n"
            "<ul id=\"mainmenu\">\n", cgiScriptName);
    o(      "\t<li><a href=\"%s\">トップ</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/list\">一覧</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/register\">新規登録</a></li>\n", cgiScriptName);
    o(      "\t<li><a href=\"%s/rss\">RSSフィード</a></li>\n", cgiScriptName);
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
            "<div><address>Copyright smeghead 2007.</address></div>\n"
            "</div>\n"
            "</div>\n"
            "</body>\n</html>\n");
}
int cgiMain() {
    register_actions();
    exec_action();
    return 0;
}
/**
 * 一覧を表示するaction。
 */
void list_action()
{
    bt_message* tickets;
    bt_element_type* e_types;
    bt_element_type* e;
    bt_condition* conditions = NULL;
    bt_condition* c = NULL;
    bt_project* project;
    bt_state* states;

    db_init();
    project = db_get_project();
    output_header(project, NULL);
    e_types = db_get_element_types(0);
    o("<h2>"); h(project->name); o(" - チケット一覧</h2>\n");
    /* 検索 */
    for (e = e_types; e != NULL; e = e->next) {
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];

        sprintf(name, "field%d", e->id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        if (strlen(value) == 0) continue;
        if (c == NULL) {
            c = conditions = (bt_condition*)xalloc(sizeof(bt_condition));
        } else {
            c->next = (bt_condition*)xalloc(sizeof(bt_condition));
            c = c->next;
        }
        c->element_type_id = e->id;
        strcpy(c->value, value);
        c->next = NULL;
    }
    tickets = db_search_tickets(conditions);
    states = db_get_states();
    /* stateの表示 */
    o("<div id=\"state_index\">\n");
    o("\t<ul>\n");
    for (; states != NULL; states = states->next) {
        o("\t\t<li>\n");
        o("\t\t\t<a href=\"%s/list?field%d=", cgiScriptName, ELEM_ID_STATUS); u(states->name); o("\">");
        h(states->name);
        o("\t\t\t</a>\n");
        o("(%d)", states->count);
        o("\t\t</li>\n");
    }
    o("\t</ul>\n");
    o("</div>\n");
    o("<div id=\"condition_form\">\n");
    o("<h3>検索条件</h3>\n");
    o("<form action=\"%s/list\" method=\"get\">\n", cgiScriptName);
    o(      "<table summary=\"condition table\">\n");
    for (e = e_types; e != NULL; e = e->next) {
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        sprintf(name, "field%d", e->id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);

        o("<tr>\n");
        o("\t<th>"); h(e->name); o("</th>\n");
        o("\t<td>\n"); 
        output_form_element_4_condition(value, e);
        o("\t</td>\n");
        o("</tr>\n");
    }
    o("</table>\n");
    o("<input class=\"button\" type=\"submit\" value=\"検索\" />");
    o("</form>\n");
    o("</div>\n");
    o("<div id=\"ticket_list\">\n");
    o("<h3>チケット一覧</h3>\n");
    if (tickets != NULL) {
        o(      "<table summary=\"ticket list\">\n"
                "\t<tr>\n"
                "\t\t<th>ID</th>\n");
        for (e = e_types; e != NULL; e = e->next) {
            o("\t\t<th>");
            h(e->name);
            o("</th>\n");
        }
        o("\t\t<th>投稿日時</th>\n");
        o("\t</tr>\n");
        for (; tickets != NULL; tickets = tickets->next) {
            bt_element* elements = db_get_last_elements_4_list(tickets->id);
            o("\t<tr>\n");
            o("\t\t<td><a href=\"%s/reply/%d\">%d</a></td>\n", cgiScriptName, tickets->id, tickets->id);
            for (e = e_types; e != NULL; e = e->next) {
                char sender[DEFAULT_LENGTH];
                o("\t\t<td>");
                if (e->id == ELEM_ID_TITLE)
                    o("<a href=\"%s/reply/%d\">", cgiScriptName, tickets->id);
                if (e->id == ELEM_ID_SENDER)
                    hmail(db_get_original_sender(tickets->id, sender)); /* 最初の投稿者を表示する。 */
                else
                    h(get_element_value_by_id(elements, e->id));
                if (e->id == ELEM_ID_TITLE)
                    o("</a>");
                o("&nbsp;</td>\n");
            }
            o("\t\t<td>"); h(tickets->registerdate); o("&nbsp;</td>\n");
            o("\t</tr>\n");
        }
        o("</table>\n");
    }
    o("</div>\n");
    output_footer();
    db_finish();
}
/**
 * form要素を表示する。
 */
void output_form_element_4_condition(char* value, bt_element_type* e_type)
{
    char id[DEFAULT_LENGTH];
    bt_list_item* items;

    sprintf(id, "%d", e_type->id);
    switch (e_type->type) {
        case ELEM_TEXT:
        case ELEM_TEXTAREA:
            o("<input type=\"text\" class=\"element\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\" value=\"");
            v(value);
            o("\" />\n");
            break;
        case ELEM_LIST_SINGLE:
        case ELEM_LIST_MULTI:
            o("<select class=\"element\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\">\n");
            items = db_get_list_item(e_type->id);

            o("<option value=\"\">&nbsp;</option>");
            for (; items != NULL; items = items->next) {
                o("<option value=\"");
                v(items->name);
                if (!strcmp(value, items->name))
                    o("\" selected=\"selected\">");
                else
                    o("\">");
                h(items->name);
                o("</option>\n");
            }
            o("</select>\n");

            break;
    }
}
/**
 * form要素を表示する。
 */
void output_form_element(bt_element* element, bt_element_type* e_type)
{
    char id[DEFAULT_LENGTH];
    char* value = "";
    bt_list_item* items;
    bt_list_item* i;
    int list_count;

    sprintf(id, "%d", e_type->id);
    if (element != NULL) {
        value = element->str_val;
    } else {
        char* user_name = getenv("REMOTE_USER");
        /* 投稿者のフィールドは、basic認証が行なわれていればそのユーザ名を表示する。 */
        if (e_type->id == ELEM_ID_SENDER && user_name)
            value = user_name;
        else
            value = "";
    }
    switch (e_type->type) {
        case ELEM_TEXT:
            o("<input type=\"text\" class=\"element\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\" value=\"");
            v(value);
            o("\" />\n");
            break;
        case ELEM_TEXTAREA:
            o("<textarea class=\"element\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\" rows=\"3\" cols=\"10\">");
            v(value);
            o("</textarea>\n");
            break;
        case ELEM_LIST_SINGLE:
            o("<select class=\"element\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\">\n");
            items = db_get_list_item(e_type->id);

            o("<option value=\"\">&nbsp;</option>");
            for (; items != NULL; items = items->next) {
                o("<option value=\"");
                v(items->name);
                if (!strcmp(value, items->name))
                    o("\" selected=\"selected\">");
                else
                    o("\">");
                h(items->name);
                o("</option>\n");
            }
            o("</select>\n");

            break;
        case ELEM_LIST_MULTI:
            items = db_get_list_item(e_type->id);
            /* リストの要素数をカウントする */
            for (list_count = 0,i = items; i != NULL; i = i->next,list_count++);
            o("<select class=\"element\" size=\"%d\" id=\"field", list_count + 1);
            h(id); o("\" name=\"field"); h(id); o("\" multiple=\"multiple\">\n");

            o("<option value=\"\">&nbsp;</option>");
            for (; items != NULL; items = items->next) {
                o("<option value=\"");
                v(items->name);
                if (contains(value, items->name))
                    o("\" selected=\"selected\">");
                else
                    o("\">");
                h(items->name);
                o("</option>\n");
            }
            o("</select>\n");

            break;
        case ELEM_UPLOADFILE:
            o("<input type=\"file\" class=\"element\" id=\"field");
            h(id);
            o("\" name=\"field");
            h(id);
            o("\" />\n");
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
    char path_info[DEFAULT_LENGTH];
    bt_element* elements = NULL;
    bt_project* project;

    strcpy(path_info, cgiPathInfo);
    db_init();
    project = db_get_project();
    output_header(project, "register.js");
    o(      "<h2>"); h(project->name);o(" - チケット登録</h2>\n"
            "<div id=\"input_form\">\n"
            "<h3>チケット登録</h3>\n"
            "<div class=\"message\">新規チケットを登録する場合は、以下のフォームを記入し登録ボタンをクリックしてください。</div>\n"
            "<div class=\"message\">※必須項目の入力チェックは、javascriptで行なっています。</div>\n");
    o(      "<form id=\"register_form\" name=\"register_form\" action=\"%s/register_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName);
    o(      "<table summary=\"input infomation\">\n");
    {
        bt_element_type* e_type = db_get_element_types(1);
        for (; e_type != NULL; e_type = e_type->next) {
            /* 返信専用属性は表示しない。 */
            if (e_type->reply_property == 1) continue;
            o("\t<tr>\n");
            o("\t\t<th>");
            h(e_type->name);
            if (e_type->required) {
                o("<span class=\"required\">※</span>");
            }
            o("</th><td>\n");
            o("\t\t\t<div id=\"field%d.required\" required=\"%d\" class=\"error\"></div>\n", e_type->id, e_type->required);
            output_form_element(elements, e_type);
            if (elements != NULL)
                elements = elements->next;
            o("\t\t\t<div class=\"description\">");h(e_type->description);o("&nbsp;</div>\n");
            o("\t\t</td>\n");
            o("\t</tr>\n");
        }
    }
    o(      "</table>\n"
            "<input class=\"button\" type=\"submit\" name=\"register\" value=\"登録\" />\n"
            "</form>\n"
            "</div>\n");
    db_finish();
    output_footer();
}
/**
 * 返信画面を表示するaction。
 */
void reply_action()
{
    char path_info[DEFAULT_LENGTH];
    char* ticket_id;
    bt_message* ticket;
    bt_message* reply;
    bt_element* elements = NULL;
    bt_element* last_elements = NULL;
    bt_element_type* element_types;
    int iid, *reply_ids, i;
    bt_project* project;

    strcpy(path_info, cgiPathInfo);
    ticket_id = strchr(path_info + 1, '/');
    if (ticket_id) ticket_id++;
    iid = atoi(ticket_id);
    db_init();
    project = db_get_project();
    output_header(project, "reply.js");
    ticket = db_get_ticket(iid);
    element_types = db_get_element_types(1);
    elements = db_get_last_elements(iid);
    o("<h2 id=\"subject\">"); h(project->name); o(" - ID:%5d ", ticket->id);
    h(get_element_value_by_id(elements, ELEM_ID_TITLE));
    o(" &nbsp;</h2>\n");
    o(      "<div id=\"ticket_newest\">\n"
            "<h3>チケット最新情報</h3>\n"
            "<table summary=\"newest table\">\n");
    {
        bt_element_type* e_type = element_types;
        for (; e_type != NULL; e_type = e_type->next) {
            char* value = get_element_value(elements, e_type);
            if (e_type->ticket_property == 0) continue;
            o("\t<tr>\n");
            o("\t\t<th>");
            h(e_type->name);
            o("&nbsp;</th>\n");
            o("\t\t<td>");
            switch (e_type->id) {
                case ELEM_ID_SENDER:
                    hmail(value);
                    break;
                default:
                    if (e_type->type == ELEM_UPLOADFILE) {
                        if (strlen(value)) {
                            o("<a href=\"%s/download/%d/", cgiScriptName, get_element_id(elements, e_type)); u(value); o("\" target=\"_blank\">");h(value); o("</a>\n");
                        }
                    } else {
                        hm(value);
                    }
            }
            o("&nbsp;</td>\n");
            o("\t</tr>\n");
        }
    }
    o(      "</table>\n"
            "</div>");
    last_elements = elements = db_get_elements(iid, 0);

    o(      "<div id=\"ticket_history\">\n"
            "<h3>チケット履歴</h3>\n"
            "<table summary=\"reply table\">\n");
    /* 投稿の表示 */
    o(      "\t<tr>\n"
            "\t\t<td colspan=\"2\" class=\"title\">投稿 ["); h(ticket->registerdate); o("]</td>\n"
            "\t</tr>\n");
    {
        bt_element_type* e_type = element_types;
        for (; e_type != NULL; e_type = e_type->next) {
            char* value = get_element_value(elements, e_type);
            o("\t<tr>\n");
            o("\t\t<th>");
            h(e_type->name);
            o("&nbsp;</th>\n");
            o("\t\t<td>");
            switch (e_type->id) {
                case ELEM_ID_SENDER:
                    hmail(value);
                    break;
                default:
                    if (e_type->type == ELEM_UPLOADFILE) {
                        if (strlen(value)) {
                            o("<a href=\"%s/download/%d/", cgiScriptName, get_element_id(elements, e_type)); u(value); o("\" target=\"_blank\">");h(value); o("</a>\n");
                        }
                    } else {
                        hm(value);
                    }
                    break;
            }
            o("&nbsp;</td>\n");
            o("\t</tr>\n");
        }
    }
    o("</table>\n");
    reply_ids = db_get_reply_ids(iid);
    /* 返信の表示 */
    for (i = 0; reply_ids[i] != 0; i++) {
        reply = db_get_reply(reply_ids[i]);
        last_elements = elements = db_get_elements(iid, reply_ids[i]);

        o(      "<table summary=\"reply table\">\n");
        o(      "\t<tr>\n"
                "\t\t<td colspan=\"2\" class=\"title\">返信: %d ", i + 1); o("["); h(reply->registerdate); o("]</td>\n"
                "\t</tr>\n");
        {
            bt_element_type* e_type = element_types;
            for (; e_type != NULL; e_type = e_type->next) {
                char* value = get_element_value(elements, e_type);
                o(      "\t<tr>\n"
                        "\t\t<th>");
                h(e_type->name);
                o(      "&nbsp;</th>\n"
                        "\t\t<td>");
                switch (e_type->id) {
                    case ELEM_ID_SENDER:
                        hmail(value);
                        break;
                    default:
                        if (e_type->type == ELEM_UPLOADFILE) {
                            if (strlen(value)) {
                                o("<a href=\"%s/download/%d/", cgiScriptName, get_element_id(elements, e_type)); u(value); o("\" target=\"_blank\">");h(value); o("</a>\n");
                            }
                        } else {
                            hm(value);
                        }
                }
                o(      "&nbsp;</td>\n"
                        "\t</tr>\n");
            }
        }
        o("</table>\n");
    }
        o(  "</div>\n");
    /* フォームの表示 */
    o(      "<div id=\"input_form\">\n"
            "<h3>チケット返信</h3>\n");
    o(      "<form id=\"reply_form\" name=\"reply_form\" action=\"%s/register_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName);
    o(      "<input type=\"hidden\" name=\"ticket_id\" value=\"%s\" />\n", ticket_id);
    o(      "<div class=\"message\">返信を行なう場合は、以下のフォームに内容を記入して返信ボタンをクリックしてください。</div>\n"
            "<div class=\"message\">※必須項目の入力チェックは、javascriptで行なっています。</div>\n"
            "<table summary=\"input table\">\n");
    {
        bt_element_type* e_type = element_types;
        for (; e_type != NULL; e_type = e_type->next) {
            o("\t<tr>\n"
                    "\t\t<th>");
            if (e_type->ticket_property)
                o("&nbsp;<span class=\"ticket_property\" title=\"チケット属性\">");
            h(e_type->name);
            if (e_type->required)
                o("<span class=\"required\">※</span>");
            if (e_type->ticket_property)
                o("</span>");
            o("</th>\n\t\t<td>");
            o("\t\t<div id=\"field%d.required\" required=\"%d\" class=\"error\"></div>\n", e_type->id, e_type->required);
            if (last_elements != NULL) {
                if (e_type->ticket_property)
                    output_form_element(last_elements, e_type);
                else
                    output_form_element(NULL, e_type);
                last_elements = last_elements->next;
            } else {
                output_form_element(NULL, e_type);
            }
            o("\t\t<div class=\"description\">");h(e_type->description);o("&nbsp;</div>\n");
            o("\t</td>\n");
            o("\t</tr>\n");
        }
    }
    o(      "</table>\n"
            "<input class=\"button\" type=\"submit\" name=\"reply\" value=\"返信\" />&nbsp;&nbsp;&nbsp;\n"
            "</form>\n"
            "</div>\n");
    db_finish();
    output_footer();
}
/**
 * 登録するaction。
 * 登録モード、編集モード、削除モードがある。
 */
void register_submit_action()
{
    bt_project* project;
    bt_element_type* e_type;
    bt_element* elements = NULL;
    bt_element* e = NULL;
    bt_message* ticket;
    char ticket_id[DEFAULT_LENGTH];
    int mode = get_mode();
    int mail_result;
    char** multi;

    if (mode == MODE_INVALID)
        die("reqired invalid mode.");
    ticket = (bt_message*)xalloc(sizeof(bt_message));
    db_init();
    db_begin();
    project = db_get_project();
    e_type = db_get_element_types(1);
    cgiFormStringNoNewlines("ticket_id", ticket_id, DEFAULT_LENGTH);
    if (mode == MODE_REGISTER)
        ticket->id = -1;
    else
        ticket->id = atoi(ticket_id);
    if (mode == MODE_REGISTER || mode == MODE_REPLY) {
        /* register, reply */
        for (; e_type != NULL; e_type = e_type->next) {
            char name[DEFAULT_LENGTH] = "";
            char value[VALUE_LENGTH] = ""; /* 1M */

            if (elements == NULL) {
                e = elements = (bt_element*)xalloc(sizeof(bt_element));
            } else {
                e->next = (bt_element*)xalloc(sizeof(bt_element));
                e = e->next;
            }
            e->next = NULL;
            e->element_type_id = e_type->id;
            e->is_file = 0;
            switch (e_type->type) {
                case ELEM_TEXT:
                    sprintf(name, "field%d", e_type->id);
                    cgiFormStringNoNewlines(name, value, VALUE_LENGTH);
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value) + 1);
                    strcpy(e->str_val, value);
                    break;
                case ELEM_TEXTAREA:
                    sprintf(name, "field%d", e_type->id);
                    cgiFormString(name, value, VALUE_LENGTH);
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value) + 1);
                    strcpy(e->str_val, value);
                    break;
                case ELEM_LIST_SINGLE:
                    sprintf(name, "field%d", e_type->id);
                    cgiFormString(name, value, VALUE_LENGTH);
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value) + 1);
                    strcpy(e->str_val, value);
                    break;
                case ELEM_LIST_MULTI:
                    sprintf(name, "field%d", e_type->id);
                    if ((cgiFormStringMultiple(name, &multi)) == cgiFormNotFound) {
                        strcpy(value, "");
                    } else {
                        int i = 0;
                        int len = 0;
                        while (multi[i]) {
                            len += strlen(multi[i]) + 1;
                            /* VALUE_LENGTH を超えない範囲で連結していく。
                             * 通常超えないはず */
                            if (len < VALUE_LENGTH) {
                                strcat(value, multi[i]);
                                strcat(value, "\t");
                            }
                            i++;
                        }
                    }
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value) + 1);
                    strcpy(e->str_val, value);
                    cgiStringArrayFree(multi);
                    break;
                case ELEM_UPLOADFILE:
                    sprintf(name, "field%d", e_type->id);
                    cgiFormFileName(name, value, VALUE_LENGTH);
                    e->str_val = (char*)xalloc(sizeof(char) * strlen(value) + 1);
                    strcpy(e->str_val, get_filename_without_path(value));
                    if (strlen(e->str_val)) {
                        e->is_file = 1;
                    }
                    break;
            }
        }
        ticket->elements = elements;
        if (mode == MODE_REGISTER) {
            ticket->id = db_register_ticket(ticket);
        } else {
            db_reply_ticket(ticket);
        }
    }
    /* mail */
    e_type = db_get_element_types(1);
    if ((mail_result = mail_send(project, ticket, elements, e_type)) != 0) {
        if (mail_result != MAIL_GAVE_UP)
            die("mail send error.");
    }
    db_commit();
    db_finish();
    output_header(project, NULL);
    o(      "<h2>処理完了</h2>\n"
            "<div id=\"complete_message\">\n"
            "<h3>おつかれさまでした。</h3>\n");
    if (mode == MODE_REGISTER)
        o("<div class=\"message\">登録しました</div>\n");
    else if (mode == MODE_REPLY)
        o("<div class=\"message\">返信しました</div>\n");
    else
        o("<div class=\"message\">感染しました。嘘</div>\n");
    o(      "</div>\n");
    output_footer();
}
/**
 * デフォルトのaction。
 */
void default_action()
{
    bt_project* project;
    bt_message* tickets;
    bt_state* states;

    db_init();
    project = db_get_project();
    output_header(project, NULL);
    states = db_get_states();
    o(      "<div id=\"info\">\n");
    /* 最新情報の表示 */
    o(      "<div id=\"top_newest\">\n");
    o(      "<h4>最新情報</h4>\n");
    o(      "\t<ul>\n");
    tickets = db_search_tickets(NULL);
    if (tickets != NULL) {
        int i;
        for (i = 0; tickets != NULL; tickets = tickets->next) {
            bt_element* elements = db_get_last_elements_4_list(tickets->id);
            o("\t\t<li>\n");
            o("\t\t\t<a href=\"%s/reply/%d=", cgiScriptName, tickets->id); o("\">");
            h(get_element_value_by_id(elements, ELEM_ID_TITLE));
            o(      "</a>\n");
            o("\t\t</li>\n");
            if (++i > 10) break;
        }
    }
    o(      "\t</ul>\n");
    o(      "</div>\n");
    /* stateの表示 */
    o(      "<div id=\"top_state_index\">\n");
    o(      "<h4>状態別件数</h4>\n");
    o(      "\t<ul>\n");
    for (; states != NULL; states = states->next) {
        o("\t\t<li>\n");
        o("\t\t\t<a href=\"%s/list?field%d=", cgiScriptName, ELEM_ID_STATUS); u(states->name); o("\">");
        h(states->name);
        o("\t\t\t</a>\n");
        o("(%d)", states->count);
        o("\t\t</li>\n");
    }
    o(      "\t</ul>\n");
    o(      "</div>\n");
    o(      "</div>\n");
    db_finish();
    o(      "<div id=\"main\">\n");
    o(      "<h2>");h(project->name);o("&nbsp;</h2>\n");
    o(      "<div id=\"main_body\">\n"
            "<div id=\"description\">");h(project->description);o("</div>\n");
    o(      "<div id=\"message\">\n"
            "Starbug1は、快適な動作をすることに主眼を置いたバグトラッキングツールです。"
            "影舞のように簡単に使えて痒いところに手が届くようになるといいと思います。\n"
            "</div>\n");
    o(      "<div class=\"top_edit\"><a href=\"%s/edit_top\">トップページの編集</a></div>\n", cgiScriptName);
    wiki_out("wiki/top.wiki");
    o(      "</div>\n");
    o(      "</div>\n");
    o(      "</div>\n");
    output_footer();
}

void rss_action()
{
    bt_message* tickets;
    bt_project* project;

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
    o(      "\t\t\t\t<rdf:li rdf:resource=\"");h(project->host_name);o("%s/list\"/>\n", cgiScriptName);
    o(      "\t\t\t\t<rdf:li rdf:resource=\"");h(project->host_name);o("%s/register\"/>\n", cgiScriptName);
    o(      "\t\t\t</rdf:Seq>\n"
            "\t\t</items>\n"
            "\t</channel>\n");

    tickets = db_search_tickets(NULL);
    if (tickets != NULL) {
        int i;
        for (i = 0; tickets != NULL; tickets = tickets->next) {
            char sender[DEFAULT_LENGTH];
            bt_element* elements = db_get_last_elements_4_list(tickets->id);
            o(      "\t<item rdf:about=\"");h(project->home_url);o("%s/reply/%d\">\n", cgiScriptName, tickets->id);
            o(      "\t\t<title>ID:%5d ", tickets->id);
            h(get_element_value_by_id(elements, ELEM_ID_TITLE));
            o(      "</title>\n");
            o(      "\t\t<link>");h(project->home_url);o("%s/reply/%d</link>\n", cgiScriptName, tickets->id);
            o(      "\t\t<description>");
            o(      "投稿者: ");
            hmail(db_get_original_sender(tickets->id, sender));
            h("<br>");
            o(      "投稿日: ");
            h(tickets->registerdate);
            h("<br>");
            for (; elements != NULL; elements = elements->next) {
                h(elements->str_val);
                h("<br>");
            }
            o(      "</description>\n"
                    "\t</item>\n");
            if (++i > 10) break;
        }
    }
    o(      "</rdf:RDF>\n");
    db_finish();
}
void help_action()
{
    bt_project* project;
    db_init();
    project = db_get_project();
    output_header(project, NULL);
    db_finish();
    o(      "<h2>");h(project->name);o(" - Starbug1</h2>\n"
            "<div id=\"top\">\n");
    wiki_out("wiki/help.wiki");
    o(      "</div>\n");
    output_footer();
}
void edit_top_action()
{
    bt_project* project;
    db_init();
    project = db_get_project();
    output_header(project, "edit_top.js");
    db_finish();
    o(      "<h2>トップページの編集</h2>\n"
            "<div id=\"top\">\n"
            "<h3>トップページの編集</h3>\n"
            "<div id=\"message\">簡易wikiの文法でトップページのコンテンツの編集を行ない、更新ボタンをクリックしてください。</div>"
            "<form action=\"%s/edit_top_submit\" method=\"post\">\n", cgiScriptName);
    o(      "<textarea name=\"edit_top\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    wiki_content_out("wiki/top.wiki");
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"更新\" />\n"
            "</form>");
    o(      "</div>\n");
    output_footer();
}
void edit_top_submit_action()
{
    char value[VALUE_LENGTH];
    bt_project* project;

    cgiFormString("edit_top", value, VALUE_LENGTH);
    wiki_save("wiki/top.wiki", value);

    db_init();
    project = db_get_project();
    output_header(project, NULL);
    db_finish();

    o(      "<h2>処理完了</h2>\n"
            "<div id=\"complete_message\">\n"
            "<h3>おつかれさまでした。</h3>\n");
    o("<div class=\"message\">更新しました</div>\n");
    o(      "</div>\n");
    output_footer();
}
void download_action()
{
    bt_element_file* file;
    char path_info[DEFAULT_LENGTH];
    char* element_id_str;
    int element_id;
    int i;
    char* p;

    strcpy(path_info, cgiPathInfo);
    element_id_str = strchr(path_info + 1, '/');
    if (element_id_str) element_id_str++;
    element_id = atoi(element_id_str);
    d("element_id: %d\n", element_id);

    db_init();
    file = db_get_element_file(element_id);
    if (!file) goto error;
    o("Content-Type: %s\r\n", file->content_type);
    o("Content-Length: %d\r\n", file->size);
    o("Content-Disposition: attachment;\r\n");
    o("\r\n");

    p = file->blob;
    for (i = 0; i < file->size; i++) {
        d("%c", *p);
        fputc(*p, cgiOut);
        p++;
    }
    db_finish();

error:
    cgiHeaderContentType("text/plain; charset=utf-8;");
    o("error: ファイルがありません。");

}
