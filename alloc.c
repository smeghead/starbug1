#include <stdio.h>
#include <stdlib.h>
#include "util.h"

int alloc_count = 0;

void* _xalloc(size_t size, char* file, int line)
{
    void* p;
    p = calloc(1, size);
    if (!p) {
        fprintf(stderr, "memory error.");
        exit(-1);
    }
    alloc_count++;
#ifdef MEMORYDEBUG
    d("xalloc: %p (%s:%d)\n", p, file, line);
#else
    if (file == NULL || line == 0) {} /* for hide a warning. */
#endif
    return p;
}
void _xfree(void* p, char* file, int line)
{
#ifdef MEMORYDEBUG
    d("xfree: %p (%s:%d)\n", p, file, line);
#else
    if (file == NULL || line == 0) {} /* for hide a warning. */
#endif
    alloc_count--;
    free(p);
    p = NULL;
}
int get_alloc_count()
{
    return alloc_count;
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
