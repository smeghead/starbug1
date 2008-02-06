#ifndef HOOK_H
#define HOOK_H
#include "list.h"
#include "data.h"

#define MAIL_GAVE_UP 99999

typedef enum _hook_mode {
    HOOK_MODE_REGISTERING,
    HOOK_MODE_REGISTERED
} HOOK_MODE;

typedef struct _hook {
    int mode;
    List* results;
} HOOK;
typedef struct _hook_result {
    char command[DEFAULT_LENGTH];
    char message[DEFAULT_LENGTH];
} HOOK_RESULT;

HOOK* init_hook(HOOK_MODE);
HOOK* exec_hook(HOOK*, Project*, Message*, List*, List*);
void clean_hook(HOOK*);
size_t get_hook_message_size(HOOK*);

#endif
/* vim: set ts=4 sw=4 sts=4 expandtab: */
