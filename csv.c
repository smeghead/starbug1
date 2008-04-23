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
Csv* csv_new(char* content)
{
    CsvMode mode = CSV_MODE_SEPARATER;
    char* p = content;
    char* mark;
    CsvLine* line;
    Csv* csv = xalloc(sizeof(Csv));
    /* create first line. */
    list_alloc(csv->lines, CsvLine);
    line = list_new_element(csv->lines);
    list_alloc(line->fields, CsvField);
    mark = p;
    d("csv_new begin\n");
    while (1) {
        int data_end;
        d("mode: %d\n", mode);
        if (mode == CSV_MODE_QUOTED_DATA) {
            if (*p == '"' && *(p + 1) == '"') {
                p++;
            } else if (*p == '"') {
                mode = CSV_MODE_DATA;
            }
        } else if (*p == '"') {
            mode = CSV_MODE_QUOTED_DATA;
        }
        if (mode == CSV_MODE_DATA) {
            data_end = (mode == CSV_MODE_DATA) && (*p == '\0' || *p == '\n' || *p == ',');
            d("csv_new %d %d\n", *p, p - mark);
            if (data_end) {
                CsvField* field = list_new_element(line->fields);
                field->data = string_new(0);
                char buf[p - mark + 1];
                memset(buf, '\0', sizeof(buf));
                strncpy(buf, mark, p - mark);
                d("col: %s\n", buf);
                string_append(field->data, buf);
                list_add(line->fields, field);
                csv->field_count++;
            }
            if (*p == '\n') {
                d("next line!!!!\n");
                list_add(csv->lines, line);
                csv->line_count++;
                csv->field_count= 0;
                line = list_new_element(csv->lines);
                list_alloc(line->fields, CsvField); /* line を初期化 */
            } else if (*p == '\0') {
                list_add(csv->lines, line);
                csv->line_count++;
                break;
            }
            if (data_end) {
                mark = ++p; /* , の分進める。 */
            }
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
