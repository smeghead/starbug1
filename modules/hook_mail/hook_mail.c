#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include "../../data.h"
#include "../../util.h"
#include "../../simple_string.h"

static int wait(int soc, struct pollfd *target, int timeout)
{
    int ret = 0;
    while(1) {
        ret = poll(target, 1, timeout);
        d("poll ret %d\n", ret);
        if (ret == -1 && errno != EINTR) {
            /* エラー */
            return -1;
        } else if (ret == 0) {
            /* timeout */
        d("poll timeout \n");
            return -2;
        }
        if (target->revents & (POLLIN | POLLERR)) {
            char buf[DEFAULT_LENGTH];
            recv(soc, buf, sizeof(buf), 0);
            d("recv: %s\n", buf);
            /* 送信準備OK */
            return 0;
        }
    }
}
int send_data(int soc, char* buf)
{
    d("send: %s\n", buf);
    send(soc, buf, strlen(buf), 0);
    return 0;
}
int build_data(String* buf, char* base_url, char* project_id, Project* project, Message* message, List* elements, List* element_types)
{
    string_appendf(buf, "From: %s\r\n", FROM);
    string_appendf(buf, "To: %s\r\n", TO);
    string_appendf(buf, "Subject: [%s:%d]Starbug1 notify.\r\n", project_id, message->id);
    string_appendf(buf, "Content-Type: text/plain;\r\n");
    string_appendf(buf, "\r\n");
    string_appendf(buf, "a ticket has updated. please check this url.\r\n");
    string_appendf(buf, " %s/%s/ticket/%d\r\n", base_url, project_id, message->id);
    string_appendf(buf, "\r\n.\r\n");
    return 0;
}
int execute(char* base_url, char* project_id, Project* project, Message* message, List* elements, List* element_types)
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
        fprintf(stderr, "getaddrinfo: %s %s:%s\n", hostnm, portnm, gai_strerror(error));
        return -1;
    }

    soc = -1;
    for (res = res0; res; res = res->ai_next) {
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hbuf, sizeof(hbuf), sbuf,
                sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
        if (error) {
            fprintf(stderr, "getnameinfo: %s %s:%s\n", hostnm, portnm, gai_strerror(error));
            continue;
        }
        fprintf(stderr, "trying %s port %s \n", hbuf, sbuf);
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
            fprintf(stderr, "connect: Success\n");
            break;
        }
    }
    freeaddrinfo(res0);

    if (soc < 0) {
        fprintf(stderr, "no destination to connect to\n");
        return -1;
    }
    target.fd = soc;
    target.events = POLLIN | POLLERR;

    setbuf(stdout, NULL);

    d("=====\n");
    {
        int ret;
        String* data_string_a = string_new(0);
        char command[DEFAULT_LENGTH];

    d("start \n");
        ret = wait(soc, &target, 5000);
    d("ret %d\n", ret);
        if (ret < 0) return ret;
        sprintf(command, "HELO %s\r\n", SMTP_SERVER);
        send_data(soc, command);
        ret = wait(soc, &target, 5000);
    d("ret %d\n", ret);
        if (ret < 0) return ret;
        sprintf(command, "MAIL FROM: %s\r\n", FROM);
        send_data(soc, command);
        ret = wait(soc, &target, 5000);
    d("ret %d\n", ret);
        if (ret < 0) return ret;
        sprintf(command, "RCPT TO: %s\r\n", TO);
        send_data(soc, command);
        ret = wait(soc, &target, 5000);
    d("ret %d\n", ret);
        if (ret < 0) return ret;
        sprintf(command, "DATA\r\n");
        send_data(soc, command);

        /* data */
        build_data(data_string_a, base_url, project_id, project, message, elements, element_types);
        ret = wait(soc, &target, 5000);
        if (ret < 0) return ret;
        send_data(soc, string_rawstr(data_string_a));

        ret = wait(soc, &target, 5000);
        if (ret < 0) return ret;
    d("ret %d\n", ret);
        send_data(soc, "QUIT\r\n");
    }
    d("=====\n");
    d("close\n");
    close(soc);

    return 0;
}
void conv(char*, char*);
int main(int argc, char** argv)
{
    char* hostnm;
    char* portnm;
    struct addrinfo hints, *res, *res0;
    int soc;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXHOST];
    int error;
    struct pollfd targets[2];
    int nready, len, end;
    char buf[1024];

    if (argc < 3) {
        fprintf(stderr, "%s host port\n", argv[0]);
        exit(1);
    }
    hostnm = argv[1];
    portnm = argv[2];

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    error = getaddrinfo(hostnm, portnm, &hints, &res0);
    if (error) {
        fprintf(stderr, "getaddrinfo: %s %s:%s\n", hostnm, portnm, gai_strerror(error));
        return -1;
    }

    soc = -1;
    for (res = res0; res; res = res->ai_next) {
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hbuf, sizeof(hbuf), sbuf,
                sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
        if (error) {
            fprintf(stderr, "getnameinfo: %s %s:%s\n", hostnm, portnm, gai_strerror(error));
            continue;
        }
        fprintf(stderr, "trying %s port %s \n", hbuf, sbuf);
        soc = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (soc < 0) {
            continue;
        }
        if (connect(soc, res->ai_addr, res->ai_addrlen) < 0) {
            close(soc);
            soc = -1;
            continue;
        } else {
            fprintf(stderr, "connect: Success\n");
            break;
        }
    }
    freeaddrinfo(res0);

    if (soc < 0) {
        fprintf(stderr, "no destination to connect to\n");
        return -1;
    }
    targets[0].fd = soc;
    targets[0].events = POLLIN | POLLERR;
    targets[1].fd = fileno(stdin);
    targets[1].events = POLLIN | POLLERR;

    setbuf(stdout, NULL);

    end = 0;
    do {
        switch ((nready = poll(targets, 2, -1))) {
            case -1:
                if (errno != EINTR) {
                    perror("poll");
                    end = 1;
                }
                break;
            case 0:
                fprintf(stderr, "Timeout\n");
                break;
            default:
                if (targets[0].revents & (POLLIN | POLLERR)) {
                    if ((len = recv(soc, buf, sizeof(buf), 0)) < 0) {
                        perror("recv");
                        end = 1;
                    } else if (len == 0) {
                        fprintf(stderr, "recv: closed\n");
                        end = 1;
                    } else {
                        buf[len] = '\0';
                        fputs(buf, stdout);
                    }
                }
                if (targets[1].revents & (POLLIN | POLLERR)) {
                    if (fgets(buf, sizeof(buf), stdin) == NULL) {
                        perror("fgets");
                        end = 1;
                    } else {
                        char b[1024];
                        strcpy(b, buf);
                        conv(buf, b);
                        send(soc, buf, strlen(buf), 0);
                    }
                }
                break;
        }
    } while (end != 1);

    close(soc);

    return 0;
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


