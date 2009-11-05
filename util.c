#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <cgic.h>
#include <errno.h>
#include "alloc.h"
#include "conv.h"
#include "util.h"
#include "hook.h"

/* プロジェクトコード */
char g_project_code[DEFAULT_LENGTH] = "";
char g_project_code_4_url[DEFAULT_LENGTH] = "";
/* アクション名 */
char g_action_name[DEFAULT_LENGTH] = "";
/* PATH_INFO */
char g_path_info[DEFAULT_LENGTH] = "";

static Action* get_actions();

void redirect_raw(const char*);
void page_not_found();

Action* actions = NULL;

static Action* get_actions()
{
    return actions;
}
void register_action_actions(char* action_name, void func(void))
{
    Action* a;
    if (actions == NULL) {
        actions = xalloc(sizeof(Action));
        actions->action_name = "__HEAD__";
        actions->next = NULL;
    }
    for (a = actions; a->next != NULL; a = a->next);
    a->next = xalloc(sizeof(Action));
    a = a->next;
    a->action_name = action_name;
    a->action_func = func;
    a->next = NULL;
}
void free_action_actions()
{
    Action* a = actions->next;
    while (a) {
        Action* old = a;
        a = a->next;
        xfree(old);
    }
    xfree(actions);
    d("alloc_count: %d\n", get_alloc_count());
}

/* 復帰できないエラーが発生した場合にエラーページを表示する。 */
void print_error_page(char* file_name, int line_number, char* message)
{
    o("Status: 500 Starbug1 Internal Error.\r\n");
    o("Content-Type: text/html\r\n\r\n");
    o(  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
        "<head>"
            "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />"
        "</head>"
        "<body>"
            "<h2>ERROR oops!</h2>"
            "エラーが発生しましたm(_ _)m"
            "<h3>detail information</h3>");
    o(      "<p>[%s(%d)] %s</p>",
                file_name,
                line_number,
                message);
    o(      "<hr />"
            "<div align=\"right\">"
            "<address>%s.</address>"
            "</div>"
        "</body>"
        "</html>", COPYRIGHT);
}

ActionType analysis_action()
{
    char* index;
    char* script_name = cgiScriptName;
    char* path_info = cgiPathInfo;
    char index_cgi_file_name[DEFAULT_LENGTH] = "";
    char admin_cgi_file_name[DEFAULT_LENGTH] = "";
    ActionType ret = ACTION_TYPE_NONE;

    memset(g_project_code, 0, DEFAULT_LENGTH);
    memset(g_action_name, 0, DEFAULT_LENGTH);
    memset(g_path_info, 0, DEFAULT_LENGTH);
    d("************************************\n");
    d("REQUEST: %s%s\n", script_name, path_info);
    if (strlen(path_info) > 1) {
        strncpy(g_project_code, path_info + 1, DEFAULT_LENGTH);
    } else {
        redirect_raw("top");
        return ret; /* ACTION_TYPE_NONE を返却する。 */
    }
    if ((index = strchr(g_project_code, '/'))) {
        *index = '\0';
        strncpy(g_action_name, index + 1, DEFAULT_LENGTH - (strlen(g_project_code) + 1));
        if ((index = strchr(g_action_name, '/'))) {
            *index = '\0';
            /* 残りをpath_infoとする */
            strcpy(g_path_info, index + 1);
        }
    }
    /* URLとして使われるプロジェクトID。URL encodeする。 */
    url_encode((unsigned char*)g_project_code, (unsigned char*)g_project_code_4_url, DEFAULT_LENGTH);

    sprintf(index_cgi_file_name, "index.%s", get_ext(cgiScriptName));
    sprintf(admin_cgi_file_name, "admin.%s", get_ext(cgiScriptName));
    /* 実行するモードの判定 */
    if (strstr(script_name, index_cgi_file_name)) {
        if (strcmp(g_project_code, "top") == 0) {
            d("actiontype top\n");
            ret = ACTION_TYPE_INDEX_TOP;
        } else {
            d("actiontype project\n");
            ret = ACTION_TYPE_INDEX;
        }
    } else if (strstr(script_name, admin_cgi_file_name)) {
        d("g_project_code: %s\n", g_project_code);
        if (strcmp(g_project_code, "top") == 0) {
            d("actiontype admin top\n");
            ret = ACTION_TYPE_ADMIN_TOP;
        } else {
            d("actiontype admin project\n");
            ret = ACTION_TYPE_ADMIN;
        }
    }
    return ret;
}
void exec_action()
{
    Action* a;
    char action_name[DEFAULT_LENGTH];
    strcpy(action_name, g_action_name);
    for (a = get_actions(); a != NULL; a = a->next) {
/*         d("action: %s %s\n", a->action_name, action_name); */
        if (!strcmp(action_name, a->action_name)) {
            d("=exec_action start: %s\n", a->action_name);
            a->action_func();
            d("=exec_action end  : %s\n", a->action_name);
            return;
        }
    }
    if (strlen(action_name) != 0) {
        /* 知らないActionが指定されたら、リダイレクトする。  */
        redirect("", NULL);
    } else {
        /* path_infoが空なら、top_actionを呼び出す。 */
        for (a = get_actions(); a != NULL; a = a->next) {
            if (!strcmp("top", a->action_name) || !strcmp("top_top", a->action_name)) {
                d("=exec_action default_action start: %s\n", a->action_name);
                a->action_func();
                d("=exec_action default_action end  : %s\n", a->action_name);
                return;
            }
        }
    }
}
#define TRYPUTC(ch) \
    { \
        if (putc((ch), cgiOut) == EOF) { \
            return cgiFormIO; \
        } \
    } 
