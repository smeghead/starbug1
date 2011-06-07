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
#include "template.h"

void top_top_action();
void top_update_project_submit_action();
void top_add_project_submit_action();
void top_update_project_info_submit_action();

void top_register_actions()
{
    REG_ACTION(top_top);
    REG_ACTION(top_update_project_info_submit);
    REG_ACTION(top_update_project_submit);
    REG_ACTION(top_add_project_submit);
}

void top_output_header(char* title, Project* project)
{
    String* base_url_a = string_new();
    base_url_a = get_base_url(base_url_a);
    o("Pragma: no-cache\r\n");
    o("Cache-Control: no-cache\t\n");
    cgiHeaderContentType("text/html; charset=utf-8;");
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />\n");
    o(        "\t<title>Starbug1 - "); h(string_rawstr(project->name)); o(" - "); h(title); o("</title>\n");
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/top.css\" />\n", cgiScriptName);
    o(      "\t<link rel=\"shortcut icon\" type=\"image/x-ico\" href=\"%s/../favicon.ico\" />\n", cgiScriptName);
    string_free(base_url_a);
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\">"); h(string_rawstr(project->name)); o(" %s</h1>\n", _("project management"));
/*             "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://www.starbug1.com/\"><img src=\"%s/../img/starbug1.jpg\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName); */
    o(      "<div id=\"projectmenu\">\n"
            "\t<ul>\n");
    o(      "\t\t<li><a href=\"%s/../index.%s/%s/\">%s</a></li>\n", cgiScriptName, get_ext(cgiScriptName), g_project_code_4_url, _("project top"));
    o(      "\t\t<li><a href=\"%s/../admin.%s/%s/\">%s</a></li>\n", cgiScriptName, get_ext(cgiScriptName), g_project_code_4_url, _("project management"));
    o(      "\t</ul>\n"
            "</div>\n"
            "<br clear=\"all\" />\n"
            "<div id=\"main-content\">");
}

