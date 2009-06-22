#ifndef HOOK_DATA_H
#define HOOK_DATA_H
/* hook処理で使用するデータ型を定義する */

#include <stdio.h>

typedef struct {
    char name[1024];
    char value[1024];
} HookElement;

typedef struct {
    int id;
    char project_id[1024];
    char project_name[1024];
    char url[1024];
    char subject[1024];
    char status[1024];
    HookElement* elements;
    int elements_count;
} HookMessage;


#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
