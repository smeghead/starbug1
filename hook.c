#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include "data.h"
#include "alloc.h"
#include "util.h"
#include "hook.h"
#include "simple_string.h"

static void put_env_a(char* name, char* value, char* buf)
{
    sprintf(buf, "%s=%s", name, value);
    putenv(buf);
}

HOOK* init_hook(HOOK_MODE mode)
{
    HOOK* hook = xalloc(sizeof(HOOK));
    List* results;
    hook->mode = mode;
    list_alloc(results, HOOK_RESULT, NULL, NULL);
    hook->results = results;
    return hook;
}
static void escape_quot(String* dist)
{
    string_replace(dist, '\\', "\\\\");
    string_replace(dist, '"', "\\\"");
    string_replace(dist, '\n', "\\n");
}
static String* create_json(String* content, Project* project, Message* message, List* elements, List* element_types)
{
    Iterator* it;
    String* base_url_a = string_new(0);
    base_url_a = get_base_url(base_url_a);
    string_appendf(content, "{project:{name: \"%s\"}, ticket:{id: %d, url: \"%s/%s/ticket/%d\",fields:[",
            project->name,
            message->id,
            string_rawstr(base_url_a),
            g_project_name,
            message->id);
    string_free(base_url_a);
    foreach (it, element_types) {
        ElementType* et = it->element;
        String* field_a = string_new(0);
        String* name_a = string_new(0);
        String* value_a = string_new(0);
        string_append(name_a, string_rawstr(et->name));
        escape_quot(name_a);
        string_append(value_a, get_element_value(elements, et));
        escape_quot(value_a);
        string_appendf(field_a, "{name:\"%s\", value:\"%s\"}",
                string_rawstr(name_a),
                string_rawstr(value_a));
        string_free(name_a);
        string_free(value_a);
        string_append(content, string_rawstr(field_a));
        string_free(field_a);
        if (iterator_next(it)) string_append(content, ",");
    }
    string_append(content, "]}}");
    return content;
}
char* get_script_dir(char* script_dir)
{
    char* p;
    strcpy(script_dir, getenv("SCRIPT_FILENAME"));
    if ((p = strrchr(script_dir, '/'))) {
        *p = '\0';
    }
    return script_dir;
}
HOOK* exec_hook(HOOK* hook, Project* project, Message* message, List* elements, List* element_types)
{
    char hook_dir[DEFAULT_LENGTH] = "script";
    String* content_a = string_new(0);
    DIR* dir;
    struct dirent *dp;
    struct stat fi;

    if ((dir = opendir(hook_dir)) == NULL) {
        return hook;
    }
    content_a = create_json(content_a, project, message, elements, element_types);
    for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
        char hook_command[DEFAULT_LENGTH];
        char filename[DEFAULT_LENGTH];
        strcpy(filename, dp->d_name);
        sprintf(hook_command, "%s/%s", hook_dir, filename);
        stat(hook_command, &fi);
        if (!S_ISDIR(fi.st_mode) &&                        /*ファイルで、 */
                (fi.st_mode & S_IRUSR) &&                  /*所有者が読取可能で */
                (strstr(filename, "hook_") == filename) && /* ファイル名がhook_から始まる。 */
                (strcmp(get_ext(filename), "so") == 0)) {       /* 拡張子がso */
            /* プラグインの実行 */
            int ret;
            HOOK_RESULT* result;
            void* handle;
            char script_dir[DEFAULT_LENGTH];
            char dlpath[DEFAULT_LENGTH];

            result = list_new_element(hook->results);
            sprintf(dlpath, "%s/%s", get_script_dir(script_dir), hook_command);
            handle = dlopen(dlpath, RTLD_LAZY);
            if (!handle) {
                d("dlopen failed: %s\n", dlerror());
                sprintf(result->message, "[ERROR] hook処理(%s)でエラーが発生しました。(プラグインの読み込みに失敗しました。%s)", hook_command, dlerror());
            } else {
                char* error;
                int (*func)(char* base_url, char* project_id, Project* project, Message* message, List* elements, List* element_types);
                func = dlsym(handle, "execute");
                if ((error = dlerror()) != NULL) {
                    d("dlsym error %s\n", error);
                    fputs(error, stderr);
                } else {
                    String* base_url_a = string_new(0);
                    base_url_a = get_base_url(base_url_a);
                    d("execute func\n");
                    ret = func(string_rawstr(base_url_a), g_project_name, project, message, elements, element_types);
                    string_free(base_url_a);
                    if (ret == 0) {
                        d("ok\n");
                        sprintf(result->message, "hook処理(%s)を実行しました。", hook_command);
                    } else {
                        d("ng\n");
                        sprintf(result->message, "[ERROR] hook処理(%s)でエラーが発生しました。(return code: %d)", hook_command, ret);
                    }
                }
            }
            d("dlclose\n");
            dlclose(handle);
            list_add(hook->results, result);
        } else if (!S_ISDIR(fi.st_mode) &&                 /*ファイルで、 */
                (fi.st_mode & S_IXUSR) &&                  /*所有者が実行可能で */
                (strstr(filename, "hook_") == filename)) { /* ファイル名がhook_から始まる。 */
            /* 外部実行ファイルの実行 */
            int ret;
            char* val_a;
            HOOK_RESULT* result;
            result = list_new_element(hook->results);
            strcpy(result->command, hook_command);
            val_a = xalloc(sizeof(char) * string_len(content_a) + strlen("STARBUG1_CONTENT=") + 1);
            put_env_a("STARBUG1_CONTENT", string_rawstr(content_a), val_a);
            ret = system(hook_command);
            xfree(val_a);
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