void top_output_footer()
{
    /* Finish up the page */
    o(      "</div>\n"
            "<div id=\"footer\">\n"
            "<hr />\n"
            "<div align=\"right\">\n"
            "<p>\n"
            "\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
            "\t\t<img src=\"%s/../img/valid-xhtml10.gif\" alt=\"Valid XHTML 1.0 Transitional\" height=\"31\" width=\"88\" />\n"
            "\t</a>\n"
            "</p>\n"
            "<div><address>Powered by cgic &amp; SQLite3.</address></div>\n"
            "<div><address><a href=\"http://www.starbug1.com/\">Starbug1</a> version: %s. %s.</address></div>\n"
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
 * default action.
 */
void top_top_action()
{
    Database* db_a;
    List* project_infos_a;
    Iterator* it;
    char** multi;
    Project* project_a = project_new();

    d("top_top_action\n");
    list_alloc(project_infos_a, ProjectInfo, project_info_new, project_info_free);

    db_a = db_init(g_project_code);
    project_infos_a = db_top_get_all_project_infos(db_a, project_infos_a);
    project_a = db_get_project(db_a, project_a);
    d("top_top_action\n");
    top_output_header(_("project management"), project_a);
    d("top_top_action\n");
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
            "\t<h2>%s</h2>\n", _("project information settings"));
    o(      "\t<form action=\"%s/top/top_update_project_info_submit\" method=\"post\">\n", cgiScriptName);
    o(      "\t\t<table>\n"
            "\t\t\t<tr>\n"
            "\t\t\t\t<th>%s</th>\n"
            "\t\t\t\t<td><input type=\"text\" name=\"project.name\" value=\"", _("project name")); v(string_rawstr(project_a->name)); o("\" maxlength=\"1000\" /></td>\n"
            "\t\t\t</tr>\n"
            "\t\t\t<tr>\n"
            "\t\t\t\t<th>%s</th>\n", _("home link name"));
    o(      "\t\t\t\t<td><input type=\"text\" name=\"project.home_description\" value=\""); v(string_rawstr(project_a->home_description)); o("\" maxlength=\"1000\" /></td>\n"
            "\t\t\t</tr>\n"
            "\t\t\t<tr>\n"
            "\t\t\t\t<th>%s</th>\n", _("home link refference"));
    o(      "\t\t\t\t<td><input type=\"text\" name=\"project.home_url\" value=\""); v(string_rawstr(project_a->home_url)); o("\" maxlength=\"1000\" /></td>\n"
            "\t\t\t</tr>\n");
    o(      "\t\t\t<tr>\n"
            "\t\t\t\t<th>%s</th>\n", _("locale"));
    o(      "\t\t\t\t<td><input type=\"text\" name=\"project.locale\" value=\""); v(string_rawstr(project_a->locale)); o("\" maxlength=\"5\" /></td>\n"
            "\t\t\t</tr>\n");
    o(      "\t\t</table>\n");
    o(      "\t\t<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("update"));
    o(      "\t</form>\n");
    o(      "</div>\n");
    project_free(project_a);
    o(      "<div id=\"sub_project_list\">\n"
            "\t<h2>%s</h2>\n", _("sub projects list"));
    o(      "\t<form action=\"%s/top/top_update_project_submit\" method=\"post\">\n", cgiScriptName);
    o(      "\t\t<table>\n"
            "\t\t\t<tr>\n"
            "\t\t\t\t<th>%s</th>\n"
            "\t\t\t\t<th>%s</th>\n"
            "\t\t\t\t<th>%s</th>\n"
            "\t\t\t\t<th>%s</th>\n"
            "\t\t\t\t<th>%s</th>\n"
            "\t\t\t</tr>\n", _("sub project name"), _("database path"), _("sub project id"), _("sort"), _("delete"));
    foreach (it, project_infos_a) {
        ProjectInfo* p = it->element;
        Database* db_project_a;
        Project* project_a;
        char db_name[DEFAULT_LENGTH];
        if (p->id == 1) {
            /* the project that id is 1 is top project, so this will be omitted. */
            continue;
        }
        project_a = project_new();
        sprintf(db_name, "db/%d.db", p->id);
        db_project_a = db_init(string_rawstr(p->code));
        project_a = db_get_project(db_project_a, project_a);
        o(      "\t\t\t<tr>\n");
        o(      "\t\t\t\t<td>"); h(string_rawstr(project_a->name)); o("</td>\n");
        o(      "\t\t\t\t<td>"); h(db_name); o("</td>\n");
        project_free(project_a);
        db_finish(db_project_a);
        o(      "\t\t\t\t<td><input type=\"text\" name=\"project_%d.code\" class=\"project_id\" value=\"", p->id); h(string_rawstr(p->code)); o("\" /></td>\n");
        o(      "\t\t\t\t<td><input type=\"text\" name=\"project_%d.sort\" class=\"number\" value=\"%d\" /></td>\n", p->id, p->sort);
        o(      "\t\t\t\t<td><input type=\"checkbox\" class=\"checkbox\" name=\"project_%d.deleted\" id=\"project_%d.deleted\" value=\"1\" %s /></td>\n", p->id, p->id, p->deleted ? "checked=\"checked\"" : "");
        o(      "\t\t\t</tr>\n");
    }
    list_free(project_infos_a);
    o(      "\t\t</table>\n");
    o(      "\t\t<p>%s</p>\n", _("[project settings description1]"));
    o(      "\t\t<p>%s</p>\n", _("[project settings description2]"));
    o(      "\t\t<p>%s</p>\n", _("[project settings description3]"));
    o(      "\t\t<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("update"));
    o(      "\t</form>\n");
    o(      "\t</div>\n");
    o(      "\t<div id=\"project_add\">\n"
            "\t\t<h2>%s</h2>\n", _("add sub project"));
    o(      "\t\t<form action=\"%s/top/top_add_project_submit\" method=\"post\">\n", cgiScriptName);
    o(      "\t\t\t<table>\n"
            "\t\t\t\t<tr>\n"
            "\t\t\t\t\t<th>%s</th>\n"
            "\t\t\t\t\t<th>%s</th>\n"
            "\t\t\t\t\t<th>%s</th>\n"
            "\t\t\t\t</tr>\n", _("sub project id"), _("sort"), _("sub project type"));
    o(      "\t\t\t\t<tr>\n"
            "\t\t\t\t\t<td><input type=\"text\" name=\"project_new.code\" class=\"project_id\" value=\"\" /></td>\n"
            "\t\t\t\t\t<td><input type=\"text\" name=\"project_new.sort\" class=\"number\" value=\"\" /></td>\n"
            "\t\t\t\t\t<td>\n"
            "\t\t\t\t\t\t<select name=\"project_new.project_type\">\n"
            "\t\t\t\t\t\t\t<option value=\"\">%s</option>\n", _("default bts"));
    {
        String* locale = string_new();
        Iterator* it;
        List* templates;
        list_alloc(templates, Template, template_new, template_free);
        locale = db_top_get_locale(locale);
        templates = get_templates(templates, locale);
        string_free(locale);
        foreach (it, templates) {
            Template* template = it->element;
            o("\t\t\t\t\t\t<option name=\""); h(string_rawstr(template->name)); o("\">");
            h(string_rawstr(template->name));
            o("</option>\n");
        }
        list_free(templates);
    }
    o(      "\t\t\t\t\t\t</select>\n");
    o(      "\t\t\t\t\t</td>\n");
    o(      "\t\t\t\t</tr>\n");
    o(      "\t\t\t</table>\n");
    o(      "\t\t\t<p>%s</p>\n", _("[sub project name description]"));
    o(      "\t\t\t<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("add"));
    o(      "\t\t</form>\n");
    o(      "\t</div>\n");
    top_output_footer();
    db_finish(db_a);
}

void top_update_project_info_submit_action()
{
    Database* db_a;

    db_a = db_init(g_project_code);
    {
        char name[DEFAULT_LENGTH];
        char home_description[DEFAULT_LENGTH];
        char home_url[DEFAULT_LENGTH];
        char locale[16];
        Project* project_a = project_new();

        d("db->name: %s\n", db_a->name);
        cgiFormString("project.name", name, DEFAULT_LENGTH);
        string_set(project_a->name, name);
        cgiFormString("project.home_description", home_description, DEFAULT_LENGTH);
        string_set(project_a->home_description, home_description);
        cgiFormString("project.home_url", home_url, DEFAULT_LENGTH);
        string_set(project_a->home_url, home_url);
        cgiFormString("project.locale", locale, 16);
        string_set(project_a->locale, locale);
        db_top_update_project(db_a, project_a);
        project_free(project_a);
    }
    db_finish(db_a);

    redirect("", _("updated."));
}

static bool validate_project_code_exists(Database* db, char* code, int id)
{
    bool ret = true;
    List* project_infos_a;
    Iterator* it;
    list_alloc(project_infos_a, ProjectInfo, project_info_new, project_info_free);
    project_infos_a = db_top_get_all_project_infos(db, project_infos_a);
    foreach (it, project_infos_a) {
        ProjectInfo* p = it->element;
        if (p->id == id) continue;
        if (strcmp(code, string_rawstr(p->code)) == 0) {
            ret = false;
            break;
        }
    }
    list_free(project_infos_a);
    return ret;
}

static bool validate_project_code(char* name)
{
    if (strcmp(name, "top") == 0) {
        return false;
    }
    return true;
}

void top_update_project_submit_action()
{
    Database* db_a;

    db_a = db_init(g_project_code);
    {
        List* project_infos_a;
        Iterator* it;
        list_alloc(project_infos_a, ProjectInfo, project_info_new, project_info_free);

        project_infos_a = db_top_get_all_project_infos(db_a, project_infos_a);
        foreach (it, project_infos_a) {
            ProjectInfo* p = it->element;
            char code[DEFAULT_LENGTH];
            char sort[DEFAULT_LENGTH];
            char deleted[DEFAULT_LENGTH];
            char param_name[DEFAULT_LENGTH];
            if (p->id == 1) continue; /* top project database is ignored. */ 

            sprintf(param_name, "project_%d.code", p->id);
            cgiFormString(param_name, code, DEFAULT_LENGTH);
            if (!validate_project_code(code)) {
                list_free(project_infos_a);
                db_finish(db_a);
                redirect("", _("[error] projectid top is reserved."));
                return;
            }
            if (!validate_project_code_exists(db_a, code, p->id)) {
                char message[DEFAULT_LENGTH];
                list_free(project_infos_a);
                db_finish(db_a);
                sprintf(message, "%s %s %s", _("[error] projectid("), code, _(") is already exists."));
                redirect("", message);
                return;
            }
            string_set(p->code, code);
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

    redirect("", _("updated."));
}

void top_add_project_submit_action()
{
    Database* db_a;

    db_a = db_init(g_project_code);
    {
        char code[DEFAULT_LENGTH];
        char sort[DEFAULT_LENGTH];
        char project_type[DEFAULT_LENGTH];
        ProjectInfo* pi_a = project_info_new();
        int project_id;

        d("db->name: %s\n", db_a->name);
        cgiFormString("project_new.code", code, DEFAULT_LENGTH);
        if (!validate_project_code(code)) {
            project_info_free(pi_a);
            db_finish(db_a);
            redirect("", _("[error] projectid top is reserved."));
            return;
        }
        if (!validate_project_code_exists(db_a, code, 0)) {
            char message[DEFAULT_LENGTH];
            project_info_free(pi_a);
            db_finish(db_a);
            sprintf(message, "%s %s %s", _("[error] projectid("), code, _(") is already exists."));
            redirect("", message);
            return;
        }
        string_set(pi_a->code, code);
        cgiFormString("project_new.sort", sort, DEFAULT_LENGTH);
        pi_a->sort = atoi(sort);
        cgiFormString("project_new.project_type", project_type, DEFAULT_LENGTH);
        string_set(pi_a->project_type, project_type);
        project_id = db_top_register_project_info(db_a, pi_a);
        if (strlen(project_type)) {
            /* if project type is appointed, create project database now. */
            d("create_db_from_template %d %s\n", project_id, project_type);
            create_db_from_template(project_id, project_type);
        }
        project_info_free(pi_a);
    }
    db_finish(db_a);

    redirect("", _("added."));
}

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
