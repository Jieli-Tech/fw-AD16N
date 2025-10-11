
#ifndef _voiceChanger_av_api_h__
#define _voiceChanger_av_api_h__

#include "typedef.h"
/*#define  EFFECT_OLD_RECORD          0x01
#define  EFFECT_MOYIN               0x0*/
//#define  EFFECT_ROBORT_FLAG          0X04

enum {
    EFFECT_VC_AV_PITCHSHIFT = 0x00,
    EFFECT_VC_AV_F0_TD,
    EFFECT_VC_AV_F0_TD_AUTOFIT,
    EFFECT_VC_AV_PITCHSHIFT_AUTOFIT,
    EFFECT_VC_AV_BIRD,
    EFFECT_VC_AV_BIRD1,
    EFFECT_VC_AV_BIRD2,
    EFFECT_VC_AV_BIRD3,
    EFFECT_VC_AV_BIRD4,
    EFFECT_VC_AV_BIRD5,
    EFFECT_VC_AV_BIRD6
};


typedef  struct  _VOICESYN_AV_PARM_ {
    u32  vibrate_rate_u;   //0-100
    u32  vibrate_rate_d;   //0-100
    u32  vibrate_lenCtrol;   //0-100
    u32  randpercent;   //0-100
} VOICESYN_AV_PARM;


typedef struct _VOICECHANGER_AV_PARM {
    u32 effect_v;                    //
    u32 shiftv;                      //pitch rate:  40-250
    u32 formant_shift;               // 40-250
    u32 speedv;                      // 50-200
} VOICECHANGER_AV_PARM;


typedef struct _VCAD_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} VCAD_IO_CONTEXT;

typedef struct _VOICECHANGER_A_FUNC_API_ {
    u32(*need_buf)(u32 sr, VOICECHANGER_AV_PARM *vc_parm);
    int (*open)(void *ptr, u32 sr, VOICECHANGER_AV_PARM *vc_parm, VOICESYN_AV_PARM *vsyn_ctrol, VCAD_IO_CONTEXT *rs_io);       //中途改变参数，可以调init
    int (*run)(void *ptr, short *indata, int len);    //len是多少个byte
} VOICECHANGER_A_FUNC_API;

extern VOICECHANGER_A_FUNC_API *get_voiceChangerA_func_api();

#endif // reverb_api_h__

