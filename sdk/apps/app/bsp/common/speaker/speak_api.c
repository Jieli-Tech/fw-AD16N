/***********************************Jieli tech************************************************
  File : speak_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2021年6月15日
********************************************************************************************/
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "circular_buf.h"
#include "audio.h"
#include "audio_adc_api.h"
#include "audio_adc.h"
#include "audio_dac_api.h"
#include "audio_dac.h"
#include "sound_mge.h"
#include "errno-base.h"
#include "app_modules.h"
#include "app_config.h"
#include "stream_frame.h"
#if HAS_SPEED_EN
#include "speed_api.h"
#endif
#if ECHO_EN
#include "echo_api.h"
#endif
#if VO_PITCH_EN
#include "vo_pitch_api.h"
#endif
#if AUDIO_EQ_ENABLE
#include "audio_eq.h"
#endif
#if NOTCH_HOWLING_EN
#include "notch_howling_api.h"
#endif
#if PITCHSHIFT_HOWLING_EN
#include "howling_pitchshifter_api.h"
#endif

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[speak_api]"
#include "log.h"

cbuffer_t cbuf_aas_o AT(.speaker_data);
u32 obuf_aas_o[1024 / 4]  AT(.speaker_data);

cbuffer_t cbuf_ads_o AT(.speaker_data);
u32 obuf_ads_o[1024 / 4]  AT(.speaker_data);

typedef struct __audio_adc_speaker {
    sound_out_obj  sound;
    sound_out_obj  *p_dac_sound;
    /* EFFECT_OBJ    *p_src;            // p_curr_sound->effect; */
    EFFECT_OBJ    *p_vp;            // p_curr_sound->effect;
    EFFECT_OBJ    *p_echo;            // p_curr_sound->effect;
    EFFECT_OBJ    *p_eq;            // p_curr_sound->effect;
    EFFECT_OBJ    *p_sp;            // p_curr_sound->effect;
} __aa_speaker;

__aa_speaker aa_speaker AT(.speaker_data);

