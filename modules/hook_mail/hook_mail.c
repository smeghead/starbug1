#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include "../../hook_data.h"
#define d(...) {\
    FILE *fp = fopen("debug.log", "a");\
    fprintf(fp, __VA_ARGS__);\
    fclose(fp);\
}
#define SEND_DATA(soc, command) { \
    int ret; \
    ret = wait(soc, &target, 5000); \
    if (ret < 0) return ret; \
    send_data(soc, command); \
}

static void* xalloc(size_t size)
{
    void* p;
    p = calloc(1, size);
    if (!p) {
        d("memory error.");
        exit(-1);
    }
    return p;
}
static void xfree(void* p)
{
    free(p);
    p = NULL;
}
static unsigned char *base64 = (unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static void enc_char(unsigned long bb, int srclen, unsigned char *dist, int j)
{
    int x, i, base;

    for (i = srclen; i < 2; i++) 
        bb <<= 8;
    for (base = 18, x = 0; x < srclen + 2; x++, base -= 6) {
        dist[j++] = base64[(unsigned long)((bb>>base) & 0x3F)];
    }
    for (i = x; i < 4; i++) {
        dist[j++] = (unsigned char)'=';
    }
}

static void base64_enc(const unsigned char *src, unsigned char *dist)
{
    unsigned char *p = (unsigned char*)src;
    unsigned long bb = (unsigned long)0;
    int i = 0, j = 0;

    while (*p) {
        bb <<= 8;
        bb |= (unsigned long)*p;

        if (i == 2) {
            enc_char(bb, i, dist, j);
            j = j + 4;
            i = 0;
            bb = 0;
        } else {
            i++;
        }
        p++;
    }
    if (i) enc_char(bb, i - 1, dist, j);
}
static int wait(int soc, struct pollfd *target, int timeout)
{
    int ret = 0;
    while(1) {
        ret = poll(target, 1, timeout);
        d("poll ret %d\n", ret);
        if (ret == -1 && errno != EINTR) {
            /* エラー */
            ret = -1;
            break;
        } else if (ret == 0) {
            /* timeout */
            d("poll timeout \n");
            ret = -2;
            break;
        }
        if (target->revents & (POLLIN | POLLERR)) {
            char buf[1024];
            recv(soc, buf, sizeof(buf), 0);
            d("recv: %s\n", buf);
            /* 送信準備OK */
            ret = 0;
            break;
        }
    }
    d("wait ret: %d\n", ret);
    return ret;
}
static int send_data(int soc, char* buf)
{
    d("send: %s\n", buf);
    send(soc, buf, strlen(buf), 0);
    d("sended \n");
    return 0;
}
/* static char* hook_get_element_value_by_id(List* elements, const int type) */
/* { */
/*     Iterator* it; */
/*     d("hook_get_element_value_by_id 0 %d\n", type); */
/*     if (elements == NULL) return ""; */
/*     d("hook_get_element_value_by_id 1\n"); */
/*     d("hook_get_element_value_by_id 1 %d \n", elements->size); */
/*     foreach (it, elements) { */
/*         Element* e = it->element; */
/*     d("hook_get_element_value_by_id 2 \n"); */
/*     d("hook_get_element_value_by_id 2 %s\n", string_rawstr(e->str_val)); */
/*         if (type == e->element_type_id && string_rawstr(e->str_val) != NULL) */
/*             return string_rawstr(e->str_val); */
/*     } */
/*     d("hook_get_element_value_by_id 3\n"); */
/*     return ""; */
/* } */
static char* build_header(char* buf, HookMessage* message)
{
    char* realloc_p;
    char* template;
    char* subject_org_a;
    char* subject_a;
    int size;
    int id_size = 10; /* ID用に確保する文字列のサイズ */

    template = 
        "From: %s\r\n"
        "To: %s\r\n"
        "Subject: =?UTF-8?B?%s?=\r\n"
        "Content-Type: text/plain; charset=UTF-8;\r\n"
        "Content-Transfer-Encoding: base64\r\n"
        "\r\n";
    d("subject: %s\n", message->subject);
    subject_org_a = xalloc(sizeof(char) * (1024 + strlen(message->subject)) * 2); /* base64での増加分を考慮 */
    subject_a = xalloc(sizeof(char) * (1024 + strlen(message->subject)) * 2); /* base64での増加分を考慮 */
    sprintf(subject_org_a, "[%s:%d]Starbug1 notify. ", message->project_id, message->id);
    strcat(subject_org_a, message->subject);
    base64_enc((unsigned char*)subject_org_a, (unsigned char*)subject_a);
    xfree(subject_org_a);
    d("subject: %s\n", subject_a);
    size =
        strlen(template) +
        strlen(FROM) +
        strlen(TO) +
        id_size +
        strlen(subject_a) + 1;
    realloc_p = realloc(buf, size);
    if (realloc_p == NULL) {
        d("memory error.");
        exit(-1);
    }
    buf = realloc_p;
    sprintf(buf, template, FROM, TO, subject_a);
    xfree(subject_a);
    return buf;
}
static char* build_body(char* buf, HookMessage* message)
{
    char* realloc_p;
    char* template;
    char* content_a;
    char* content_b64_a;
    int size;
    int id_size = 10; /* ID用に確保する文字列のサイズ */

    d("0 \n");
    template =
        "%sのチケットが更新されました。\r\n"
        " %s\r\n"
        " #%s:%d %s\r\n";

    d("1 \n");
    size =
        strlen(template) +
        strlen(message->project_name) +
        strlen(message->url) +
        id_size +
        strlen(message->subject) + 1;
    d("2 size: %d\n", size);
    content_a = xalloc(sizeof(char) * size);
    d("3 %p\n", content_a);
    sprintf(content_a, template, message->project_name, message->url, message->project_id, message->id, message->subject);
    d("4 %s\n", buf);
    content_b64_a = xalloc(sizeof(char) * strlen(content_a) * 2); /* base64での増加分を考慮 */
    d("5 \n");
    base64_enc((unsigned char*)content_a, (unsigned char*)content_b64_a);
    d("6 %s\n", content_b64_a);
    d("6.1 %d\n", sizeof(char) * strlen(content_b64_a) + 1);
    d("6.2 %p\n", buf);
    realloc_p = realloc(buf, sizeof(char) * (strlen(buf) + strlen(content_b64_a) + strlen("\r\n.\r\n") + 1));
    d("7 \n");
    d("7.5 \n");
    if (realloc_p == NULL) {
        d("memory error.");
        exit(-1);
    }
    d("8 \n");
    buf = realloc_p;
    strcat(buf, content_b64_a);
    d("9 \n");
    strcat(buf, "\r\n.\r\n"); /* 終端を追加する。 */
    d("10 \n");
    xfree(content_b64_a);
    d("11 \n");
    return buf;
}
int execute(HookMessage* message)
{
    char hostnm[1024];
    char portnm[1024];
    struct addrinfo hints, *res, *res0;
    int soc;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXHOST];
    int error;
    struct pollfd target;

    d("start %s:%s\n", SMTP_SERVER, SMTP_PORT);
    strcpy(hostnm, SMTP_SERVER);
    strcpy(portnm, SMTP_PORT);

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    error = getaddrinfo(hostnm, portnm, &hints, &res0);
    if (error) {
        d("getaddrinfo: %s %s:%s\n", hostnm, portnm, gai_strerror(error));
        return -1;
    }

    soc = -1;
    for (res = res0; res; res = res->ai_next) {
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hbuf, sizeof(hbuf), sbuf,
                sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
        if (error) {
            d("getnameinfo: %s %s:%s\n", hostnm, portnm, gai_strerror(error));
            continue;
        }
        d("trying %s port %s \n", hbuf, sbuf);
        soc = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (soc < 0) {
            continue;
        }
        if (connect(soc, res->ai_addr, res->ai_addrlen) < 0) {
            close(soc);
            soc = -1;
            continue;
        } else {
            d("connected\n");
            d("connect: Success\n");
            break;
        }
    }
    freeaddrinfo(res0);

    if (soc < 0) {
        d("no destination to connect to\n");
        return -1;
    }
    target.fd = soc;
    target.events = POLLIN | POLLERR;

    setbuf(stdout, NULL);

    d("=====\n");
    {
        char* data_string_a = xalloc(sizeof(char));
        char command[1024];
        strcpy(data_string_a, "");

        d("start \n");
        sprintf(command, "HELO %s\r\n", SMTP_SERVER);
        SEND_DATA(soc, command);
        sprintf(command, "MAIL FROM: %s\r\n", FROM);
        SEND_DATA(soc, command);
        sprintf(command, "RCPT TO: %s\r\n", TO);
        SEND_DATA(soc, command);
        sprintf(command, "DATA\r\n");
        SEND_DATA(soc, command);

        /* data */
        data_string_a = build_header(data_string_a, message);
        d("build_body \n");
        data_string_a = build_body(data_string_a, message);
        d("build_body end\n");
        SEND_DATA(soc, data_string_a);
        d("data send \n");
        xfree(data_string_a);

        SEND_DATA(soc, "QUIT\r\n");
    }
    d("=====\n");
    d("close\n");
    close(soc);

    return 0;
}

