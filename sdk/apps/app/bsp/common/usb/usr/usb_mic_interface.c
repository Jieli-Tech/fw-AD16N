
#pragma bss_seg(".usb_mic_if.data.bss")
#pragma data_seg(".usb_mic_if.data")
#pragma const_seg(".usb_mic_if.text.const")
#pragma code_seg(".usb_mic_if.text")
#pragma str_literal_override(".usb_mic_if.text.const")
#include "cpu.h"
#include "config.h"
#include "typedef.h"
/* #include "audio.h" */
/* #include "dac.h" */
#include "audio_adc.h"
/* #include "msg.h" */
/* #include "circular_buf.h" */
/* #include "dac_api.h" */
/* #include "errno-base.h" */
/* #include "app_config.h" */
#include "stream_frame.h"
#include "usb/device/msd.h"
#include "usb/device/usb_stack.h"
#include "usb/device/uac_audio.h"
#include "usb/usr/usb_mic_interface.h"
#include "usb/usr/uac_sync.h"

#if ( TCFG_PC_ENABLE && (USB_DEVICE_CLASS_CONFIG & MIC_CLASS))

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[UMIC]"
#include "log.h"
#include "uart.h"

/* SEC(.uac_var); */
static u16 g_usb_mic_vol = 31;
static u32 usb_mic_cnt;
void uac_mic_vol(u8 vol_l, u8 vol_r)
{
    if (vol_l > 31) {
        vol_l = 31;
    }
    log_info("mic vol:%d;", vol_l);
    g_usb_mic_vol = vol_l;
}

extern const bool config_usbslave_ctl_mic;

cbuffer_t mic_out_cbuf SEC(.uac_var);
u16 mic_out_obuf[AUDIO_ADC_PACKET_SIZE * 2] SEC(.uac_var);

cbuffer_t mic_run_cbuf SEC(.uac_var);
u16 mic_run_obuf[AUDIO_ADC_PACKET_SIZE * 4] SEC(.uac_var);

typedef struct __uac_mic_read {
    sound_out_obj mic_out_sound;
    sound_out_obj *read_sound;
    EFFECT_OBJ *p_src;
} uac_mic_read;

uac_mic_read uac_read SEC(.uac_var);

u32 uac_mic_stream_size()
{
    if (NULL != uac_read.read_sound) {
        return cbuf_get_data_size(uac_read.read_sound->p_obuf);
    }
    return 50;
}
u32 uac_mic_stream_buf_length()
{
    if (NULL != uac_read.read_sound) {
        return cbuf_get_space(uac_read.read_sound->p_obuf);
    }
    return 100;
}
uac_sync uac_mic_sync;
u32 usb_slave_mic_open(u32 sr, u32 frame_len, u32 ch)
{
    log_info("USB MIC OPEN");
    /* return 0; */
    //-----
    u32 err = 0;
    usb_mic_cnt = 0;

    memset(&uac_read, 0, sizeof(uac_read));
    /*---------------------------------------------------------------*/
    cbuf_init(&mic_out_cbuf, &mic_out_obuf[0], sizeof(mic_out_obuf));
    sound_out_init(&uac_read.mic_out_sound, (void *)&mic_out_cbuf, 0);

    /*---------------------------------------------------------------*/
    cbuf_init(&mic_run_cbuf, &mic_run_obuf[0], sizeof(mic_run_obuf));
    /*---------------------------------------------------------------*/

    sound_out_obj *p_curr_sound;

    log_info("usmo ---- 000");
    p_curr_sound = &uac_read.mic_out_sound;
    log_info("usmo ---- 001");

#if TCFG_MIC_SRC_ENABLE
    ///*
    p_curr_sound = link_src_sound(
                       p_curr_sound,
                       &mic_run_cbuf,
                       (void **)&uac_read.p_src,
                       sr,
                       sr,
                       1
                   );
    uac_sync_init(&uac_mic_sync, sr);
    //*/
#endif
    log_info("usmo ---- 002");



    void *kick = NULL;

    if (&uac_read.mic_out_sound != p_curr_sound) {
        log_info("usmo ---- 003");
        kick = regist_stream_channel(&uac_read.mic_out_sound);

    }
    /* p_curr_sound = &uac_read.mic_out_sound; */
    log_info("usmo ---- 004");
    regist_audio_adc_channel((void *)&uac_read.mic_out_sound, kick);
    log_info("usmo ---- 005");
    uac_read.read_sound = p_curr_sound;
    if (config_usbslave_ctl_mic) {
        log_info("usmo ---- 006");
        err = audio_adc_init_api(sr, AUDIO_ADC_MIC, 0);
    }
    if (0 == err) {
        log_info("usmo ---- 007");
        if (config_usbslave_ctl_mic) {
            audio_adc_enable();
        }
        p_curr_sound->enable |= B_DEC_RUN_EN;
        uac_read.mic_out_sound.enable |= B_DEC_RUN_EN;
    } else {
        log_info("err : 0x%x", err);
    }
    return 0;
}

