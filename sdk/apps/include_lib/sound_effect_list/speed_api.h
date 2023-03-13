#ifndef __SPEED_API_H__
#define __SPEED_API_H__
#include "typedef.h"
#include "app_modules.h"

#if HAS_SPEED_EN
#include "lib_speed_api.h"
#include "sound_mge.h"

#if 0
typedef struct _RS_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} RS_IO_CONTEXT;


typedef struct _RS_PARA_STRUCT_ {
    unsigned short insample;
    unsigned short outsample;
} RS_PARA_STRUCT;


typedef struct _SPEED_PITCH_PARA_STRUCT_ {
    unsigned short insample;
    unsigned short pitchrate;        //128<=>1
    unsigned char  quality;
    unsigned char speedin;
    unsigned char speedout;
    unsigned char pitchflag;
} SPEED_PITCH_PARA_STRUCT;

typedef struct  _RS_STUCT_API_ {
    unsigned int(*need_buf)();
    int (*open)(unsigned int *ptr, RS_PARA_STRUCT *rs_para, RS_IO_CONTEXT *rs_io);
    int (*run)(unsigned int *ptr, short *inbuf, int len);
} RS_STUCT_API;

RS_STUCT_API *get_resample_context();

typedef struct  _SPEEDPITCH_STUCT_API_ {
    unsigned int(*need_buf)(int srv);
    int(*open)(unsigned int *ptr, SPEED_PITCH_PARA_STRUCT *speedpitch_para, RS_IO_CONTEXT *rs_io);
    int(*run)(unsigned int *ptr, short *inbuf, int len);
} SPEEDPITCH_STUCT_API;

SPEEDPITCH_STUCT_API *get_sppitch_context();


#endif



// void *speed_api(void *obuf, u32 insample);
//void *speed_api(void *obuf,u32 insample, u32 outsample);
void set_speed_parm(EFFECT_OBJ *e_obj, SPEED_PITCH_PARA_STRUCT *new_sp_parm);
void *speed_api(void *obuf, u32 insample, void **ppsound);
void *speed_phy(void *obuf, SPEED_PITCH_PARA_STRUCT *psp_parm, void **ppsound);
void *link_speed_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 in_sr);


#endif
#endif