static const char PROJECT_SPLITTER = ':';

/**
 * リンクかどうかの判定を行なう。
 * @return リンクと判定された文字列の長さ。
 *         リンクではないと判定された場合は、0
 */
int get_link_syntax_len(char* data, size_t len, char* link)
{
    char string[1024];
    int index = 0;
    char* block_p;
    char* line_p;

    memset(string, '\0', 1024);
    strncpy(string, data, MIN(len, 1024 - 1));
    if (len == 0) return index; /* 最後の文字だった */

    block_p = MIN(strchr(string, ' '), strchr(string, '\t'));
    line_p = strchr(string, '\n');
    if (!block_p)
        block_p = string + strlen(string);
    if (!line_p)
        line_p = string + strlen(string);

    /* 解析対象の文字列を特定 */
    string[MIN(block_p - string, line_p - string)] = '\0';
    if (strstr(string, "http://") == NULL && strstr(string, "https://") == NULL) {
        /* http://かhttps://で始まっていない場合は、linkではない。 */
        return 0;
    }
    if (strchr(string, '<') != NULL || strchr(string, '>') != NULL) {
        /* XSS対策のため、<>があったらリンクにしない。 */
        return 0;
    }

    {
        char* link_p = string + 1;

        while (index++, 1) { /* each char */
            /* 
             * 一文字つづ進めていく。
             * 途中でリンクでないと判定された場合は、index を 0 にして、breakする。
             * リンクが終了したら、breakする。
             */
            if (string[index] == '\0') {
                break;
            } else {
                if (string[index] >= ' ' && string[index] <= '~') {
                    /* 文字を検出した場合 (正常) */
                } else {
                    /* reached to end. */
                    string[index] = '\0'; break; /* 一つ前までがリンクとなる。 */
                }
            }
        }
        if (index < 2) {
            /* リンクではない。 */
            return 0;
        }
        strcpy(link, link_p);
    }
    return index;
}
/**
 * チケットリンクかどうかの判定を行なう。
 * @return チケットリンクと判定された文字列の長さ。
 *         チケットリンクではないと判定された場合は、0
 */
