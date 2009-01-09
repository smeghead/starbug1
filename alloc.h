#ifndef ALLOC_H
#define ALLOC_H
/* メモリ関連ユーティリティ関数を定義するファイル */

void* xalloc(size_t);
void xfree(void*);
int get_alloc_count();
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
