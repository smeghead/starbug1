#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "db_project.h"
#include "db_top.h"
#include "dbutil.h"
#include "alloc.h"
#include "util.h"
#include "wiki.h"
#include "template.h"

#define ADD_ITEM_COUNT 10

typedef enum _NAVI {
    NAVI_OTHER,
    NAVI_MENU,
    NAVI_PROJECT,
    NAVI_ITEM,
    NAVI_STYLE,
    NAVI_EXPORT,
    NAVI_ADMIN_HELP
} NaviType;

/* prototype declares */
void register_actions();
void top_action();
void project_action();
void project_submit_action();
void items_action();
void items_submit_action();
void style_action();
void style_submit_action();
void export_action();
void export_submit_action();
void display_action();
void new_item_action();
void new_item_submit_action();
void delete_item_action();
void delete_item_submit_action();
void admin_help_action();
void output_header(Project*, char*, char*, NaviType);
void output_footer();
int admin_project_main();
void update_elements(Database*);

void register_actions()
{
    REG_ACTION(top);
    REG_ACTION(project);
    REG_ACTION(project_submit);
    REG_ACTION(items);
    REG_ACTION(items_submit);
    REG_ACTION(style);
    REG_ACTION(style_submit);
    REG_ACTION(export);
    REG_ACTION(export_submit);
    REG_ACTION(new_item);
    REG_ACTION(new_item_submit);
    REG_ACTION(delete_item);
    REG_ACTION(delete_item_submit);
    REG_ACTION(admin_help);
}

void output_header(Project* project, char* title, char* script_name, NaviType navi)
{
    String* top_project_name = string_new();

    cgiHeaderContentType("text/html; charset=utf-8;");
    top_project_name = db_top_get_top_project_name(top_project_name);
    /* Top of the page */
    o(      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "<head>\n"
            "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            "\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n"
            "\t<meta http-equiv=\"Content-Style-type\" content=\"text/css\" />");
    o(        "\t<title>%s - ", _("management tool")); h(string_rawstr(project->name)); o(" - "); h(title); o("</title>\n");
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../css/style.css\" />\n", cgiScriptName);
    o(      "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/../index.%s/%s/setting_file/user.css\" />\n", cgiScriptName, get_ext(cgiScriptName), g_project_code_4_url);
    o(      "\t<link rel=\"shortcut icon\" type=\"image/x-ico\" href=\"%s/../favicon.ico\" />\n", cgiScriptName);
    if (script_name) {
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/prototype.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/tooltip.js\"></script>\n", cgiScriptName);
        o(  "\t<script type=\"text/javascript\" src=\"%s/../index.%s/top/top_gettext_js\"></script>\n", cgiScriptName, get_ext(cgiScriptName));
        o(  "\t<script type=\"text/javascript\" src=\"%s/../js/%s\"></script>\n", cgiScriptName, script_name);
    }
    o(      "</head>\n"
            "<body>\n"
            "<a name=\"top\"></a>\n"
            "<h1 id=\"toptitle\" title=\"Starbug1\"><a href=\"http://www.starbug1.com/\"><img src=\"%s/../index.%s/%s/setting_file/top_image\" alt=\"Starbug1\" /></a></h1>\n", cgiScriptName, get_ext(cgiScriptName), g_project_code_4_url);
    o(      "<div id='pankuzu'>\n"
            "<ul>\n");
    o(      "\t<li><a href='%s/top/' title=\"%s\">", cgiScriptName, _("display sub projects list at top page.")); h(string_rawstr(top_project_name)); o("</a> &gt; </li>\n");
    string_free(top_project_name);
    o(      "\t<li><a href='%s/../index.%s/%s'>", cgiScriptName, get_ext(cgiScriptName), g_project_code_4_url); h(string_rawstr(project->name)); o("</a></li>\n"); 
    o(      "</ul>\n"
            "<br clear='all' />\n"
            "</div>\n");
    o(      "<div id='menu'>\n"
            "<ul>\n");
    o(      "\t<li><a href=\"%s/../index.%s/top/\" title=\"%s\">%s</a></li>\n",
            cgiScriptName, get_ext(cgiScriptName),
            _("display sub projects list at top page."),
            _("top page(sub projects list)"));
    o(      "</ul>\n"
            "<br clear=\"all\" />\n"
            "</div>\n");
    o(      "<ul id=\"projectmenu\">\n");
    o(      "\t<li><a %s href=\"%s/%s/\">%s</a></li>\n", navi == NAVI_MENU ? "class=\"current\"" : "", cgiScriptName, g_project_code_4_url, _("management tool menu"));
    o(      "\t\t<li><a %s href=\"%s/%s/project\">%s</a></li>\n", navi == NAVI_PROJECT ? "class=\"current\"" : "", cgiScriptName, g_project_code_4_url, _("sub project settings"));
    o(      "\t\t<li><a %s href=\"%s/%s/items\">%s</a></li>\n", navi == NAVI_ITEM ? "class=\"current\"" : "", cgiScriptName, g_project_code_4_url, _("columns settings"));
    o(      "\t\t<li><a %s href=\"%s/%s/style\">%s</a></li>\n", navi == NAVI_STYLE ? "class=\"current\"" : "", cgiScriptName, g_project_code_4_url, _("style settings"));
    o(      "\t\t<li><a %s href=\"%s/%s/export\">%s</a></li>\n", navi == NAVI_EXPORT ? "class=\"current\"" : "", cgiScriptName, g_project_code_4_url, _("export settings"));
    o(      "\t\t<li><a %s href=\"%s/%s/admin_help\">%s</a></li>\n", navi == NAVI_ADMIN_HELP ? "class=\"current\"" : "", cgiScriptName, g_project_code_4_url, _("help"));
    o(      "\t<li><a href=\"%s/../index.%s/%s/\">%s", cgiScriptName, get_ext(cgiScriptName), g_project_code_4_url, _("to"));h(string_rawstr(project->name)); o("%s</a></li>\n", _("to(suffix)"));
    o(      "</ul>\n"
            "<br clear=\"all\" />\n");
}
void output_footer()
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
            "<div><address><a href=\"http://www.starbug1.com/\">Starbug1</a> version: %s. %s.</address></div>\n"
            "</div>\n"
            "</div>\n"
            "</body>\n</html>\n", cgiScriptName, VERSION, COPYRIGHT);
}
void output_field_information_js() {
    o("<script type=\"text/javascript\" src=\"%s/../js/validate.js\"></script>\n", cgiScriptName); 
}
int admin_project_main() {
    register_actions();
    exec_action();
    free_action_actions();
    return 0;
}
/**
 * action that displays management menu.
 */
