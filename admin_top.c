#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db_top.h"
#include "db_project.h"
#include "dbutil.h"
#include "list.h"
#include "alloc.h"
#include "util.h"
#include "wiki.h"
#include "hook.h"
#include "csv.h"
#include "simple_string.h"

void top_top_action();
void top_update_project_submit_action();
void top_add_project_submit_action();
void top_update_project_info_submit_action();

/* prototype declares */
int index_top_main();

void top_register_actions()
{
    REG_ACTION(top_top);
    REG_ACTION(top_update_project_info_submit);
    REG_ACTION(top_update_project_submit);
    REG_ACTION(top_add_project_submit);
}

void top_output_header(char* title, Project* project)
{
    String* base_url_a = string_new(0);
    base_url_a = get_base_url(base_url_a);
    o("Pragma: no-cache\r\n");
    o("Cache-Control: no-cache\t\n");
    cgiHeaderContentType("text/html; charset=utf-8;");
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />\n");
    o(        "\t<title>Starbug1 - "); h(string_rawstr(project->name)); o(" - "); h(title); o("</title>\n");
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/top.css\" />\n", cgiScriptName);
    string_free(base_url_a);
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\">"); h(string_rawstr(project->name)); o(" プロジェクトの管理</h1>\n");
/*             "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://starbug1.sourceforge.jp/\"><img src=\"%s/../img/starbug1.jpg\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName); */
    o(      "<div id=\"projectmenu\">\n"
            "\t<ul>\n");
    o(      "\t\t<li><a href=\"%s/../index.%s/%s/\">プロジェクトトップ</a></li>\n", cgiScriptName, get_ext(cgiScriptName), g_project_name_4_url);
    o(      "\t\t<li><a href=\"%s/../admin.%s/%s/\">プロジェクトの管理</a></li>\n", cgiScriptName, get_ext(cgiScriptName), g_project_name_4_url);
    o(      "\t</ul>\n"
            "</div>\n");
}
void top_output_footer()
{
    /* Finish up the page */
    o(      "<div id=\"footer\">\n"
            "<hr />\n"
            "<div align=\"right\">\n"
            "<p>\n"
            "\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
            "\t\t<img src=\"%s/../img/valid-xhtml10.gif\" alt=\"Valid XHTML 1.0 Transitional\" height=\"31\" width=\"88\" />\n"
            "\t</a>\n"
            "</p>\n"
            "<div><address>Powered by cgic &amp; SQLite3.</address></div>\n"
            "<div><address>Starbug1 version: %s. %s.</address></div>\n"
            "</div>\n"
            "</div>\n"
            "</body>\n</html>\n", cgiScriptName, VERSION, COPYRIGHT);
}
int admin_top_main() {
    top_register_actions();
    exec_action();
    free_action_actions();
    return 0;
}
/**
 * デフォルトのaction。
 */
