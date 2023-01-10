
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
#include "audio.h"
#include "audio_analog.h"
#include "app_config.h"

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


