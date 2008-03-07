#ifndef STRING_H
#define STRING_H

#include <string.h>

#define STRING_DEFAULT_SIZE 10

typedef struct _String {
    char* raw_chars;
    size_t buf_size;
    size_t current_size;
} String;

String* string_new(size_t);
void string_add(String*, char*);
char* string_rawstr(String*);
void string_free(String*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