void top_action()
{
    Project* project_a = project_new();
    char message[DEFAULT_LENGTH];
    Database* db_a;

    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, _("menu"), NULL, NAVI_MENU);

    cgiFormStringNoNewlines("message", message, DEFAULT_LENGTH);
    if (strlen(message) > 0) {
        o("<div class=\"complete_message\">"); h(message); o("&nbsp;</div>\n");
    }
    o("<h2>"); h(string_rawstr(project_a->name)); o(" %s</h2>", _("management tool"));
    project_free(project_a);
    o("<div id=\"main_body\">\n");
    o("<h3>%s</h3>\n", _("management tool menu"));
    o("<div id=\"admin_menu\">\n");
    o("\t<dl>\n");
    o("\t\t<dt><a href=\"%s/%s/project\">%s</a></dt><dd>%s</dd>\n", cgiScriptName, g_project_code_4_url, _("sub project settings"), _("this is basic sub projects settings."));
    o("\t\t<dt><a href=\"%s/%s/items\">%s</a></dt><dd>%s</dd>\n", cgiScriptName, g_project_code_4_url, _("columns settings"), _("this is columns settings of tickets."));
    o("\t\t<dt><a href=\"%s/%s/style\">%s</a></dt><dd>%s</dd>\n", cgiScriptName, g_project_code_4_url, _("style settings"), _("this is stylesheet settings."));
    o("\t\t<dt><a href=\"%s/%s/export\">%s</a></dt><dd>%s</dd>\n", cgiScriptName, g_project_code_4_url, _("export settings"), _("this is export settings."));
    o("\t</dl>\n");
    o("</div>\n");
    o("<h3>%s</h3>\n", _("about mail notify features"));
    o("<div class=\"description\">\n");
    o("\t%s\n", _("[mail notify messages]"));
    o("\t<ul>\n"
      "\t\t<li><a href=\"http://www.starbug1.com/\">http://www.starbug1.com/</a></li>\n"
      "\t</ul>\n");
    o("</div>\n");
    o("</div>\n");

    output_footer();
    db_finish(db_a);
}
/**
 * action that displays project information setting.
 */
