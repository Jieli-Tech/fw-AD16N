#ifndef __AUDIO_DAC_FADE_H__
#define __AUDIO_DAC_FADE_H__
#include "typedef.h"
#include "audio_dac.h"
#include "app_config.h"

#define B_DAC_MUTE		BIT(0)
#define B_DAC_FADE_EN   BIT(1)
#define B_DAC_FADE_OUT  BIT(2)


/*---------DAC FADE IN & OUT Configuration---------------*/
#if defined(DAC_SOFT_FADE_EN) && (0 != DAC_SOFT_FADE_EN)
#define DAC_FADE_ENABLE     1
#else
#define DAC_FADE_ENABLE     0
#endif

#if TCFG_DAC_VOL_FADE_EN
void hardware_fade_in(void);
void hardware_fade_out(void);
void hardware_fade(void);
#define dac_fade_api()
#define dac_fade_in_api       hardware_fade_in
#define dac_fade_out_api      hardware_fade_out
#elif DAC_FADE_ENABLE
void dac_fade(void);
void dac_fade_in(void);
void dac_fade_out(u32 delay);
#define dac_fade_api          dac_fade
#define dac_fade_in_api       dac_fade_in
#define dac_fade_out_api()    dac_fade_out(200)
#else
void dac_fade(void);
void dac_fade_in(void);
void dac_fade_out(u32 delay);
#define dac_fade_api()
#define dac_fade_in_api()
#define dac_fade_out_api(...)
#endif

#endif
