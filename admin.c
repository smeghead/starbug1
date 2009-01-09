#include <stdio.h>
#include <cgic.h>
#include "alloc.h"
#include "util.h"
#include "admin_project.h"
#include "admin_top.h"
/* 繧ｨ繝ｳ繝医Μ繝昴う繝ｳ繝医→縺ｪ繧九ヵ繧｡繧､繝ｫ */
int cgiMain() {
    ActionType type = analysis_action();
    int ret = 0;
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