/* 以下、テスト用関数 */
void conv(char*, char*);
int main(int argc, char** argv)
{
/*     char* subject = "日本語"; */
/*     char* subject_b64_a = xalloc(sizeof(char) * strlen(subject) * 1.5); |+base64での増加分を考慮+| */
/*     base64_encode((unsigned char*)subject, (unsigned char*)subject_b64_a); */
/*     d("base64_encode: %s\n", subject_b64_a); */
/*     xfree(subject_b64_a); */
    return 0;
/*    char* hostnm;*/
/*    char* portnm;*/
/*    struct addrinfo hints, *res, *res0;*/
/*    int soc;*/
/*    char hbuf[NI_MAXHOST], sbuf[NI_MAXHOST];*/
/*    int error;*/
/*    struct pollfd targets[2];*/
/*    int nready, len, end;*/
/*    char buf[1024];*/

/*    if (argc < 3) {*/
/*        fprintf(stderr, "%s host port\n", argv[0]);*/
/*        exit(1);*/
/*    }*/
/*    hostnm = argv[1];*/
/*    portnm = argv[2];*/

/*    memset(&hints, 0, sizeof(hints));*/
/*    hints.ai_socktype = SOCK_STREAM;*/
/*    error = getaddrinfo(hostnm, portnm, &hints, &res0);*/
/*    if (error) {*/
/*        fprintf(stderr, "getaddrinfo: %s %s:%s\n", hostnm, portnm, gai_strerror(error));*/
/*        return -1;*/
/*    }*/

/*    soc = -1;*/
/*    for (res = res0; res; res = res->ai_next) {*/
/*        error = getnameinfo(res->ai_addr, res->ai_addrlen, hbuf, sizeof(hbuf), sbuf,*/
/*                sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);*/
/*        if (error) {*/
/*            fprintf(stderr, "getnameinfo: %s %s:%s\n", hostnm, portnm, gai_strerror(error));*/
/*            continue;*/
/*        }*/
/*        fprintf(stderr, "trying %s port %s \n", hbuf, sbuf);*/
/*        soc = socket(res->ai_family, res->ai_socktype, res->ai_protocol);*/
/*        if (soc < 0) {*/
/*            continue;*/
/*        }*/
/*        if (connect(soc, res->ai_addr, res->ai_addrlen) < 0) {*/
/*            close(soc);*/
/*            soc = -1;*/
/*            continue;*/
/*        } else {*/
/*            fprintf(stderr, "connect: Success\n");*/
/*            break;*/
/*        }*/
/*    }*/
/*    freeaddrinfo(res0);*/

/*    if (soc < 0) {*/
/*        fprintf(stderr, "no destination to connect to\n");*/
/*        return -1;*/
/*    }*/
/*    targets[0].fd = soc;*/
/*    targets[0].events = POLLIN | POLLERR;*/
/*    targets[1].fd = fileno(stdin);*/
/*    targets[1].events = POLLIN | POLLERR;*/

/*    setbuf(stdout, NULL);*/

/*    end = 0;*/
/*    do {*/
/*        switch ((nready = poll(targets, 2, -1))) {*/
/*            case -1:*/
/*                if (errno != EINTR) {*/
/*                    perror("poll");*/
/*                    end = 1;*/
/*                }*/
/*                break;*/
/*            case 0:*/
/*                fprintf(stderr, "Timeout\n");*/
/*                break;*/
/*            default:*/
/*                if (targets[0].revents & (POLLIN | POLLERR)) {*/
/*                    if ((len = recv(soc, buf, sizeof(buf), 0)) < 0) {*/
/*                        perror("recv");*/
/*                        end = 1;*/
/*                    } else if (len == 0) {*/
/*                        fprintf(stderr, "recv: closed\n");*/
/*                        end = 1;*/
/*                    } else {*/
/*                        buf[len] = '\0';*/
/*                        fputs(buf, stdout);*/
/*                    }*/
/*                }*/
/*                if (targets[1].revents & (POLLIN | POLLERR)) {*/
/*                    if (fgets(buf, sizeof(buf), stdin) == NULL) {*/
/*                        perror("fgets");*/
/*                        end = 1;*/
/*                    } else {*/
/*                        char b[1024];*/
/*                        strcpy(b, buf);*/
/*                        conv(buf, b);*/
/*                        send(soc, buf, strlen(buf), 0);*/
/*                    }*/
/*                }*/
/*                break;*/
/*        }*/
/*    } while (end != 1);*/

/*    close(soc);*/

/*    return 0;*/
}
void conv(char* dest, char* src)
{
    char* p = src;
    char* p_dest = dest;
    while (*p) {
        if (*p == '\n') {
            *p_dest = '\r';
            p_dest++;
        }
        *p_dest = *p;
        p++;
        p_dest++;
    }
    *p_dest = '\0';
    printf("conv: %s\n", dest);
}
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
