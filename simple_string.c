#include <stdio.h>
#include <stdlib.h>
#include "simple_string.h"

#define STRING_DEFAULT_SIZE 1024

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

String* string_new(size_t block_size)
{
    String* str = xalloc(sizeof(String));
    if (block_size == 0) {
        str->block_size = STRING_DEFAULT_SIZE;
    } else {
        str->block_size = block_size;
    }
    str->raw_chars = xalloc(sizeof(char) * str->block_size);
    str->buf_size = str->block_size;
    strcpy(str->raw_chars, "");
    return str;
}
void string_append(String* str, char* addstr)
{
    size_t len = strlen(addstr);
    int size_needed = str->current_size + len + 1;
    if (size_needed > str->buf_size) {
        int new_size = str->block_size * (size_needed / str->block_size + 1);
        str->raw_chars = realloc(str->raw_chars, sizeof(char) * new_size);
        if (!str->raw_chars) {
            fprintf(stderr, "memory error.");
            exit(-1);
        }
        str->buf_size = new_size;
    }
    strcat(str->raw_chars, addstr);
    str->current_size += len;
/*     printf("%d (size: %d)\n", str->current_size, str->buf_size); */
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

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
