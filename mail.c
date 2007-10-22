#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "util.h"

int mail_send(bt_project* project, bt_message* message, bt_element* elements, bt_element_type* e_types)
{
    char command[DEFAULT_LENGTH];
    char server[DEFAULT_LENGTH];
    char from[DEFAULT_LENGTH];
    char to[DEFAULT_LENGTH];
    char subject[DEFAULT_LENGTH];
    char content[VALUE_LENGTH];
    char env_value[DEFAULT_LENGTH];
    if (strlen(project->smtp_server) == 0 ||
            strlen(project->admin_address) == 0 ||
            strlen(project->notify_address) == 0) {
        d("invalid settings. gave up to send mail.");
        return 0;
    }
    strcpy(server, project->smtp_server);
    strcpy(from, project->notify_address);
    strcpy(to, project->admin_address);
    sprintf(subject, "[%s:%d] %s", 
            project->name, 
            message->id,
            get_element_value_by_id(elements, ELEM_ID_TITLE));
    sprintf(env_value, "SB_SUBJECT=%s", subject);
    putenv(env_value);
    strcat(content, 
            "更新されました。\n"
            "更新内容は以下です。\n"
            "----------------------------------------------\n");
    for (; e_types != NULL; e_types = e_types->next) {
        strcat(content, e_types->name);
        strcat(content, ":\n");
        strcat(content, get_element_value(elements, e_types));
        strcat(content, "\n\n");
    }
    sprintf(env_value, "SB_CONTENT=%s", content);
    putenv(env_value);
    sprintf(command,
            "script/smtp.pl "
            "-x '%s' "
            "-p %d "
            "-f '%s' "
            "-t '%s' ",
            server, project->smtp_port, from, to);
    return system(command);
}
