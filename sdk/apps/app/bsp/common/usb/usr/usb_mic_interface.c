
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
#include "usb/usr/auadc_2_usbmic.h"
#include "uac_sync.h"
#include "sound_kick.h"

#if ( TCFG_PC_ENABLE && (USB_DEVICE_CLASS_CONFIG & MIC_CLASS))

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[UMIC]"
#include "log.h"
#include "uart.h"

/* SEC(.uac_var); */
uac_sync uac_mic_sync AT(.uac_var);
sound_out_obj *(*mic_open_func)(uac_mic_read *);
void (*mic_close_func)(void **);
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

#define DECAY_TIME_MS  100
static u16 decay_size = DECAY_TIME_MS * (2 * MIC_AUDIO_RATE / 1000);

uac_mic_read uac_read;

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

u32 usb_slave_mic_open(u32 sr, u32 frame_len, u32 ch)
{
    /* memset(&uac_read, 0, sizeof(uac_read)); */
#if TCFG_MIC_SRC_ENABLE
    uac_sync_init(&uac_mic_sync, sr);
#endif

    uac_read.self.sr = sr;
    uac_read.self.frame_len = frame_len;
    uac_read.self.ch = ch;

    if (mic_open_func) {
        sound_out_obj *psound = mic_open_func(&uac_read);
        if (NULL == psound) {
            log_info("USB MIC OPEN FAIL!");
            return E_USBMIC_SOUND_NULL;
        }
        uac_read.read_sound = psound;
    }
    log_info("USB MIC OPEN SUCC!");
    return 0;
}

void usb_slave_mic_close(void)
{
    if (mic_close_func) {
        mic_close_func((void **)&uac_read.p_src);
    }
    memset(&uac_read, 0, sizeof(uac_read));
    /* uac_read.read_sound = NULL; */
}
static u32 uac_mic_all;
static u32 uac_mic_cnt;
static u32 uac_mic_cnt_last;
EFFECT_OBJ *uac_mic_percent(u32 *p_percent)
{
    if (uac_mic_cnt == 0) {
        return NULL;
    }
    if ((uac_mic_cnt_last + 0) == uac_mic_cnt) {
        return NULL;
    }
    uac_mic_cnt_last = uac_mic_cnt;
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
    u32 tlen = 0;

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
        sound_out_obj *psound = uac_read.read_sound;
        if (0 == (psound->enable & B_DEC_RUN_EN)) {
            /* 解码停止后，sound_input不再消耗数据，需修改为消耗完obuf数据:TODO */
            psound->enable &= ~B_DEC_OBUF_EN;
            goto __no_read;
        }
        if (psound->enable & B_DEC_FIRST) {
            if (cbuf_get_data_size(psound->p_obuf) < (cbuf_get_space(psound->p_obuf) / 2)) {
                goto __no_read;
            } else {
                psound->enable &= ~B_DEC_FIRST;
            }
        }
        tlen = sound_input(uac_read.read_sound, buf, len);
        /* log_char('B'); */
        u32 uac_mic_data = uac_mic_stream_size();
        u32 uac_mic_size = uac_mic_stream_buf_length();
        /* log_char('C'); */
        if (0 != uac_mic_size) {
            u32 inpcm_data  = source_data_pcm(uac_read.source, uac_read.self.sr);
            u32 inpcm_space = source_space_pcm(uac_read.source, uac_read.self.sr);
            u32 t_decay_size = inpcm_space;
            /* u32 t_decay_size = decay_size > inpcm_space ? inpcm_space : decay_size; */
            /* if (t_decay_size < inpcm_data) { */
            /*     inpcm_data = t_decay_size; */
            /* } */
            uac_mic_data += inpcm_data;
            uac_mic_size += t_decay_size;
            u32 percent = (uac_mic_data * 100) / uac_mic_size;
            uac_mic_all += percent;
            uac_mic_cnt++;
            /* static u32 wptr_last = 0; */
            /* u32 wptr = cbuf_get_writeptr((cbuffer_t *)psound->p_obuf); */
            /* if (wptr_last != wptr) { */
            /*     wptr_last = wptr; */
            /*     uac_mic_new_data = 1; */
            /* } */
        }
        /* log_char('D'); */

    } else {
        memset(buf, 0, len * MIC_CHANNEL);
        return (len * MIC_CHANNEL);
    }
__no_read:
    //数据不够则部分清零
    sound_kick(uac_read.read_sound, (void *)uac_read.pkick);
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

void set_usb_mic_func(void *open, void *close)
{
    local_irq_disable();
    mic_open_func = open;
    mic_close_func = close;
    local_irq_enable();
}

void set_usb_mic_info(void *source)
{
    local_irq_disable();
    /* uac_read.read_sound = psound; */
    /* uac_read.pkick = kick; */
    /* uac_read.p_src = p_src; */
    uac_read.source = source;
    if ((NULL == uac_read.read_sound) && (mic_open_func)) {
        uac_read.read_sound = mic_open_func(&uac_read);
    }
    local_irq_enable();
}

bool clr_usb_mic_info(void *psound)
{
    if (psound == uac_read.read_sound) {
        local_irq_disable();
        memset(&uac_read, 0, sizeof(uac_read));
        /* uac_read.read_sound = NULL; */
        /* uac_read.pkick = NULL; */
        /* uac_read.p_src = NULL; */
        local_irq_enable();
        return true;
    }
    return false;
}
#endif

