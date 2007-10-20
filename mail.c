#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "util.h"

int mail_send(bt_project* project, bt_message* message)
{
    char command[2048];
    char server[1024];
    char from[1024];
    char to[1024];
    char subject[1024];
    char url[1024];
    if (strlen(project->smtp_server) == 0) {
        d("invalid smtp_server.");
        return 0;
    }
    strcpy(server, project->smtp_server);
    strcpy(from, project->notify_address);
    strcpy(to, project->admin_address);
    sprintf(subject, "[%s:%d] updated.", project->name, message->id);
    strcpy(url, project->smtp_server);
    sprintf(command,
            "script/smtp.pl "
            "-x '%s' "
            "-p %d "
            "-f '%s' "
            "-t '%s' "
            "-s '%s' "
            "-u '%s' ",
            server, project->smtp_port, from, to, subject, url);
    return system(command);
}
