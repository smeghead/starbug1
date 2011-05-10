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

#ifdef _WIN32
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
    char template_dir[DEFAULT_LENGTH];
    DIR* dir;
    struct dirent *dp;
    struct stat fi;

    sprintf(template_dir, "template%s%s", PATH_SEPERATOR, string_rawstr(locale));
    if ((dir = opendir(template_dir)) == NULL) {
        return templates;
    }
    for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
        char template_filename[DEFAULT_LENGTH];
        char filename[DEFAULT_LENGTH];
        strcpy(filename, dp->d_name);
        sprintf(template_filename, "%s%s%s", template_dir, PATH_SEPERATOR, filename);
        stat(template_filename, &fi);
        if (!S_ISDIR(fi.st_mode)) {  /*ファイルの場合 */
            /* テンプレート情報の取得 */
            FILE* fp = fopen(template_filename, "r");
            char first_line[DEFAULT_LENGTH];
            char* template_name;
            char* p;
            memset(first_line, '\0', DEFAULT_LENGTH);
            if (fgets(first_line, DEFAULT_LENGTH - 1, fp) == NULL) {
                fclose(fp);
                die("fail to read template");
            }
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
            fclose(fp);
        }
    }
    return templates;
}

void create_db_from_template(int project_id, char* project_type)
{
    String* db_path_a = string_new();
    Iterator* it;
    List* templates;
    String* locale_a = string_new();
    locale_a = db_top_get_locale(locale_a);
    string_appendf(db_path_a, "db/%d.db", project_id);
    list_alloc(templates, Template, template_new, template_free);
    templates = get_templates(templates, locale_a);
    string_free(locale_a);
    d("project_type: [%s]\n", project_type);
    foreach (it, templates) {
        Template* template = it->element;
        d("template project_type: [%s]\n", string_rawstr(template->name));
        if (strncmp(project_type, string_rawstr(template->name), DEFAULT_LENGTH) == 0) {
            String* command_a = string_new();
            int ret;
            string_appendf(command_a, "%s %s | sqlite3 %s", CAT, string_rawstr(template->path), string_rawstr(db_path_a));
            string_free(db_path_a);
            d("command_a: %s\n", string_rawstr(command_a));
            if ((ret = system(string_rawstr(command_a))) != 0) {
                d("return code: %d\n", ret);
                die("外部プログラムの実行に失敗しました。");
            }
            string_free(command_a);
            return;
        }
    }
    die("テンプレートの取得に失敗しました。");
}
void pipe_to_file(char* command_str, char* db_name, FILE* out)
{
    FILE* fp;
    String* command_a = string_new();
#ifdef _WIN32
    /* windowsの場合は、db_nameのパス区切り文字を変更する。 */
    char* p = db_name;
    while (*p) {
        if (*p == '/')
            *p = '\\';
        p++;
    }
    /* Windowsではechoの引数をクオートしない。 */
    string_appendf(command_a, "echo %s | cmd.exe /c sqlite3 %s", command_str, db_name);
#else
    string_appendf(command_a, "echo '%s' | sqlite3 %s", command_str, db_name);
#endif
    d("%s\n", string_rawstr(command_a));

    if ((fp = popen(string_rawstr(command_a), "r")) == NULL) {
        die("外部プログラム実行に失敗しました。");
    }
    string_free(command_a);
    while (1) {
        char str[DEFAULT_LENGTH];
        if (fgets(str, DEFAULT_LENGTH, fp) == NULL) {
            break;
        }
        if(feof(fp)){
            break;
        }
        fprintf(out, "%s", str);
    }
    pclose(fp);
}
void save_template(char* project_type, String* locale)
{
    String* template_filename_a = string_new();
    FILE* out;
    char timestamp[DEFAULT_LENGTH];
    char db_name[DEFAULT_LENGTH];
    char template_dir[DEFAULT_LENGTH];
    sprintf(template_dir, "template%s%s", PATH_SEPERATOR, string_rawstr(locale));
    set_timestamp_string(timestamp);
    /* テンプレートファイル名 */
    string_appendf(template_filename_a, "%s%s%s.template", template_dir, PATH_SEPERATOR, timestamp);
    if (!(out = fopen(string_rawstr(template_filename_a), "w"))) {
        die("テンプレートファイルのオープンに失敗しました。");
    }
    string_free(template_filename_a);
    db_top_get_project_db_name(g_project_code, db_name);
    fprintf(out, "-- name: %s\n", project_type);
    pipe_to_file(".dump project_info", db_name, out);
    pipe_to_file(".dump setting", db_name, out);
    pipe_to_file(".dump setting_file", db_name, out);
    pipe_to_file(".dump element_type", db_name, out);
    pipe_to_file(".dump list_item", db_name, out);
    pipe_to_file(".schema ticket", db_name, out);
    pipe_to_file(".schema message", db_name, out);
    pipe_to_file(".schema element_file", db_name, out);
    pipe_to_file(".dump wiki", db_name, out);
    fclose(out);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
