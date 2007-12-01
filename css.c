#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include "css.h"
#include "util.h"

#define MAX_WIDTH 10000

void css_content_out(char* filename)
{
    FILE* in;
    char line[MAX_WIDTH];

    if ((in  = fopen(filename, "r")) == NULL)
        die("file open error.");
    while (fgets(line, sizeof(line), in)) {
        h(line);
    }
    fclose(in);
}
void css_save(char* filename, char* content)
{
    FILE* out;

    /* TODO ロック処理 */
    if ((out  = fopen(filename, "w")) == NULL)
        die("file open error.");
    fputs(content, out);
    fclose(out);
}
/* vim: set ts=4 sw=4 sts=4 expandtab: */
