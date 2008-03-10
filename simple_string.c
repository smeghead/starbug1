#include <stdio.h>
#include <stdlib.h>
#include "simple_string.h"

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

String* string_new(size_t buf_size)
{
    String* str = xalloc(sizeof(String));
    str->buf_size = buf_size;
    str->raw_chars = xalloc(sizeof(char) * buf_size);
    strcpy(str->raw_chars, "");
    return str;
}
void string_append(String* str, char* append_str)
{
    size_t len = strlen(append_str);
/*     printf("%s (size: %d) STRING_DEFAULT_SIZE:%d\n", str->raw_chars, str->buf_size, STRING_DEFAULT_SIZE); */
    if (str->current_size + len - 1 > str->buf_size) {
        int new_size = str->buf_size + STRING_DEFAULT_SIZE * (len / STRING_DEFAULT_SIZE + 1);
        str->raw_chars = realloc(str->raw_chars, new_size);
        if (!str->raw_chars) {
            fprintf(stderr, "memory error.");
            exit(-1);
        }
        str->buf_size = new_size;
    }
    strcat(str->raw_chars, append_str);
    str->current_size += len;
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
