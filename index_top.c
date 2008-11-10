#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db_top.h"
#include "dbutil.h"
#include "list.h"
#include "util.h"
#include "wiki.h"
#include "hook.h"
#include "csv.h"
#include "simple_string.h"

void top_top_action();
void top_edit_top_action();
void top_edit_top_submit_action();

/* prototype declares */
int index_top_main();

void top_register_actions()
{
    REG_ACTION(top_top);
    REG_ACTION(top_edit_top);
    REG_ACTION(top_edit_top_submit);
}

void top_output_header(char* title)
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
    o(        "\t<title>Starbug1 - %s</title>\n", title);
/*     o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName); */
    string_free(base_url_a);
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://starbug1.sourceforge.jp/\">Starbug1</a> トップページ</h1>\n");
/*             "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://starbug1.sourceforge.jp/\"><img src=\"%s/../img/starbug1.jpg\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName); */
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
            "<div><address>Starbug1 version: %s. Copyright smeghead 2007 - 2008.</address></div>\n"
            "</div>\n"
            "</div>\n"
            "</body>\n</html>\n", cgiScriptName, VERSION);
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
    List* project_infos;
    Iterator* it;

    d("path\n");
    list_alloc(project_infos, ProjectInfo);
    d("top_top_action\n");

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_infos = db_top_get_all_project_infos(db_a, project_infos);
    top_output_header("トップページ");
    o(      "<div id=\"project_menu\">\n"
            "\t<a href=\"%s/../admin.%s/%s/\">各プロジェクトの管理</a>\n", cgiScriptName, get_ext(cgiScriptName), g_project_name);
    o(      "<div id=\"project_list\">\n"
            "\t<h2>プロジェクト一覧</h2>\n"
            "\t<ul>\n");
    foreach (it, project_infos) {
        ProjectInfo* p = it->element;
        if (p->id == 1) {
            /* idが1のプロジェクトはトップなので、表示しない。 */
            continue;
        }
        o(      "\t\t\t<li><a href=\"%s/%s\">%s</a></li>\n", cgiScriptName, p->name, p->name);
    }
    o(      "\t</ul>\n");
    o(      "</div>\n");
    o(      "<div id=\"dashboard\">\n");
    o(      "<a href=\"%s/%s/top_edit_top\">編集</a>\n", cgiScriptName, g_project_name);
    wiki_out(db_a, "top");
    o(      "</div>\n");
    top_output_footer();
    db_finish(db_a);
}
void top_edit_top_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    top_output_header("トップページの編集");
    o(      "<h2>トップページの編集</h2>\n"
            "<div id=\"top\">\n"
            "<h3>トップページの編集</h3>\n"
            "<div id=\"description\">簡易wikiの文法でトップページのコンテンツの編集を行ない、更新ボタンを押してください。</div>\n"
            "<form id=\"edit_top_form\" action=\"%s/%s/top_edit_top_submit\" method=\"post\">\n", cgiScriptName, g_project_name);
    o(      "<textarea name=\"edit_top\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    wiki_content_out(db_a, "top");
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"更新\" />\n"
            "</form>"
            "<div>\n"
            "<h3>簡易wikiのサポートする文法</h3>\n"
            "<ul>\n"
            "<li>行頭に*を記述した行は、大見出しになります。</li>\n"
            "<li>行頭に**を記述した行は、中見出しになります。</li>\n"
            "<li>行頭に***を記述した行は、小見出しになります。</li>\n"
            "<li>行頭に****を記述した行は、極小見出しになります。</li>\n"
            "<li>行頭に-を記述した行は、箇条書きになります。</li>\n"
            "<li>行頭に----を記述した行は、区切り線になります。</li>\n"
            "<li>行頭が >| から始まる行から、行頭が |< から始まる行までは、整形済みブロックになります。</li>\n"
            "</ul>\n"
            "<h5>例</h5>\n"
            "<pre>\n"
            "*編集可能領域\n"
            "自由に編集できます。\n"
            "右側の「トップページの編集」のリンクから編集してください。\n"
            "色々な用途に使用してください。\n"
            "-お知らせ\n"
            "-Starbug1の使い方についての注意事項など\n"
            "\n"
            ">|\n"
            "void displayWidgets (Iterable<Widget> widgets) {\n"
            "  for (Widget w : widgets) {\n"
            "    w.display();\n"
            "  }\n"
            "}\n"
            "|<\n"
            "</pre>\n"
            "</div>\n"
            "</div>\n");
    db_finish(db_a);
    top_output_footer();
}

void top_edit_top_submit_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    char* value_a = xalloc(sizeof(char) * VALUE_LENGTH);

    d("pass\n");
    cgiFormString("edit_top", value_a, VALUE_LENGTH);
    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    wiki_save(db_a, "top", value_a);
    db_finish(db_a);
    xfree(value_a);

    redirect("", NULL);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
