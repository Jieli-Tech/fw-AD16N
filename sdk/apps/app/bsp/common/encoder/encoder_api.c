#include "encoder_mge.h"
#include "audio_adc_api.h"
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "hwi.h"
#include "app_modules.h"
/* #include "dev_manage.h" */
/* #include "fs_io.h" */
#include "vfs.h"
#include "circular_buf.h"
#include "a_encoder.h"
#include "mp3_encoder.h"
#include "clock.h"
#include "stream_frame.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[encoder_api]"
#include "log.h"

#if (defined(ANS_EN) && (ANS_EN))
#include "ans_api.h"
cbuffer_t cbuf_ans AT(.ans_data);
u32 ans_buff[512 / 4] AT(.ans_data);
#endif

cbuffer_t cbuf_adc AT(.rec_data);
static u8 adc_buff[7680] AT(.rec_data);//norfs擦写时间最长不超过80ms,最小缓存不小于sr*2*80ms

/* #if (0 == FPGA) */
#ifndef FPGA
sound_out_obj rec_sound;
sound_out_obj *enc_in_sound;

#define START_ADC_RUN  rec_sound.enable |= (B_DEC_RUN_EN | B_REC_RUN)
#define STOP_ADC_RUN  rec_sound.enable &= ~(B_DEC_RUN_EN | B_REC_RUN)

_WEAK_
void rec_cbuf_init(void *cbuf_t)
{
    cbuf_init(cbuf_t, &adc_buff[0], sizeof(adc_buff));
}

void rec_phy_init(void)
{
    memset(&rec_sound, 0, sizeof(rec_sound));

    stream_frame_init(IRQ_SPEAKER_IP);

    cbuf_init(&cbuf_adc, &adc_buff[0], sizeof(adc_buff));
    rec_sound.p_obuf = &cbuf_adc;        //cbuf_adc为audio_adc的数据

    sound_out_obj *p_curr_sound = &rec_sound;
#if (defined(ANS_EN) && (ANS_EN))
    cbuf_init(&cbuf_ans, &ans_buff[0], sizeof(ans_buff));   //cbuf_ans 为link
    p_curr_sound = link_ans_sound(p_curr_sound, &cbuf_ans, read_audio_adc_sr());
#endif

    if (p_curr_sound != &rec_sound) {
        void *kick = NULL;
        kick = regist_stream_channel(&rec_sound, kick_encode_api);
        regist_audio_adc_channel(&rec_sound, NULL, (void *)kick);
    } else {
        regist_audio_adc_channel(&rec_sound, NULL, (void *)kick_encode_api);
    }

    enc_in_sound = p_curr_sound;
    rec_sound.enable |= B_DEC_RUN_EN | B_DEC_FIRST;

}
void rec_phy_suspend(void)
{
    stream_frame_uninit();
    unregist_stream_channel(&rec_sound);
    unregist_audio_adc_channel(&rec_sound);
}

AT(.audio_a.text.cache.L2)
void kick_encode_api(void *obj)
{
    kick_encode_isr();
}
#endif




void start_encode(void)
{
    audio_adc_enable();
    mdelay(10);
    START_ADC_RUN;
}
bool stop_encode_phy(enc_obj *obj, IS_WAIT wait)
{
    u32 res;
    audio_adc_disable();
    STOP_ADC_RUN;
    log_info("stop encode\n");
    if (NULL == obj) {
        rec_phy_suspend();
        return false;
    }
    obj->enable |= B_ENC_STOP;

    if (NEED_WAIT == wait) {
        log_info("stop encode A\n");
        u32 i = 0x10000;
        while (0 != cbuf_get_data_size(obj->p_ibuf) && (0 != i)) {
            if (obj->enable & B_ENC_FULL) {
                break;
            }
            kick_encode_isr();
            delay(100);
            i--;
        }
        obj->enable &= ~B_ENC_ENABLE;
        if (NULL != obj->wait_output_empty) {
            obj->wait_output_empty(obj);
        }
    }

    res = unregist_encode_channel(obj);
    if (0 != res) {
        log_error("unregist_encode_channel err 0x%x\n", res);
    }
    if (is_encode_channel_empty()) {
        HWI_Uninstall(IRQ_SOFT1_IDX);
    }
    rec_phy_suspend();
    return true;
}

enc_obj *stop_encode_file(enc_obj *p_enc_obj, u32 dlen)
{
    bool flag = stop_encode_phy(p_enc_obj, NEED_WAIT);
    u32 flen = dlen;
    vfs_ioctl(p_enc_obj->p_file, FS_IOCTL_FILE_SYNC, (int)&flen);
    if (flag) {
        p_enc_obj = NULL;
    }
    return p_enc_obj;
}

extern void enc_soft1_isr(void);
enc_obj *encoder_io(u32(*fun)(void *, void *, void *), void *input_func, void *output_func, void *pfile)
{
    u32 res;
    rec_phy_init();
    if (NULL == fun) {
        return NULL;
    }
    enc_obj *enc_hdl = (void *)fun(pfile, input_func, output_func);
    if (0 != enc_hdl) {
        res = regist_encode_channel(enc_hdl);
        if (0 != res) {
            log_error("regist_encode_channel err 0x%x\n", res);
        } else {
            log_info("regist_encode_channel succ\n");
            HWI_Install(IRQ_SOFT1_IDX, (u32)enc_soft1_isr,  IRQ_ENCODER_IP) ;
        }
        log_info("encode_io succ: \n");
        return enc_hdl;
    } else {
        log_info("encode_io fail \n");
        return NULL;
    }
    //while(1)clear_wdt();
}


u16 enc_input(void *priv, s16 *buf, u8 channel, u16 len)
{
    u16 rlen;
    enc_obj *obj = priv;
    //log_info("channel %d, %d", channel, len);
    if (2 != obj->info.nch) {
        len = len * 2; //点数转化位字节长度
    }
    if (obj->enable & B_ENC_STOP) {
        u16 tlen = cbuf_get_data_size(obj->p_ibuf);
        tlen = tlen > len ? len : tlen;
        rlen = cbuf_read(obj->p_ibuf, buf, tlen);

    } else {
        rlen = cbuf_read(obj->p_ibuf, buf, len);
    }
    if (2 == obj->info.nch) {
        u8 *rptr = (u8 *)buf;
        memcpy((void *)&rptr[rlen], (void *)&rptr[0], rlen);
        rlen += rlen;
    }

    /* if ((0 != rlen) && (2 == channel)) { //单声道转化为立体声 */
    /* log_char('2'); */
    /* u32 plen = rlen / 2; */
    /* s16 *rptr = buf; */
    /* while (plen--) { */
    /* rptr[plen * 2 + 1] = rptr[plen]; */
    /* rptr[plen * 2] = rptr[plen]; */
    /* } */
    /* } */

    rlen = rlen / 2;//字节长度转化为样点数
    //log_info("rlen  %d", rlen);

    return rlen;
}






