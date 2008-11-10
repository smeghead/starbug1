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

/* prototype declares */
int index_top_main();

void top_register_actions()
{
    register_action_actions("top", top_top_action);
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
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    string_free(base_url_a);
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://starbug1.sourceforge.jp/\"><img src=\"%s/../img/starbug1.jpg\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName);
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
 * 一覧を表示するaction。
 */
/* void list_action() */
/* { */
/*     List* element_types_a; */
/*     List* conditions_a = NULL; */
/*     Project* project_a = project_new(); */
/*     List* states_a; */
/*     Iterator* it; */
/*     char** multi; */

/*     db_init(); */
/*     project_a = db_get_project(project_a); */
/*     output_header(project_a, "状態別チケット一覧", NULL, NAVI_LIST); */
/*     |+メッセージの取得+| */
/*     if ((cgiFormStringMultiple("message", &multi)) != cgiFormNotFound) { */
/*         int i = 0; */
/*         o("<div class=\"complete_message\">"); */
/*         while (multi[i]) { */
/*             if (strstr(multi[i], "[ERROR]") != NULL) { */
/*                 o("<div class=\"error\">"); */
/*             } else { */
/*                 o("<div>"); */
/*             } */
/*             h(multi[i]); */
/*             o("</div>"); */
/*             i++; */
/*         } */
/*         o("</div>\n"); */
/*     } */
/*     cgiStringArrayFree(multi); */
/*     list_alloc(element_types_a, ElementType); */
/*     element_types_a = db_get_element_types_4_list(element_types_a); */
/*     o("<h2>"); h(project_a->name); o(" - 状態別チケット一覧</h2>\n"); */
/*     project_free(project_a); */
/*     list_alloc(states_a, State); */
/*     states_a = db_get_states(states_a); */
/*     output_states(states_a, true); */
/*     list_free(states_a); */
/*     fflush(cgiOut); */
/*     o("<div id=\"ticket_list\">\n" */
/*       "<h3>状態別チケット一覧</h3>\n" */
/*       "<div class=\"description\">未クローズの状態毎にチケットを表示しています。\n"); */
/*     list_alloc(states_a, State); */
/*     states_a = db_get_states_has_not_close(states_a); */
/*     foreach (it, states_a) { */
/*         State* s = it->element; */
/*         o("\t\t\t<a href=\"#"); */
/*         h(s->name); */
/*         o("\" title=\"ページ内へのリンク\">"); */
/*         h(s->name); */
/*         o("</a>"); */
/*     } */
/*     o("</div>\n" */
/*       "<br clear=\"all\" />\n"); */
/*     foreach (it, states_a) { */
/*         State* s = it->element; */
/*         SearchResult* result_a = search_result_new(); */

/*         |+検索+| */
/*         list_alloc(conditions_a, Condition); */
/*         result_a = db_get_tickets_by_status(s->name, result_a); */
/*         list_free(conditions_a); */

/*         o("<a name=\""); h(s->name); o("\"></a>\n"); */
/*         o("<div>\n"); */
/*         o("<h4 class=\"status\">");h(s->name);o("&nbsp;(%d件)&nbsp;<a href=\"#top\">↑</a></h4>\n", s->count); */
/*         if (result_a->hit_count == LIST_COUNT_PER_LIST_PAGE) { */
/*             o("\t\t<div class=\"infomation\">新しい%d件のみを表示しています。<a href=\"%s/search?field%d=",  */
/*                     result_a->hit_count,  */
/*                     cgiScriptName, */
/*                     ELEM_ID_STATUS); */
/*             u(s->name); */
/*             o("\">状態が"); h(s->name); o("である全てのチケットを表示する</a></div>\n"); */
/*         } */
/*         output_ticket_table_status_index(result_a, element_types_a); */
/*         if (result_a->hit_count == LIST_COUNT_PER_LIST_PAGE) { */
/*             o("\t\t<div class=\"infomation\">続きがあります。<a href=\"%s/search?field%d=",  */
/*                     cgiScriptName, */
/*                     ELEM_ID_STATUS); */
/*             u(s->name); */
/*             o("\">状態が"); h(s->name); o("である全てのチケットを表示する</a></div>\n"); */
/*         } */
/*         search_result_free(result_a); */
/*         o("</div>\n"); */
/*         fflush(cgiOut); */
/*     } */
/*     list_free(states_a); */
/*     list_free(element_types_a); */
/*     o("</div>\n"); */
/*     output_footer(); */
/*     db_finish(); */
/* } */
/**
 * デフォルトのaction。
 */
void top_top_action()
{
    Database* db_a;
    char buffer[DEFAULT_LENGTH];
    List* project_infos;
    Iterator* it;

    list_alloc(project_infos, ProjectInfo);
    d("top_top_action\n");

    db_a = db_init(db_top_get_project_db_name(g_project_name, buffer));
    project_infos = db_top_get_all_project_infos(db_a, project_infos);
    top_output_header("トップページ");
    o("<h2>全プロジェクトページ</h2>\n");
    o(      "<div id=\"project_list\">\n"
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
    top_output_footer();
    db_finish(db_a);
/*     Project* project_a = project_new(); */
/*     List* tickets_a; */
/*     List* states_a; */
/*     Iterator* it; */

/*     db_init(); */
/*     project_a = db_get_project(project_a); */
/*     output_header(project_a, "トップページ", NULL, NAVI_TOP); */
/*     list_alloc(states_a, State); */
/*     states_a = db_get_states(states_a); */
/*     o(      "<div id=\"info\">\n"); */
/*     |+最新情報の表示+| */
/*     o(      "<div id=\"top_newest\">\n" */
/*             "<h4>最新情報</h4>\n" */
/*             "\t<ul>\n"); */
/*     list_alloc(tickets_a, Message); */
/*     tickets_a = db_get_newest_information(10, tickets_a); */
/*     if (tickets_a->size) { */
/*         foreach (it, tickets_a) { */
/*             Message* ticket = it->element; */
/*             List* elements_a; */
/*             list_alloc(elements_a, Element); */
/*             elements_a = db_get_last_elements_4_list(ticket->id, elements_a); */
/*             o("\t\t<li>\n"); */
/*             o("\t\t\t<a href=\"%s/ticket/%d", cgiScriptName, ticket->id); o("\">"); */
/*             h(get_element_value_by_id(elements_a, ELEM_ID_TITLE)); */
/*             o(      "</a>\n"); */
/*             o("\t\t</li>\n"); */
/*             free_element_list(elements_a); */
/*         } */
/*     } else { */
/*         o("<li>最新情報がありません。</li>\n"); */
/*     } */
/*     list_free(tickets_a); */
/*     o(      "\t</ul>\n" */
/*             "</div>\n"); */
/*     |+stateの表示+| */
/*     o(      "<div id=\"top_state_index\">\n" */
/*             "<h4>状態別件数</h4>\n" */
/*             "\t<ul>\n"); */
/*     if (states_a->size) { */
/*         foreach (it, states_a) { */
/*             State* s = it->element; */
/*             o("\t\t<li>\n"); */
/*             o("\t\t\t<a href=\"%s/search?field%d=", cgiScriptName, ELEM_ID_STATUS); u(s->name); o("\">"); */
/*             h(s->name); */
/*             o("\t\t\t</a>\n"); */
/*             o("(%d)", s->count); */
/*             o("\t\t</li>\n"); */
/*         } */
/*     } else { */
/*         o("<li>チケット情報がありません。</li>\n"); */
/*     } */
/*     list_free(states_a); */
/*     o(      "\t</ul>\n" */
/*             "</div>\n"); */
/*     |+ID検索フォームの表示+| */
/*     o(      "<div id=\"top_id_search\">\n" */
/*             "<h4>ID検索</h4>\n" */
/*             "\t<form action=\"%s/search\" method=\"get\">\n", cgiScriptName); */
/*     o(      "\t\t<input type=\"text\" class=\"number\" name=\"id\" title=\"入力したIDのチケットを表示します。\" maxlength=\"%d\" />\n", DEFAULT_LENGTH - 1); */
/*     o(      "\t\t<input type=\"submit\" class=\"button\" value=\"表示\" />\n" */
/*             "\t</form>\n" */
/*             "</div>\n" */
/*             "</div>\n" */
/*             "<div id=\"main\">\n" */
/*             "<h2>");h(project_a->name);o("&nbsp;</h2>\n"); */
/*     project_free(project_a); */
/*     o(      "<div id=\"main_body\">\n" */
/*             "<div class=\"top_edit\">\n" */
/*             "<a id=\"new_ticket_link\" href=\"%s/register\">新しいチケットを登録する</a>&nbsp;\n", cgiScriptName); */
/*     o(      "<a href=\"%s/edit_top\">トップページの編集</a>\n", cgiScriptName); */
/*     o(      "</div>\n"); */
/*     wiki_out("top"); */
/*     o(      "</div>\n" */
/*             "</div>\n"); */
/*     db_finish(); */
/*     output_footer(); */
}

void top_edit_top_action()
{
/*     Project* project_a = project_new(); */
/*     db_init(); */
/*     project_a = db_get_project(project_a); */
/*     output_header(project_a, "トップページの編集", "edit_top.js", NAVI_OTHER); */
/*     project_free(project_a); */
/*     o(      "<h2>トップページの編集</h2>\n" */
/*             "<div id=\"top\">\n" */
/*             "<h3>トップページの編集</h3>\n" */
/*             "<div id=\"description\">簡易wikiの文法でトップページのコンテンツの編集を行ない、更新ボタンを押してください。</div>\n" */
/*             "<form id=\"edit_top_form\" action=\"%s/edit_top_submit\" method=\"post\">\n", cgiScriptName); */
/*     o(      "<textarea name=\"edit_top\" id=\"edit_top\" rows=\"3\" cols=\"10\">"); */
/*     wiki_content_out("top"); */
/*     o(      "</textarea>\n" */
/*             "<div>&nbsp;</div>\n" */
/*             "<input class=\"button\" type=\"submit\" value=\"更新\" />\n" */
/*             "</form>" */
/*             "<div>\n" */
/*             "<h3>簡易wikiのサポートする文法</h3>\n" */
/*             "<ul>\n" */
/*             "<li>行頭に*を記述した行は、大見出しになります。</li>\n" */
/*             "<li>行頭に**を記述した行は、中見出しになります。</li>\n" */
/*             "<li>行頭に***を記述した行は、小見出しになります。</li>\n" */
/*             "<li>行頭に****を記述した行は、極小見出しになります。</li>\n" */
/*             "<li>行頭に-を記述した行は、箇条書きになります。</li>\n" */
/*             "<li>行頭に----を記述した行は、区切り線になります。</li>\n" */
/*             "<li>行頭が >| から始まる行から、行頭が |< から始まる行までは、整形済みブロックになります。</li>\n" */
/*             "</ul>\n" */
/*             "<h5>例</h5>\n" */
/*             "<pre>\n" */
/*             "*編集可能領域\n" */
/*             "自由に編集できます。\n" */
/*             "右側の「トップページの編集」のリンクから編集してください。\n" */
/*             "色々な用途に使用してください。\n" */
/*             "-お知らせ\n" */
/*             "-Starbug1の使い方についての注意事項など\n" */
/*             "\n" */
/*             ">|\n" */
/*             "void displayWidgets (Iterable<Widget> widgets) {\n" */
/*             "  for (Widget w : widgets) {\n" */
/*             "    w.display();\n" */
/*             "  }\n" */
/*             "}\n" */
/*             "|<\n" */
/*             "</pre>\n" */
/*             "</div>\n" */
/*             "</div>\n"); */
/*     db_finish(); */
/*     output_footer(); */
}
void top_edit_top_submit_action()
{
/*     char* value_a = xalloc(sizeof(char) * VALUE_LENGTH); */

/*     cgiFormString("edit_top", value_a, VALUE_LENGTH); */
/*     db_init(); */
/*     wiki_save("top", value_a); */
/*     db_finish(); */
/*     xfree(value_a); */

/*     redirect("", NULL); */
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
