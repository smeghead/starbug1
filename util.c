#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <cgic.h>
#include <iconv.h>
#include "util.h"
#include "hook.h"

unsigned long url_encode(unsigned char*, unsigned char*, unsigned long);
static action* get_actions();

action* actions = NULL;

void* xalloc(size_t size)
{
    void* p;
    p = calloc(1, size);
    if (!p) {
        d("memory error!!");
        die("memory error.");
    }
    return p;
}
void xfree(void* p)
{
    free(p);
}
static action* get_actions()
{
    return actions;
}
void register_action_actions(char* action_name, void func(void))
{
    action* a;
    if (actions == NULL) {
        actions = xalloc(sizeof(action));
        actions->action_name = "__HEAD__";
        actions->next = NULL;
    }
    for (a = actions; a->next != NULL; a = a->next);
    a->next = xalloc(sizeof(action));
    a = a->next;
    a->action_name = action_name;
    a->action_func = func;
    a->next = NULL;
}

void exec_action()
{
    char* index;
    char* path_info = cgiPathInfo;
    char action_name[1024];
    action* a;
    if (strlen(path_info) > 1) {
        strncpy(action_name, path_info + 1, 1024);
    } else {
        strcpy(action_name, "");
    }
    if ((index = strchr(action_name, '/'))) {
        *index = '\0';
    }

    for (a = get_actions(); a != NULL; a = a->next) {
        if (!strcmp(action_name, a->action_name)) {
            d("exec_action start: %s\n", a->action_name);
            a->action_func();
            d("exec_action end  : %s\n", a->action_name);
            return;
        }
    }
    if (strlen(action_name) != 0) {
        /* 知らないactionが指定されたら、リダイレクトする。  */
        redirect("", NULL);
    } else {
        /* path_infoが空なら、top_actionを呼び出す。 */
        for (a = get_actions(); a != NULL; a = a->next) {
            if (!strcmp("top", a->action_name)) {
                a->action_func();
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
static cgiFormResultType cgiHtmlEscapeDataMultiLine(char *data, int len)
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
        } else if (*data == '\n') {
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
    url_encode(str, buf, DEFAULT_LENGTH);
    fprintf(cgiOut, "%s", buf);
}
unsigned long url_encode(unsigned char* csource, unsigned char* cbuffer, unsigned long lbuffersize)
{
    unsigned long   llength;                                        /* csource のサイズを格納 */
    unsigned long   lcount = 0;                                     /* csource の読み込み位置カウンタ */
    unsigned char   cbyte;                                          /* 抜き出された 1 バイト分のデータを格納 */
    unsigned char   ctemp[4];                                       /* 変換結果(1 文字分)一時格納バッファ */
    unsigned long   lresultcount = 0;                               /* cbuffer の書き込み位置カウンタ */

    llength = (unsigned long)strlen(csource);                       /* csource の文字サイズを得る */
    if(!llength) { return lresultcount; }                           /* csource が 0 文字の場合、関数を抜ける */
    if(lbuffersize < (llength * 3 + 1)) { return lresultcount; }    /* バッファサイズが足りない場合、関数を抜ける */

    while(1) {
        cbyte = *(csource + lcount);                                /* 1 バイトを抜き出す */
        if( ((cbyte >= 0x81) && (cbyte <= 0x9F)) ||
                ((cbyte >= 0xE0) && (cbyte <= 0xEF)) ) {                /* Shift-JIS 2 バイト文字だった場合 */
            sprintf(ctemp, "%%%02X", cbyte);                        /* URL エンコード(上位バイト) */
            strncpy(cbuffer + lresultcount, ctemp, 4);              /* cbuffer にコピー */
            lcount++;                                               /* 読み込みカウンタをインクリメント */
            lresultcount += 3;                                      /* 書き込みカウンタを 3 増やす */
            if(lcount == llength) { break; }                        /* 文字列の終端に達した場合、ループを抜ける */
            sprintf(ctemp, "%%%02X", *(csource + lcount));          /* URL エンコード(下位バイト) */
            strncpy(cbuffer + lresultcount, ctemp, 4);              /* cbuffer にコピー */
            lcount++;                                               /* 読み込みカウンタをインクリメント */
            lresultcount += 3;                                      /* 書き込みカウンタを 3 増やす */
        } else if(cbyte == 0x20) {                                  /* 1 バイト半角スペース(" ")だった場合 */
            strncpy(cbuffer + lresultcount, "+", 2);                /* "+" を cbuffer にコピー */
            lcount++;                                               /* 読み込みカウンタをインクリメント */
            lresultcount++;                                         /* 書き込みカウンタをインクリメント */
        } else if( ((cbyte >= 0x40) && (cbyte <= 0x5A)) ||          /* @A-Z */
                ((cbyte >= 0x61) && (cbyte <= 0x7A)) ||          /* a-z */
                ((cbyte >= 0x30) && (cbyte <= 0x39)) ||          /* 0-9 */
                (cbyte == 0x2A) ||                               /* "*" */
                (cbyte == 0x2D) ||                               /* "-" */
                (cbyte == 0x2E) ||                               /* "." */
                (cbyte == 0x5F) ) {                              /* "_" */ /* 無変換文字だった場合 */
            strncpy(cbuffer + lresultcount, csource + lcount, 2);   /* そのまま cbuffer にコピー */
            lcount++;                                               /* 読み込みカウンタをインクリメント */
            lresultcount++;                                         /* 書き込みカウンタをインクリメント */
        } else {                                                    /* その他の文字の場合 */
            sprintf(ctemp, "%%%02X", cbyte);                        /* URL エンコード */
            strncpy(cbuffer + lresultcount, ctemp, 4);              /* cbuffer にコピー */
            lcount++;                                               /* 読み込みカウンタをインクリメント */
            lresultcount += 3;                                      /* 書き込みカウンタを 3 増やす */
        }
        if(lcount == llength) { break; }                            /* 文字列の終端に達した場合、ループを抜ける */
    }
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
char* get_upload_filename(int element_id, char* buf)
{
    char name[DEFAULT_LENGTH];
    sprintf(name, "field%d", element_id);
    cgiFormFileName(name, buf, DEFAULT_LENGTH);
    return buf;
}
int get_upload_size(int element_id)
{
    char name[DEFAULT_LENGTH];
    int size;
    sprintf(name, "field%d", element_id);
    cgiFormFileSize(name, &size);
    return size;
}
char* get_upload_content_type(int element_id, char* buf)
{
    char name[DEFAULT_LENGTH];
    sprintf(name, "field%d", element_id);
    cgiFormFileContentType(name, buf, DEFAULT_LENGTH);
    return buf;
}
ElementFile* get_upload_content(int element_id)
{
    int got = 0;
    char* buffer_org;
    char* buffer;
    char b[DEFAULT_LENGTH];
    char name[DEFAULT_LENGTH];
    cgiFilePtr file;
    ElementFile* content = xalloc(sizeof(ElementFile));
    content->size = get_upload_size(element_id);
    buffer = buffer_org = xalloc(sizeof(char) * content->size);
    sprintf(name, "field%d", element_id);
    if (cgiFormFileOpen(name, &file) != cgiFormSuccess) {
        die("Could not open the file.");
    }

    while (cgiFormFileRead(file, b, sizeof(b), &got) == cgiFormSuccess) {
        char* p = b;
        int i;
        for (i = 0; i < got; i++) {
            *buffer = *p;
            buffer++;
            p++;
        }
    }
    content->blob = buffer_org;
    cgiFormFileClose(file);
    return content;
}

static unsigned char *base64 = (unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void enclode_char(unsigned long bb, int srclen, unsigned char *dest, int j)
{
    int x, i, base;

    for (i = srclen; i < 2; i++) 
        bb <<= 8;
    for (base = 18, x = 0; x < srclen + 2; x++, base -= 6) {
        dest[j++] = base64[(unsigned long)((bb>>base) & 0x3F)];
    }
    for (i = x; i < 4; i++) {
        dest[j++] = (unsigned char)'=';
    }
}

void base64_encode(const unsigned char *src, unsigned char *dest)
{
    unsigned char *p = (char *)src;
    unsigned long bb = (unsigned long)0;
    int i = 0, j = 0;

    while (*p) {
        bb <<= 8;
        bb |= (unsigned long)*p;

        if (i == 2) {
            enclode_char(bb, i, dest, j);
            j = j + 4;
            i = 0;
            bb = 0;
        } else {
            i++;
        }
        p++;
    }
    if (i) enclode_char(bb, i - 1, dest, j);
}
void redirect(char* path, char* message)
{
    char redirecturi[DEFAULT_LENGTH];
    char uri[DEFAULT_LENGTH];
    char param[DEFAULT_LENGTH];
    char parambuf[DEFAULT_LENGTH];

    strcpy(uri, path);
    if (message) {
        strcpy(param, message);
        url_encode(param, parambuf, DEFAULT_LENGTH);
        strcat(uri, "?message=");
        strcat(uri, parambuf);
    }
    sprintf(redirecturi, "%s%s", cgiScriptName, uri);
    o("Status: 302 Temporary Redirection\r\n");
    cgiHeaderLocation(redirecturi);
}
void redirect_with_hook_messages(char* path, char* message, List* results)
{
    char redirecturi[DEFAULT_LENGTH];
    char uri[DEFAULT_LENGTH];
    char param[DEFAULT_LENGTH];
    char parambuf[DEFAULT_LENGTH];
    Iterator* it;

    strcpy(uri, path);
    if (message) {
        strcpy(param, message);
        url_encode(param, parambuf, DEFAULT_LENGTH);
        strcat(uri, "?message=");
        strcat(uri, parambuf);
    }
    if (results) {
        int i = 0;
        foreach (it, results) {
            HOOK_RESULT* result = it->element;
            strcpy(param, result->message);
            url_encode(param, parambuf, DEFAULT_LENGTH);
            if (!message && i == 0)
                strcat(uri, "?");
            else
                strcat(uri, "&");
            strcat(uri, "message=");
            strcat(uri, parambuf);
            i++;
        }
    }
    sprintf(redirecturi, "%s%s", cgiScriptName, uri);
    o("Status: 302 Temporary Redirection\r\n");
    cgiHeaderLocation(redirecturi);
}
void free_element_list(List* elements)
{
    Iterator* it;
    foreach (it, elements) {
        Element* e = it->element;
        xfree(e->str_val);
    }
    list_free(elements);
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
    char dest_buf[DEFAULT_LENGTH];
    size_t src_size, dest_size, ret_len;
    char* dest = dest_buf;

    src_size = strlen(src) + 1;
    dest_size = src_size;  /* UTF-8からCP932なので、長さが短かくなることはない。そのためdest_sizeにも同じ長さを指定する。*/
    /* 文字コード変換処理 *          */
    ic = iconv_open("CP932", "UTF-8");
    ret_len = iconv(ic, &src, &src_size, &dest, &dest_size);
    o("\"");
    if (ret_len >= 0) csv_escape(dest_buf, strlen(dest_buf));
    o("\"");
    iconv_close(ic);
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
    char dest[DEFAULT_LENGTH];
    char* p = src;

    strcpy(src, str);
    if (strlen(src) > 100) {
        p += 100;
        p = '\0';
    }
        
    memset(dest, 0, DEFAULT_LENGTH);
    base64_encode(src, dest);
    cgiCssClassName(dest, strlen(dest));
}
/* vim: set ts=4 sw=4 sts=4 expandtab: */
