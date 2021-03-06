#include <stdio.h>
#include <cgic.h>
#include "alloc.h"
#include "util.h"
#include "admin_project.h"
#include "admin_top.h"
#include "db_top.h"

/* management main */
int cgiMain() {
    ActionType type = analysis_action();
    int ret = 0;

    db_top_set_locale();

    switch (type) {
        case ACTION_TYPE_ADMIN_TOP:
            ret = admin_top_main();
            break;
        case ACTION_TYPE_ADMIN:
            ret = admin_project_main();
            break;
        default:
            break;
    }
    return ret;
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
