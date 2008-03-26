#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include <dirent.h>
#include <sys/stat.h>
#include "data.h"
#include "util.h"
#include "hook.h"
#include "simple_string.h"

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
static void escape_quot(String* dist)
{
    string_replace(dist, '"', "\"\"");
    string_replace(dist, '\\', "\\\\");
    string_replace(dist, '\n', "\\n");
}
static String* create_json(String* content, Project* project, Message* message, List* elements, List* element_types)
{
    Iterator* it;
    char buf[DEFAULT_LENGTH];
    string_append(content, "{project:{name: \"");
    string_append(content, project->name);
    string_append(content, "\"}, ticket:{id: ");
    sprintf(buf, "%d", message->id);
    string_append(content, buf);
    string_append(content, ", url: \"http://");
    string_append(content, cgiServerName);
    string_append(content, cgiScriptName);
    string_append(content, "/ticket/");
    string_append(content, buf);
    string_append(content, "\",fields:[");
    foreach (it, element_types) {
        ElementType* et = it->element;
        String* field_a = string_new(0);
        String* name_a = string_new(0);
        String* value_a = string_new(0);
        string_append(name_a, et->name);
        escape_quot(name_a);
        string_append(value_a, get_element_value(elements, et));
        escape_quot(value_a);
        string_append(field_a, "{name:\"");
        string_append(field_a, string_rawstr(name_a));
        string_append(field_a, "\", value:\"");
        string_append(field_a, string_rawstr(value_a));
        string_append(field_a, "\"}");
        string_free(name_a);
        string_free(value_a);
        string_append(content, string_rawstr(field_a));
        string_free(field_a);
        if (iterator_next(it)) string_append(content, ",");
    }
    string_append(content, "]}}");
    return content;
}
HOOK* exec_hook(HOOK* hook, Project* project, Message* message, List* elements, List* element_types)
{
    char hook_dir[DEFAULT_LENGTH] = "script";
    String* content_a = string_new(0);
    DIR* dir;
    struct dirent *dp;
    struct stat fi;

    if ((dir=opendir(hook_dir)) == NULL) {
        return hook;
    }
    content_a = create_json(content_a, project, message, elements, element_types);
    d("json: %s\n", string_rawstr(content_a));
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
            put_env("STARBUG1_CONTENT", string_rawstr(content_a));
            ret = system(hook_command);
            if (ret == 0) {
                sprintf(result->message, "hook処理(%s)を実行しました。", hook_command);
            } else {
                sprintf(result->message, "[ERROR] hook処理(%s)でエラーが発生しました。(return code: %d)", hook_command, ret);
            }
            list_add(hook->results, result);
        }
    }
    string_free(content_a);
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
