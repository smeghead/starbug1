#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

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


