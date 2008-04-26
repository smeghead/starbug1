#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include "csv.h"
#include "list.h"
#include "util.h"
#include "simple_string.h"

#define MAX_WIDTH 1000

typedef enum _mode {
    CSV_MODE_SEPARATER,
    CSV_MODE_DATA,
    CSV_MODE_QUOTED_DATA
} CsvMode;

#define is_space(c) (c == ' ' || c == '\t')
static void decord_csv_field(char* src, char* dist)
{
    int quote = 0;
    char* p = src;
    char* p_dist = dist;
    /* 前のスペースを削除する。 */
    while (*p != '\0') {
        if (!(strlen(dist) == 0 && is_space(*p))) { /* 最初の空白は追加しない */
            if (strlen(dist) > 0 || *p != '"') { /* 最初の" は追加しない。 */
                if (strlen(dist) == 0 || !(*p == '"' && *(p - 1) == '"')) { /* 重なった " は追加しない */ 
                    *p_dist++ = *p;
                }
            } else if (strlen(dist) == 0 && *p == '"') {
                quote = 1; /* " モード */
            }
        }
        p++;
    }
    p_dist--;
    /* 後のスペースを削除する。 */
    while (is_space(*p_dist)) {
        *p_dist-- = '\0';
    }
    /* quoteモードで最後が " なら削除 */
    d("qote %d\n", quote);
    if (quote && *p_dist == '"') {
        *p_dist = '\0';
    }
}
Csv* csv_new(char* content)
{
    CsvMode mode = CSV_MODE_DATA;
    char* p = content;
    char* mark;
    CsvLine* line;
    int field_count = 0;
    Csv* csv = xalloc(sizeof(Csv));
    /* create first line. */
    list_alloc(csv->lines, CsvLine);
    line = list_new_element(csv->lines);
    list_alloc(line->fields, CsvField);
    mark = p;
    d("csv_new begin\n");
    while (1) {
        int data_end = 0;
        d("mode: %d c: %c %c\n", mode, *p, *(p + 1));
        if (mode == CSV_MODE_QUOTED_DATA) {
            if (*p == '"' && *(p + 1) == '"') {
                /* escaped quote */
                p++;
            } else if (*p == '"') {
                /* quote mode ends */
                mode = CSV_MODE_DATA;
            }
        } else if (mode == CSV_MODE_DATA && *p == '"') {
            mode = CSV_MODE_QUOTED_DATA;
        }
        if (mode == CSV_MODE_DATA) {
            data_end = (*p == '\0' || *p == '\n' || *p == ',');
            d("csv_new %d %d\n", *p, p - mark);
            if (data_end) {
                CsvField* field = list_new_element(line->fields);
                char buf[p - mark + 1];
                char buf_decorded[p - mark + 1];
                memset(buf, '\0', p - mark + 1);
                memset(buf_decorded, '\0', p - mark + 1);
                field->data = string_new(0);
                strncpy(buf, mark, p - mark);
                decord_csv_field(buf, buf_decorded);
                d("col: %s\n", buf);
                string_append(field->data, buf_decorded);
                list_add(line->fields, field);
                field_count++;
                if (csv->field_count < field_count)
                    csv->field_count = field_count;
            }
        }
        if (*p == '\n') {
            d("next line!!!!\n");
            list_add(csv->lines, line);
            csv->line_count++;
            field_count = 0;
            line = list_new_element(csv->lines);
            list_alloc(line->fields, CsvField); /* line を初期化 */
        } else if (*p == '\0') {
            list_add(csv->lines, line);
            csv->line_count++;
            break;
        }
        if (data_end) {
            mark = p + 1; /* 次のフィールドの開始点をマークする。 */
        }
        p++;
    }
    d("csv_new end\n");
    return csv;
}
void csv_free(Csv* csv)
{
    Iterator* it;
    foreach (it, csv->lines) {
        CsvLine* line = it->element;
        Iterator* it_field;
        foreach (it_field, line->fields) {
            CsvField* field = it_field->element;
            string_free(field->data);
        }
        xfree(line);
    }
    xfree(csv);
}

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
