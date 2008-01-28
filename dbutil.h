#ifndef DBUTIL_H
#define DBUTIL_H
#include <stdio.h>
#include <sqlite3.h>
#include <stdarg.h>

#define ERROR_LABEL error: \
    d("ERR: %s\n", sqlite3_errmsg(db)); \
    sqlite3_finalize(stmt); \
    sqlite3_close(db); \
    die("failed to dbaccess.");

void db_init();
void db_finish();
void db_begin();
void db_rollback();
void db_commit();

int exec_query(const char* sql,...);
int exec_query_scalar_int(const char* sql, ...);
int exec_and_wait_4_done(sqlite3_stmt*);

#define INVALID_INT -99999

enum COLUMN_TYPE {
    COLUMN_TYPE_INT,
    COLUMN_TYPE_TEXT,
    COLUMN_TYPE_BLOB,
    COLUMN_TYPE_END
};
enum SQLITE_RETURN {
    SQLITE_RETURN_OK,
    SQLITE_RETURN_ERROR
};
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab: */
