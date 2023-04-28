
/***********************************Jieli tech************************************************
  File : dac_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2019-1-14
********************************************************************************************/

#pragma bss_seg(".audio_dac_cpu.data.bss")
#pragma data_seg(".audio_dac_cpu.data")
#pragma const_seg(".audio_dac_cpu.text.const")
#pragma code_seg(".audio_dac_cpu.text")
#pragma str_literal_override(".audio_dac_cpu.text.const")

#include "audio_dac.h"
#include "audio_dac_api.h"
#include "audio.h"
#include "audio_analog.h"
#include "app_config.h"
#include "efuse.h"

#define LOG_TAG_CONST       NORM
/* #define LOG_TAG_CONST       OFF */
#define LOG_TAG             "[audac_cpu]"
#include "log.h"

u32 const audio_dac_con1 = audac_fade_slow(TCFG_DAC_VOL_FADE_SLOW) | audac_fade_step(TCFG_DAC_VOL_FADE_STEP) | AUDAC_SRC_EN;

/* *******************
 * audio dac 功夫接线方式
 *
 * */
u32 audac_track_number(void)
{
    if (A_DAC_STERO == AUDAC_OUTPUT_MODE) {
        return 2;
    } else {
        return 1;
    }
}

#include "audio_dac_api.h"
#include "app_config.h"
u16 audio_dac_buf[DAC_PACKET_SIZE * 2];

#define DAC_MAX_SP  (sizeof(audio_dac_buf) / (DAC_TRACK_NUMBER * (AUDAC_BIT_WIDE / 8)))

#define DAC_PNS     (DAC_MAX_SP/3)

#if (TCFG_DAC_VOL_FADE_EN == 0)
#define A_DAC_FADE   A_DAC_FADE_DIS
#else
#define A_DAC_FADE   0
#endif

/* #define sound_out_obj void */
_OTP_CONST_  DAC_CTRL_HDL audio_dac_ops = {
    .buf         = audio_dac_buf,
    .sp_total    = DAC_MAX_SP,
    .sp_max_free = (DAC_MAX_SP - DAC_PNS) / 2,
    .sp_size     = DAC_TRACK_NUMBER * (AUDAC_BIT_WIDE / 8),
    .con         = DAC_CON0_DEFAULT | A_DAC_FADE,
    .pns         = DAC_PNS,
};

void audac_clr_buf(void)
{
    memset(&audio_dac_buf[0], 0, sizeof(audio_dac_buf));
}

void dac_trim_api(void)
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

void audac_analog_open_api(u32 delay_flag)
{
    audio_dac_analog_open(delay_flag);
    audio_dac_analog_vol(audio_dac_analog_vol_l, audio_dac_analog_vol_r);
}
