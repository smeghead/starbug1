#ifndef CSV_H
#define CSV_H

#include "list.h"

typedef struct _csv {
    int row_count;
    int col_count;
    List* lines;
} Csv;
Csv* csv_new(char*);
void csv_free(Csv*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
