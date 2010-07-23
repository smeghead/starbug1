#ifndef CONV_H
#define CONV_H
/* string convert utility functions. */
int base64_decode(const unsigned char*, unsigned char*);
void base64_encode(const unsigned char*, unsigned char*);
unsigned long url_encode(unsigned char*, unsigned char*, unsigned long);
#endif
/* vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8: */