void audio_adc_speaker_start(u32 sr)
{
    /* if (aa_speaker.sound.enable & B_DEC_RUN_EN) { */
    /*     log_info("SPEAKER START ERR !!! \n"); */
    /*     return; */
    /* } */

    log_info("SPEAKER START !!! \n");

    u32 adc_sr = sr;
    memset(&aa_speaker, 0, sizeof(aa_speaker));
    cbuf_init(&cbuf_aas_o, &obuf_aas_o[0], sizeof(obuf_aas_o));
    stream_frame_init(IRQ_SPEAKER_IP);
    sound_out_init(&aa_speaker.sound, (void *)&cbuf_aas_o, 0);

    cbuf_init(&cbuf_ads_o, &obuf_ads_o[0], sizeof(obuf_ads_o));
    audio_adc_init_api(adc_sr, AUDIO_ADC_MIC, adc_sr * 1);

    sound_out_obj *p_curr_sound = 0;
    p_curr_sound = &aa_speaker.sound;

    /* HWI_Uninstall(IRQ_SOFT1_IDX); */
    /* HWI_Install(IRQ_SOFT1_IDX, (u32)speaker_soft1_isr, IRQ_DECODER_IP); */
    /*  */
    /* memset(&aa_speaker, 0, sizeof(aa_speaker)); */
    /* cbuf_init(&cbuf_aas_o, &obuf_aas_o[0], sizeof(obuf_aas_o)); */
    /* aa_speaker.sound.p_obuf = &cbuf_aas_o; */
    /* aa_speaker.sound.info &= ~B_STEREO;//单声道 */
    /*  */
    /* u32 adc_sr = sr; */
    /* audio_adc_init_api(adc_sr, AUDIO_ADC_MIC, adc_sr * 1); */
    /*  */
    /* sound_out_obj *p_curr_sound = 0; */
    /* p_curr_sound = &aa_speaker.sound; */
    /*  */
    /* cbuf_init(&cbuf_ads_o, &obuf_ads_o[0], sizeof(obuf_ads_o)); */

    /* **************************************************** */
    /* speed、echo、vo_pitcch音效资源互斥，同时只可以开一个 */
    /* **************************************************** */
#if HAS_SPEED_EN
    /* p_curr_sound = link_speed_sound(p_curr_sound, &cbuf_ads_o, (void **)&aa_speaker.p_sp, adc_sr); */
#endif
#if ECHO_EN         //混响
    /* p_curr_sound = link_echo_sound(p_curr_sound, &cbuf_ads_o, (void **)&aa_speaker.p_echo, adc_sr); */
#endif
#if VO_PITCH_EN     //多种音效集合库：机器人、rap、两种变速变调；
    p_curr_sound = link_voice_pitch_sound(p_curr_sound, &cbuf_ads_o, (void **)&aa_speaker.p_vp, adc_sr, VP_CMD_PITCHSHIFT);
#endif

#if (HOWLING_SEL == NOTCH_HOWLING_EN)          //陷波抑制啸叫
    p_curr_sound = link_notch_howling_sound(p_curr_sound, &cbuf_ads_o, 0, adc_sr);
#elif (HOWLING_SEL == PITCHSHIFT_HOWLING_EN)     //移频抑制啸叫
    p_curr_sound = link_pitchshift_howling_sound(p_curr_sound, &cbuf_ads_o, 0, adc_sr);
#endif
#if AUDIO_EQ_ENABLE
    /* p_curr_sound = link_eq_sound(p_curr_sound, &cbuf_ads_o, (void **)&aa_speaker.p_eq, adc_sr, 1); */
#endif

    /* if (adc_sr != dac_sr) { */
    /*     p_curr_sound = link_src_sound(p_curr_sound, &cbuf_ads_o, (void **)&aa_speaker.p_src, adc_sr, dac_sr, 1); */
    /* }; */
    if (&aa_speaker.sound == p_curr_sound) {
        regist_audio_adc_channel(&aa_speaker.sound, (void *) NULL);
    } else {
        void *kick = NULL;
        kick = regist_stream_channel(&aa_speaker.sound);
        regist_audio_adc_channel(&aa_speaker.sound, (void *) kick);
    }

    regist_dac_channel(p_curr_sound, (void *) NULL);//注册到DAC;
    aa_speaker.p_dac_sound = p_curr_sound;

    audio_adc_enable();
    aa_speaker.sound.enable |= B_DEC_RUN_EN | B_DEC_FIRST;
}

void audio_adc_speaker_reless(void)
{
    if (aa_speaker.sound.enable & B_DEC_RUN_EN) {
        log_info("SPEAKER RELESS !!! \n");
        aa_speaker.sound.enable &= ~B_DEC_RUN_EN;
        unregist_audio_adc_channel(&aa_speaker.sound);
        unregist_stream_channel(&aa_speaker.sound);
        unregist_dac_channel(aa_speaker.p_dac_sound);
        stream_frame_uninit();

        /* if (NULL != aa_speaker.p_src) { */
        /*     src_reless((void **)&aa_speaker.p_src); */
        /* } */

        memset(&aa_speaker, 0, sizeof(aa_speaker));
    }
}

#if 0
AT(.audio_a.text.cache.L2)
void kick_sound(void *_sound)
{
    sound_out_obj *sound = (sound_out_obj *)_sound;
    if (sound && sound->p_obuf) {
        u32 size = cbuf_get_data_size(sound->p_obuf);
        if (size >= 64) {
            sound->enable |= B_DEC_KICK;
            bit_set_swi(1);
        }
    }
}

SET(interrupt(""))
void speaker_soft1_isr()
{
    bit_clr_swi(1);
    sound_out_obj *sound = &aa_speaker.sound;
    if (NULL == sound) {
        return;
    }

    if (0 == (sound->enable & B_DEC_RUN_EN)) {
        return;
    }

    if (sound->enable & B_DEC_KICK) {
        sound->enable &= ~B_DEC_KICK;
        u32 rlen = 0;
        u32 wlen = 0;
        s16 *data = cbuf_read_alloc(sound->p_obuf, &rlen);
        if (rlen) {
            wlen = sound_output(sound, data, rlen);
        }
        cbuf_read_updata(sound->p_obuf, wlen);
    }
}
#endif
