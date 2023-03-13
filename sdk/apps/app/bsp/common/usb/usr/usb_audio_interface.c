
#pragma bss_seg(".usb_audio_if.data.bss")
#pragma data_seg(".usb_audio_if.data")
#pragma const_seg(".usb_audio_if.text.const")
#pragma code_seg(".usb_audio_if.text")
#pragma str_literal_override(".usb_audio_if.text.const")
#include "cpu.h"
#include "config.h"
#include "typedef.h"
/* #include "audio.h" */
/* #include "dac.h" */
/* #include "audio_adc.h" */
/* #include "msg.h" */
/* #include "circular_buf.h" */
/* #include "dac_api.h" */
/* #include "errno-base.h" */
/* #include "app_config.h" */
#include "usb/device/msd.h"
#include "usb/device/usb_stack.h"
#include "usb/device/uac_audio.h"
#include "usb/usr/usb_audio_interface.h"
#include "usb/usr/uac_sync.h"

#if  TCFG_PC_ENABLE

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[UAC]"
#include "log.h"
#include "uart.h"

EFFECT_OBJ *usb_src_obj;


#if 0
static bool uac_is_need_sync(void)
{
    // u32 in_sr;
    /* [> log_info("O\n"); <] */
    if (NULL == usb_src_obj) {
        return 0;
    }
    /*
    in_sr = uac_speaker_sound();
    u32 milli = uac_sr_milli(in_sr);
    if (milli > 50) {
        return 0;
    }
    */


    /* log_info("A\n"); */
    sound_in_obj *p_src_si = usb_src_obj->p_si;
    if (NULL == p_src_si) {
        return 0;
    }
    /* log_info("B\n"); */
    SRC_STUCT_API *p_ops =  p_src_si->ops;
    if (NULL == p_ops) {
        return 0;
    }
    return 1;
}
#endif






void usb_slave_sound_close(sound_out_obj *p_sound)
{
    log_info("usb slave sound off\n");
    p_sound->enable &= ~B_DEC_RUN_EN;
    unregist_dac_channel(p_sound);
    usb_src_obj = NULL;
    if (NULL != p_sound->effect) {
        src_reless(&p_sound->effect);
    } else {
        log_info("usb slave sound effect null\n");
    }
}
uac_sync uac_spk_sync;
void usb_slave_sound_open(sound_out_obj *p_sound, u32 sr)
{
    sound_out_obj *p_curr_sound = 0;
    sound_out_obj *p_next_sound = 0;
    uac_sync_init(&uac_spk_sync, sr);
    if (0 != sr) {
        p_curr_sound = p_sound;
        void *cbuf_o = p_curr_sound->p_obuf;

        /* p_curr_sound = link_src_sound(p_curr_sound, cbuf_o, (void **)&usb_src_obj, sr, dac_sr_read()); */
#if TCFG_SPK_SRC_ENABLE
        ///*
        p_curr_sound = link_src_sound(
                           p_curr_sound,
                           cbuf_o,
                           (void **)&usb_src_obj,
                           sr,
                           sr,
                           2
                       ); //为了省代码，没有写成上一句的样子
        //*/
#endif
#if (1 == DAC_TRACK_NUMBER)
        /* DAC差分输出时双声道音源融合成单声道 */
        p_curr_sound->info |= B_LR_COMB;
#endif
        if ((NULL != usb_src_obj) && (NULL != usb_src_obj->p_si)) {
            sound_in_obj *p_src_si = usb_src_obj->p_si;
            SRC_STUCT_API *p_ops =  p_src_si->ops;
            if (NULL != p_ops) {
                p_ops->config(p_src_si->p_dbuf,
                              SRC_CMD_INSR_INC_INIT,
                              (void *)uac_spk_sync.uac_sync_parm);
            }
        }

        usb_src_obj = p_sound->effect;
        regist_dac_channel(p_sound, NULL);//注册到DAC;
        p_sound->enable |=  B_DEC_RUN_EN;
    }
}

extern sound_out_obj uac_spk_sound;
u32 uac_spk_all;
u32 uac_spk_cnt;
static u8 spk_buffer[SPK_FRAME_LEN + 2] ALIGNED(4) AT(.uac_spk);
void usb_slave_sound_write(const u8 *pbuf, u32 len)
{
    int wlen = 0;
    u8 *obuf = spk_buffer;

    memcpy(spk_buffer, pbuf, len);

#if (SPK_AUDIO_RES == 24)
    //24转16bit--去掉低8bit
    u8 *sbuf = (u8 *)obuf;
    u32 i, j;
    j = 0;
    for (i = 0; i < len; i++) {
        if ((i % 3) == 0) {
            continue;
        }
        sbuf[j] = sbuf[i];
        j++;
    }
    len = len * 2 / 3;
#endif

    wlen = sound_output(&uac_spk_sound, (u8 *)obuf, len);
    u32 uac_spk_data = uac_speaker_stream_size();
    u32 uac_spk_size = uac_speaker_stream_length();
    u32 percent = (uac_spk_data * 100) / uac_spk_size;
    uac_spk_all += percent;
    uac_spk_cnt++;

    if (wlen != len) {
        log_char('W');
    }
}

EFFECT_OBJ *uac_spk_percent(u32 *p_percent)
{
    if (0 == uac_spk_cnt) {
        return NULL;
    }
    *p_percent = uac_spk_all / uac_spk_cnt;
    uac_spk_all = 0;
    uac_spk_cnt = 0;
    if (NULL == usb_src_obj) {
        return NULL;
    }
    sound_in_obj *p_src_si = usb_src_obj->p_si;
    if (NULL == p_src_si) {
        return 0;
    }
    SRC_STUCT_API *p_ops =  p_src_si->ops;
    if (NULL == p_ops) {
        return 0;
    }
    return usb_src_obj;

}

#if 0//USB_DEVICE_CLASS_CONFIG & MIC_CLASS
sound_out_obj usb_mic_sound;
u8 obuf_usb_mic_o[1024] ;
cbuffer_t cbuf_usb_mic_o;
void usb_mic_init(void)
{
    log_info("usb mic init\n");
    u32 err;
    memset(&usb_mic_sound, 0, sizeof(usb_mic_sound));
    usb_mic_sound.info &= ~B_STEREO;

    cbuf_init(&cbuf_usb_mic_o, &obuf_usb_mic_o[0], sizeof(obuf_usb_mic_o));
    usb_mic_sound.p_obuf = &cbuf_usb_mic_o;

    //16k 采样,单声道
    err = audio_adc_init_api(MIC_AUDIO_RATE, ADC_MIC, BIT(0)); //PA13 -> mic
    regist_audio_adc_channel(&usb_mic_sound, NULL); //注册到ADC;
    audio_adc_enable(14);
    usb_mic_sound.enable |= B_DEC_RUN_EN;
}
void usb_mic_uninit()
{
    log_info("usb slave mic off \n");
    usb_mic_sound.enable &= ~B_DEC_RUN_EN;
    audio_adc_off_api();
    unregist_audio_adc_channel(&usb_mic_sound); //卸载ADC
    audio_adc_disable();
}
#endif

#endif
