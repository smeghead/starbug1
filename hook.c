#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include <dirent.h>
#include <sys/stat.h>
#include "data.h"
#include "util.h"
#include "hook.h"

static void put_env(char* name, char* value)
{
    char* val = (char*)xalloc(sizeof(char) * VALUE_LENGTH);
    sprintf(val, "%s=%s", name, value);
    putenv(val);
}

HOOK* init_hook(HOOK_MODE mode)
{
    HOOK* hook = xalloc(sizeof(HOOK));
    List* results;
    hook->mode = mode;
    list_alloc(results, HOOK_RESULT);
    hook->results = results;
    return hook;
}
static void escape_quot(char* dist, char* src)
{
    char* d = dist;
    char* p = src;
    while (*p) {
        if (*p == '"') {
            *d = '\\';
            d++;
        }
        *d = *p;
        d++;
        p++;
    }
    *d = '\0';
}
static char* create_json(char* content, Project* project, Message* message, List* elements, List* element_types)
{
    Iterator* it;
    sprintf(content, 
            "{project:{name: \"%s\"},"
            "ticket:{id: %d, url: \"http://%s%s/ticket/%d\",fields:[",
            project->name, message->id, cgiServerName, cgiScriptName, message->id);
    foreach (it, element_types) {
        ElementType* et = it->element;
        char field[DEFAULT_LENGTH * 2];
        char name[DEFAULT_LENGTH];
        char* value_p;
        char* value_a;
        char* value_quoted_a;
        escape_quot(name, et->name);
        value_p = get_element_value(elements, et);
        value_a = xalloc(sizeof(char) * DEFAULT_LENGTH + 10);
        value_quoted_a = xalloc(sizeof(char) * DEFAULT_LENGTH * 2); /* quoteするとサイズが増えるので、余分にバッファを用意する。 */
        strncpy(value_a, value_p, DEFAULT_LENGTH);
        escape_quot(value_quoted_a, value_a);
        xfree(value_a);
        sprintf(field, "{name:\"%s\", value:\"%s\"}", name, value_quoted_a);
        xfree(value_quoted_a);
        strcat(content, field);
        if (iterator_next(it)) strcat(content, ",");
    }
    strcat(content, "]}}");
    return content;
}
HOOK* exec_hook(HOOK* hook, Project* project, Message* message, List* elements, List* element_types)
{
    char hook_dir[DEFAULT_LENGTH] = "script";
    char* content_a;
    DIR* dir;
    struct dirent *dp;
    struct stat fi;

    if ((dir=opendir(hook_dir)) == NULL) {
        return hook;
    }
    content_a = (char*)xalloc(sizeof(char) * VALUE_LENGTH);
    content_a = create_json(content_a, project, message, elements, element_types);
    d("json: %s\n", content_a);
    for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
        char hook_command[DEFAULT_LENGTH];
        char filename[DEFAULT_LENGTH];
        strcpy(filename, dp->d_name);
        sprintf(hook_command, "%s/%s", hook_dir, filename);
        stat(hook_command, &fi);
        if (!S_ISDIR(fi.st_mode) &&                        //ファイルで、
                (fi.st_mode & S_IXUSR) &&                  //所有者が実行可能で
                (strstr(filename, "hook_") == filename)) { // ファイル名がhook_から始まる。
            int ret;
            HOOK_RESULT* result;
            result = list_new_element(hook->results);
            strcpy(result->command, hook_command);
            put_env("STARBUG1_CONTENT", content_a);
            ret = system(hook_command);
            if (ret == 0) {
                sprintf(result->message, "hook処理(%s)を実行しました。", hook_command);
            } else {
                sprintf(result->message, "[ERROR] hook処理(%s)でエラーが発生しました。(return code: %d)", hook_command, ret);
            }
            list_add(hook->results, result);
        }
    }
    xfree(content_a);
    return hook;
}
size_t get_hook_message_size(HOOK* hook)
{
    Iterator* it;
    size_t message_size = 0;
    foreach (it, hook->results) {
        HOOK_RESULT* result = it->element;
        message_size += strlen(result->message);
    }
    return message_size;
}
void clean_hook(HOOK* hook)
{
    if (hook == NULL) return;
    if (hook->results != NULL) {
        list_free(hook->results);
    }
    xfree(hook);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