void top_top_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    List* project_infos_a;
    Iterator* it;
    char** multi;
    Project* project_a = project_new();

    list_alloc(project_infos_a, ProjectInfo, project_info_new, project_info_free);

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_infos_a = db_top_get_all_project_infos(db_a, project_infos_a);
    project_a = db_get_project(db_a, project_a);
    top_output_header("プロジェクトの管理", project_a);
    /* メッセージの取得 */
    if ((cgiFormStringMultiple("message", &multi)) != cgiFormNotFound) {
        int i = 0;
        o("<div class=\"complete_message\">");
        while (multi[i]) {
            if (strstr(multi[i], "[ERROR]") != NULL) {
                o("<div class=\"error\">");
            } else {
                o("<div>");
            }
            h(multi[i]);
            o("</div>");
            i++;
        }
        o("</div>\n");
    }
    cgiStringArrayFree(multi);
    o(      "<div id=\"project_info\">\n"
            "\t<h2>プロジェクト情報の設定</h2>\n");
    o(      "\t<form action=\"%s/top/top_update_project_info_submit\" method=\"post\">\n", cgiScriptName);
    o(      "\t\t<table>\n"
            "\t\t\t<tr>\n"
            "\t\t\t\t<th>プロジェクト名</th>\n"
            "\t\t\t\t<td><input type=\"text\" name=\"project.name\" value=\""); v(string_rawstr(project_a->name)); o("\" maxlength=\"1000\" /></td>\n"
            "\t\t\t</tr>\n"
            "\t\t\t<tr>\n"
            "\t\t\t\t<th>ホームリンク名</th>\n"
            "\t\t\t\t<td><input type=\"text\" name=\"project.home_description\" value=\""); v(string_rawstr(project_a->home_description)); o("\" maxlength=\"1000\" /></td>\n"
            "\t\t\t</tr>\n"
            "\t\t\t<tr>\n"
            "\t\t\t\t<th>ホームリンク先</th>\n"
            "\t\t\t\t<td><input type=\"text\" name=\"project.home_url\" value=\""); v(string_rawstr(project_a->home_url)); o("\" maxlength=\"1000\" /></td>\n"
            "\t\t\t</tr>\n");
    o(      "\t\t</table>\n");
    o(      "\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o(      "\t</form>\n");
    o(      "</div>\n");
    project_free(project_a);
    o(      "<div id=\"project_list\">\n"
            "\t<h2>サブプロジェクト一覧</h2>\n");
    o(      "\t<form action=\"%s/top/top_update_project_submit\" method=\"post\">\n", cgiScriptName);
    o(      "\t\t<table>\n"
            "\t\t\t<tr>\n"
            "\t\t\t\t<th>サブプロジェクト名</th>\n"
            "\t\t\t\t<th>データベースパス</th>\n"
            "\t\t\t\t<th>サブプロジェクトID</th>\n"
            "\t\t\t\t<th>並び順</th>\n"
            "\t\t\t\t<th>削除</th>\n"
            "\t\t\t</tr>\n");
    foreach (it, project_infos_a) {
        ProjectInfo* p = it->element;
        Database* db_project_a;
        Project* project_a;
        char db_name[DEFAULT_LENGTH];
        if (p->id == 1) {
            /* idが1のプロジェクトはトップなので、表示しない。 */
            continue;
        }
        project_a = project_new();
        sprintf(db_name, "db/%d.db", p->id);
        db_project_a = db_init(db_name);
        project_a = db_get_project(db_project_a, project_a);
        o(      "\t\t\t<tr>\n");
        o(      "\t\t\t\t<td>"); h(string_rawstr(project_a->name)); o("</td>\n");
        o(      "\t\t\t\t<td>"); h(db_name); o("</td>\n");
        project_free(project_a);
        db_finish(db_project_a);
        o(      "\t\t\t\t<td><input type=\"text\" name=\"project_%d.name\" class=\"project_id\" value=\"", p->id); h(string_rawstr(p->name)); o("\" /></td>\n");
        o(      "\t\t\t\t<td><input type=\"text\" name=\"project_%d.sort\" class=\"number\" value=\"%d\" /></td>\n", p->id, p->sort);
        o(      "\t\t\t\t<td><input type=\"checkbox\" class=\"checkbox\" name=\"project_%d.deleted\" id=\"project_%d.deleted\" value=\"1\" %s /></td>\n", p->id, p->id, p->deleted ? "checked=\"checked\"" : "");
        o(      "\t\t\t</tr>\n");
    }
    list_free(project_infos_a);
    o(      "\t\t</table>\n");
    o(      "\t\t<p>サブプロジェクト名は各サブプロジェクトの管理ツールから設定してください。</p>\n");
    o(      "\t\t<p>既存のサブプロジェクトIDを変更すると、サブプロジェクトのURLが変わってしまうので注意してください。</p>\n");
    o(      "\t\t<p>削除すると、トップページの一覧から表示されなくなります。データは消去されません。再度削除チェックボックスのチェックを外すことで、再び参照することができるようになります。</p>\n");
    o(      "\t\t<input class=\"button\" type=\"submit\" value=\"更新\" />\n");
    o(      "\t</form>\n");
    o(      "\t</div>\n");
    o(      "\t<div id=\"project_add\">\n"
            "\t\t<h2>サブプロジェクトの追加</h2>\n");
    o(      "\t\t<form action=\"%s/top/top_add_project_submit\" method=\"post\">\n", cgiScriptName);
    o(      "\t\t\t<table>\n"
            "\t\t\t\t<tr>\n"
            "\t\t\t\t\t<th>サブプロジェクトID</th>\n"
            "\t\t\t\t\t<th>並び順</th>\n"
            "\t\t\t\t</tr>\n"
            "\t\t\t\t<tr>\n"
            "\t\t\t\t\t<td><input type=\"text\" name=\"project_new.name\" class=\"project_id\" value=\"\" /></td>\n"
            "\t\t\t\t\t<td><input type=\"text\" name=\"project_new.sort\" class=\"number\" value=\"\" /></td>\n");
    o(      "\t\t\t\t</tr>\n");
    o(      "\t\t\t</table>\n");
    o(      "\t\t\t<p>サブプロジェクト名は各サブプロジェクトの管理ツールから設定してください。</p>\n");
    o(      "\t\t\t<input class=\"button\" type=\"submit\" value=\"追加\" />\n");
    o(      "\t\t</form>\n");
    o(      "\t</div>\n");
    top_output_footer();
    db_finish(db_a);
}
void top_update_project_info_submit_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    {
        char name[DEFAULT_LENGTH];
        char home_description[DEFAULT_LENGTH];
        char home_url[DEFAULT_LENGTH];
        Project* project_a = project_new();

        d("db->name: %s\n", db_a->name);
        cgiFormString("project.name", name, DEFAULT_LENGTH);
        string_set(project_a->name, name);
        cgiFormString("project.home_description", home_description, DEFAULT_LENGTH);
        string_set(project_a->home_description, home_description);
        cgiFormString("project.home_url", home_url, DEFAULT_LENGTH);
        string_set(project_a->home_url, home_url);
        db_top_update_project(db_a, project_a);
        project_free(project_a);
    }
    db_finish(db_a);

    redirect("", "修正しました。");
}
static bool validate_project_id_exists(Database* db, char* name, int id)
{
    bool ret = true;
    List* project_infos_a;
    Iterator* it;
    list_alloc(project_infos_a, ProjectInfo, project_info_new, project_info_free);
    project_infos_a = db_top_get_all_project_infos(db, project_infos_a);
    foreach (it, project_infos_a) {
        ProjectInfo* p = it->element;
        if (p->id == id) continue;
        if (strcmp(name, string_rawstr(p->name)) == 0) {
            ret = false;
            break;
        }
    }
    list_free(project_infos_a);
    return ret;
}
static bool validate_project_id(char* name)
{
    if (strcmp(name, "top") == 0) {
        return false;
    }
    return true;
}
void top_update_project_submit_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    {
        List* project_infos_a;
        Iterator* it;
        list_alloc(project_infos_a, ProjectInfo, project_info_new, project_info_free);

        project_infos_a = db_top_get_all_project_infos(db_a, project_infos_a);
        foreach (it, project_infos_a) {
            ProjectInfo* p = it->element;
            char name[DEFAULT_LENGTH];
            char sort[DEFAULT_LENGTH];
            char deleted[DEFAULT_LENGTH];
            char param_name[DEFAULT_LENGTH];
            if (p->id == 1) continue; /* topは更新対象外 */ 

            sprintf(param_name, "project_%d.name", p->id);
            cgiFormString(param_name, name, DEFAULT_LENGTH);
            if (!validate_project_id(name)) {
                list_free(project_infos_a);
                db_finish(db_a);
                redirect("", "[ERROR] プロジェクトID topは予約されています。");
                return;
            }
            if (!validate_project_id_exists(db_a, name, p->id)) {
                char message[DEFAULT_LENGTH];
                list_free(project_infos_a);
                db_finish(db_a);
                sprintf(message, "[ERROR] プロジェクトID %sは既に存在しています。", name);
                redirect("", message);
                return;
            }
            string_set(p->name, name);
            sprintf(param_name, "project_%d.sort", p->id);
            cgiFormString(param_name, sort, DEFAULT_LENGTH);
            p->sort = atoi(sort);
            sprintf(param_name, "project_%d.deleted", p->id);
            cgiFormString(param_name, deleted, DEFAULT_LENGTH);
            if (strlen(deleted)) {
                p->deleted = 1;
            } else {
                p->deleted = 0;
            }
        }
        db_top_update_project_infos(db_a, project_infos_a);
        list_free(project_infos_a);
    }
    db_finish(db_a);

    redirect("", "更新しました。");
}
void top_add_project_submit_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    {
        char name[DEFAULT_LENGTH];
        char sort[DEFAULT_LENGTH];
        ProjectInfo* pi_a = project_info_new();

        d("db->name: %s\n", db_a->name);
        cgiFormString("project_new.name", name, DEFAULT_LENGTH);
        if (!validate_project_id(name)) {
            project_info_free(pi_a);
            db_finish(db_a);
            redirect("", "[ERROR] プロジェクトID topは予約されています。");
            return;
        }
        if (!validate_project_id_exists(db_a, name, 0)) {
            char message[DEFAULT_LENGTH];
            project_info_free(pi_a);
            db_finish(db_a);
            sprintf(message, "[ERROR] プロジェクトID %sは既に存在しています。", name);
            redirect("", message);
            return;
        }
        string_set(pi_a->name, name);
        cgiFormString("project_new.sort", sort, DEFAULT_LENGTH);
        pi_a->sort = atoi(sort);
        db_top_register_project_info(db_a, pi_a);
        project_info_free(pi_a);
    }
    db_finish(db_a);

    redirect("", "追加しました。");
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
