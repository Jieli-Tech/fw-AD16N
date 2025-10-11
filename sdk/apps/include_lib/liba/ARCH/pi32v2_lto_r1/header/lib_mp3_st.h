#ifndef __LIB_MP3_ST_H__
#define __LIB_MP3_ST_H__

#define MP3_ST_DBUF_SIZE        (0x3cfc)
#define MP3_ST_TRACK            B_STEREO
#define MP3_ST_OUTPUT_MAX_SIZE  (32 * 2 * 2)//32 * 2byte * 2ch

// 格式检查需要做超时处理
#define MP3_ST_INPUT            fix_mp3_format_check_input
#define MP3_ST_FORMAT_CHECK_ENTER() \
    do { \
        dec_mp3_st_hld.function |= DEC_FORMAT_CHECK_FIX; \
        format_check_start_jiffies = maskrom_get_jiffies(); \
    } while(0)
#define MP3_ST_FORMAT_CHECK_EXIT()  \
    do { \
        dec_mp3_st_hld.function &= ~DEC_FORMAT_CHECK_FIX; \
    } while(0)

#define MP3_ST_DEC_CONFING() \
    do { \
        modevalue.mode = 1;/*output是否判断返回值*/ \
        ops->dec_confing(MP3_ST_CAL_BUF, SET_DECODE_MODE, &modevalue); \
    } while(0)

#endif
