/***********************************Jieli tech************************************************
  File : dac_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2019-1-14
********************************************************************************************/

#include "audac_basic.h"
#include "audio_dac_api.h"
#include "sine_play.h"
#include "circular_buf.h"
#include "app_modules.h"

#if HAS_MIO_EN
#include "mio_api.h"
#endif

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[audio_dac_api]"
#include "log.h"

AT(.audio_isr_text)
void fifo_dac_fill_kick(u32 i, u32 dac_packet_num)
{
#if HAS_MIO_EN
    d_mio_kick(dac_mge.sound[i]->mio, dac_packet_num/*DAC_PACKET_SIZE*/);
#endif
}

AT(.audio_isr_text)
__attribute__((weak))
int dac_kick_api(void *sound_hld, void *pkick)
{
    return -1;
}

AT(.audio_isr_text)
void dac_kick(void *sound_hld, void *pkick)
{
    sound_out_obj *psound = sound_hld;
    if (psound->enable & B_DEC_RUN_EN) {
        if (0 == (psound->enable & B_DEC_PAUSE)) {
            /* debug_putchar('K'); */
            /* if_kick_decoder(psound, pkick); */
            dac_kick_api(psound, pkick);
        }
    } else {
        if (0 == cbuf_get_data_size(psound->p_obuf)) {
            /* log_char('L'); */
            psound->enable &= ~B_DEC_OBUF_EN;
        } else {
            /* log_char('M'); */
        }
    }
}



AT(.audio_isr_text)
u32 fill_dac_fill_phy(u8 *buf, u32 len)
{
    u32 i, sp_cnt, active_flag;
    s32 t_sp;
    s16 r_sp;
    s16 *sp_buf;
    s16 *rptr[AUDAC_CHANNEL_TOTAL];
    u32 olen[AUDAC_CHANNEL_TOTAL];
    u32 p_cnt[AUDAC_CHANNEL_TOTAL];
    u32 sp_number = len / 2;
    s16 all_cnt = 0;
    memset(p_cnt, 0, sizeof(p_cnt));
    memset(rptr,  0, sizeof(rptr));
    memset(olen,  0, sizeof(olen));
    memset(buf,   0, len);

    d_key_voice_read(buf, len);

    sp_buf = (void *)buf;
    /* log_info("a %d\n",sp_number); */

    active_flag = 0;
    for (i = 0; i < AUDAC_CHANNEL_TOTAL; i++) {
        if (0 == (dac_mge.ch & BIT(i))) {
            continue;
        }
        if (false == dac_cbuff_active(dac_mge.sound[i])) {
            continue;
        }
        /* if ((cbuf_get_data_size(dac_mge.sound[i]->p_obuf) >= len) || \ */
        /*     (!(dac_mge.sound[i]->enable & B_DEC_RUN_EN))) { */
        /*     rptr[i] = cbuf_read_alloc(dac_mge.sound[i]->p_obuf, &olen[i]); */
        /* } */
        rptr[i] = cbuf_read_alloc(dac_mge.sound[i]->p_obuf, &olen[i]);
        if (0 == olen[i]) {
            log_char('z');
            rptr[i] = 0;
        }
        fifo_dac_fill_kick(i, sp_number / DAC_TRACK_NUMBER);//mio依赖单声道数据量
    }


    for (sp_cnt = 0; sp_cnt < sp_number; sp_cnt++) {
        t_sp = 0;
        for (i = 0; i < AUDAC_CHANNEL_TOTAL; i++) {
            if (0 == rptr[i]) {
                continue;
            }
            if (0 == olen[i]) {
                rptr[i] = cbuf_read_alloc(dac_mge.sound[i]->p_obuf, &olen[i]);
                if (0 == olen[i]) {
                    log_char('x');

                }
                continue;
            }

            t_sp += *rptr[i];
            if ((0 == (B_STEREO & dac_mge.sound[i]->info)) && \
                (2 == DAC_TRACK_NUMBER)) {
                if (1 == (sp_cnt & 0x01)) {
                    p_cnt[i]++;
                    rptr[i]++;
                }
            } else {
                p_cnt[i]++;
                rptr[i]++;
            }

            //uc03 是立体声
            /* audio_dac_stero_cnt(\ */
            /* (B_STEREO & dac_mge.sound[i]->info), \ */
            /* sp_cnt, \ */
            /* rptr[i]) */

            if ((p_cnt[i] * 2) >= olen[i]) {
                cbuf_read_updata(dac_mge.sound[i]->p_obuf, p_cnt[i] * 2);
                /* rptr[i] = 0; */
                olen[i] = 0;
                p_cnt[i] = 0;
                rptr[i] = cbuf_read_alloc(dac_mge.sound[i]->p_obuf, &olen[i]);
                if (0 == olen[i]) {
                    log_char('y');

                }

            }
        }
#if 0
        if (MAX_PHY_VOL != dac_mge.vol_phy) {
            /* t_sp = (t_sp / (MAX_PHY_VOL + 1)) * dac_mge.vol_phy; */
            t_sp = (t_sp * dac_mge.vol_phy) / (MAX_PHY_VOL + 1);
        }
#endif
        t_sp += sp_buf[sp_cnt];
        if (t_sp > 32767) {
            t_sp = 32767;
        } else if (t_sp < -32768) {
            t_sp = -32768;
        }
        sp_buf[sp_cnt] = t_sp;
        all_cnt++;
        /* if (0 == active_flag) { */
        /* break; */
        /* } */
    }
    if (sp_number > all_cnt) {
        log_info("*");
    }
    /* log_info("b"); */

    for (i = 0; i < AUDAC_CHANNEL_TOTAL; i++) {
        if (0 == (dac_mge.ch & BIT(i))) {
            continue;
        }
        if (0 != rptr[i]) {
            cbuf_read_updata(dac_mge.sound[i]->p_obuf, p_cnt[i] * 2);
        }
        dac_kick(dac_mge.sound[i], dac_mge.kick[i]);
    }

#if TCFG_AUDIO_AUTO_MUTE_ENABLE
    audio_energy_detect_run((s16 *)buf, len);
#endif
    return all_cnt * 2;
}



AT(.audio_isr_text)
u32 fill_dac_only_one(u8 *buf, u32 len)
{
    u32 rlen = 0;
    memset(buf,   0, len);
    u32 i;
    /* for (i = 0; i < AUDAC_CHANNEL_TOTAL; i++) { */
    for (i = 0; i < 1; i++) {
        if (0 == (dac_mge.ch & BIT(i))) {
            continue;
        }
        if (false == dac_cbuff_active(dac_mge.sound[i])) {
            continue;
        }
        rlen = cbuf_read(dac_mge.sound[i]->p_obuf, buf, len);
        fifo_dac_fill_kick(i, len / 2 / DAC_TRACK_NUMBER);//mio依赖单声道数据量

        dac_kick(dac_mge.sound[i], dac_mge.kick[i]);
    }
#if TCFG_AUDIO_AUTO_MUTE_ENABLE
    audio_energy_detect_run((s16 *)buf, len);
#endif
    return rlen;
}

AT(.audio_isr_text)
u32 fifo_dac_fill(u8 *buf, u32 len, AUDIO_TYPE type)
{
    return fill_dac_fill_phy(buf, len);
}
