#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <cgic.h>
#include "util.h"

unsigned long url_encode(unsigned char*, unsigned char*, unsigned long);
static action* get_actions();

action* actions = NULL;

static action* get_actions()
{
    return actions;
}
void register_action_actions(char* action_name, void func(void))
{
    action* a;
    if (actions == NULL) {
        actions = (action*)xalloc(sizeof(action));
        actions->action_name = "__HEAD__";
        actions->next = NULL;
    }
    for (a = actions; a->next != NULL; a = a->next);
    a->next = (action*)xalloc(sizeof(action));
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
    strncpy(action_name, path_info + 1, 1024);
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
    for (a = get_actions(); a != NULL; a = a->next) {
        if (!strcmp("default", a->action_name)) {
            a->action_func();
            return;
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

/* 失敗
#define list_add(target, type) \
    if (target == NULL) { \
        target = (type*)xalloc(sizeof(type)); \
    } else { \
        target->next = (type*)xalloc(sizeof(type)); \
        target->next-next = NULL; \
    }
*/
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

