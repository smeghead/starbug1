#ifndef DBUTIL_H
#define DBUTIL_H
/*
 * database access utility functions.
 */
#include <stdio.h>
#include <sqlite3.h>
#include <stdarg.h>
#include "util.h"

#define ERROR_LABEL(db) error: \
    d("ERR: %s\n", sqlite3_errmsg(db)); \
    sqlite3_finalize(stmt); \
    sqlite3_close(db); \
    die("failed to dbaccess.");

typedef struct {
    char name[DEFAULT_LENGTH];
    sqlite3* handle;
} Database;

Database* db_init(char*);
void db_finish(Database*);
void db_begin(Database*);
void db_rollback(Database*);
void db_commit(Database*);

int exec_query(Database*, const char* sql,...);
int exec_query_scalar_int(Database*, const char* sql, ...);
int exec_and_wait_4_done(sqlite3_stmt*);

#define INVALID_INT -99999

enum COLUMN_TYPE {
    COLUMN_TYPE_INT,
    COLUMN_TYPE_TEXT,
    COLUMN_TYPE_BLOB_ELEMENT_FILE,
    COLUMN_TYPE_BLOB_SETTING_FILE,
    COLUMN_TYPE_END
};
enum SQLITE_RETURN {
    SQLITE_RETURN_OK,
    SQLITE_RETURN_ERROR
};
List* parse_keywords(List*, char*);
String* create_columns_like_exp(List*, char*, List*, String*);
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
