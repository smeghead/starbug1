#ifndef WIKI_H
#define WIKI_H
#include "dbutil.h"

void wiki_out(Database*, char*);
void wiki_content_out(Database*, char*);
void wiki_save(Database*, char*, char*);
void print_wiki_help();

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
