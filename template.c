#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "db_top.h"
#include "util.h"
#include "data.h"
#include "template.h"
#include "alloc.h"

#ifndef _WIN32
  #define CAT "type"
#else
  #define CAT "cat"
#endif

Template* template_new()
{
    Template* template =  xalloc(sizeof(Template));
    template->name = string_new();
    template->path = string_new();
    return template;
}

void template_free(Template* template)
{
    string_free(template->name);
    string_free(template->path);
    xfree(template);
} 

/* 破壊的に処理する */
static char* trim(char* src)
{
    char* p;
    /* 後ろの空白文字を消す。 */
    for (p = src + strlen(src); *p; p--) {
        /* 改行文字を消す */
        if (*p == ' ' || *p == '\t')
            *p = '\0';
        else
            break;
    }
    for (p = src; *p; p++) {
        if (*p == ' ' || *p == '\t')
            continue;
        return p;
    }
    /* 本来到達しない */
    return src;
}
List* get_templates(List* templates, String* locale)
{
    char template_dir[DEFAULT_LENGTH] = "template/";
    DIR* dir;
    struct dirent *dp;
    struct stat fi;

    strcat(template_dir, string_rawstr(locale));
    if ((dir = opendir(template_dir)) == NULL) {
        return templates;
    }
    for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
        char template_filename[DEFAULT_LENGTH];
        char filename[DEFAULT_LENGTH];
        strcpy(filename, dp->d_name);
        sprintf(template_filename, "%s/%s", template_dir, filename);
        stat(template_filename, &fi);
        if (!S_ISDIR(fi.st_mode)) {  /*ファイルの場合 */
            /* テンプレート情報の取得 */
            FILE* fp = fopen(template_filename, "r");
            char first_line[DEFAULT_LENGTH];
            char* template_name;
            char* p;
            memset(first_line, '\0', DEFAULT_LENGTH);
            fgets(first_line, DEFAULT_LENGTH - 1, fp);
            d("first_line: %s\n", first_line);
            for (p = first_line; *p; p++) {
                /* 改行文字を消す */
                if (*p == '\r' || *p == '\n')
                    *p = '\0';
            }
            d("first_line: %s\n", first_line);
            if ((template_name = strstr(first_line, "name:")) == NULL) {
                /* name: が無かったら、取得した行全体を名前とする。 */
                template_name = first_line;
            } else {
                /* name: があった場合、その後を名前とする。 */
                template_name = template_name + strlen("name:");
            }
            Template* template = list_new_element(templates);
            string_set(template->name, trim(template_name));
            string_set(template->path, template_filename);
            list_add(templates, template);
        }
    }
    return templates;
}

void create_db_from_template(int project_id, char* project_type)
{
    String* db_path = string_new();
    Iterator* it;
    List* templates;
    String* locale = string_new();
    locale = db_top_get_locale(locale);
    string_appendf(db_path, "db/%d.db", project_id);
    list_alloc(templates, Template, template_new, template_free);
    templates = get_templates(templates, locale);
    string_free(locale);
    d("project_type: [%s]\n", project_type);
    foreach (it, templates) {
        Template* template = it->element;
        d("template project_type: [%s]\n", string_rawstr(template->name));
        if (strncmp(project_type, string_rawstr(template->name), DEFAULT_LENGTH) == 0) {
            String* command = string_new();
            string_appendf(command, "%s %s | sqlite3 db/%d.db", CAT, string_rawstr(template->path), project_id);
            if (system(string_rawstr(command)) != 0) {
                die("外部プログラムの実行に失敗しました。");
            }
            string_free(command);
            return;
        }
    }
    die("テンプレートの取得に失敗しました。");
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