int get_ticket_syntax_len(char* data, size_t len, char* project_id, char* ticket_id)
{
    char string[1024];
    int index = 0;
    char* block_p;
    char* line_p;

    memset(string, '\0', 1024);
    strncpy(string, data, MIN(len, 1024 - 1));
    if (len == 0) return index; /* 最後の文字だった */

    block_p = strchr(string, ' ');
    line_p = strchr(string, '\n');
    if (!block_p)
        block_p = string + strlen(string);
    if (!line_p)
        line_p = string + strlen(string);

    /* 解析対象の文字列を特定 */
    string[MIN(block_p - string, line_p - string)] = '\0';

    {
        char* project_mode = strchr(string, PROJECT_SPLITTER);
        char* project_id_p = string + 1;
        char* ticket_no_p = string + 1;

        while (index++, 1) { /* each char */
            /* 
             * 一文字つづ進めていく。
             * 途中でチケットリンクでないと判定された場合は、index を 0 にして、breakする。
             * チケットリンクが終了したら、breakする。
             */
            if (string[index] == '\0') {
                break;
            } else if (project_mode) {
                /* #プロジェクト:チケットモード */
                if (string[index] == PROJECT_SPLITTER) {
                    /* プロジェクトIDモードであり、:を検出した場合 */
                    ticket_no_p = &string[index + 1];
                    /* :で終了している場合は、チケットリンクではない。 */
                    if (string[index + 1] < '0' || string[index + 1] > '9') {
                        index = 0; break;
                    }
                } else if (project_id_p != ticket_no_p && (string[index] < '0' || string[index] > '9')) {
                    /* チケットNO検索中に、数字以外を検出した場合 */
                    string[index] = '\0'; break; /* 一つ前までがチケットリンクとなる。 */
                }
            } else if (!project_mode) {
                /* #チケットモード */
                if (string[index] >= '0' && string[index] <= '9') {
                    /* プロジェクトIDモードではなく、数字を検出した場合 (正常) */
                } else {
                    /* reached to end. */
                    string[index] = '\0'; break; /* 一つ前までがチケットリンクとなる。 */
                }
            }
        }
        if (index < 2) {
            /* チケットリンクではない。 */
            return 0;
        }
        if (project_id_p != ticket_no_p) {
            strncpy(project_id, project_id_p, (ticket_no_p - 1) - project_id_p);
        }
        strcpy(ticket_id, ticket_no_p);
        /*         index = (ticket_no_p - string) + strlen(ticket_no_p); */
    }
    return index;
}

/*
 * 複数行テキストの領域では、pre記法をサポートする。
 * チケットリンクをサポートする。
 */
static cgiFormResultType cgiHtmlEscapeDataMultiLine(char *data, int len)
{
    bool printing_pre = false;
    while (len--) {
        if (*data == 'h') {
            int link_syntax_len;
            char project_id[DEFAULT_LENGTH];
            char link[DEFAULT_LENGTH];
            memset(project_id, '\0', DEFAULT_LENGTH);
            memset(link, '\0', DEFAULT_LENGTH);
            /* ループの最初でlenが減っているので、get_ticket_syntax_lenに渡すときは戻して呼び出す。 */
            link_syntax_len = get_link_syntax_len(data, len + 1, link);
            d("link_syntax_len: %d \n", link_syntax_len);
            if (link_syntax_len == 0) {
                TRYPUTC(*data);
            } else {
                o("<a href=\"h%s\">h%s</a>",
                        link, 
                        link);
                data += link_syntax_len - 1;
                len -= link_syntax_len - 1;
            }
        } else if (*data == '#') {
            int ticket_syntax_len;
            char project_id[DEFAULT_LENGTH];
            char ticket_id[DEFAULT_LENGTH];
            memset(project_id, '\0', DEFAULT_LENGTH);
            memset(ticket_id, '\0', DEFAULT_LENGTH);
            /* ループの最初でlenが減っているので、get_ticket_syntax_lenに渡すときは戻して呼び出す。 */
            ticket_syntax_len = get_ticket_syntax_len(data, len + 1, project_id, ticket_id);
            d("ticket_syntax_len: %d \n", ticket_syntax_len);
            if (ticket_syntax_len == 0) {
                TRYPUTC(*data);
            } else {
                char display_text[DEFAULT_LENGTH];
                if (strlen(project_id) == 0) {
                    strcpy(project_id, g_project_code_4_url);
                    strcpy(display_text, ticket_id);
                } else {
                    sprintf(display_text, "%s:%s", project_id, ticket_id);
                }
                o("<a href=\"%s/%s/ticket/%s\">#%s</a>",
                        cgiScriptName, 
                        project_id, 
                        ticket_id, 
                        display_text);
                data += ticket_syntax_len - 1;
                len -= ticket_syntax_len - 1;
            }
        } else if (len > 2 && *data == '>' &&
                *(data + 1) == '|' &&
                (*(data + 2) == '\r' || *(data + 2) == '\n')) {
            /* pre記法 */
            TRYPUTC('<');
            TRYPUTC('p');
            TRYPUTC('r');
            TRYPUTC('e');
            TRYPUTC('>');
            data += 2;
            len -= 2;
            printing_pre = true;
        } else if (len > 0 && *data == '|' &&
                *(data + 1) == '<' &&
                (*(data + 2) == '\r' || *(data + 2) == '\n' || len == 1)) {
            /* pre記法 */
            TRYPUTC('<');
            TRYPUTC('/');
            TRYPUTC('p');
            TRYPUTC('r');
            TRYPUTC('e');
            TRYPUTC('>');
            data += (len == 1) ? 1 : 2; /* |< で終わっている場合(改行が無い場合) は、1を足す。改行が付いている場合は、2を足す。*/
            len -= (len == 1) ? 1 : 2;
            printing_pre = false;
        } else if (*data == '<') {
            TRYPUTC('&');
            TRYPUTC('l');
            TRYPUTC('t');
            TRYPUTC(';');
        } else if (*data == '&') {
            TRYPUTC('&');
            TRYPUTC('a');
            TRYPUTC('m');
            TRYPUTC('p');
            TRYPUTC(';');
        } else if (*data == '>') {
            TRYPUTC('&');
            TRYPUTC('g');
            TRYPUTC('t');
            TRYPUTC(';');
        } else if (printing_pre == false && *data == '\n') {
            TRYPUTC('<');
            TRYPUTC('b');
            TRYPUTC('r');
            TRYPUTC(' ');
            TRYPUTC('/');
            TRYPUTC('>');
        } else {
            TRYPUTC(*data);
        }
        data++;
    }
    return cgiFormSuccess;
}

