#ifndef _PRINTF_DEFINED_
#define _PRINTF_DEFINED_
#include "typedef.h"
int sprintf(char *out, const char *format, ...);



int puts(const char *out);
int printf(const char *format, ...);
int vprintf(const char *fmt, __builtin_va_list va);
int vsnprintf(char *, unsigned long, const char *, __builtin_va_list);
int snprintf(char *buf, unsigned long size, const char *fmt, ...);
int sscanf(const char *buf, const char *fmt, ...);
void printf_buf(const u8 *buf, int len);



#ifndef _FCVT_DEFINED_
#define _FCVT_DEFINED_

#define CVTBUFSIZE  64

char *ecvtbuf(double arg, int ndigits, int *decpt, int *sign, char *buf);
char *fcvtbuf(double arg, int ndigits, int *decpt, int *sign, char *buf);

#endif



#endif
