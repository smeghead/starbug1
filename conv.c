#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_string.h"

static unsigned char *base64 = (unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void encode_char(unsigned long bb, int srclen, unsigned char *dist, int j)
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

void base64_encode(const unsigned char *src, unsigned char *dist)
{
    unsigned char *p = (unsigned char*)src;
    unsigned long bb = (unsigned long)0;
    int i = 0, j = 0;

    while (*p) {
        bb <<= 8;
        bb |= (unsigned long)*p;

        if (i == 2) {
            encode_char(bb, i, dist, j);
            j = j + 4;
            i = 0;
            bb = 0;
        } else {
            i++;
        }
        p++;
    }
    if (i) encode_char(bb, i - 1, dist, j);
}

typedef union {
    unsigned int    data;
    unsigned char   xyz[4];
} Union_data_t;

static long codetovalue(unsigned char c)
{
    if( (c >= (unsigned char)'A') && (c <= (unsigned char)'Z') ) {
        return (long)(c - (unsigned char)'A');
    }
    else if( (c >= (unsigned char)'a') && (c <= (unsigned char)'z') ) {
        return ((long)(c - (unsigned char)'a') +26);
    }
    else if( (c >= (unsigned char)'0') && (c <= (unsigned char)'9') ) {
        return ((long)(c - (unsigned char)'0') +52);
    }
    else if( (unsigned char)'+' == c ) {
        return (long)62;
    }
    else if( (unsigned char)'/' == c ) {
        return (long)63;
    }
    else if( (unsigned char)'=' == c ) {
        return (long)0;
    }
    else {
        return -1;
    }
}

static int decode_str(int enc_ptr, const unsigned char *src, unsigned char *dest)
{
    int i, j;
    unsigned long base64 = 0;
    unsigned char x;
    Union_data_t bb;

    for (i = enc_ptr; i < enc_ptr + 4; i++) {
        if (src[i] == '\0') return -1; /* '\0' apeared, and exit. */
        x = codetovalue(src[i]);
        base64 |= x;
        if ((i - enc_ptr) != 3) base64 <<= 6;
    }
    base64 <<= 8;
    bb.data = base64;

    for (j = 0, i = 3; i >= 1; i--) {
        dest[j++] = bb.xyz[i];
    }
    return 0;
}

int base64_decode(const unsigned char *src, unsigned char *dest)
{
    int i = 0;
    int srclen;
    char tmp[4];

    /* NULL pointer */
    if (src == NULL) return -1;

    srclen = strlen((char*)src);

    if (!srclen % 4) return -2;

    while (srclen) {
        memset(tmp, 0, sizeof(tmp));
        if (decode_str(i, src, (unsigned char*)tmp) != 0)
            return -1; /* 失敗した場合は、終了 */
        strcat((char*)dest, tmp);
        i += 4;
        srclen -= 4;
    }

    return 0;
}

unsigned long url_encode(unsigned char* csource, unsigned char* cbuffer, unsigned long lbuffersize)
{
    unsigned long llength;
    unsigned long lcount = 0;
    unsigned char cbyte;
    unsigned char ctemp[4];
    unsigned long lresultcount = 0;

    llength = (unsigned long)strlen((char*)csource);
    if (!llength) { return lresultcount; }
    if (lbuffersize < (llength * 3 + 1)) { return lresultcount; }

    while (1) {
        cbyte = *(csource + lcount);
        if(((cbyte >= 0x81) && (cbyte <= 0x9F)) ||
                ((cbyte >= 0xE0) && (cbyte <= 0xEF))) {            /* Shift-JIS 2 バイト文字だった場合 */
            sprintf((char*)ctemp, "%%%02X", cbyte);                        /* URL エンコード(上位バイト) */
            strncpy((char*)(cbuffer + lresultcount), (char*)ctemp, 4);
            lcount++;
            lresultcount += 3;
            if (lcount == llength) { break; }                        /* 文字列の終端に達した場合、ループを抜ける */
            sprintf((char*)ctemp, "%%%02X", *(csource + lcount));          /* URL エンコード(下位バイト) */
            strncpy((char*)cbuffer + lresultcount, (char*)ctemp, 4);
            lcount++;
            lresultcount += 3;
        } else if (((cbyte >= 0x40) && (cbyte <= 0x5A)) ||          /* @A-Z */
                ((cbyte >= 0x61) && (cbyte <= 0x7A)) ||             /* a-z */
                ((cbyte >= 0x30) && (cbyte <= 0x39)) ||             /* 0-9 */
                (cbyte == 0x2A) ||                                  /* "*" */
                (cbyte == 0x2D) ||                                  /* "-" */
                (cbyte == 0x2E) ||                                  /* "." */
                (cbyte == 0x5F)) {                                 /* "_" */ /* 無変換文字だった場合 */
            strncpy((char*)cbuffer + lresultcount, (char*)csource + lcount, 2);
            lcount++;
            lresultcount++;
        } else {                                                    /* その他の文字の場合 */
            sprintf((char*)ctemp, "%%%02X", cbyte);                        /* URL エンコード */
            strncpy((char*)cbuffer + lresultcount, (char*)ctemp, 4);
            lcount++;
            lresultcount += 3;
        }
        if (lcount == llength) { break; }
    }
    return lresultcount;                                            /* cbuffer に書き込んだ文字列のサイズを返す */
}

/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
