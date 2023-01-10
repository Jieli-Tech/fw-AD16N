#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "typedef.h"

/************DAC_CLK**********************/
//for CLK_CON1
#define AUDIO_CLKDIV_BITS   (3<<23)
#define AUDIO_CLK_PLL48M    (0<<23)
#define AUDIO_CLK_OSC       (1<<23)
#define AUDIO_CLK_LSB       (2<<23)
#define AUDIO_CLK_DISABLE   (3<<23)

void audio_variate_init(void);

void audio_init(void);
void audio_off(void);
void audio_lookup(void);


void audio_clock_open(void);
void audio_clock_off(void);

void audio_common_digital_open();
void audio_common_digital_off();

void audio_analog_off(void);
void audio_analog_open(void);

bool ladc_capless_init(u32 delay);
void dac_power_on(u32 sr);


#endif