void usb_slave_mic_close(void)
{
    /* return ; */
    log_info("usmc ---- 00s");
    if (uac_read.mic_out_sound.enable & B_DEC_RUN_EN) {
        log_info("usmc ---- 0s1");
        uac_read.mic_out_sound.enable &= ~B_DEC_RUN_EN;
        unregist_audio_adc_channel(&uac_read.mic_out_sound);
        unregist_stream_channel(&uac_read.mic_out_sound);
        log_info("usmc ---- 0s1, 0x%x", (u32)uac_read.p_src);
        if (NULL != uac_read.p_src) {
            src_reless((void **)&uac_read.p_src);
        }
        memset(&uac_read.mic_out_sound, 0, sizeof(uac_read.mic_out_sound));
        uac_read.read_sound = NULL;

    }
    log_info("USB MIC CLOSE");
    if (config_usbslave_ctl_mic) {
        log_info("usmc ---- 001");
        audio_adc_off_api();
    } else {
        log_info("usmc ---- 002");
        /* audio_adc_disable(); */
    }
    log_info("usmc ---- 003");
}
static u32 uac_mic_all;
static u32 uac_mic_cnt;
EFFECT_OBJ *uac_mic_percent(u32 *p_percent)
{
    if (uac_mic_cnt == 0) {
        return NULL;
    }
    *p_percent = uac_mic_all / uac_mic_cnt;
    uac_mic_all = 0;
    uac_mic_cnt = 0;
    if (NULL == uac_read.p_src) {
        return NULL;
    }
    sound_in_obj *p_src_si = uac_read.p_src->p_si;
    if (NULL == p_src_si) {
        return 0;
    }
    SRC_STUCT_API *p_ops =  p_src_si->ops;
    if (NULL == p_ops) {
        return 0;
    }
    return uac_read.p_src;

}
int usb_slave_mic_read(u8 *buf, u32 len)
{
    /* return 0; */

    /* JL_PORTA->DIR &= ~BIT(9); */
    /* JL_PORTA->OUT ^= BIT(9); */
    u32 tlen;

#if (MIC_CHANNEL == 2)
    len = len / 2;//双声道读取一半的数据
#endif

    if (uac_read.read_sound) {
        /* log_char('A'); */
        /* extern void run_a_stream(sound_out_obj * sound, u32); */
        /* uac_read.mic_out_sound.enable |= B_DEC_KICK; */
        /* run_a_stream(&uac_read.mic_out_sound, -1);//len); */
#if (MIC_AUDIO_RATE == 44100) || (MIC_AUDIO_RATE == 22050) || (MIC_AUDIO_RATE == 11025)
        usb_mic_cnt++;
        if (usb_mic_cnt < (10 * 44100 / MIC_AUDIO_RATE)) {
            len = len - 2;
        } else {
            usb_mic_cnt = 0;
        }
#endif

        tlen = sound_input(uac_read.read_sound, buf, len);
        /* log_char('B'); */
        u32 uac_mic_data = uac_mic_stream_size();
        u32 uac_mic_size = uac_mic_stream_buf_length();
        /* log_char('C'); */
        if (0 != uac_mic_size) {
            u32 percent = (uac_mic_data * 100) / uac_mic_size;
            uac_mic_all += percent;
            uac_mic_cnt++;
        }
        /* log_char('D'); */

    } else {
        memset(buf, 0, len * MIC_CHANNEL);
        return (len * MIC_CHANNEL);
    }

    //数据不够则部分清零
    if (tlen != len) {
        log_char('.');
        memset(buf + tlen, 0, len - tlen);
        tlen = len;
    }

#if TCFG_USB_APPLE_DOCK_EN
    u32 sp = tlen / 2;
    s16 *sbuf = (void *)buf;
    s32 s_tmp;

    for (u32 i = 0; i < sp; i++) {
        s_tmp = sbuf[i];
        s_tmp = s_tmp * g_usb_mic_vol / 31;
        sbuf[i] = s_tmp;
    }
#endif

#if (MIC_CHANNEL == 2)
    //把单声道数据拼成双声道,为兼容一些手机,MIC必须为双声道
    s16 *pdata = (s16 *)buf;
    len = len / 2;
    while (len) {
        pdata[len * 2 - 1] = pdata[len - 1];
        pdata[len * 2 - 2] = pdata[len - 1];
        len--;
    }
#endif

    return (tlen * MIC_CHANNEL);
}




#endif

