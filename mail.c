#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include "data.h"
#include "util.h"
#include "mail.h"

static void put_env(char* name, char* value)
{
    char* val = (char*)xalloc(sizeof(char) * VALUE_LENGTH);
    sprintf(val, "%s=%s", name, value);
    putenv(val);
}

int mail_send(Project* project, Message* message, List* elements, List* element_types)
{
    Iterator* it;
    int error_code;
    char command[DEFAULT_LENGTH] = "script/smtp.pl";
    char port[DEFAULT_LENGTH];
    char subject[DEFAULT_LENGTH];
    char* content;
    char header[DEFAULT_LENGTH];
    char footer[DEFAULT_LENGTH];

    content = (char*)xalloc(sizeof(char) * VALUE_LENGTH);
    if (strcmp(project->smtp_server, "") == 0 ||
            strcmp(project->admin_address, "") == 0 ||
            strcmp(project->notify_address, "") == 0) {
        d("invalid settings. gave up to send mail.");
        return MAIL_GAVE_UP;
    }
    put_env("SB_SERVER", project->smtp_server);
    sprintf(port, "%d", project->smtp_port);
    put_env("SB_PORT", port);
    put_env("SB_FROM", project->admin_address);
    put_env("SB_TO", project->notify_address);
    sprintf(subject, "[%s:%d] %s", 
            project->name, 
            message->id,
            get_element_value_by_id(elements, ELEM_ID_TITLE));
    d("subject is %s\n", subject);
    put_env("SB_SUBJECT", subject);
    sprintf(header, 
            "[%s]でチケット情報が更新されました。\n"
            "以下のURLから確認してください。\n"
            " %s%s/reply/%d\n\n",
            project->name, cgiServerName, cgiScriptName, message->id);
    strcat(content, header);
    strcat(content, 
            "更新内容は以下です。\n"
            "--------------------------------------------------------------\n");
    foreach (it, element_types) {
        ElementType* et = it->element;
        strcat(content, et->name);
        strcat(content, ":");
        strcat(content, get_element_value(elements, et));
        strcat(content, "\n\n");
    }
    sprintf(footer, 
            "--------------------------------------------------------------\n"
            "by starbug1(%s).\n"
            "--------------------------------------------------------------\n", project->home_url);
    strcat(content, footer);
    put_env("SB_CONTENT", content);
    d("command: %s\n", command);
    error_code = system(command);
    free(content);
    return error_code;
}
/* vim: set ts=4 sw=4 sts=4 expandtab: */