void hm(char *s)
{
    cgiHtmlEscapeDataMultiLine(s, (int) strlen(s));
}
static cgiFormResultType cgiHtmlEscapeDataMailaddress(char *data, int len)
{
    while (len--) {
        if (*data == '<') {
            TRYPUTC('&');
            TRYPUTC('l');
            TRYPUTC('t');
            TRYPUTC(';');
        } else if (*data == '&') {
            TRYPUTC('&');
            TRYPUTC('a');
            TRYPUTC('m');
            TRYPUTC('p');
            TRYPUTC(';');
        } else if (*data == '>') {
            TRYPUTC('&');
            TRYPUTC('g');
            TRYPUTC('t');
            TRYPUTC(';');
        } else if (*data == '@') {
            TRYPUTC(' ');
            TRYPUTC('_');
            TRYPUTC('a');
            TRYPUTC('t');
            TRYPUTC('_');
            TRYPUTC(' ');
        } else {
            TRYPUTC(*data);
        }
        data++;
    }
    return cgiFormSuccess;
}
static cgiFormResultType cgi_html_escape(char *data, int len)
{
    while (len--) {
        if (*data == '<') {
            TRYPUTC('&');
            TRYPUTC('l');
            TRYPUTC('t');
            TRYPUTC(';');
        } else if (*data == '&') {
            TRYPUTC('&');
            TRYPUTC('a');
            TRYPUTC('m');
            TRYPUTC('p');
            TRYPUTC(';');
        } else if (*data == '>') {
            TRYPUTC('&');
            TRYPUTC('g');
            TRYPUTC('t');
            TRYPUTC(';');
        } else if (*data == '"') {
            TRYPUTC('&');
            TRYPUTC('q');
            TRYPUTC('u');
            TRYPUTC('o');
            TRYPUTC('t');
            TRYPUTC(';');
        } else if (*data == '\'') {
            TRYPUTC('&');
            TRYPUTC('#');
            TRYPUTC('3');
            TRYPUTC('9');
            TRYPUTC(';');
        } else {
            TRYPUTC(*data);
        }
        data++;
    }
    return cgiFormSuccess;
}
void cgi_escape(char *s)
{
    cgi_html_escape(s, (int) strlen(s));
}

void hmail(char *s)
{
    cgiHtmlEscapeDataMailaddress(s, (int) strlen(s));
}

