#ifndef SIMPLE_STRING_H
#define SIMPLE_STRING_H

#include <string.h>

typedef struct _String {
    char* raw_chars;
    size_t block_size;
    size_t buf_size;
    size_t current_size;
} String;

String* string_new(size_t);
void string_append(String*, char*);
size_t string_len(String*);
char* string_rawstr(String*);
void string_free(String*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */