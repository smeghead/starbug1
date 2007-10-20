#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <cgic.h>
#include "util.h"

static action* get_actions();

action* actions = NULL;

static action* get_actions()
{
    return actions;
}
void register_action_actions(char* action_name, void func(void))
{
    action* a;
    if (actions == NULL) {
        actions = (action*)xalloc(sizeof(action));
        actions->action_name = "__HEAD__";
        actions->next = NULL;
    }
    for (a = actions; a->next != NULL; a = a->next);
    a->next = (action*)xalloc(sizeof(action));
    a = a->next;
    a->action_name = action_name;
    a->action_func = func;
    a->next = NULL;
}

void exec_action()
{
    char* index;
    char* path_info = cgiPathInfo;
    char action_name[1024];
    action* a;
    strncpy(action_name, path_info + 1, 1024);
    if ((index = strchr(action_name, '/'))) {
        *index = '\0';
    }

    for (a = get_actions(); a != NULL; a = a->next) {
        if (!strcmp(action_name, a->action_name)) {
            d("exec_action start: %s\n", a->action_name);
            a->action_func();
            d("exec_action end  : %s\n", a->action_name);
            return;
        }
    }
    for (a = get_actions(); a != NULL; a = a->next) {
        if (!strcmp("default", a->action_name)) {
            a->action_func();
            return;
        }
    }
}
#define TRYPUTC(ch) \
	{ \
		if (putc((ch), cgiOut) == EOF) { \
			return cgiFormIO; \
		} \
	} 
static cgiFormResultType cgiHtmlEscapeDataMultiLine(char *data, int len)
{
	while (len--) {
		if (*data == '<') {
			TRYPUTC('&');
			TRYPUTC('l');
			TRYPUTC('t');
			TRYPUTC(';');
		} else if (*data == '&') {
			TRYPUTC('&');
			TRYPUTC('a');
			TRYPUTC('m');
			TRYPUTC('p');
			TRYPUTC(';');
		} else if (*data == '>') {
			TRYPUTC('&');
			TRYPUTC('g');
			TRYPUTC('t');
			TRYPUTC(';');
		} else if (*data == '\n') {
			TRYPUTC('<');
			TRYPUTC('b');
			TRYPUTC('r');
			TRYPUTC(' ');
			TRYPUTC('/');
			TRYPUTC('>');
		} else {
			TRYPUTC(*data);
		}
		data++;
	}
	return cgiFormSuccess;
}

void hm(char *s)
{
	cgiHtmlEscapeDataMultiLine(s, (int) strlen(s));
}

/* ¼ºÇÔ
#define list_add(target, type) \
    if (target == NULL) { \
        target = (type*)xalloc(sizeof(type)); \
    } else { \
        target->next = (type*)xalloc(sizeof(type)); \
        target->next-next = NULL; \
    }
*/
