#include <stdio.h>
#include <stdlib.h>
#include "simple_string.h"

#define STRING_DEFAULT_SIZE 10

static void* xalloc(size_t size)
{
    void* p = calloc(1, size);
    if (!p) {
        fprintf(stderr, "memory error.");
        exit(-1);
    }
    return p;
}
static void xfree(void* p)
{
    free(p);
}

String* string_new(size_t pool_size)
{
    String* str = xalloc(sizeof(String));
    if (pool_size == 0) {
        str->pool_size = STRING_DEFAULT_SIZE;
    } else {
        str->pool_size = pool_size;
    }
    str->buf_size = str->pool_size;
    str->raw_chars = xalloc(sizeof(char) * str->pool_size);
    strcpy(str->raw_chars, "");
    return str;
}
void string_append(String* str, char* addstr)
{
    size_t len = strlen(addstr);
    if (str->current_size + len - 1 > str->buf_size) {
        int new_size = str->buf_size + str->pool_size * (len / str->pool_size + 1);
        str->raw_chars = realloc(str->raw_chars, new_size);
        if (!str->raw_chars) {
            fprintf(stderr, "memory error.");
            exit(-1);
        }
        str->buf_size = new_size;
    }
    strcat(str->raw_chars, addstr);
    str->current_size += len;
    printf("%s (size: %d)\n", str->raw_chars, str->buf_size);
}
char* string_rawstr(String* str)
{
    return str->raw_chars;
}
void string_free(String* str)
{
    if (str->raw_chars)
        xfree(str->raw_chars);
    xfree(str);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
