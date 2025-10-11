#ifndef __AUDAC_BASIC_H__
#define __AUDAC_BASIC_H__

#include "typedef.h"
#include "audio_dac.h"
#include "sound_mge.h"
#include "app_config.h"

#define B_DAC_MUTE		BIT(0)
#define B_DAC_FADE_EN   BIT(1)
#define B_DAC_FADE_OUT  BIT(2)
#define B_DAC_DIG_VOL	BIT(3)

typedef struct _DAC_MANAGE {
    /*多路数据流句柄*/
    sound_out_obj *sound_pre[AUDAC_CHANNEL_TOTAL];
    sound_out_obj *sound_later[AUDAC_CHANNEL_TOTAL];
    /*多路数据流句柄所对应的触发函数*/
    void (*kick[AUDAC_CHANNEL_TOTAL])(void *);
    u8 ch;             /*通路标记*/
    u16 vol_l_phy;     /*左声道物理音量*/
    u16 vol_r_phy;     /*右声道物理音量*/
    u8 vol_l;          /*左声道音量*/
    u8 vol_r;          /*右声道音量*/
    u8 flag;           /*静音等控制标记*/
    /* u8 track; */
} DAC_MANAGE;


extern DAC_MANAGE dac_mge;


#endif

