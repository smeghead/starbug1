#ifndef ALLOC_H
#define ALLOC_H
/* メモリ関連ユーティリティ関数を定義するファイル */
#ifdef MEMORYDEBUG
#define xalloc(x) _xalloc(x, __FILE__, __LINE__);
#define xfree(x) _xfree(x, __FILE__, __LINE__);
#else
#define xalloc(x) _xalloc(x, NULL, 0)
#define xfree(x) _xfree(x, NULL, 0);
#endif

void* _xalloc(size_t, char*, int);
void _xfree(void*, char*, int);
int get_alloc_count();
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
