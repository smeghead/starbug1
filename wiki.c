#include <stdio.h>
#include <stdlib.h>
#include "wiki.h"
#include "util.h"

#define MAX_WIDTH 10000

void buf_clear();
void buf_add(int, char*);
void buf_flush();
void text_out(char*);
void element_out_without_content(char*);
void element_out(char*, char*);

/* char pre[100][MAX_WIDTH]; */
/* void pre_clear() { */
/*     int i; */
/*     for (i = 0; i < 100; i++) { */
/*         strcpy(pre[i], ""); */
/*     } */
/* } */
/* void pre_add(char* str) */
/* { */
/*     int i; */
/*     for (i = 0; i < 100; i++) { */
/*         if (strcmp(pre[i], "") == 0) { */
/*             strcpy(pre[i], str); */
/*             break; */
/*         } */
/*     } */
/* } */
/* void pre_flush() */
/* { */
/*     int i; */
/*     if (strcmp(pre[0], "") == 0) return; */
/*     printf("<pre>\n"); */
/*     for (i = 0; i < 100; i++) { */
/*         printf("%s", pre[i]); */
/*     } */
/*     printf("</pre>\n"); */
/*     pre_clear(); */
/* } */
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
        printf("%s", text[i]);
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
    printf("%s", str);
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
void wiki_out(char* filename)
{
    FILE* in;
    char line[MAX_WIDTH];

    if ((in  = fopen(filename, "r")) == NULL)
        die("file open error.");
    buf_clear();
    while (fgets(line, sizeof(line), in)) {
        if (strncmp(line, "**", strlen("**")) == 0) {
            buf_flush();
            element_out("h3", line + strlen("**"));
        } else if (strncmp(line, "*", strlen("*")) == 0) {
            buf_flush();
            element_out("h2", line + strlen("*"));
        } else if (strncmp(line, "----", strlen("----")) == 0) {
            buf_flush();
            element_out_without_content("hr");
        } else if (strncmp(line, "-", strlen("-")) == 0) {
            buf_add(TYPE_LI, line + strlen("-"));
        } else if (strncmp(line, " ", strlen(" ")) == 0) {
            buf_add(TYPE_PRE, line);
        } else if (strncmp(line, "\n", strlen("\n")) == 0) {
            buf_flush();
        } else {
            buf_add(TYPE_TEXT, line);
        }
    }
    buf_flush();
    fclose(in);
}
void wiki_content_out(char* filename)
{
    FILE* in;
    char line[MAX_WIDTH];

    if ((in  = fopen(filename, "r")) == NULL)
        die("file open error.");
    while (fgets(line, sizeof(line), in)) {
        h(line);
    }
    fclose(in);
}
void wiki_save(char* filename, char* content)
{
    FILE* out;

    /* TODO ロック処理 */
    if ((out  = fopen(filename, "w")) == NULL)
        die("file open error.");
    fputs(content, out);
    fclose(out);
}

