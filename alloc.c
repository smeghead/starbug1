#include <stdio.h>
#include <stdlib.h>
/*#include "util.h"*/

int alloc_count = 0;

void* xalloc(size_t size)
{
    void* p;
    p = calloc(1, size);
    if (!p) {
        fprintf(stderr, "memory error.");
        exit(-1);
    }
    alloc_count++;
/*    d("xalloc: %p\n", p); */
    return p;
}
void xfree(void* p)
{
/*    d("xfree: %p\n", p); */
    alloc_count--;
    free(p);
    p = NULL;
}
int get_alloc_count()
{
    return alloc_count;
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
