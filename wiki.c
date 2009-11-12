#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include "wiki.h"
#include "alloc.h"
#include "util.h"
#include "db_project.h"

#define MAX_WIDTH 1000

void buf_clear();
void buf_add(unsigned int, char*);
void buf_flush();
void text_out(char*);
void element_out_without_content(char*);
void element_out(char*, char*);

char text[100][MAX_WIDTH];
enum BUF_TYPE_ENUM {
    TYPE_TEXT,
    TYPE_PRE,
    TYPE_LI
} buf_type;

void buf_clear() {
    int i;
    for (i = 0; i < 100; i++) {
        strcpy(text[i], "");
    }
}
void buf_add(unsigned int type, char* str)
{
    int i;
    if (buf_type != type)
        buf_flush();
    buf_type = type;
    for (i = 0; i < 100; i++) {
        if (strcmp(text[i], "") == 0) {
            strcpy(text[i], str);
            break;
        }
    }
}
void buf_flush()
{
    int i;
    if (strcmp(text[0], "") == 0) return;
    switch (buf_type) {
        case TYPE_TEXT:
            printf("<p>\n");
            break;
        case TYPE_PRE:
            printf("<pre>\n");
            break;
        case TYPE_LI:
            printf("<ul>\n");
            break;
    }
    for (i = 0; i < 100; i++) {
        if (strcmp(text[i], "") == 0) break;
        if (buf_type == TYPE_LI)
            printf("<li>");
        hm(text[i]);
        o("\n");
        if (buf_type == TYPE_LI)
            printf("</li>");
    }
    switch (buf_type) {
        case TYPE_TEXT:
            printf("</p>\n");
            break;
        case TYPE_PRE:
            printf("</pre>\n");
            break;
        case TYPE_LI:
            printf("</ul>\n");
            break;
    }
    buf_clear();
}
void buf_out(char* str)
{
    h(str);
    o("\n");
}
void element_out_without_content(char* tag_name)
{
    buf_flush();
    printf("<%s />\n", tag_name);
}
void element_out(char* tag_name, char* content)
{
    printf("<%s>", tag_name);
    buf_out(content);
    printf("</%s>\n", tag_name);
}
typedef enum LINE_MODE {
    LINE_MODE_NORMAL,
    LINE_MODE_PRE
} LineMode;
void wiki_out(Database* db, char* page_name)
{
    Wiki* wiki_a = wiki_new();
    char* line_a;
    char* c;
    char* p;
    LineMode mode = LINE_MODE_NORMAL;

    wiki_a = db_get_newest_wiki(db, page_name, wiki_a);
    p = wiki_a->content;
    /* FIXME 非効率だけど、安全のために、1行分のバッファに全文の長さを確保する。 */
    line_a = xalloc(sizeof(char) * strlen(wiki_a->content));
    buf_clear();
    while ((strlen(p) != 0)) {
        if ((c = strchr(p, '\n')) != NULL) {
            int len = c - p;
            strncpy(line_a, p, len);
            line_a[len] = '\0';
            p += len + 1;
        } else {
            strcpy(line_a, p);
            p += strlen(p);
        }
        if (mode == LINE_MODE_PRE && strncmp(line_a, "|<", strlen("|<")) != 0) {
            /* LINE_MODE_PRE中は、成形済ブロック */
            buf_add(TYPE_PRE, line_a);
        } else if (mode == LINE_MODE_NORMAL && strncmp(line_a, ">|", strlen(">|")) == 0) {
            /* 成形済ブロック終了 */
            buf_flush();
            mode = LINE_MODE_PRE;
        } else if (mode == LINE_MODE_PRE && strncmp(line_a, "|<", strlen("|<")) == 0) {
            /* 成形済ブロック開始 */
            buf_flush();
            mode = LINE_MODE_NORMAL;
        } else if (strncmp(line_a, "****", strlen("****")) == 0) {
            buf_flush();
            element_out("h6", line_a + strlen("****"));
        } else if (strncmp(line_a, "***", strlen("***")) == 0) {
            buf_flush();
            element_out("h5", line_a + strlen("***"));
        } else if (strncmp(line_a, "**", strlen("**")) == 0) {
            buf_flush();
            element_out("h4", line_a + strlen("**"));
        } else if (strncmp(line_a, "*", strlen("*")) == 0) {
            buf_flush();
            element_out("h3", line_a + strlen("*"));
        } else if (strncmp(line_a, "----", strlen("----")) == 0) {
            buf_flush();
            element_out_without_content("hr");
        } else if (strncmp(line_a, "-", strlen("-")) == 0) {
            buf_add(TYPE_LI, line_a + strlen("-"));
        } else if (strncmp(line_a, "\n", strlen("\n")) == 0) {
            buf_flush();
        } else {
            buf_add(TYPE_TEXT, line_a);
        }
    }
    buf_flush();
    xfree(line_a);
    wiki_free(wiki_a);
}
void wiki_content_out(Database* db, char* page_name)
{
    Wiki* wiki_a = wiki_new();
    wiki_a = db_get_newest_wiki(db, page_name, wiki_a);
    h(wiki_a->content);
    wiki_free(wiki_a);
}
void wiki_save(Database* db, char* page_name, char* content)
{
    Wiki* wiki_a = wiki_new();
    wiki_a->content = xalloc(sizeof(char) * strlen(content) + 1);
    string_set(wiki_a->name, page_name);
    strcpy(wiki_a->content, content);
    db_register_wiki(db, wiki_a);
    wiki_free(wiki_a);
}
void print_wiki_help()
{
    o(      "<div>\n"
            "<h3>%s</h3>\n"
            "<ul>\n"
            "<li>%s</li>\n"
            "<li>%s</li>\n"
            "<li>%s</li>\n"
            "<li>%s</li>\n"
            "<li>%s</li>\n"
            "<li>%s</li>\n"
            "<li>%s</li>\n",
            _("simple wiki syntax"),
            _("a line starts by * become big headline."),
            _("a line starts by ** become medium headline."),
            _("a line starts by *** become small headline."),
            _("a line starts by **** become super small headline."),
            _("a line starts by - become item."),
            _("a line starts by ---- become delimitation."),
            _("from line starts by &gt;| to line starts by |&lt;, become block."));
    o(      "</ul>\n"
            "<h5>%s</h5>\n"
            "<pre>\n"
            "%s"
            "%s"
            "</pre>\n"
            "</div>\n"
            "</div>\n",
            _("example"),
            _("[wiki syntax example]"),
            _("[wiki syntax example code]"));
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
