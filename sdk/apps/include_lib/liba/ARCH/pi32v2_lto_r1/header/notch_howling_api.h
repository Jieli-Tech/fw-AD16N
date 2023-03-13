#ifndef NOTCH_HOWLING_API_H
#define NOTCH_HOWLING_API_H
#include "typedef.h"
#include "app_modules.h"
#if 1//def WIN32

#define AT_NOTCHHOWLING(x)
#define AT_NOTCHHOWLING_CODE
#define AT_NOTCHHOWLING_CONST
#define AT_NOTCHHOWLING_SPARSE_CODE
#define AT_NOTCHHOWLING_SPARSE_CONST

#else
#define AT_NOTCHHOWLING(x)           __attribute((section(#x)))
#define AT_NOTCHHOWLING_CODE         AT_NOTCHHOWLING(.notchhowling_code)
#define AT_NOTCHHOWLING_CONST        AT_NOTCHHOWLING(.notchhowling_const)
#define AT_NOTCHHOWLING_SPARSE_CODE  AT_NOTCHHOWLING(.notchhowling_sparse_code)
#define AT_NOTCHHOWLING_SPARSE_CONST AT_NOTCHHOWLING(.notchhowling_sparse_const)
#endif

typedef struct _NotchHowlingParam {
    int gain;
    int Q;
    int fade_time;
    int threshold;
    int SampleRate;
} NotchHowlingParam;

typedef struct _NH_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} NH_IO_CONTEXT;

typedef struct _NH_STRUCT_API_ {
    int(*need_buf)(NotchHowlingParam *param);
    void(*open)(void *workBuf, NotchHowlingParam *para, NH_IO_CONTEXT *io);
    void(*update)(void *workBuf, NotchHowlingParam *para);
    int(*run)(void *workBuf, short *indata, int len); //len 为indata 字节数，返回值为实际消耗indata的字节数
} NH_STRUCT_API;

NH_STRUCT_API *get_notchHowling_ops();


void *link_notch_howling_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr);

#endif // !NOTCH_HOWLING_API_H

