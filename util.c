#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <cgic.h>
#include <iconv.h>
#include <errno.h>
#include "util.h"
#include "hook.h"

/* プロジェクト名 */
char g_project_name[DEFAULT_LENGTH] = "";
char g_project_name_4_url[DEFAULT_LENGTH] = "";
/* アクション名 */
char g_action_name[DEFAULT_LENGTH] = "";
/* PATH_INFO */
char g_path_info[DEFAULT_LENGTH] = "";

unsigned long url_encode(unsigned char*, unsigned char*, unsigned long);
static Action* get_actions();

void redirect_raw(const char*);

Action* actions = NULL;
int alloc_count = 0;

void* xalloc(size_t size)
{
    void* p;
    p = calloc(1, size);
    if (!p) {
        die("memory error.");
    }
    alloc_count++;
/*     d("xalloc: %p\n", p); */
    return p;
}
void xfree(void* p)
{
/*     d("xfree: %p\n", p); */
    alloc_count--;
    free(p);
    p = NULL;
}
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
    d("alloc_count: %d\n", alloc_count);
}

/* 復帰できないエラーが発生した場合にエラーページを表示する。 */
void print_error_page(char* file_name, int line_number, char* message)
{
    o("Status: 500 Starbug1 Internal Error.\r\n");
    o("Content-Type: text/html\r\n\r\n");
    o(  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
        "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">"
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
            "<address>Copyright smeghead 2007 - 2008.</address>"
            "</div>"
        "</body>"
        "</html>");
}

