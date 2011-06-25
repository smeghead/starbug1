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
void top_edit_top_action();
void top_edit_top_submit_action();
void top_search_action();

/* prototype declares */
int index_top_main();

void top_register_actions()
{
    REG_ACTION(top_top);
    REG_ACTION(top_edit_top);
    REG_ACTION(top_edit_top_submit);
    REG_ACTION(top_search);
}

void top_output_header(char* title, Project* project)
{
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
    o(      "</head>\n"
            "<body>\n"
            "<div id=\"header\">\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\">"); h(string_rawstr(project->name)); o("</h1>\n");
    o(      "<div id=\"icon\"><a href=\"http://starbug1.com/\"><img src=\"%s/../img/starbug1.jpg\" alt=\"Starbug1\" /></a></div>\n", cgiScriptName);
    o(      "<div id=\"projectmenu\">\n"
            "\t<ul>\n"
            "\t\t<li class=\"blank\">&nbsp;</li>\n");
    o(      "\t\t<li><a href=\""); h(string_rawstr(project->home_url)); o("\">"); h(string_rawstr(project->home_description)); o("</a></li>\n");
    o(      "\t\t<li><a href=\"%s/../index.%s/%s/\">%s</a></li>\n", cgiScriptName, get_ext(cgiScriptName), g_project_code_4_url, _("project top"));
    o(      "\t\t<li><a href=\"%s/../admin.%s/%s/\">%s</a></li>\n", cgiScriptName, get_ext(cgiScriptName), g_project_code_4_url, _("project management"));
    o(      "\t</ul>\n");
    o(      "</div>\n"
            "<br clear=\"all\" />\n"
            "</div>\n"
            "<div id=\"main-content\">");
}

void top_output_footer()
{
    /* Finish up the page */
    o(      "</div>\n"
            "<div id=\"footer\">\n"
            "\t<div align=\"right\">\n"
            "\t<address>Powered by <a href=\"%s/../COPYING_cgic\">cgic</a> &amp; SQLite3.\n"
            "\t<a href=\"http://www.starbug1.com/\">Starbug1</a> version: %s. %s.</address>\n"
            "\t<p>\n"
            "\t\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
            "\t\t\t<img src=\"%s/../img/valid-xhtml10.gif\" alt=\"Valid XHTML 1.0 Transitional\" height=\"31\" width=\"88\" />\n"
            "\t\t</a>\n"
            "\t</p>\n"
            "\t</div>\n"
            "</div>\n"
            "</body>\n</html>\n", cgiScriptName, VERSION, COPYRIGHT, cgiScriptName);
}

int index_top_main() {
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
    List* project_infos_a;
    Iterator* it;
    Project* top_project_a = project_new();

    list_alloc(project_infos_a, ProjectInfo, project_info_new, project_info_free);

    db_a = db_init(g_project_code);
    project_infos_a = db_top_get_all_project_infos(db_a, project_infos_a);
    top_project_a = db_get_project(db_a, top_project_a);
    top_output_header(_("top page"), top_project_a);
    o(      "<div id=\"project_list\">\n"
            "\t<h2 class=\"radius-right\">%s</h2>\n"
            "\t<ul>\n", _("sub projects list"));
    foreach (it, project_infos_a) {
        ProjectInfo* p = it->element;
        Database* db_project_a;
        Project* project_a;
        if (p->id == 1) {
            /* idが1のプロジェクトはトップなので、表示しない。 */
            continue;
        }
        if (p->deleted) {
            continue;
        }
        project_a = project_new();
        db_project_a = db_init(string_rawstr(p->code));
        project_a = db_get_project(db_project_a, project_a);
        o(      "\t\t\t\t<li><a href=\"%s/", cgiScriptName); u(string_rawstr(p->code)); o("\">"); h(string_rawstr(project_a->name)); o("</a></li>\n");
        project_free(project_a);
        db_finish(db_project_a);
    }
    list_free(project_infos_a);
    o(      "\t</ul>\n");
    o(      "\t<h2 class=\"radius-right\">%s</h2>\n"
            "\t<form action=\"%s/%s/top_search\" method=\"get\">\n", _("search"), cgiScriptName, g_project_code_4_url);
    o(      "\t\t<div>%s</div>\n", _("search by keyword for all sub projects."));
    o(      "\t\t<input type=\"text\" name=\"q\" />\n"
            "\t\t<input type=\"submit\" value=\"%s\" />\n"
            "\t</form>\n", _("search"));

    o(      "</div>\n");
    o(      "<div id=\"dashboard\">\n"
            "\t<h2 class=\"radius-left\">%s</h2>\n", _("description"));
    o(      "<div id=\"top_page_edit_link\"><a href=\"%s/%s/top_edit_top\">%s</a></div>\n", cgiScriptName, g_project_code_4_url, _("edit top page"));
    wiki_out(db_a, "top");
    o(      "</div>\n");
    o(      "<br clear=\"all\" />\n");
    project_free(top_project_a);
    top_output_footer();
    db_finish(db_a);
}

void top_edit_top_action()
{
    Database* db_a;
    Project* project_a = project_new();

    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    top_output_header(_("edit page"), project_a);
    project_free(project_a);
    o(      "<div id=\"top_page_edit\">\n"
            "<h2>%s</h2>\n"
            "<div id=\"top\">\n"
            "<h3>%s</h3>\n"
            "<div class=\"description\">%s</div>\n",
            _("edit page"), _("edit page"), _("[wiki syntax description1]"));
    o(      "<form id=\"edit_top_form\" action=\"%s/%s/top_edit_top_submit\" method=\"post\">\n", cgiScriptName, g_project_code_4_url);
    o(      "<textarea name=\"edit_top\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    wiki_content_out(db_a, "top");
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("update"));
    o(      "</form>");
    print_wiki_help();
    o(      "</div>");
    db_finish(db_a);
    top_output_footer();
}

void top_edit_top_submit_action()
{
    Database* db_a;
    char* value_a = xalloc(sizeof(char) * VALUE_LENGTH);

    cgiFormString("edit_top", value_a, VALUE_LENGTH);
    db_a = db_init(g_project_code);
    wiki_save(db_a, "top", value_a);
    db_finish(db_a);
    xfree(value_a);

    redirect("", NULL);
}

void top_search_action()
{
    Database* db_a;
    List* tickets_a;
    Iterator* it;
    char q[DEFAULT_LENGTH];
    Project* top_project_a = project_new();

    cgiFormStringNoNewlines("q", q, DEFAULT_LENGTH);
    list_alloc(tickets_a, Ticket, ticket_new, ticket_free);

    db_a = db_init(g_project_code);
    tickets_a = db_top_search(db_a, q, tickets_a);
    top_project_a = db_get_project(db_a, top_project_a);
    top_output_header(_("top page"), top_project_a);
    project_free(top_project_a);
    o(      "<div id=\"search_result\">\n"
            "\t<h2>%s</h2>\n"
            "\t<ul>\n", _("search result"));
    foreach (it, tickets_a) {
        Ticket* t = it->element;
        o(      "\t\t\t\t<li><a href=\"%s/%s/ticket/%d", cgiScriptName, string_rawstr(t->project_code), t->id);
        o("\">"); h(string_rawstr(t->project_name)); o(" "); h(string_rawstr(t->title)); o("</a></li>\n");
    }
    list_free(tickets_a);
    o(      "</ul>\n");
    o(      "</div>\n");
    o(      "<br clear=\"all\">\n");
    o(      "<div class=\"navigation\"><a href=\"%s/top/\">%s</a></div>\n", cgiScriptName, _("back to top"));
    top_output_footer();
    db_finish(db_a);
}

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
