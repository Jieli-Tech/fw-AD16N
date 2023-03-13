/***********************************Jieli tech************************************************
  File : dac_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2019-1-14
********************************************************************************************/

#pragma bss_seg(".audio_dac_api.data.bss")
#pragma data_seg(".audio_dac_api.data")
#pragma const_seg(".audio_dac_api.text.const")
#pragma code_seg(".audio_dac_api.text")
#pragma str_literal_override(".audio_dac_api.text.const")

#include "app_modules.h"
#include "audio_dac_api.h"
#include "audac_basic.h"
/* #include "circular_buf.h" */
#include "string.h"
#include "uart.h"
#include "config.h"
#include "audio.h"
/* #include "decoder_api.h" */
#include "audio_analog.h"
#include "clock.h"
#include "efuse.h"
#include "audio_energy_detect.h"
#include "circular_buf.h"
#include "audio_dac_fade.h"
#include "app_config.h"

#if HAS_MIO_EN
#include "mio_api.h"
#endif

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[audio_dac_api]"
#include "log.h"

/* #include "printf.h" */
u16 audio_dac_buf[DAC_PACKET_SIZE * 2];

#define DAC_MAX_SP  (sizeof(audio_dac_buf) / (DAC_TRACK_NUMBER * (AUDAC_BIT_WIDE / 8)))

#define DAC_PNS     (DAC_MAX_SP/3)

#if (TCFG_DAC_VOL_FADE_EN == 0)
#define A_DAC_FADE   A_DAC_FADE_DIS
#else
#define A_DAC_FADE   0
#endif

_OTP_CONST_  DAC_CTRL_HDL audio_dac_ops = {
    .buf         = audio_dac_buf,
    .sp_total    = DAC_MAX_SP,
    .sp_max_free = (DAC_MAX_SP - DAC_PNS) / 2,
    .sp_size     = DAC_TRACK_NUMBER * (AUDAC_BIT_WIDE / 8),
    .con         = DAC_CON0_DEFAULT | A_DAC_FADE,
    .pns         = DAC_PNS,
};

/* #define sound_out_obj void */


const u16 vol_tab[] = {
    0	,
    164	,
    191	,
    223	,
    260	,
    303	,
    353	,
    412	,
    480	,
    559	,
    652	,
    760	,
    887	,
    1034	,
    1205	,
    1405	,
    1638	,
    1910	,
    2227	,
    2597	,
    3028	,
    3530	,
    4115	,
    4798	,
    5594	,
    6523	,
    7605	,
    8867	,
    10338	,
    12053	,
    14052	,
    16384
};




#define MAX_VOL_LEVEL ((sizeof(vol_tab)/2) - 1)
#define MAX_PHY_VOL   vol_tab[MAX_VOL_LEVEL]

DAC_MANAGE dac_mge;



/* s16 sp_dac_buf[DAC_PACKET_SIZE] AT(.DAC_BUFFER); */
/*----------------------------------------------------------------------------*/
/**@brief   dac模块初始化
   @param   无
   @return  无
   @author  liujie
   @note    void dac_mode_init(u16 vol)
*/
/*----------------------------------------------------------------------------*/
void dac_mode_init(u16 vol)
{
    memset(&dac_mge, 0, sizeof(dac_mge));
#if DAC_FADE_ENABLE

    if (vol > MAX_VOL_LEVEL) {
        vol = MAX_VOL_LEVEL;
    }
    dac_mge.vol_l = vol;
    dac_mge.vol_r = vol;

#else
    stereo_dac_vol(0, vol, vol);
#endif

    memset(&audio_dac_buf[0], 0, sizeof(audio_dac_buf));
    /* u32 con = dac_mode_check(DAC_DEFAULT); */
    dac_resource_init((void *)&audio_dac_ops);
}

static void dac_trim_api(void)
{
    u32 dac_trim_value = efuse_get_audio_rdac_trim();
    /* log_info("dac_trim_value : 0x%x\n", dac_trim_value); */
    if (dac_trim_value != 0xffffffff) {
        JL_AUDIO->DAC_TM0 = dac_trim_value;
    } else {
        /* default */
        JL_AUDIO->DAC_TM0 = ((-1024 & 0xffff) << 16) | (-1024 & 0xffff);
    }
}

void dac_init_api(u32 sr)
{
    dac_phy_init(dac_sr_lookup(sr));
    udelay(2000);
    dac_trim_api();
    audio_dac_analog_open();
    audio_dac_analog_vol(audio_dac_analog_vol_l, audio_dac_analog_vol_r);
}

void dac_sr_api(u32 sr)
{
    /* u32 dac_sr_set(u32 sr) */
    dac_sr_set(dac_sr_lookup(sr));
    /* dac_analog_init(); */
}

