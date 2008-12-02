#include <stdio.h>
#include <string.h>
#include "../../util.h"

char PROJECT_SPLITTER = ':';

int get_ticket_syntax_len(char* data, size_t len, char* project_id, char* ticket_id)
{
    char string[1024];
    int index = 0;
    char* block_p;
    char* line_p;

    memset(string, '\0', 1024);
    strncpy(string, data, MIN(len, 1024 - 1));
    if (len == 0) return index; /* �Ǹ��ʸ�����ä� */

    block_p = strchr(string, ' ');
    line_p = strchr(string, '\n');
    if (!block_p)
        block_p = string + strlen(string);
    if (!line_p)
        line_p = string + strlen(string);

    /* �����оݤ�ʸ��������� */
    string[MIN(block_p - string, line_p - string)] = '\0';
        
    printf("string [%s]\n", string);
    {
        char* project_mode = strchr(string, PROJECT_SPLITTER);
        char* project_id_p = string + 1;
        char* ticket_no_p = string + 1;

        while (index++, 1) { /* each char */
            /* 
             * ��ʸ���Ĥſʤ�Ƥ�����
             * ����ǥ����åȥ�󥯤Ǥʤ���Ƚ�ꤵ�줿���ϡ�index �� 0 �ˤ��ơ�break���롣
             * �����åȥ�󥯤���λ�����顢break���롣
             */
            printf(" %d [%c]\n", index, string[index]);
            if (string[index] == '\0') {
                break;
            } else if (project_mode) {
                /* #�ץ�������:�����åȥ⡼�� */
                if (string[index] == PROJECT_SPLITTER) {
                    /* �ץ�������ID�⡼�ɤǤ��ꡢ:�򸡽Ф������ */
                    ticket_no_p = &string[index + 1];
                    printf("project_id_p [%s] \n", project_id_p);
                    printf("ticket_no_p [%s] \n", ticket_no_p);
                    printf("&string [%s] \n", &string[index + 1]);
                    /* :�ǽ�λ���Ƥ�����ϡ������åȥ�󥯤ǤϤʤ��� */
                    if (string[index + 1] < '0' || string[index + 1] > '9') {
                        index = 0; break;
                    }
                } else if (project_id_p != ticket_no_p && (string[index] < '0' || string[index] > '9')) {
                    /* �����å�NO������ˡ������ʳ��򸡽Ф������ */
                    string[index] = '\0'; break; /* ������ޤǤ������åȥ�󥯤Ȥʤ롣 */
                }
            } else if (!project_mode) {
                /* #�����åȥ⡼�� */
                if (string[index] >= '0' && string[index] <= '9') {
                    /* �ץ�������ID�⡼�ɤǤϤʤ��������򸡽Ф������ (����) */
                } else {
                    /* reached to end. */
                    string[index] = '\0'; break; /* ������ޤǤ������åȥ�󥯤Ȥʤ롣 */
                }
            }
        }
        if (index < 2) {
            /* �����åȥ�󥯤ǤϤʤ��� */
            return 0;
        }
        if (project_id_p != ticket_no_p) {
            strncpy(project_id, project_id_p, (ticket_no_p - 1) - project_id_p);
        }
        strcpy(ticket_id, ticket_no_p);
/*         index = (ticket_no_p - string) + strlen(ticket_no_p); */
    }
    return index;
}

int main(int argc, char** argv)
{
    char project_id[1024] = "";
    char ticket_id[1024] = "";
    char teststring[1024];
    char* teststring_arg;
    int ret = -1;

    /* test */
    strcpy(teststring, "#123");
    printf("[test 1] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 4) goto error;
    if (strcmp(project_id, "")) goto error;
    if (strcmp(ticket_id, "123")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#1");
    printf("[test 2] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 2) goto error;
    if (strcmp(project_id, "")) goto error;
    if (strcmp(ticket_id, "1")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "# 123");
    printf("[test 3] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 0) goto error;
    if (strcmp(project_id, "")) goto error;
    if (strcmp(ticket_id, "")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#123a");
    printf("[test 4] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 4) goto error;
    if (strcmp(project_id, "")) goto error;
    if (strcmp(ticket_id, "123")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#1 23");
    printf("[test 5] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 2) goto error;
    if (strcmp(project_id, "")) goto error;
    if (strcmp(ticket_id, "1")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#bts:123");
    printf("[test 6] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 8) goto error;
    if (strcmp(project_id, "bts")) goto error;
    if (strcmp(ticket_id, "123")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#bts:");
    printf("[test 7] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 0) goto error;
    if (strcmp(project_id, "")) goto error;
    if (strcmp(ticket_id, "")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#bts:1");
    printf("[test 8] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 6) goto error;
    if (strcmp(project_id, "bts")) goto error;
    if (strcmp(ticket_id, "1")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#bts :1");
    printf("[test 9] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 0) goto error;
    if (strcmp(project_id, "")) goto error;
    if (strcmp(ticket_id, "")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#bts:123a");
    printf("[test 10] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 8) goto error;
    if (strcmp(project_id, "bts")) goto error;
    if (strcmp(ticket_id, "123")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);


    strcpy(teststring, "#bts: 3");
    printf("[test 11] %s -------------------------------------------\n", teststring);
    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    if (ret != 0) goto error;
    if (strcmp(project_id, "")) goto error;
    if (strcmp(ticket_id, "")) goto error;
    memset(project_id, '\0', 1024);
    memset(ticket_id, '\0', 1024);




    if (argc == 1) return 0;
    teststring_arg = argv[1];

    ret = get_ticket_syntax_len(teststring, strlen(teststring), project_id, ticket_id);
    printf("ret: %d.\tproject_id: [%s]\tticket_id: [%s]\n", ret, project_id, ticket_id);
    printf("project_id: [%s]\n", project_id);
    printf("ticket_id: [%s]\n", ticket_id);
    return 0;

error:
    printf("ERROR test failed.\n");
    return -1;
}