void project_action()
{
    Project* project_a = project_new();
    Database* db_a;

    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, _("sub project settings"), "management.js", NAVI_PROJECT);

    o("<h2>%s %s</h2>", string_rawstr(project_a->name), _("management tool"));
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/%s/project_submit\" method=\"post\" enctype=\"multipart/form-data\">\n", cgiScriptName, g_project_code_4_url);
    o("\t\t<h3>%s</h3>\n", _("sub project settings"));
    o("\t\t<table summary=\"project table\">\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("sub project name"));
    o("\t\t\t\t<td><input type=\"text\" name=\"project.name\" value=\"");h(string_rawstr(project_a->name));o("\" maxlength=\"1000\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("icon"));
    o("\t\t\t\t<td><input type=\"file\" name=\"project.file\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("file upload max size(kb)"));
    o("\t\t\t\t<td><input type=\"text\" name=\"project.upload_max_size\" value=\"%d\" /></td>\n", project_a->upload_max_size);
    o("\t\t\t</tr>\n");
    o("\t\t</table>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("update"));
    o("\t</form>\n");
    o("</div>\n");
    project_free(project_a);
    output_footer();
    db_finish(db_a);
}
void fill_upload_content_setting_file(SettingFile* sf)
{
    int got_count = 0;
    char* buffer;
    char file_name[DEFAULT_LENGTH];
    char b[DEFAULT_LENGTH];
    cgiFilePtr file;
    string_set(sf->name, "top_image");
    cgiFormFileSize("project.file", &sf->size);
    cgiFormFileName("project.file", file_name, DEFAULT_LENGTH);
    string_set(sf->file_name, file_name);
    cgiFormFileContentType("project.file", string_rawstr(sf->mime_type), DEFAULT_LENGTH);
    buffer = sf->content = xalloc(sizeof(char) * sf->size);
    if (cgiFormFileOpen("project.file", &file) != cgiFormSuccess) {
        die("Could not open the file.");
    }

    while (cgiFormFileRead(file, b, sizeof(b), &got_count) == cgiFormSuccess) {
        char* p = b;
        int i;
        for (i = 0; i < got_count; i++) {
            *buffer = *p;
            buffer++;
            p++;
        }
    }
    cgiFormFileClose(file);
}
/**
 * action that submits project information setting.
 */
void project_submit_action()
{
    Project* project_a = project_new();
    SettingFile* sf_a = setting_file_new();
    Database* db_a;
    char name[DEFAULT_LENGTH];
    char upload_max_size_str[DEFAULT_LENGTH];

    db_a = db_init(g_project_code);
    db_begin(db_a);
    project_a = db_get_project(db_a, project_a);
    cgiFormStringNoNewlines("project.name", name, DEFAULT_LENGTH);
    string_set(project_a->name, name);
    cgiFormStringNoNewlines("project.upload_max_size", upload_max_size_str, DEFAULT_LENGTH);
    project_a->upload_max_size = atoi(upload_max_size_str);
    db_update_project(db_a, project_a);
    project_free(project_a);

    /* update project image. */
    cgiFormFileSize("project.file", &(sf_a->size));
    d("image size %d.\n", sf_a->size);
    if (sf_a->size > 0) {
        d("try to save top image.\n");
        fill_upload_content_setting_file(sf_a);
        db_update_top_image(db_a, sf_a);
    }
    setting_file_free(sf_a);
    db_commit(db_a);
    db_finish(db_a);
    redirect("", _("updated."));
}
/**
 * action that display item settings.
 */
void items_action()
{
    Project* project_a = project_new();
    List* element_types_a;
    Iterator* it;
    Database* db_a;

    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, _("columns settings"), "management.js", NAVI_ITEM);

    o("<h2>%s %s</h2>", string_rawstr(project_a->name), _("management tool"));
    o("<div id=\"top\">\n");
    project_free(project_a);
    o("<div id=\"setting_form\">\n"
      "\t<form id=\"management_form\" action=\"%s/%s/items_submit\" method=\"post\">\n", cgiScriptName, g_project_code_4_url);
    o("\t\t<h3>%s</h3>\n"
      "\t\t<div class=\"description\">%s</div>\n"
      "\t\t<div><a href=\"%s/%s/new_item\">%s</a></div>\n",
      _("columns settings"),
      _("[columns setting description]"),
      cgiScriptName, g_project_code_4_url,
      _("add new column"));

    list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
    element_types_a = db_get_element_types_all(db_a, NULL, element_types_a);
    output_field_information_js();
    o("\t\t<ul id=\"field_list\">\n");
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        o("\t\t<li><a href=\"#field%d\">", et->id); hs(et->name); o("</a></li>\n");
    }
    o("\t\t</ul>\n"
      "\t\t<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("update"));
    foreach (it, element_types_a) {
        ElementType* et = it->element;
        List* items_a;
        Iterator* it;
        list_alloc(items_a, ListItem, list_item_new, list_item_free);
        items_a = db_get_list_item(db_a, et->id, items_a);
        o("\t\t<a name=\"field%d\"></a>\n", et->id);
        o("\t\t<h4>"); hs(et->name); o("&nbsp;<a href=\"#top\">↑</a></h4>\n");
        o("\t\t<input type=\"hidden\" name=\"field_ids\" value=\"%d\" />\n", et->id);
        o("\t\t<table class=\"item_table\" summary=\"item table\">\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th class=\"required\">%s<span class=\"required\">%s</span></th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input class=\"text required\" id=\"field%d.name\" type=\"text\" name=\"field%d.name\" ", _("column name"), _("*"), et->id, et->id);
        o(                  "value=\"");hs(et->name);o("\" maxlength=\"1000\" />\n"
          "\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.name\">%s</div>\n"
          "\t\t\t\t\t<div id=\"field%d.name.required\" class=\"error\"></div>\n", et->id, _("this is name of ticket column."), et->id);
        o("\t\t\t\t\t<script type=\"text/javascript\">\n"
          "\t\t\t\t\t\tnew Tooltip('field%d.name', 'description%d.name');\n"
          "\t\t\t\t\t</script>\n", et->id, et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>%s</th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input type=\"text\" class=\"text\" id=\"field%d.description\" name=\"field%d.description\" ", _("column description"), et->id, et->id);
        o(                  "value=\"");hs(et->description);o("\" maxlength=\"1000\" />\n"
          "\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.description\">%s</div>\n", et->id, _("this is description of ticket column. when register or replay, it is displayed.")); 
        o("\t\t\t\t\t<script type=\"text/javascript\">\n"
          "\t\t\t\t\t\tnew Tooltip('field%d.description', 'description%d.description');\n"
          "\t\t\t\t\t</script>\n", et->id, et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>%s</th>\n"
          "\t\t\t\t<td>\n", _("required"));
        o("\t\t\t\t\t<input id=\"field%d.required\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.required\" ", et->id, et->id);
        o(                  "value=\"1\" %s />\n", et->required == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.required\">%s</label>\n", et->id, _("this column must not be empty."));
        o("\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.required\">%s</div>\n", et->id, _("[require description]"));
        o("\t\t\t\t\t<script type=\"text/javascript\">\n"
          "\t\t\t\t\t\tnew Tooltip('field%d.required', 'description%d.required');\n"
          "\t\t\t\t\t</script>\n", et->id, et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>%s</th>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input id=\"field%d.ticket_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.ticket_property\" ",
          _("attribute of ticket"), et->id, et->id);
        o(                  "value=\"1\" %s %s />\n", et->ticket_property == 1 ? "checked=\"checked\"" : "", et->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.ticket_property\">%s</label>\n", et->id, _("this column will be attribute of ticket."));
        o("\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.ticket_property\">\n"
          "\t\t\t\t\t\t%s\n"
          "\t\t\t\t\t\t%s\n"
          "\t\t\t\t\t\t%s\n"
          "\t\t\t\t\t</div>\n",
          et->id,
          _("[attribute of ticket description1]"),
          _("[attribute of ticket description2]"),
          _("[attribute of ticket description3]"));
        o("\t\t\t\t\t<script type=\"text/javascript\">\n"
          "\t\t\t\t\t\tnew Tooltip('field%d.ticket_property', 'description%d.ticket_property');\n"
          "\t\t\t\t\t</script>\n", et->id, et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>%s</th>\n", _("reply only"));
        o("\t\t\t\t<td>\n"
          "\t\t\t\t\t<input id=\"field%d.reply_property\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.reply_property\" ", et->id, et->id);
        o(                  "value=\"1\" %s %s />\n", et->reply_property == 1 ? "checked=\"checked\"" : "", et->id <= BASIC_ELEMENT_MAX ? "disabled=\"disabled\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.reply_property\">%s</label>\n", et->id, _("this column will be reply only."));
        o("\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.reply_property\">\n"
          "\t\t\t\t\t\t%s\n"
          "\t\t\t\t\t\t%s\n"
          "\t\t\t\t\t</div>\n",
          et->id,
          _("[reply only description1]"),
          _("[reply only description2]"));
        o("\t\t\t\t\t<script type=\"text/javascript\">\n"
          "\t\t\t\t\t\tnew Tooltip('field%d.reply_property', 'description%d.reply_property');\n"
          "\t\t\t\t\t</script>\n", et->id, et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>%s</th>\n", _("column type"));
        o("\t\t\t\t<td>\n"
          "\t\t\t\t\t<input type=\"hidden\" name=\"field%d.element_type_id\" value=\"%d\" />\n", et->id, et->type);
        switch (et->type) {
            case ELEM_TYPE_TEXT:
                o(_("one line text(input[type=text])"));
                break;
            case ELEM_TYPE_TEXTAREA:
                o(_("multi line text(textarea)"));
                break;
            case ELEM_TYPE_NUM:
                o(_("number(input[type=text])"));
                break;
            case ELEM_TYPE_CHECKBOX:
                o(_("boolean(input[type=checkbox])"));
                break;
            case ELEM_TYPE_LIST_SINGLE:
                o(_("selection list(select)"));
                break;
            case ELEM_TYPE_LIST_SINGLE_RADIO:
                o(_("selection list(input[type=radio])"));
                break;
            case ELEM_TYPE_LIST_MULTI:
                o(_("multi selection list(select[multiple=multiple])"));
                break;
            case ELEM_TYPE_UPLOADFILE:
                o(_("file(input[type=file])"));
                break;
            case ELEM_TYPE_DATE:
                o(_("date(input[type=text])"));
                break;
        }
        o("\t\t\t\t</td>\n");
        o("\t\t\t</tr>\n");
        switch (et->type) {
            case ELEM_TYPE_LIST_SINGLE:
            case ELEM_TYPE_LIST_SINGLE_RADIO:
            case ELEM_TYPE_LIST_MULTI:
                o("\t\t\t<tr>\n"
                  "\t\t\t\t<th>%s</th>\n"
                  "\t\t\t\t<td>\n"
                  "\t\t\t\t\t<table class=\"list_item\">\n"
                  "\t\t\t\t\t\t<tr>\n"
                  "\t\t\t\t\t\t\t<th>%s</th>\n"
                  "\t\t\t\t\t\t\t<th class=\"close\">%s</th>\n"
                  "\t\t\t\t\t\t\t<th>%s</th>\n"
                  "\t\t\t\t\t\t\t<th class=\"delete\">%s</th>\n"
                  "\t\t\t\t\t\t</tr>\n", _("selection element"), _("element name"), _("means closed"), _("sort"), _("delete"));
                foreach (it, items_a) {
                    ListItem* item = it->element;
                    o("\t\t\t\t\t\t<tr>\n"
                      "\t\t\t\t\t\t\t<td>\n"
                      "\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item%d.name\" ", et->id, item->id);
                    o(                       "value=\"");hs(item->name);o("\" maxlength=\"1000\" />\n"
                      "\t\t\t\t\t\t\t</td>\n"
                      "\t\t\t\t\t\t\t<td class=\"center\">\n"
                      "\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.close\" value=\"1\" %s />\n", et->id, item->id, (item->close == 1) ? "checked=\"checked\"" : "");
                    o("\t\t\t\t\t\t\t</td>\n"
                      "\t\t\t\t\t\t\t<td>\n"
                      "\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item%d.sort\" value=\"%d\" maxlength=\"1000\" />\n", et->id, item->id, item->sort);
                    o("\t\t\t\t\t\t\t</td>\n"
                      "\t\t\t\t\t\t\t<td class=\"center\">\n"
                      "\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item%d.delete\" value=\"1\" />\n", et->id, item->id);
                    o("\t\t\t\t\t\t\t</td>\n"
                      "\t\t\t\t\t\t</tr>\n");
                }
                o("\t\t\t\t\t\t<tr>\n"
                  "\t\t\t\t\t\t\t<td>\n"
                  "\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field%d.list_item_new.name0\" value=\"\" maxlength=\"1000\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n"
                  "\t\t\t\t\t\t\t<td class=\"center\">\n"
                  "\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field%d.list_item_new.close0\" value=\"\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n"
                  "\t\t\t\t\t\t\t<td>\n"
                  "\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field%d.list_item_new.sort0\" value=\"\" />\n", et->id);
                o("\t\t\t\t\t\t\t</td>\n"
                  "\t\t\t\t\t\t\t<td class=\"center\">\n"
                  "\t\t\t\t\t\t\t\t<script type=\"text/javascript\">\n"
                  "\t\t\t\t\t\t\t\t\t<!--\n"
                  "\t\t\t\t\t\t\t\t\tdocument.write('<input type=\"button\" value=\"%s\" id=\"add_new_row.field%d\" class=\"add_new_row\" />');\n"
                  "\t\t\t\t\t\t\t\t\t//-->\n"
                  "\t\t\t\t\t\t\t\t</script>\n", _("add line"), et->id);
                o("\t\t\t\t\t\t\t</td>\n"
                  "\t\t\t\t\t\t</tr>\n"
                  "\t\t\t\t\t</table>\n"
                  "\t\t\t\t\t<div class=\"description\">%s</div>\n"
                  "\t\t\t\t\t<input id=\"field%d.auto_add_item\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.auto_add_item\" ",
                  _("selection element when this column is selection list."),
                  et->id, et->id);
                o(                  "value=\"1\" %s />\n", et->auto_add_item == 1 ? "checked=\"checked\"" : "");
                o("\t\t\t\t\t<label for=\"field%d.auto_add_item\">%s</label>\n", et->id, _("when register, you can input new selection element."));
                o("\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.auto_add_item\">%s</div>\n", et->id, _("[add line description]"));
                o("\t\t\t\t\t<script type=\"text/javascript\">\n"
                  "\t\t\t\t\t\tnew Tooltip('field%d.auto_add_item', 'description%d.auto_add_item');\n"
                  "\t\t\t\t\t</script>\n", et->id, et->id);
                o("\t\t\t\t</td>\n"
                  "\t\t\t</tr>\n");
                break;
        }
        o("\t\t\t<tr>\n"
          "\t\t\t\t<th>%s</th>\n"
          "\t\t\t\t<td>\n", _("default value"));
        if (et->type == ELEM_TYPE_TEXTAREA) {
            o("\t\t\t\t\t<textarea id=\"field%d.default_value\" name=\"field%d.default_value\" rows=\"2\" cols=\"10\" >", et->id, et->id);hs(et->default_value);o("</textarea>\n");
        } else {
            o("\t\t\t\t\t<input class=\"text\" type=\"text\" id=\"field%d.default_value\" name=\"field%d.default_value\" value=\"%s\" maxlength=\"1000\" />\n", et->id, et->id, string_rawstr(et->default_value));
        }
        o("\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.default_value\">%s</div>\n", et->id, _("this is default value."));
        o("\t\t\t\t\t<script type=\"text/javascript\">\n"
          "\t\t\t\t\t\tnew Tooltip('field%d.default_value', 'description%d.default_value');\n"
          "\t\t\t\t\t</script>\n", et->id, et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n");
        o("\t\t\t\t<th>%s</th>\n", _("display in ticket list"));
        o("\t\t\t\t<td>\n"
          "\t\t\t\t\t<input id=\"field%d.display_in_list\" class=\"checkbox\" type=\"checkbox\" name=\"field%d.display_in_list\" ", et->id, et->id);
        o(                  "value=\"1\" %s maxlength=\"1000\" />\n", et->display_in_list == 1 ? "checked=\"checked\"" : "");
        o("\t\t\t\t\t<label for=\"field%d.display_in_list\">%s</label>\n", et->id, _("display this column in ticket list."));
        o("\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.display_in_list\">%s</div>\n", et->id, _("[display in ticket list description]"));
        o("\t\t\t\t\t<script type=\"text/javascript\">\n"
          "\t\t\t\t\t\tnew Tooltip('field%d.display_in_list', 'description%d.display_in_list');\n"
          "\t\t\t\t\t</script>\n", et->id, et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<th>%s</th>\n", _("sort"));
        o("\t\t\t\t<td>\n"
          "\t\t\t\t\t<input class=\"number\" type=\"text\" id=\"field%d.sort\" name=\"field%d.sort\" value=\"%d\" maxlength=\"5\" />\n", et->id, et->id, et->sort);
        o("\t\t\t\t\t<div class=\"description tooltip\" id=\"description%d.sort\">%s</div>\n", et->id, _("sort no in ticket lists."));
        o("\t\t\t\t\t<script type=\"text/javascript\">\n"
          "\t\t\t\t\t\tnew Tooltip('field%d.sort', 'description%d.sort');\n"
          "\t\t\t\t\t</script>\n", et->id, et->id);
        o("\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t</table>\n");
        o("\t\t<table style=\"display:none\">\n"
          "\t\t\t<tbody id=\"row_template\">\n"
          "\t\t\t<tr>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field[id].list_item_new.name[no]\" value=\"\" maxlength=\"1000\" />\n"
          "\t\t\t\t</td>\n"
          "\t\t\t\t<td class=\"center\">\n"
          "\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field[id].list_item_new.close[no]\" value=\"\" />\n"
          "\t\t\t\t</td>\n"
          "\t\t\t\t<td>\n"
          "\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field[id].list_item_new.sort[no]\" value=\"\" />\n"
          "\t\t\t\t</td>\n"
          "\t\t\t\t<td class=\"center\">\n"
          "\t\t\t\t\t&nbsp;\n"
          "\t\t\t\t</td>\n"
          "\t\t\t</tr>\n"
          "\t\t\t</tbody>\n"
          "\t\t</table>\n");
        if (et->id > BASIC_ELEMENT_MAX) {
            /* basic item must not delete.  */
            o("\t\t<div class=\"delete_item\"><a href=\"%s/%s/delete_item/%d\">", cgiScriptName, g_project_code_4_url, et->id);o(_("delete this column("));hs(et->name);o("%s</a></div>\n", _(")(delete this column)"));
        }
        list_free(items_a);
        o("\t\t<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("update"));
    }
    list_free(element_types_a);
    o("\t</form>\n"
      "</div>\n"
      "</div>\n");

    output_footer();
    db_finish(db_a);
}
/**
 * action that submits item settings.
 */
void items_submit_action()
{
    Database* db_a;
    db_a = db_init(g_project_code);
    db_begin(db_a);
    update_elements(db_a);
    db_commit(db_a);
    db_finish(db_a);
    redirect("", _("updated."));
}

void update_elements(Database* db)
{
    char** ids;
    int i = -1;

    if ((cgiFormStringMultiple("field_ids", &ids)) == cgiFormNotFound) {
        die("cannot find field_ids.");
    }
    d("1");
    while (ids[++i]) {
        int j;
        char* id = ids[i];
        char name[DEFAULT_LENGTH];
        char value[DEFAULT_LENGTH];
        List* items_a;
        Iterator* it;
        ElementType* et_a = element_type_new();

    d("2 id:%s\n", id);
        et_a = db_get_element_type(db, atoi(id), et_a);

        sprintf(name, "field%s.name", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        string_set(et_a->name, value);

        sprintf(name, "field%s.description", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        string_set(et_a->description, value);

        sprintf(name, "field%s.ticket_property", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->ticket_property = atoi(value);

        sprintf(name, "field%s.reply_property", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->reply_property = atoi(value);

        sprintf(name, "field%s.required", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->required = atoi(value);

        sprintf(name, "field%s.display_in_list", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->display_in_list = atoi(value);

        sprintf(name, "field%s.sort", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->sort = atoi(value);

        sprintf(name, "field%s.default_value", id);
        cgiFormString(name, value, DEFAULT_LENGTH);
        string_set(et_a->default_value, value);

        sprintf(name, "field%s.auto_add_item", id);
        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
        et_a->auto_add_item = atoi(value);

        db_update_element_type(db, et_a);

        /* list_item */
        switch (et_a->type) {
            case ELEM_TYPE_LIST_SINGLE:
            case ELEM_TYPE_LIST_SINGLE_RADIO:
            case ELEM_TYPE_LIST_MULTI:
                /* only elements has selection, update list_item. */
                list_alloc(items_a, ListItem, list_item_new, list_item_free);
                items_a = db_get_list_item(db, et_a->id, items_a);
                foreach (it, items_a) {
                    ListItem* item = it->element;
                    strcpy(name, "");
                    strcpy(value, "");
                    sprintf(name, "field%d.list_item%d.name", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    string_set(item->name, value);
                    sprintf(name, "field%d.list_item%d.close", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->close = atoi(value);
                    sprintf(name, "field%d.list_item%d.sort", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item->sort = atoi(value);
                    sprintf(name, "field%d.list_item%d.delete", et_a->id, item->id);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    if (atoi(value) == 1)
                        db_delete_list_item(db, item->id);
                    else
                        db_update_list_item(db, item);
                }
                list_free(items_a);
                /* insert brand new element. */
                for (j = 0; j < 100; j++) {
                    strcpy(name, "");
                    strcpy(value, "");
                    sprintf(name, "field%d.list_item_new.name%d", et_a->id, j);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    if (strlen(value) > 0) {
                        ListItem* item_a = list_item_new();
                        item_a->element_type_id = et_a->id;
                        d("%s\n", value);
                        string_set(item_a->name, value);
                        sprintf(name, "field%d.list_item_new.close%d", et_a->id, j);
                        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                        item_a->close = atoi(value);
                        sprintf(name, "field%d.list_item_new.sort%d", et_a->id, j);
                        cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                        item_a->sort = atoi(value);
                        db_register_list_item(db, item_a);
                        xfree(item_a);
                    } else {
                        /* not exists name, break. */
                        break;
                    }
                }
                break;
        }

        element_type_free(et_a);
    }
    cgiStringArrayFree(ids);
    d("end");
}
void new_item_action()
{
    Project* project_a = project_new();
    int i;
    Database* db_a;

    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, _("add new column"), "new_item.js", NAVI_OTHER);

    o("<h2>%s %s</h2>", string_rawstr(project_a->name), _("management tool"));
    project_free(project_a);
    o(      "<div id=\"new_item\">\n"
            "<h3>%s</h3>\n"
            "<div class=\"description\">%s</div>\n"
            "<form id=\"new_item_form\" action=\"%s/%s/new_item_submit\" method=\"post\">\n"
            "<table class=\"item_table\" summary=\"new item table\">\n",
            _("add new column"),
            _("[ticket add description]"),
            cgiScriptName, g_project_code_4_url);
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th class=\"required\">%s<span class=\"required\">※</span></th>\n", _("column name"));
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.name\" type=\"text\" name=\"field.name\" value=\"\" maxlength=\"1000\" />\n");
    o("\t\t\t\t\t<div class=\"description\">%s</div>\n", _("this is name of ticket column."));
    o("\t\t\t\t\t<div id=\"field.name.required\" class=\"error\"></div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("column description"));
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input type=\"text\" name=\"field.description\" value=\"\" maxlength=\"1000\" />\n");
    o("\t\t\t\t\t<div class=\"description\">%s</div>\n", _("this is description of ticket column. when register or replay, it is displayed."));
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("attribute of ticket"));
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.ticket_property\" class=\"checkbox\" type=\"checkbox\" name=\"field.ticket_property\" ");
    o(                  "value=\"1\" />\n");
    o("\t\t\t\t\t<label for=\"field.ticket_property\">%s</label>\n", _("this column will be attribute of ticket."));
    o("\t\t\t\t\t<div class=\"description\">\n");
    o("\t\t\t\t\t\t%s\n", _("[attribute of ticket description1]"));
    o("\t\t\t\t\t\t%s\n", _("[attribute of ticket description2]"));
    o("\t\t\t\t\t\t%s\n", _("[attribute of ticket description3]"));
    o("\t\t\t\t\t</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("reply only"));
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.reply_property\" class=\"checkbox\" type=\"checkbox\" name=\"field.reply_property\" ");
    o(                  "value=\"1\" />\n");
    o("\t\t\t\t\t<label for=\"field.reply_property\">%s</label>\n", _("this column will be reply only."));
    o("\t\t\t\t\t<div class=\"description\">\n");
    o("\t\t\t\t\t\t%s\n", _("[reply only description1]"));
    o("\t\t\t\t\t\t%s\n", _("[reply only description2]"));
    o("\t\t\t\t\t</div>\n");
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("column type"));
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" type=\"radio\" name=\"field.type\" ", ELEM_TYPE_TEXT);
    o(                  "value=\"%d\" checked=\"checked\" />\n", ELEM_TYPE_TEXT);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_TEXT, _("one line text(input[type=text])"));
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_TEXTAREA);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_TEXTAREA);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_TEXTAREA, _("multi line text(textarea)"));
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_CHECKBOX);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_CHECKBOX);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_CHECKBOX, _("boolean(input[type=checkbox])"));
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_LIST_SINGLE);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_LIST_SINGLE);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_LIST_SINGLE, _("selection list(select)"));
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_LIST_SINGLE_RADIO);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_LIST_SINGLE_RADIO);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_LIST_SINGLE_RADIO, _("selection list(input[type=radio])"));
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_LIST_MULTI);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_LIST_MULTI);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_LIST_MULTI, _("multi selection list(select[multiple=multiple])"));
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_UPLOADFILE);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_UPLOADFILE);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_UPLOADFILE, _("file(input[type=file])"));
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" "
            "type=\"radio\" name=\"field.type\" ", ELEM_TYPE_DATE);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_DATE);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_DATE, _("date(input[type=text])"));
    o("\t\t\t\t\t<input id=\"field.type%d\" class=\"radio\" type=\"radio\" name=\"field.type\" ", ELEM_TYPE_NUM);
    o(                  "value=\"%d\" />\n", ELEM_TYPE_NUM);
    o("\t\t\t\t\t<label for=\"field.type%d\">%s</label><br />\n", ELEM_TYPE_NUM, _("number(input[type=text])"));
    o("\t\t\t\t\t<div class=\"description\">%s</div>\n", _("[ticket add column type description]"));
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
            o("\t\t\t<tr>\n");
            o("\t\t\t\t<th>%s</th>\n", _("selection element"));
            o("\t\t\t\t<td>\n");
            o("\t\t\t\t\t<table class=\"list_item\">\n");
            o("\t\t\t\t\t\t<tr>\n");
            o("\t\t\t\t\t\t\t<th>%s</th>\n", _("element name"));
            o("\t\t\t\t\t\t\t<th class=\"close\">%s</th>\n", _("means closed"));
            o("\t\t\t\t\t\t\t<th>%s</th>\n", _("sort"));
            o("\t\t\t\t\t\t</tr>\n");
            for (i = 0; i < ADD_ITEM_COUNT; i++) {
                o("\t\t\t\t\t\t<tr>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"text\" type=\"text\" name=\"field.list_item_new%d.name\" value=\"\" maxlength=\"1000\" />\n", i);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field.list_item_new%d.close\" value=\"\" />\n", i);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t\t<td>\n");
                o("\t\t\t\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field.list_item_new%d.sort\" value=\"\" maxlength=\"5\" />\n", i);
                o("\t\t\t\t\t\t\t</td>\n");
                o("\t\t\t\t\t\t</tr>\n");
            }
            o("\t\t\t\t\t</table>\n");
            o("\t\t\t\t\t<div class=\"description\">%s</div>\n", _("selection element when this column is selection list."));
            o("\t\t\t\t\t<input id=\"field.auto_add_item\" class=\"checkbox\" type=\"checkbox\" name=\"field.auto_add_item\" ");
            o(                  "value=\"1\" />\n");
            o("\t\t\t\t\t<label for=\"field.auto_add_item\">%s</label>\n", _("when register, you can input new selection element."));
            o("\t\t\t\t\t<div class=\"description\">%s</div>\n", _("[add line description]"));
            o("\t\t\t\t</td>\n");
            o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("display in ticket list"));
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input class=\"checkbox\" type=\"checkbox\" name=\"field.display_in_list\" ");
    o(                  "value=\"1\" />\n");
    o("\t\t\t\t\t<div class=\"description\">%s</div>\n", _("display this column in ticket list."));
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("sort"));
    o("\t\t\t\t<td>\n");
    o("\t\t\t\t\t<input class=\"number\" type=\"text\" name=\"field.sort\" value=\"\" maxlength=\"5\" />\n");
    o("\t\t\t\t\t<div class=\"description\">%s</div>\n", _("sort no in ticket lists."));
    o("\t\t\t\t</td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t\t</table>\n");
    o("\t\t\t<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("add"));
    o("\t\t\t</form>\n");
    o(      "</div>\n");
    db_finish(db_a);
    output_footer();
}
void new_item_submit_action()
{
    ElementType* et_a = element_type_new();
    char value[DEFAULT_LENGTH];
    int i, e_type_id;
    Database* db_a;

    db_a = db_init(g_project_code);
    db_begin(db_a);

    cgiFormStringNoNewlines("field.name", value, DEFAULT_LENGTH);
    string_set(et_a->name, value);

    cgiFormStringNoNewlines("field.description", value, DEFAULT_LENGTH);
    string_set(et_a->description, value);

    cgiFormStringNoNewlines("field.type", value, DEFAULT_LENGTH);
    et_a->type = atoi(value);

    cgiFormStringNoNewlines("field.ticket_property", value, DEFAULT_LENGTH);
    et_a->ticket_property = atoi(value);

    cgiFormStringNoNewlines("field.reply_property", value, DEFAULT_LENGTH);
    et_a->reply_property = atoi(value);

    cgiFormStringNoNewlines("field.display_in_list", value, DEFAULT_LENGTH);
    et_a->display_in_list = atoi(value);
    cgiFormStringNoNewlines("field.auto_add_item", value, DEFAULT_LENGTH);
    et_a->auto_add_item = atoi(value);
    cgiFormStringNoNewlines("field.sort", value, DEFAULT_LENGTH);
    et_a->sort = atoi(value);
    e_type_id = db_register_element_type(db_a, et_a);
    switch (et_a->type) {
        case ELEM_TYPE_LIST_SINGLE:
        case ELEM_TYPE_LIST_SINGLE_RADIO:
        case ELEM_TYPE_LIST_MULTI:
            for (i = 0; i < ADD_ITEM_COUNT; i++) {
                char name[DEFAULT_LENGTH];
                ListItem* item_a = list_item_new();
                item_a->element_type_id = e_type_id;
                sprintf(name, "field.list_item_new%d.name", i);
                cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                string_set(item_a->name, value);
                if (string_len(item_a->name) > 0) {
                    sprintf(name, "field.list_item_new%d.close", i);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item_a->close = atoi(value);
                    sprintf(name, "field.list_item_new%d.sort", i);
                    cgiFormStringNoNewlines(name, value, DEFAULT_LENGTH);
                    item_a->sort = atoi(value);
                    db_register_list_item(db_a, item_a);
                }
                xfree(item_a);
            }
            break;
    }
    element_type_free(et_a);
    db_commit(db_a);
    db_finish(db_a);
    redirect("", _("added."));
}
void delete_item_action()
{
    char* e_type_id;
    int iid;
    Project* project_a = project_new();
    ElementType* et_a = element_type_new();
    Database* db_a;

    d("g_path_info: %s\n", g_path_info);
    e_type_id = g_path_info;
    iid = atoi(e_type_id);
    d("iid: %d\n", iid);
    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, _("column delete"), "delete_item.js", NAVI_OTHER);

    et_a = db_get_element_type(db_a, iid, et_a);
    o("<h2>%s %s</h2>", string_rawstr(project_a->name), _("management tool"));
    project_free(project_a);
    o(      "<div id=\"delete_item/%d\">\n", iid);
    o(      "<h3>%s", _("delete this column("));hs(et_a->name);o("%s</h3>\n", _(")(delete this column)"));
    o(      "<form id=\"delete_item_form\" action=\"%s/%s/delete_item_submit/%d\" method=\"post\">\n"
            "<div class=\"infomation\"><strong>%s"
            "%s</strong></div>"
            "<div class=\"infomation\">%s</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"%s\" />\n",
            cgiScriptName, g_project_code_4_url, iid,
            _("if column has deleted, it's forever gone."),
            _("this will disappear column of exists ticket."),
            _("if you really want to delete, push delete button."),
            _("delete"));
    o(      "</form>\n");
    o(      "</div>\n");
    db_finish(db_a);
    output_footer();
    element_type_free(et_a);
}
void delete_item_submit_action()
{
    char* e_type_id;
    int iid;
    Database* db_a;

    /* request method must be POST. */
    if (strcmp(cgiRequestMethod, "POST") != 0) {
        die(_("it is a invalid request."));
    }
    d("g_path_info: %s\n", g_path_info);
    e_type_id = g_path_info;
    iid = atoi(e_type_id);
    db_a = db_init(g_project_code);
    db_begin(db_a);

    db_delete_element_type(db_a, iid);
    db_commit(db_a);
    redirect("", _("deleted."));
    db_finish(db_a);

}
void style_action()
{
    Database* db_a;
    Project* project_a = project_new();
    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, _("style settings"), "style.js", NAVI_STYLE);
    o(      "<h2>%s %s</h2>", string_rawstr(project_a->name), _("management tool"));
    project_free(project_a);
    o(      "<div id=\"top\">\n"
            "<h3>%s</h3>\n"
            "<div id=\"description\">%s</div>\n", _("edit style sheet"), _("edit style sheet setting and push update button."));
    o(      "<form id=\"edit_css_form\" action=\"%s/%s/style_submit\" method=\"post\">\n", cgiScriptName, g_project_code_4_url);
    o(      "<textarea name=\"edit_css\" id=\"edit_top\" rows=\"3\" cols=\"10\">");
    {
        /* output user.css. */
        SettingFile* file_a = setting_file_new();
        file_a = db_get_setting_file(db_a, "user.css", file_a);
        fwrite(file_a->content, sizeof(char), file_a->size, cgiOut);
        setting_file_free(file_a);
    }
    o(      "</textarea>\n"
            "<div>&nbsp;</div>\n"
            "<input class=\"button\" type=\"submit\" value=\"%s\" />\n"
            "</form>", _("update"));
    {
        List* element_types_a;
        Iterator* it;
        o(      "<div class=\"description\">%s</div>\n", _("below block is sample setting of ticket list table column."));
        o(      "<pre>\n");
        list_alloc(element_types_a, ElementType, element_type_new, element_type_free);
        element_types_a = db_get_element_types_all(db_a, NULL, element_types_a);
        foreach (it, element_types_a) {
            ElementType* et = it->element;
            if ((et->type == ELEM_TYPE_LIST_SINGLE || et->type == ELEM_TYPE_LIST_SINGLE_RADIO) && et->display_in_list) {
                List* items_a;
                Iterator* it_item;
                list_alloc(items_a, ListItem, list_item_new, list_item_free);
                items_a = db_get_list_item(db_a, et->id, items_a);
                o(  "/* ================================ */ \n"
                    "/* %s", _("background-color settings of ")); hs(et->name); o("%s     */\n", _(".(background-color settings)"));
                o(  "/* ================================ */ \n");
                foreach (it_item, items_a) {
                    ListItem* item = it_item->element;
                    o("/* "); hs(item->name); o(" */\n");
                    o("#ticket_list td.field%d-%d", et->id, item->id);
                    o(" {\n");
                    o(" background-color: lightyellow !important;\n");
                    o("}\n");
                }
                list_free(items_a);
            }
        }
        list_free(element_types_a);
        o(      "</pre>\n");
    }
    o(      "</div>\n");
    db_finish(db_a);
    output_footer();
}
void style_submit_action()
{
    SettingFile* sf_a = setting_file_new();
    Database* db_a;
    string_set(sf_a->name, "user.css");
    string_set(sf_a->file_name, "");
    sf_a->content = xalloc(sizeof(char) * VALUE_LENGTH);

    cgiFormString("edit_css", sf_a->content, VALUE_LENGTH);
    sf_a->size = strlen(sf_a->content);
    string_set(sf_a->mime_type, "text/css");
    db_a = db_init(g_project_code);
    db_setting_file_save(db_a, sf_a);
    db_finish(db_a);

    redirect("", _("updated."));
    setting_file_free(sf_a);
}
void admin_help_action()
{
    Project* project_a = project_new();
    Database* db_a;
    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, "ヘルプ", NULL, NAVI_ADMIN_HELP);
    o(      "<h2>");h(string_rawstr(project_a->name));o("</h2>\n"
            "<div id=\"top\">\n");
    wiki_out(db_a, "adminhelp");
    o(      "</div>\n");
    project_free(project_a);
    db_finish(db_a);
    output_footer();
}
void export_action()
{
    Project* project_a = project_new();
    Database* db_a;

    db_a = db_init(g_project_code);
    project_a = db_get_project(db_a, project_a);
    output_header(project_a, _("sub project settings"), "management.js", NAVI_PROJECT);

    o("<h2>%s %s</h2>", string_rawstr(project_a->name), _("management tool"));
    o("<div id=\"setting_form\">\n");
    o("\t<form id=\"management_form\" action=\"%s/%s/export_submit\" method=\"post\">\n", cgiScriptName, g_project_code_4_url);
    o("\t\t<h3>%s</h3>\n", _("export settings"));
    o("\t\t<div class=\"description\">\n");
    o("\t\t\t%s\n", _("this sub project columns settings are able to exported."));
    o("\t\t\t%s\n", _("when you create sub project, you can select exported sub project types."));
    o("\t\t</div>\n");
    o("\t\t<table summary=\"project table\">\n");
    o("\t\t\t<tr>\n");
    o("\t\t\t\t<th>%s</th>\n", _("export sub project type name"));
    o("\t\t\t\t<td><input type=\"text\" name=\"export_sub_project_type_name\" value=\"\" maxlength=\"1000\" /></td>\n");
    o("\t\t\t</tr>\n");
    o("\t\t</table>\n");
    o("\t\t<input class=\"button\" type=\"submit\" value=\"%s\" />\n", _("register"));
    o("\t</form>\n");
    o("</div>\n");
    project_free(project_a);
    output_footer();
    db_finish(db_a);
}
void export_submit_action()
{
    char project_type_name[DEFAULT_LENGTH];
    String* locale = string_new();

    cgiFormStringNoNewlines("export_sub_project_type_name", project_type_name, DEFAULT_LENGTH);
    locale = db_top_get_locale(locale);
    save_template(project_type_name, locale);
    string_free(locale);

    redirect("", _("updated."));
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