ActionType analysis_action()
{
    char* index;
    char* script_name = cgiScriptName;
    char* path_info = cgiPathInfo;
    char index_cgi_file_name[DEFAULT_LENGTH] = "";
    char admin_cgi_file_name[DEFAULT_LENGTH] = "";
    ActionType ret = ACTION_TYPE_NONE;

    memset(g_project_name, 0, DEFAULT_LENGTH);
    memset(g_action_name, 0, DEFAULT_LENGTH);
    memset(g_path_info, 0, DEFAULT_LENGTH);
    d("************************************\n");
    d("REQUEST: %s%s\n", script_name, path_info);
    if (strlen(path_info) > 1) {
        strncpy(g_project_name, path_info + 1, DEFAULT_LENGTH);
    } else {
        redirect_raw("top");
        die("redirect to top.");
    }
    if ((index = strchr(g_project_name, '/'))) {
        *index = '\0';
        strncpy(g_action_name, index + 1, DEFAULT_LENGTH - (strlen(g_project_name) + 1));
        if ((index = strchr(g_action_name, '/'))) {
            *index = '\0';
            /* 残りをpath_infoとする */
            strcpy(g_path_info, index + 1);
        }
    }
    /* URLとして使われるプロジェクトID。URL encodeする。 */
    url_encode((unsigned char*)g_project_name, (unsigned char*)g_project_name_4_url, DEFAULT_LENGTH);

    sprintf(index_cgi_file_name, "index.%s", get_ext(cgiScriptName));
    sprintf(admin_cgi_file_name, "admin.%s", get_ext(cgiScriptName));
    /* 実行するモードの判定 */
    if (strstr(script_name, index_cgi_file_name)) {
        if (strcmp(g_project_name, "top") == 0) {
            d("actiontype top\n");
            ret = ACTION_TYPE_INDEX_TOP;
        } else {
            d("actiontype project\n");
            ret = ACTION_TYPE_INDEX;
        }
    } else if (strstr(script_name, admin_cgi_file_name)) {
        d("g_project_name: %s\n", g_project_name);
        if (strcmp(g_project_name, "top") == 0) {
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
int get_ticket_syntax_len(char* data, size_t len)
{
    int ticket_syntax_len = 0;
    if (len == 0) return ticket_syntax_len; /* 最後の文字だった */
    data += 1; /* 1文字進める。 */
    while (len--) {
        if (*data < '0' || *data > '9') 
            break; /* 数字でない場合は、breakする。 */
        data++;
        ticket_syntax_len++;
    }
    return ticket_syntax_len;
}
/*
 * 複数行テキストの領域では、pre記法をサポートする。
 * チケットリンクをサポートする。
 */
static cgiFormResultType cgiHtmlEscapeDataMultiLine(char *data, int len)
{
    bool printing_pre = false;
    while (len--) {
        if (*data == '#') {
            int ticket_syntax_len;
            ticket_syntax_len = get_ticket_syntax_len(data, len);
            if (ticket_syntax_len == 0) {
                TRYPUTC(*data);
            } else {
                char ticket_id[DEFAULT_LENGTH];
                strncpy(ticket_id, ++data, ticket_syntax_len);
                ticket_id[ticket_syntax_len] = '\0';
                o("<a href=\"%s/ticket/%s\">#%s</a>", cgiScriptName, ticket_id, ticket_id);
                data += ticket_syntax_len - 1;
                len -= ticket_syntax_len;
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
unsigned long url_encode(unsigned char* csource, unsigned char* cbuffer, unsigned long lbuffersize)
{
    unsigned long llength;
    unsigned long lcount = 0;
    unsigned char cbyte;
    unsigned char ctemp[4];
    unsigned long lresultcount = 0;

    llength = (unsigned long)strlen((char*)csource);
    if(!llength) { return lresultcount; }
    if(lbuffersize < (llength * 3 + 1)) { return lresultcount; }

    d("encoding: %s\n", csource);
    while(1) {
        cbyte = *(csource + lcount);
        if( ((cbyte >= 0x81) && (cbyte <= 0x9F)) ||
                ((cbyte >= 0xE0) && (cbyte <= 0xEF)) ) {            /* Shift-JIS 2 バイト文字だった場合 */
            sprintf((char*)ctemp, "%%%02X", cbyte);                        /* URL エンコード(上位バイト) */
            strncpy((char*)(cbuffer + lresultcount), (char*)ctemp, 4);
            lcount++;
            lresultcount += 3;
            if(lcount == llength) { break; }                        /* 文字列の終端に達した場合、ループを抜ける */
            sprintf((char*)ctemp, "%%%02X", *(csource + lcount));          /* URL エンコード(下位バイト) */
            strncpy((char*)cbuffer + lresultcount, (char*)ctemp, 4);
            lcount++;
            lresultcount += 3;
        } else if( ((cbyte >= 0x40) && (cbyte <= 0x5A)) ||          /* @A-Z */
                ((cbyte >= 0x61) && (cbyte <= 0x7A)) ||             /* a-z */
                ((cbyte >= 0x30) && (cbyte <= 0x39)) ||             /* 0-9 */
                (cbyte == 0x2A) ||                                  /* "*" */
                (cbyte == 0x2D) ||                                  /* "-" */
                (cbyte == 0x2E) ||                                  /* "." */
                (cbyte == 0x5F) ) {                                 /* "_" */ /* 無変換文字だった場合 */
            strncpy((char*)cbuffer + lresultcount, (char*)csource + lcount, 2);
            lcount++;
            lresultcount++;
        } else {                                                    /* その他の文字の場合 */
            sprintf((char*)ctemp, "%%%02X", cbyte);                        /* URL エンコード */
            strncpy((char*)cbuffer + lresultcount, (char*)ctemp, 4);
            lcount++;
            lresultcount += 3;
        }
        if(lcount == llength) { break; }
    }
    d("encoded: %s\n", cbuffer);
    return lresultcount;                                            /* cbuffer に書き込んだ文字列のサイズを返す */
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

static unsigned char *base64 = (unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void enclode_char(unsigned long bb, int srclen, unsigned char *dist, int j)
{
    int x, i, base;

    for (i = srclen; i < 2; i++) 
        bb <<= 8;
    for (base = 18, x = 0; x < srclen + 2; x++, base -= 6) {
        dist[j++] = base64[(unsigned long)((bb>>base) & 0x3F)];
    }
    for (i = x; i < 4; i++) {
        dist[j++] = (unsigned char)'=';
    }
}

void base64_encode(const unsigned char *src, unsigned char *dist)
{
    unsigned char *p = (unsigned char*)src;
    unsigned long bb = (unsigned long)0;
    int i = 0, j = 0;

    while (*p) {
        bb <<= 8;
        bb |= (unsigned long)*p;

        if (i == 2) {
            enclode_char(bb, i, dist, j);
            j = j + 4;
            i = 0;
            bb = 0;
        } else {
            i++;
        }
        p++;
    }
    if (i) enclode_char(bb, i - 1, dist, j);
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
    sprintf(redirecturi, "%s/%s%s", cgiScriptName, g_project_name, uri);
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
    sprintf(redirecturi, "%s/%s%s", cgiScriptName, g_project_name, uri);
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
    iconv_t ic;
    size_t src_size, dist_size, ret_len;
    char* dist_a;
    char* dist_p;
    bool has_error = false;

    src_size = strlen(src) + 1;
    dist_size = src_size;  /* UTF-8からCP932なので、長さが短かくなることはない。そのためdist_sizeにも同じ長さを指定する。*/
    dist_p = dist_a = xalloc(sizeof(char) * dist_size);
    /* 文字コード変換処理 *          */
    ic = iconv_open("CP932", "UTF-8");
    ret_len = iconv(ic, &src, &src_size, &dist_p, &dist_size);
    if (ret_len == (size_t)-1) {
        d("iconv failed: %s", strerror(errno));
        has_error = true;
    }
    o("\"");
    if (!has_error) csv_escape(dist_a, strlen(dist_a));
    o("\"");
    iconv_close(ic);
    xfree(dist_a);
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
    int is_ssl = strcmp(cgiServerProtocol, "HTTPS") == 0 ? 1 : 0;
    int is_default_port_no = 
        (is_ssl && strcmp(cgiServerPort, "443") == 0) || (is_ssl == 0 && strcmp(cgiServerPort, "80") == 0);
    string_appendf(buf, "http%s://%s%s%s%s",
            is_ssl ? "s" : "",
            is_default_port_no ? "" : ":",
            is_default_port_no ? "" : cgiServerPort,
            cgiServerName,
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
typedef union {
    unsigned int    data;
    unsigned char   xyz[4];
} Union_data_t;

static long codetovalue(unsigned char c)
{
    if( (c >= (unsigned char)'A') && (c <= (unsigned char)'Z') ) {
        return (long)(c - (unsigned char)'A');
    }
    else if( (c >= (unsigned char)'a') && (c <= (unsigned char)'z') ) {
        return ((long)(c - (unsigned char)'a') +26);
    }
    else if( (c >= (unsigned char)'0') && (c <= (unsigned char)'9') ) {
        return ((long)(c - (unsigned char)'0') +52);
    }
    else if( (unsigned char)'+' == c ) {
        return (long)62;
    }
    else if( (unsigned char)'/' == c ) {
        return (long)63;
    }
    else if( (unsigned char)'=' == c ) {
        return (long)0;
    }
    else {
        return -1;
    }
}

static int decode_str(int enc_ptr, const unsigned char *src, unsigned char *dest)
{
    int i, j;
    unsigned long base64 = 0;
    unsigned char x;
    Union_data_t bb;

    for (i = enc_ptr; i < enc_ptr + 4; i++) {
        if (src[i] == '\0') return -1; /* '\0'が出現したら終了。 */
        x = codetovalue(src[i]);
        base64 |= x;
        if ((i - enc_ptr) != 3) base64 <<= 6;
    }
    base64 <<= 8;
    bb.data = base64;

    for(j = 0, i = 3; i >= 1; i--) {
        dest[j++] = bb.xyz[i];
    }
    return 0;
}

int base64_decode(const unsigned char *src, unsigned char *dest)
{
    int i = 0;
    int srclen;
    char tmp[4];

    /* NULL pointer */
    if (src == NULL) return -1;

    srclen = strlen((char*)src);

    if (!srclen % 4) return -2;

    while(srclen) {
        memset(tmp, 0, sizeof(tmp));
        if (decode_str(i, src, (unsigned char*)tmp) != 0)
            return -1; /* 失敗した場合は、終了 */
        strcat((char*)dest, tmp);
        i += 4;
        srclen -= 4;
    }

    return 0;
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
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
