#include <stdio.h>
#include <cgic.h>
#include "util.h"
#include "index_top.h"
#include "index_project.h"

/*  */
int cgiMain() {
    ActionType type = analysis_action();
    int ret = 0;
    switch (type) {
        case ACTION_TYPE_INDEX_TOP:
            ret = index_top_main();
            break;
        case ACTION_TYPE_INDEX:
            ret = index_project_main();
            break;
        default:
            break;
    }
    return ret;
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
