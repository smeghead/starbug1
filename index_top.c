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
            "<html xml:lang=\"ja\" lang=\"ja\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />\n");
    o(        "\t<title>Starbug1 - "); h(string_rawstr(project->name)); o(" - "); h(title); o("</title>\n");
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/top.css\" />\n", cgiScriptName);
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\">"); h(string_rawstr(project->name)); o("</h1>\n");
/*             "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://starbug1.sourceforge.jp/\"><img src=\"%s/../img/starbug1.jpg\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName); */
    o(      "<div id=\"projectmenu\">\n"
            "\t<ul>\n"
            "\t\t<li><a href=\""); h(string_rawstr(project->home_url)); o("\">"); h(string_rawstr(project->home_description)); o("</a></li>\n");
    o(      "\t\t<li><a href=\"%s/../index.%s/%s/\">%s</a></li>\n", cgiScriptName, get_ext(cgiScriptName), g_project_name_4_url, _("Project Top"));
    o(      "\t\t<li><a href=\"%s/../admin.%s/%s/\">%s</a></li>\n", cgiScriptName, get_ext(cgiScriptName), g_project_name_4_url, _("プロジェクト管理"));
    o(      "\t</ul>\n");
    o(      "</div>\n");
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
    char buffer[DEFAULT_LENGTH];
    List* project_infos_a;
    Iterator* it;
    Project* top_project_a = project_new();

    list_alloc(project_infos_a, ProjectInfo, project_info_new, project_info_free);

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_infos_a = db_top_get_all_project_infos(db_a, project_infos_a);
    top_project_a = db_get_project(db_a, top_project_a);
    top_output_header(_("トップページ"), top_project_a);
    o(      "<div id=\"project_list\">\n"
            "\t<h2>%s</h2>\n"
            "\t<ul>\n", _("サブプロジェクト一覧"));
    foreach (it, project_infos_a) {
        ProjectInfo* p = it->element;
        Database* db_project_a;
        Project* project_a;
        char db_name[DEFAULT_LENGTH];
        if (p->id == 1) {
            /* idが1のプロジェクトはトップなので、表示しない。 */
            continue;
        }
        if (p->deleted) {
            continue;
        }
        project_a = project_new();
        sprintf(db_name, "db/%d.db", p->id);
        db_project_a = db_init(db_name);
        project_a = db_get_project(db_project_a, project_a);
        o(      "\t\t\t\t<li><a href=\"%s/", cgiScriptName); u(string_rawstr(p->name)); o("\">"); h(string_rawstr(project_a->name)); o("</a></li>\n");
        project_free(project_a);
        db_finish(db_project_a);
    }
    list_free(project_infos_a);
    o(      "\t</ul>\n");
    o(      "\t<h2>%s</h2>\n"
            "\t<form action=\"%s/%s/top_search\" method=\"get\">\n", _("検索"), cgiScriptName, g_project_name_4_url);
    o(      "\t\t<input type=\"text\" name=\"q\" />\n"
            "\t\t<input type=\"submit\" value=\"%s\" />\n"
            "\t</form>\n", _("検索"));

    o(      "</div>\n");
    o(      "<div id=\"dashboard\">\n"
            "\t<h2>%s</h2>\n", _("説明"));
    o(      "<a href=\"%s/%s/top_edit_top\">%s</a>\n", cgiScriptName, g_project_name_4_url, _("トップページの編集"));
    wiki_out(db_a, "top");
    d("pass wiki_out end\n");
    o(      "</div>\n");
    o(      "<br clear=\"all\" />\n");
    project_free(top_project_a);
    top_output_footer();
    db_finish(db_a);
}
void top_edit_top_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    Project* project_a = project_new();

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_a = db_get_project(db_a, project_a);
    top_output_header(_("ページの編集"), project_a);
    project_free(project_a);
    o(      "<h2>%s</h2>\n"
            "<div id=\"top\">\n"
            "<h3>%s</h3>\n"
            "<div id=\"description\">%s</div>\n",
            _("ページの編集"), _("ページの編集"), _("簡易wikiの文法でページのコンテンツの編集を行ない、更新ボタンを押してください。"));
    o(      "<form id=\"edit_top_form\" action=\"%s/%s/top_edit_top_submit\" method=\"post\">\n", cgiScriptName, g_project_name_4_url);
    o(      "<textarea name=\"edit_top\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    wiki_content_out(db_a, "top");
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("Update"));
    o(      "</form>");
    print_wiki_help();
    db_finish(db_a);
    top_output_footer();
}

void top_edit_top_submit_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    char* value_a = xalloc(sizeof(char) * VALUE_LENGTH);

    cgiFormString("edit_top", value_a, VALUE_LENGTH);
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    wiki_save(db_a, "top", value_a);
    db_finish(db_a);
    xfree(value_a);

    redirect("", NULL);
}
void top_search_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    List* tickets_a;
    Iterator* it;
    char q[DEFAULT_LENGTH];
    Project* top_project_a = project_new();

    cgiFormStringNoNewlines("q", q, DEFAULT_LENGTH);
    list_alloc(tickets_a, Ticket, ticket_new, ticket_free);

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    tickets_a = db_top_search(db_a, q, tickets_a);
    top_project_a = db_get_project(db_a, top_project_a);
    top_output_header(_("トップページ"), top_project_a);
    o(      "<div id=\"project_list\">\n"
            "\t<h2>%s</h2>\n"
            "\t<ul>\n", _("検索結果"));
    foreach (it, tickets_a) {
        Ticket* t = it->element;
        o(      "\t\t\t\t<li><a href=\"%s/%s/ticket/%d", cgiScriptName, string_rawstr(t->project_name), t->id);
        o("\">a"); o("</a></li>\n");
    }
    list_free(tickets_a);
    o(      "</div>\n");
    o(      "<br clear=\"all\" />\n");
    project_free(top_project_a);
    top_output_footer();
    db_finish(db_a);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
