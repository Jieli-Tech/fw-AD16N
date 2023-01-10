#ifndef _LOG_H_
#define _LOG_H_

#include "cpu.h"
#include "typedef.h"
#include "uart.h"

#define LOG_ASSERT_ENABLE

// void printf_buf(u8 *buf, u32 len);
int printf(const char *format, ...);
void printf_buf(const u8 *buf, int len);

#define PRINTF(format, ...)         printf(format, ## __VA_ARGS__)

#define LOG_VERBOSE     v
#define LOG_INFO        i
#define LOG_DEBUG       d
#define LOG_WARN        w
#define LOG_ERROR       e
#define LOG_CHAR        c

#define _STR(x)     #x
#define STR(x)     "["_STR(x)"]"


#define _LOG_TAG_CONST_DECLARE(level, name)         extern const char log_tag_const_##level##_##name
#define LOG_TAG_CONST_DECLARE(level, name)          _LOG_TAG_CONST_DECLARE(level, name)

#define ___LOG_IS_ENABLE(level, name)   (log_tag_const_##level##_##name)
#define __LOG_IS_ENABLE(level, name)    ___LOG_IS_ENABLE(level, name)
#define _LOG_IS_ENABLE(level)           __LOG_IS_ENABLE(level, LOG_TAG_CONST)

#ifdef LOG_TAG_CONST
LOG_TAG_CONST_DECLARE(LOG_VERBOSE,  LOG_TAG_CONST);
LOG_TAG_CONST_DECLARE(LOG_INFO,     LOG_TAG_CONST);
LOG_TAG_CONST_DECLARE(LOG_DEBUG,    LOG_TAG_CONST);
LOG_TAG_CONST_DECLARE(LOG_WARN,     LOG_TAG_CONST);
LOG_TAG_CONST_DECLARE(LOG_ERROR,    LOG_TAG_CONST);
LOG_TAG_CONST_DECLARE(LOG_CHAR,     LOG_TAG_CONST);
// #define _LOG_TAG                        STR(LOG_TAG_CONST)
#define _LOG_TAG            LOG_TAG
#define LOG_IS_ENABLE(level)            _LOG_IS_ENABLE(level)
#else
#define _LOG_TAG            LOG_TAG
#define LOG_IS_ENABLE(x)    1
#endif

// #define log_print printf
#define log_xprint _log

extern int _log(const char *str1, const char *str2, const char *format, ...);

#define log_info(format, ...)       \
    if (LOG_IS_ENABLE(LOG_INFO)) \
        log_xprint("[Info]", _LOG_TAG, format , ## __VA_ARGS__)

#define log_noinfo(format, ...)       \
    if (LOG_IS_ENABLE(LOG_INFO)) \
        printf(format , ## __VA_ARGS__)

#define log_info_hexdump(x, y)     \
    if (LOG_IS_ENABLE(LOG_INFO)) \
        printf_buf(x, y)


#define log_debug(format, ...)       \
    if (LOG_IS_ENABLE(LOG_DEBUG)) \
        log_xprint("[Debug]", _LOG_TAG, format , ## __VA_ARGS__)

#define log_debug_hexdump(x, y)     \
    if (LOG_IS_ENABLE(LOG_DEBUG)) \
        printf_buf(x, y)

#define log_error(format, ...)       \
    if (LOG_IS_ENABLE(LOG_ERROR)) \
        log_xprint("<Error>", _LOG_TAG, format , ## __VA_ARGS__)

#define log_error_hexdump(x, y)     \
    if (LOG_IS_ENABLE(LOG_ERROR)) \
        printf_buf(x, y)

#define log_char(x)       \
    if (LOG_IS_ENABLE(LOG_CHAR)) \
		putchar(x)



#define traceSUPER_MODE() \

/*{int icfg, rets, reti; \
__asm__ volatile("%0 = icfg" : "=r"(icfg)); \
__asm__ volatile("%0 = rets" :"=r"(rets)); \
__asm__ volatile("%0 = reti" :"=r"(reti)); \
ASSERT(icfg & BIT(10), "icfg 0x%x/ rets 0x%x / reti 0x%x", icfg, rets, reti)}*/

#define r_printf(x, ...)  log_info("\e[31m\e[1m" x "\e[0m", ## __VA_ARGS__)
#define g_printf(x, ...)  log_info("\e[32m\e[1m" x "\e[0m", ## __VA_ARGS__)
#define y_printf(x, ...)  log_info("\e[33m\e[1m" x "\e[0m", ## __VA_ARGS__)
#define r_f_printf(x, ...)  log_info("\e[31m\e[5m\e[1m" x "\e[0m", ## __VA_ARGS__)
#define g_f_printf(x, ...)  log_info("\e[32m\e[5m\e[1m" x "\e[0m", ## __VA_ARGS__)
#define y_f_printf(x, ...)  log_info("\e[33m\e[5m\e[1m" x "\e[0m", ## __VA_ARGS__)



#endif//__DEBUG_LOG_H_