void u(char* str)
{
    char buf[DEFAULT_LENGTH];
    url_encode((unsigned char*)str, (unsigned char*)buf, DEFAULT_LENGTH);
    fprintf(cgiOut, "%s", buf);
}
char* get_filename_without_path(char* path)
{
    char* p = path;
    register char* c;
    if (strlen(p) == 0) return p;
    while ((c = strstr(p, "\\")) != NULL) {
        p = ++c;
    }
    while ((c = strstr(p, "/")) != NULL) {
        p = ++c;
    }
    return p;
}
char* get_upload_filename(const int element_id, char* buf)
{
    char name[DEFAULT_LENGTH];
    sprintf(name, "field%d", element_id);
    cgiFormFileName(name, buf, DEFAULT_LENGTH);
    return buf;
}
int get_upload_size(const int element_id)
{
    char name[DEFAULT_LENGTH];
    int size;
    sprintf(name, "field%d", element_id);
    cgiFormFileSize(name, &size);
    return size;
}
char* get_upload_content_type(const int element_id, char* buf)
{
    char name[DEFAULT_LENGTH];
    sprintf(name, "field%d", element_id);
    cgiFormFileContentType(name, buf, DEFAULT_LENGTH);
    return buf;
}
ElementFile* get_upload_content(const int element_id)
{
    int got_count = 0;
    char* buffer_org;
    char* buffer;
    char b[DEFAULT_LENGTH];
    char name[DEFAULT_LENGTH];
    cgiFilePtr file;
    ElementFile* content = element_file_new();
    content->size = get_upload_size(element_id);
    buffer = buffer_org = xalloc(sizeof(char) * content->size);
    sprintf(name, "field%d", element_id);
    if (cgiFormFileOpen(name, &file) != cgiFormSuccess) {
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
    content->content = buffer_org;
    cgiFormFileClose(file);
    return content;
}

void page_not_found()
{
    o("Status: 404 Not Found.\r\n");
    o("Content-Type: text/plain; charset=utf-8;\r\n\r\n");
    o("404 Not Found.\r\n");
}
void redirect_raw(const char* path)
{
    char redirecturi[DEFAULT_LENGTH];
    sprintf(redirecturi, "%s/%s", cgiScriptName, path);
    o("Status: 302 Temporary Redirection\r\n");
    cgiHeaderLocation(redirecturi);
}
void redirect(const char* path, const char* message)
{
    char redirecturi[DEFAULT_LENGTH];
    char uri[DEFAULT_LENGTH];
    char param[DEFAULT_LENGTH];
    char parambuf[DEFAULT_LENGTH];

    strcpy(uri, path);
    if (message) {
        strcpy(param, message);
        url_encode((unsigned char*)param, (unsigned char*)parambuf, DEFAULT_LENGTH);
        strcat(uri, "?message=");
        strcat(uri, parambuf);
    }
    sprintf(redirecturi, "%s/%s%s", cgiScriptName, g_project_code, uri);
    o("Status: 302 Temporary Redirection\r\n");
    cgiHeaderLocation(redirecturi);
}
void redirect_with_hook_messages(const char* path, const char* message, List* results)
{
    char redirecturi[DEFAULT_LENGTH];
    char uri[DEFAULT_LENGTH];
    char param[DEFAULT_LENGTH];
    char parambuf[DEFAULT_LENGTH];
    Iterator* it;

    strcpy(uri, path);
    if (message) {
        strcpy(param, message);
        url_encode((unsigned char*)param, (unsigned char*)parambuf, DEFAULT_LENGTH);
        strcat(uri, "?message=");
        strcat(uri, parambuf);
    }
    if (results) {
        int i = 0;
        foreach (it, results) {
            HOOK_RESULT* result = it->element;
            strcpy(param, result->message);
            url_encode((unsigned char*)param, (unsigned char*)parambuf, DEFAULT_LENGTH);
            if (!message && i == 0)
                strcat(uri, "?");
            else
                strcat(uri, "&");
            strcat(uri, "message=");
            strcat(uri, parambuf);
            i++;
        }
    }
    sprintf(redirecturi, "%s/%s%s", cgiScriptName, g_project_code, uri);
    o("Status: 302 Temporary Redirection\r\n");
    cgiHeaderLocation(redirecturi);
}
static cgiFormResultType csv_escape(char *data, int len)
{
    while (len--) {
        if (*data == '"') {
            TRYPUTC('"');
            TRYPUTC('"');
        } else {
            TRYPUTC(*data);
        }
        data++;
    }
    return cgiFormSuccess;
}
void csv_field(char* src)
{
    o("\"");
    csv_escape(src, strlen(src));
    o("\"");
}
static cgiFormResultType cgiCssClassName(char *data, int len)
{
    while (len--) {
        /* css の classnameとして使用するため、+ と / は出力しない。 */
        if (*data == '+') {
        } else if (*data == '/') {
        } else {
            TRYPUTC(*data);
        }
        data++;
    }
    return cgiFormSuccess;
}
void css_field(char* str)
{
    char src[DEFAULT_LENGTH];
    char dist[DEFAULT_LENGTH];
    char* p = src;

    strcpy(src, str);
    if (strlen(src) > 100) {
        p += 100;
        p = '\0';
    }
        
    memset(dist, 0, DEFAULT_LENGTH);
    base64_encode((unsigned char*)src, (unsigned char*)dist);
    cgiCssClassName(dist, strlen(dist));
}
String* get_base_url(String* buf)
{
    char* https = getenv("HTTPS");
    bool is_ssl = (https == '\0') ? false : true;
    int server_port = atoi(cgiServerPort);
    int is_default_port_no = 
        (is_ssl && server_port == 443) || (is_ssl == 0 && server_port == 80);
    string_appendf(buf, "http%s://%s%s%s%s",
            is_ssl ? "s" : "",
            cgiServerName,
            is_default_port_no ? "" : ":",
            is_default_port_no ? "" : cgiServerPort,
            cgiScriptName);
    return buf;
}
bool contains(char* const value, const char* name)
{
    char* p = value;
    do {
        if (*p == '\t') p++;
        if (strncmp(p, name, strlen(name)) == 0)
            return true;
    } while ((p = strstr(p, "\t")) != NULL);
    return false;
}
void set_cookie(char* key, char* value)
{
    char value_base64[DEFAULT_LENGTH];
    memset(value_base64, 0, DEFAULT_LENGTH);
    base64_encode((unsigned char*)value, (unsigned char*)value_base64);
    cgiHeaderCookieSetString(key, value_base64, 86400 * 30, cgiScriptName, cgiServerName);
}
void clear_cookie(char* key)
{
    cgiHeaderCookieSetString(key, "", 0, cgiScriptName, cgiServerName);
}
void get_cookie_string(char* key, char* buf)
{
    char value[DEFAULT_LENGTH];
    char value_base64[DEFAULT_LENGTH];
    cgiCookieString(key, value, DEFAULT_LENGTH);
    memset(value_base64, 0, DEFAULT_LENGTH);
    base64_decode((unsigned char*)value, (unsigned char*)value_base64);
    strcpy(buf, value_base64);
}

void set_date_string(char* buf)
{
    struct timeval tv;
    struct tm *tp;

    gettimeofday(&tv, NULL);
    tp = localtime(&tv.tv_sec);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
            tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
            tp->tm_hour, tp->tm_min, tp->tm_sec);
}
/*
 * 引数の文字列から拡張子部分のポインタを返す。
 */
