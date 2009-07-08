#include <stdio.h>
#include <cgic.h>
#include <libintl.h>
#include "alloc.h"
#include "util.h"
#include "index_top.h"
#include "index_project.h"

/* index.cgi縺ｮ繧ｨ繝ｳ繝医Μ繝昴う繝ｳ繝医↓縺ｪ繧矩未謨ｰ  */
int cgiMain() {
    ActionType type = analysis_action();
    int ret = 0;
    d("pass %d\n", type);

    bindtextdomain("starbug1", "locale");
    textdomain("starbug1");
    setlocale(LC_ALL, "ja_JP.UTF-8");

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
