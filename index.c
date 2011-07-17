#include <stdio.h>
#include <cgic.h>
#include <libintl.h>
#include "alloc.h"
#include "util.h"
#include "index_top.h"
#include "index_project.h"
#include "db_top.h"

/* index.cgi */
int cgiMain() {
    double start = get_microseconds();
    ActionType type = analysis_action();
    int ret = 0;
    db_top_set_locale();

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
    d("time: %f\n", get_microseconds() - start);
    return ret;
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
