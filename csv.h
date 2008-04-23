#ifndef CSV_H
#define CSV_H

#include "list.h"
#include "simple_string.h"

typedef struct _csv {
    int line_count;
    int field_count;
    List* lines;
} Csv;
typedef struct _csv_line {
    List* fields;
} CsvLine;
typedef struct _csv_field {
    String* data;
} CsvField;
Csv* csv_new(char*);
void csv_free(Csv*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
