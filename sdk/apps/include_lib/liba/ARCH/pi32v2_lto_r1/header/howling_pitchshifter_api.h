#ifndef HOWLING_pitchshifer_api_h__
#define HOWLING_pitchshifer_api_h__
#if PITCHSHIFT_HOWLING_EN
#include "typedef.h"
#include "app_modules.h"
/*#define  EFFECT_OLD_RECORD          0x01
#define  EFFECT_MOYIN               0x0*/
//#define  EFFECT_ROBORT_FLAG          0X04

enum {
    EFFECT_HOWLING_PS        = 0x01,              //1.5《=》12 ms
    EFFECT_HOWLING_FS       = 0x02
};

typedef struct HOWLING_PITCHSHIFT_PARM_ {
    s16 ps_parm;
    s16 fs_parm;
    u32 effect_v;
} HOWLING_PITCHSHIFT_PARM;


typedef struct _HOWL_PS_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} HOWL_PS_IO_CONTEXT;



typedef struct _HOWLING_PITCHSHIFT_FUNC_API_ {
    u32(*need_buf)(int flag);
    void (*open)(void *ptr, int sr, HOWLING_PITCHSHIFT_PARM *pitchshift_obj, HOWL_PS_IO_CONTEXT *how_ps_io);        //中途改变参数，可以调init
    int (*run)(void *ptr, short *indata,  int len);   //len是多少个byte
} HOWLING_PITCHSHIFT_FUNC_API;

extern HOWLING_PITCHSHIFT_FUNC_API *get_howling_ps_func_api();

void *link_pitchshift_howling_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr);
#endif
#endif // reverb_api_h__

