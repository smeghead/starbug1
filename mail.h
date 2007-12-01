#ifndef MAIL_H
#define MAIL_H
#include "list.h"
#include "data.h"

#define MAIL_GAVE_UP 99999
int mail_send(Project*, Message*, List*, List*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab: */
