#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include "csv.h"
#include "list.h"
#include "util.h"
#include "simple_string.h"

#define MAX_WIDTH 1000

Csv* csv_new(char* content)
{
    char* p = content;
    char* mark;
    List* cols;
    Csv* csv = xalloc(sizeof(Csv));
    /* create first line. */
    list_alloc(csv->lines, List);
    list_alloc(cols, String);
    mark = p;
    d("csv_new begin\n");
    while (1) {
        int data_end = *p == '\0' || *p == '\n' || *p == ',';
        d("csv_new %d %d\n", *p, p - mark);
        if (data_end) {
            String* col = string_new(0);
            char buf[p - mark + 1];
            strncpy(buf, mark, p - mark);
            d("col: %s\n", buf);
            string_append(col, buf);
            list_add(cols, col);
            csv->col_count++;
        }
        if (*p == '\n') {
            d("next line!!!!\n");
            list_add(csv->lines, cols);
            csv->row_count++;
            csv->col_count= 0;
            list_alloc(cols, String); /* cols を初期化 */
        } else if (*p == '\0') {
            list_add(csv->lines, cols);
            csv->row_count++;
            break;
        }
        if (data_end) {
            mark = ++p; /* , の分進める。 */
        }
        p++;
    }
    d("csv_new end\n");
    return csv;
}
void csv_free(Csv* csv)
{
}

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
