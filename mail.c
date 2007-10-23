#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include "data.h"
#include "util.h"
#include "mail.h"

int mail_send(bt_project* project, bt_message* message, bt_element* elements, bt_element_type* e_types)
{
    char command[DEFAULT_LENGTH];
    char server[DEFAULT_LENGTH];
    char from[DEFAULT_LENGTH];
    char to[DEFAULT_LENGTH];
    char subject[DEFAULT_LENGTH];
    char content[VALUE_LENGTH];
    char header[DEFAULT_LENGTH];
    char footer[DEFAULT_LENGTH];
    if (strlen(project->smtp_server) == 0 ||
            strlen(project->admin_address) == 0 ||
            strlen(project->notify_address) == 0) {
        d("invalid settings. gave up to send mail.");
        return MAIL_GAVE_UP;
    }
    strcpy(server, project->smtp_server);
    strcpy(from, project->notify_address);
    strcpy(to, project->admin_address);
    sprintf(subject, "SB_SUBJECT=[%s:%d] %s", 
            project->name, 
            message->id,
            get_element_value_by_id(elements, ELEM_ID_TITLE));
    d("subject is %s\n", subject);
    putenv(subject);
    sprintf(header, 
            "SB_CONTENT=[%s]でチケット情報が更新されました。\n"
            "以下のURLから確認してください。\n"
            " %s%s/reply/%d\n\n",
            project->name, project->host_name, cgiScriptName, message->id);
    strcat(content, header);
    strcat(content, 
            "更新内容は以下です。\n"
            "--------------------------------------------------------------\n");
    for (; e_types != NULL; e_types = e_types->next) {
        strcat(content, e_types->name);
        strcat(content, ":");
        strcat(content, get_element_value(elements, e_types));
        strcat(content, "\n\n");
    }
    sprintf(footer, 
            "--------------------------------------------------------------\n"
            "by starbug1(%s).\n"
            "--------------------------------------------------------------\n", project->home_url);
    strcat(content, footer);
    putenv(content);
    sprintf(command,
            "script/smtp.pl "
            "-x '%s' "
            "-p %d "
            "-f '%s' "
            "-t '%s' ",
            server, project->smtp_port, from, to);
    d("command: %s\n", command);
    return system(command);

}
