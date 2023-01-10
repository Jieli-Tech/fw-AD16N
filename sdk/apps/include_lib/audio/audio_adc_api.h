#ifndef __AU_ADC_API_H__
#define __AU_ADC_API_H__

#include "typedef.h"
// #include "audio_analog.h"

typedef enum __AUDIO_ADC_MODE {
    AUDIO_ADC_MIC,
    AUDIO_ADC_LINEIN,
} AUDIO_ADC_MODE;




void single_micin_analog_open(void);
void single_linin_analog_open(void);



void audio_adc_mode_init(void);
void audio_adc_isr(void);
u32 audio_adc_init_api(u32 sr, AUDIO_ADC_MODE mode, u32 ch);
void audio_adc_off_api(void);


void fill_audio_adc_fill(u8 *buf, u32 len);
bool regist_audio_adc_channel(void *psound, void *kick);
bool unregist_audio_adc_channel(void *psound);



#endif
