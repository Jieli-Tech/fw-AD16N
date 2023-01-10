#ifndef __AUDIO_ANALOG_H__
#define __AUDIO_ANALOG_H__

#include "config.h"
// #define BIT_APA     BIT(0)
// #define BIT_DAA     BIT(1)
// #define BIT_CAPLESS BIT(2)
// extern u32 audio_analog;
// extern u32 app_power_get_vbat(void);
// #define audio_ask_power() app_power_get_vbat()

// typedef enum __ADC_MODE {
// ADC_MIC = 0,
// ADC_LINE_IN,
// } ADC_MODE;

// extern u32 audio_analog;

// void audio_analog_open(void);
// void audio_analog_close(void);
// void audio_analog_lookup(u32 flag, bool mode);
// void apa_analog_open(void);
// void apa_analog_close(void);
// void rdac_analog_open(void);
// void rdac_analog_close(void);

// u32 adc_analog_open(ADC_MODE mode, u32 ch);
// void adc_analog_close(void);
// void dacvdd_ldo(bool mode);

void audio_analog_open(void);
void audio_dac_analog_open(void);

#endif
