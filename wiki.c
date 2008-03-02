#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include "wiki.h"
#include "util.h"
#include "db.h"

#define MAX_WIDTH 1000

void buf_clear();
void buf_add(int, char*);
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
void buf_add(int type, char* str)
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
        h(text[i]);
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
enum LINE_MODE {
    LINE_MODE_NORMAL,
    LINE_MODE_PRE
};
void wiki_out(char* page_name)
{
    Wiki* wiki_a = xalloc(sizeof(Wiki));
    char line[MAX_WIDTH];
    char* c;
    char* p;
    int mode = LINE_MODE_NORMAL;

    wiki_a = db_get_newest_wiki(page_name, wiki_a);
    p = wiki_a->content;
    buf_clear();
    while ((strlen(p) != 0)) {
        if ((c = strchr(p, '\n')) != NULL) {
            int len = c - p;
            strncpy(line, p, len);
            line[len] = '\0';
            p += len + 1;
        } else {
            strcpy(line, p);
            p += strlen(p);
        }
        if (mode == LINE_MODE_PRE && strncmp(line, "|<", strlen("|<")) != 0) {
            /* LINE_MODE_PRE中は、成形済ブロック */
            buf_add(TYPE_PRE, line);
        } else if (mode == LINE_MODE_NORMAL && strncmp(line, ">|", strlen(">|")) == 0) {
            /* 成形済ブロック終了 */
            buf_flush();
            mode = LINE_MODE_PRE;
        } else if (mode == LINE_MODE_PRE && strncmp(line, "|<", strlen("|<")) == 0) {
            /* 成形済ブロック開始 */
            buf_flush();
            mode = LINE_MODE_NORMAL;
        } else if (strncmp(line, "****", strlen("****")) == 0) {
            buf_flush();
            element_out("h6", line + strlen("****"));
        } else if (strncmp(line, "***", strlen("***")) == 0) {
            buf_flush();
            element_out("h5", line + strlen("***"));
        } else if (strncmp(line, "**", strlen("**")) == 0) {
            buf_flush();
            element_out("h4", line + strlen("**"));
        } else if (strncmp(line, "*", strlen("*")) == 0) {
            buf_flush();
            element_out("h3", line + strlen("*"));
        } else if (strncmp(line, "----", strlen("----")) == 0) {
            buf_flush();
            element_out_without_content("hr");
        } else if (strncmp(line, "-", strlen("-")) == 0) {
            buf_add(TYPE_LI, line + strlen("-"));
/*         } else if (strncmp(line, " ", strlen(" ")) == 0) { */
/*             buf_add(TYPE_PRE, line); */
        } else if (strncmp(line, "\n", strlen("\n")) == 0) {
            buf_flush();
        } else {
            buf_add(TYPE_TEXT, line);
        }
    }
    buf_flush();
    xfree(wiki_a->content);
    xfree(wiki_a);
}
void wiki_content_out(char* page_name)
{
    Wiki* wiki_a = xalloc(sizeof(Wiki));
    wiki_a = db_get_newest_wiki(page_name, wiki_a);
    h(wiki_a->content);
    xfree(wiki_a->content);
    xfree(wiki_a);
}
void wiki_save(char* page_name, char* content)
{
    Wiki* wiki_a = xalloc(sizeof(Wiki));
    wiki_a->content = xalloc(sizeof(char) * strlen(content) + 1);
    strcpy(wiki_a->name, page_name);
    strcpy(wiki_a->content, content);
    db_register_wiki(wiki_a);
    xfree(wiki_a->content);
    xfree(wiki_a);
}
/* vim: set ts=4 sw=4 sts=4 expandtab: */