char* get_ext(char* path)
{
    char* p = path;
    /* 後ろから探して.があったら、そこの文字配列のポインタを返却する。 */
    for (p += strlen(path); p > path; p--) {
        switch (*p) {
            case '.':
                return (char*)p + 1;
            case '\\':
            case '/':
                return "";
        }
    }
    return "";
}
void print_field_help()
{
    o(      "<div class=\"description\">\n"
            "\t<ul>\n"
            "\t\t<li>%s</li>\n"
            "\t\t<li>%s</li>\n"
            "\t\t<li>%s</li>\n"
            "\t</ul>\n"
            "</div>\n",
            _("[ticket form syntax description1]"),
            _("[ticket form syntax description2]"),
            _("[ticket form syntax description3]"));
}
void set_locale(char* locale)
{
    char locale_utf8[DEFAULT_LENGTH];
    sprintf(locale_utf8, "%s.UTF-8", locale);
    d("locale: %s\n", locale_utf8);
#ifndef _WIN32
    setenv("LANG", locale_utf8, 1); /* FreeBSD(さくらインターネット)でsetlocaleが動作しない場合があったため、環境変数を書き換える。 */
#else
    {
        char envstr[256];
        strcpy(envstr, "LANG=");
        strcat(envstr, locale_utf8);
        putenv(envstr);
    }
#endif
    d("setlocale: %s\n", setlocale(LC_ALL, locale_utf8));
    d("bindtextdomain: %s\n", bindtextdomain("starbug1", "locale"));
    d("textdomain: %s\n", textdomain("starbug1"));
#ifdef _WIN32
    bind_textdomain_codeset("starbug1", "utf-8");
#endif
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
