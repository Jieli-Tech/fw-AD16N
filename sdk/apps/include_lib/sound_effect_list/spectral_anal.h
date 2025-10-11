#ifndef __SPECTRAL_ANAL_H__
#define __SPECTRAL_ANAL_H__

#include "typedef.h"
#include "sound_mge.h"
#include "hw_eq.h"
#include "follow.h"

#define MAX_POINT    (320)

typedef struct _SA_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} _SPECTRAL_ANAL_IO_CONTEXT;

typedef struct  _SPECTRAL_ANAL_PARA_STRUCT_ {
    struct eq_seg_info *seg;
    unsigned short sample_rate;
    unsigned short max_point;
    unsigned char max_seg;
    unsigned char ch;
} _SPECTRAL_ANAL_PARA_STRUCT;

typedef struct  _SPECTRAL_ANAL_OPS_ {
    unsigned int(*need_buf)(u32 max_seg, FollowParam *fo_parm, u16 max_point);
    int *(*open)(unsigned int *ptr, _SPECTRAL_ANAL_IO_CONTEXT *rsio, _SPECTRAL_ANAL_PARA_STRUCT *parm, FollowParam *fo_parm);
    int (*run)(unsigned int *ptr, short *inbuf, int len);
    u32(*config)(void);
} _SPECTRAL_ANAL_OPS;

typedef struct {
    void *start_offset;
    u32 size;
    u32 part_size;
} BUFF_INFO;

typedef struct  _SPECTRAL_ANAL_BUFF_ {
    _SPECTRAL_ANAL_IO_CONTEXT   *io;
    _SPECTRAL_ANAL_OPS          *ops;
    _SPECTRAL_ANAL_PARA_STRUCT  para;
    BUFF_INFO                   coeff_buff_info;
    BUFF_INFO                   eq_buff_info;
    BUFF_INFO                   follow_buff_info;
    u16                         rmlen;
    u16                         rmcnt;
    u32                         buff[0];
} _SPECTRAL_ANAL_BUFF;

typedef struct _SPECTRAL_ANAL_HDL_ {
    EFFECT_OBJ obj;
    sound_in_obj si;
    _SPECTRAL_ANAL_IO_CONTEXT   io;
    _SPECTRAL_ANAL_BUFF         buff;
} _SPECTRAL_ANAL_HDL;

void *link_spectral_anal_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr, u8 ch);
extern const _SPECTRAL_ANAL_OPS *get_spectral_anal_context(void);
#endif
