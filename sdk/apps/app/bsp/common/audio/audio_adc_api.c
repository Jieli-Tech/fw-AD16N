/***********************************Jieli tech************************************************
  File : adc_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2019-1-14
********************************************************************************************/
/* #include "adc_api.h" */
/* #include "adc.h" */
#include "circular_buf.h"
#include "string.h"
#include "uart.h"
#include "config.h"
#include "audio.h"
#include "audio_adc.h"
#include "audio_adc_api.h"
/* #include "audio_analog.h" */
#include "sound_mge.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"



#define AUADC_CHANNEL_TOTAL 2

typedef struct _AUDIO_ADC_MANAGE {
    /*多路数据流句柄*/
    sound_out_obj *sound[AUADC_CHANNEL_TOTAL];
    /*多路数据流句柄所对应的触发函数*/
    void (*kick[AUADC_CHANNEL_TOTAL])(void *);
    u8 ch;          /*通路标记*/
    u8 track;       /*声道数目*/
} AUDIO_ADC_MANAGE;

AUDIO_ADC_MANAGE audio_adc_mge ;//AT(.AUDIO_ADC_BUFFER);



u32 audio_adc_buf[AUDIO_ADC_PACKET_SIZE * AUDIO_ADC_SP_SIZE  / 4 ] ;//AT(.AUDIO_ADC_BUFFER);

_OTP_CONST_ AUDIO_ADC_CTRL_HDL c_audio_adc_hdl = {
    .buf      = (void *) &audio_adc_buf[0],
    .pns      = AUDIO_ADC_SP_PNS,
    .sp_total = sizeof(audio_adc_buf) / AUDIO_ADC_SP_SIZE,
    .sp_size  = AUDIO_ADC_SP_SIZE,
};

/*----------------------------------------------------------------------------*/
/**@brief   adc模块初始化
   @param   无
   @return  无
   @author  liujie
   @note    void adc_init_api(void)
*/
/*----------------------------------------------------------------------------*/
void audio_adc_mode_init(void)
{
    memset(&audio_adc_mge, 0, sizeof(audio_adc_mge));
    /* audio_adc_resource_init(audio_adc_buf, sizeof(audio_adc_buf), 25); */
}


u32 audio_adc_init_api(u32 sr, AUDIO_ADC_MODE mode, u32 throw_sp_num)
{
    u32 res = 0;
    if (AUDIO_ADC_MIC == mode) {
        single_micin_analog_open();
        audio_adc_mge.track = 1;
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
    } else if (AUDIO_ADC_LINEIN == mode) {
        single_linin_analog_open();
        audio_adc_mge.track = 1;
        JL_PORTA->DIR |= BIT(1);
        JL_PORTA->DIE &= ~BIT(1);
        JL_PORTA->PU0 &= ~BIT(1);
        JL_PORTA->PD0 &= ~BIT(1);
        JL_PORTA->DIR |= BIT(2);
        JL_PORTA->DIE &= ~BIT(2);
        JL_PORTA->PU0 &= ~BIT(2);
        JL_PORTA->PD0 &= ~BIT(2);
    } else {
        audio_adc_mge.track = 0;
    }
    /* res = adc_analog_open(mode, ch); */
    /* void dac_analog_init_t(); */
    /* dac_analog_init_t(); */
    if (0 == res) {
        res = audio_adc_phy_init((void *)&c_audio_adc_hdl, sr, AUDIO_ADC_CON_DEFAULT, throw_sp_num);
    }
    return res;

}
void audio_adc_off_api(void)
{
    audio_adc_phy_off();
    audio_adc_analog_off();
    /* adc_analog_close(); */
}

/* extern void kick_encode_api(void); */
/* //弱定义函数，定义不可修改，由扩音模式重新实现 */
/* __attribute__((weak))void kick_loudsperaker(void *_sound)AT(.adc_oput_code) */
/* { */

/* } */
/* AT(.adc_oput_code) */
AT(.audio_isr_text)
void fill_audio_adc_fill(u8 *buf, u32 len)
{
    u32 i;
    for (i = 0; i < AUADC_CHANNEL_TOTAL; i++) {
        if (0 == (audio_adc_mge.ch & BIT(i))) {
            continue;
        }
        if (0 == (audio_adc_mge.sound[i]->enable & B_DEC_RUN_EN)) {
            continue;
        }
        u32 wlen;
        wlen = cbuf_write(audio_adc_mge.sound[i]->p_obuf, buf, len);
        if (wlen != len) {
            log_char('*');
        }
        /* if (audio_adc_mge.sound[i]->enable & B_REC_RUN) { */
        if (NULL !=  audio_adc_mge.kick[i]) {
            audio_adc_mge.kick[i](audio_adc_mge.sound[i]);
        }
        /* kick_encode_api(); */
        /* } */

        /* if (audio_adc_mge.sound[i]->enable & B_LOUDSPEAKER) { */
        /* kick_loudsperaker((void *)audio_adc_mge.sound[i]); */
        /* } */
    }
}

bool regist_audio_adc_channel(void *psound, void *kick)
{
    u8 i;
    for (i = 0; i < AUADC_CHANNEL_TOTAL; i++) {
        if (audio_adc_mge.ch & BIT(i)) {
            continue;
        }
        if (1 == audio_adc_mge.track) {
            ((sound_out_obj *)psound)->info &= ~B_STEREO;
        } else if (2 == audio_adc_mge.track) {
            ((sound_out_obj *)psound)->info |= B_STEREO;
        }

        audio_adc_mge.sound[i] = psound;
        audio_adc_mge.kick[i] = kick;
        audio_adc_mge.ch |= BIT(i);
        return true;
    }
    return false;
}

bool unregist_audio_adc_channel(void *psound)
{
    u8 i;
    sound_out_obj *ps = psound;

    for (i = 0; i < AUADC_CHANNEL_TOTAL; i++) {
        if (0 == (audio_adc_mge.ch & BIT(i))) {
            continue;
        }

        if (audio_adc_mge.sound[i] == psound) {
            local_irq_disable();
            audio_adc_mge.ch &= ~BIT(i);
            /* audio_adc_mge.obuf[i] = 0; */
            audio_adc_mge.sound[i] = 0;
            audio_adc_mge.kick[i] = NULL;
            ps->enable &= ~B_DEC_OBUF_EN;
            local_irq_enable();
            break;
        }
    }
    return true;
}




