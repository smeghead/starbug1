#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"
#include "util.h"
#include "simple_string.h"

#define STRING_DEFAULT_SIZE 1024

void string_init(String* str, size_t block_size)
{
    d("string_init\n");
    if (block_size == 0) {
        str->block_size = STRING_DEFAULT_SIZE;
    } else {
        str->block_size = block_size;
    }
    str->raw_chars = xalloc(sizeof(char) * str->block_size);
    str->buf_size = str->block_size;
    strcpy(str->raw_chars, "");
    d("string_init end\n");
}
String* string_new(size_t block_size)
{
    String* str;
/*     d("string size: %d\n", sizeof(String)); */
    str = xalloc(sizeof(String));
    string_init(str, block_size);
    return str;
}
void string_append(String* str, const char* addstr)
{
    size_t len = strlen(addstr);
    size_t size_needed = str->current_size + len + 1;
    if (size_needed > str->buf_size) {
        int new_size = str->block_size * (size_needed / str->block_size + 1);
        str->raw_chars = realloc(str->raw_chars, sizeof(char) * new_size);
        if (!str->raw_chars) {
            fprintf(stderr, "memory error.");
            exit(-1);
        }
        str->buf_size = new_size;
    }
    strcat(str->raw_chars, (char*)addstr);
    str->current_size += len;
/*     printf("%d (size: %d)\n", str->current_size, str->buf_size); */
}
void string_set(String* str, const char* newstr)
{
    /* 存在していた文字列は破棄する。 */
    d("string_set\n");
    d("string_set %s\n", string_rawstr(str));
    xfree(str->raw_chars);
    string_init(str, str->block_size);
    string_append(str, newstr);
    d("string_set end\n");
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
size_t string_len(String* str)
{
    return str->current_size;
}
void string_replace(String* str, char target_char, char* newchars)
{
    String* new_string_a = string_new(str->block_size);
    char* src = str->raw_chars;
    while (*src) {
        if (*src == target_char) {
            string_append(new_string_a, newchars);
        } else {
            char dist[2];
            sprintf(dist, "%c", *src);
            string_append(new_string_a, dist);
        }
        src++;
    }
    xfree(str->raw_chars);
    str->raw_chars = new_string_a->raw_chars;
    str->block_size = new_string_a->block_size;
    str->buf_size = new_string_a->buf_size;
    str->current_size = new_string_a->current_size;
    xfree(new_string_a);
}
void string_appendf(String* str, char* fmt, ...)
{
    va_list ap;
    int i;
    char cs[DEFAULT_LENGTH];
    char* s;
    char c;
    char char_prefix = '\0';

    va_start(ap, fmt);
    while (*fmt) {
        if (char_prefix == '%') {
            switch (*fmt) {
                case 's':              /* string */
                    s = va_arg(ap, char*);
                    string_append(str, s);
                    break;
                case 'd':              /* int */
                    i = va_arg(ap, int);
                    sprintf(cs, "%d", i);
                    string_append(str, cs);
                    break;
                case 'c':              /* char */
                    c = (char) va_arg(ap, int);
                    sprintf(cs, "%c", c);
                    string_append(str, cs);
                    break;
                default:
                    sprintf(cs, "%c", char_prefix);
                    string_append(str, cs);
                    sprintf(cs, "%c", *fmt);
                    string_append(str, cs);
                    break;
            }
        } else if (*fmt != '%') {
            sprintf(cs, "%c", *fmt);
            string_append(str, cs);
        }
        char_prefix = *fmt++;
    }
    va_end(ap);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