void dac_off_api(void)
{
    /* rdac_analog_close(); */
    /* apa_analog_close(); */
    dac_phy_off();
}
/* void dac_sr_api(u32 sr) */
/* { */
/* dac_sr_set(dac_sr_lookup(sr)); */
/* } */

void sound_out_hook(sound_out_obj *psound)
{
#if HAS_MIO_EN
    if (NULL != psound) {
        d_mio_start(psound->mio);
    }
#endif
}

/* #include "common/audio/fill_audac.c" */

u8 stereo_dac_vol(char set, u8 vol_l, u8 vol_r)
{

    if ('+' == set) {
        dac_mge.vol_l++;
        dac_mge.vol_r++;
    } else if ('-' == set) {
        if (0 != dac_mge.vol_l) {
            dac_mge.vol_l--;
        }
        if (0 != dac_mge.vol_r) {
            dac_mge.vol_r--;
        }
    } else if ('r' == set) {
        return dac_mge.vol_l;
    } else if ('R' == set) {
        return dac_mge.vol_r;
    } else {
        /* log_info("vol_l: %d , vol_r: %d", vol_l, vol_r); */
        dac_mge.vol_l = vol_l;
        dac_mge.vol_r = vol_r;
    }
    if (dac_mge.vol_l > MAX_VOL_LEVEL) {
        dac_mge.vol_l = MAX_VOL_LEVEL;
    }
    if (dac_mge.vol_r > MAX_VOL_LEVEL) {
        dac_mge.vol_r = MAX_VOL_LEVEL;
    }
    if (0 == (dac_mge.flag & B_DAC_MUTE)) {
#if DAC_FADE_ENABLE
        if (0 == (dac_mge.flag & B_DAC_FADE_EN))
#endif
        {
            dac_mge.vol_l_phy = vol_tab[dac_mge.vol_l];
            dac_mge.vol_r_phy = vol_tab[dac_mge.vol_r];
        }
        dac_mge.flag &= ~B_DAC_MUTE;
    }
    /* log_info(" dac vol %d, 0x%x\n", dac_mge.vol, dac_mge.vol_phy); */
__dac_vol_end:
    dac_phy_vol(dac_mge.vol_l_phy, dac_mge.vol_r_phy);
    return dac_mge.vol_l;
}
u8 dac_vol(char set, u8 vol)
{
    return stereo_dac_vol(set, vol, vol);
}


bool dac_mute(bool mute)
{
    if (mute) {
        dac_mge.flag |= B_DAC_MUTE;
        dac_mge.vol_l_phy = 0;
        dac_mge.vol_r_phy = 0;
    } else {
        dac_mge.flag &= ~B_DAC_MUTE;
        dac_mge.vol_l_phy = vol_tab[dac_mge.vol_l];
        dac_mge.vol_r_phy = vol_tab[dac_mge.vol_r];
    }
    dac_phy_vol(dac_mge.vol_l_phy, dac_mge.vol_r_phy);
    return true;
}

bool regist_dac_channel(void *psound, void *kick)
{
    u8 i;
    for (i = 0; i < AUDAC_CHANNEL_TOTAL; i++) {
        if (dac_mge.ch & BIT(i)) {
            continue;
        }
        /* dac_mge.obuf[i] = obuf; */
        dac_mge.kick[i] = kick;
        dac_mge.sound[i] = psound;
        dac_mge.ch |= BIT(i);
        /* log_info("dac_channel :0x%x 0x%x\n", i, dac_mge.ch); */
        return true;
    }
    return false;
}

bool unregist_dac_channel(void *psound)
{
    u8 i;
    sound_out_obj *ps = psound;

    for (i = 0; i < AUDAC_CHANNEL_TOTAL; i++) {
        if (0 == (dac_mge.ch & BIT(i))) {
            continue;
        }

        if (dac_mge.sound[i] == psound) {
            local_irq_disable();
            dac_mge.ch &= ~BIT(i);
            /* dac_mge.obuf[i] = 0; */
            dac_mge.sound[i] = 0;
            dac_mge.kick[i] = NULL;
            ps->enable &= ~B_DEC_OBUF_EN;
            local_irq_enable();
            break;
        }
    }
    return true;
}


AT(.audio_isr_text)
bool dac_cbuff_active(void *sound_hld)
{
    sound_out_obj *psound = sound_hld;
    if (psound->enable & (B_DEC_PAUSE | B_DEC_FIRST)) {
        if (cbuf_get_data_size(psound->p_obuf) >= (cbuf_get_space(psound->p_obuf) / 2)) {
            psound->enable &= ~B_DEC_FIRST;
        }
        return false;
    } else {
        return true;
    }
    /* if (psound->enable & B_DEC_PAUSE) { */
    /*     return false; */
    /* } else { */
    /*     return true; */
    /* } */
}

