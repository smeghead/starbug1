#ifndef DBUTIL_H
#define DBUTIL_H
#include <stdio.h>
#include <sqlite3.h>
#include <stdarg.h>

void db_init();
void db_finish();
void db_begin();
void db_rollback();
void db_commit();

int exec_query(const char* sql,...);
int exec_query_scalar_int(const char* sql, ...);

#define INVALID_INT -99999

enum COLUMN_TYPE {
    COLUMN_TYPE_INT,
    COLUMN_TYPE_TEXT,
    COLUMN_TYPE_BLOB,
    COLUMN_TYPE_END
};
#endif
