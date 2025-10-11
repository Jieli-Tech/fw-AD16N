/***********************************Jieli tech************************************************
  File : adc_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2019-1-14
********************************************************************************************/
#include "uart.h"
#include "config.h"
#include "audio.h"
#include "audio_adc.h"
#include "audio_adc_api.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[audio_adc_cpu]"
#include "log.h"


void auadc_open_mic(void)
{
    single_micin_analog_open();
    if (mic_rs_outside == audio_adc_mic_rs_mode) {
        JL_PORTA->DIR |= BIT(0);
        JL_PORTA->DIE &= ~BIT(0);
        JL_PORTA->PU0 &= ~BIT(0);
        JL_PORTA->PD0 &= ~BIT(0);
    }
    if (mic_input_pa1 == audio_adc_mic_input_mode) {
        JL_PORTA->DIR |= BIT(1);
        JL_PORTA->DIE &= ~BIT(1);
        JL_PORTA->PU0 &= ~BIT(1);
        JL_PORTA->PD0 &= ~BIT(1);
    } else if (mic_input_pa2 == audio_adc_mic_input_mode) {
        JL_PORTA->DIR |= BIT(2);
        JL_PORTA->DIE &= ~BIT(2);
        JL_PORTA->PU0 &= ~BIT(2);
        JL_PORTA->PD0 &= ~BIT(2);
    } else {
        JL_PORTA->DIR |= BIT(1);
        JL_PORTA->DIE &= ~BIT(1);
        JL_PORTA->PU0 &= ~BIT(1);
        JL_PORTA->PD0 &= ~BIT(1);
        JL_PORTA->DIR |= BIT(2);
        JL_PORTA->DIE &= ~BIT(2);
        JL_PORTA->PU0 &= ~BIT(2);
        JL_PORTA->PD0 &= ~BIT(2);
    }
}

void auadc_open_linein(void)
{
    single_linin_analog_open();
    JL_PORTA->DIR |= BIT(1);
    JL_PORTA->DIE &= ~BIT(1);
    JL_PORTA->PU0 &= ~BIT(1);
    JL_PORTA->PD0 &= ~BIT(1);
    JL_PORTA->DIR |= BIT(2);
    JL_PORTA->DIE &= ~BIT(2);
    JL_PORTA->PU0 &= ~BIT(2);
    JL_PORTA->PD0 &= ~BIT(2);
}







