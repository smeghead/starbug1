#ifndef SIMPLE_STRING_H
#define SIMPLE_STRING_H
/*
 * 単純な文字列操作クラス
 */
#include <string.h>
#include <stdarg.h>

typedef struct _String {
    char* raw_chars;
    size_t block_size;
    size_t buf_size;
    size_t current_size;
} String;

String* string_new();
String* string_new_by_block_size(size_t);
void string_append(String*, const char*);
size_t string_len(String*);
char* string_rawstr(String*);
void string_free(String*);
void string_replace(String*, char, char*);
void string_appendf(String*, char*, ...);
void string_set(String*, const char*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
