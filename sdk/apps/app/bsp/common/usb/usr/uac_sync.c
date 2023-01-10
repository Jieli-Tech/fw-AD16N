
#pragma bss_seg(".usb_audio_if.data.bss")
#pragma data_seg(".usb_audio_if.data")
#pragma const_seg(".usb_audio_if.text.const")
#pragma code_seg(".usb_audio_if.text")
#pragma str_literal_override(".usb_audio_if.text.const")
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "usb/device/uac_audio.h"
#include "usb/usr/usb_audio_interface.h"
#include "usb/usr/usb_mic_interface.h"
#include "usb/usr/uac_sync.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[SYNC]"
#include "log.h"
#include "uart.h"

/* static u8 pe5_cnt; */
/* static s16 x_step = 5; */
/* static u8 pe5_step  = 2; */
/* static u16 pe_inc_data; */
/* static u16 pe_sub_data; */
/* static u8 pe_cnt  = 0; */
/* static u8 last_pe = 0xff; */
/* static u32 sync_cnt; */
/* u32 uac_sync_parm; */


void uac_sync_init(uac_sync *sync, s32 sr)
{
    if (NULL == sync) {
        return;
    }
    memset(sync, 0, sizeof(uac_sync));
    /* sync->sr_curr = 0; */
    /* sync->pe_cnt  = 0; */
    /* sync->pe5_cnt  = 0; */
    sync->pe5_step  = 2;
    /* sync->pe_inc_data = 0; */
    /* sync->pe_sub_data = 0; */
    sync->last_pe = 0xff;
    /* sync->last_sr = 0; */
    sync->x_step = sr * 5 / 10000;

}
static u32 uac_sr_milli(u32 sr)
{
#if 0
    u32 tmp;

    if (sr > sr_curr) {
        tmp = sr - sr_curr;
    } else {
        tmp = sr_curr - sr ;
    }
    u32 milli = tmp * 1000 / sr;
    return milli;
#else
    return 0;
#endif

}



void uac_1s_sync(void)
{
    return;
#if 0
    u32 sr_1s = uac_speaker_stream_1s();
    sr_curr = sr_1s;
    if (0 == sr_1s) {
        return;
    }
    if (0 == uac_is_need_sync()) {
        return;
    }
    sound_in_obj *p_src_si = usb_src_obj->p_si;
    SRC_STUCT_API *p_ops =  p_src_si->ops;
    if (0 != sync->last_sr) {
        u32 milli = uac_sr_milli(sync->last_sr);
        if (milli < 4) {
            return;
        }
    }
    log_info(" 1S SR %d\n", sr_1s);
    /*uac_sync_parm =  p_ops->config(p_src_si->p_dbuf, SRC_CMD_INSR_SET, (void *)sr_1s); */
    sync->last_sr = sr_1s;
#endif
}

static void uac_inc_sync_pe_reset(uac_sync *sync)
{
    sync->pe5_step += 4;
    sync->pe5_cnt = sync->pe5_step;
    sync->pe_inc_data = 0;
    sync->pe_sub_data = 0;
    /* sync->pe_cnt = 0; */
    sync->last_pe = 0xff;
}

void uac_inc_sync_one(EFFECT_OBJ *e_obj, u32 percent, uac_sync *sync)
{
    /* if (0 == uac_is_need_sync()) { */
    /* return; */
    /* } */
    sound_in_obj *p_src_si = e_obj->p_si;
    SRC_STUCT_API *p_ops =  p_src_si->ops;

    char c = 0;
    s32 step = 0;
    /* u32 percent;// = uac_read_cnt(); */
    /* if (0 != uac_spk_cnt) { */
    /* local_irq_disable(); */
    /* percent = uac_spk_all / uac_spk_cnt; */
    /* uac_spk_all = 0; */
    /* uac_spk_cnt = 0; */
    /* local_irq_enable(); */
    /* } else { */
    /* return; */
    /* } */
    /* log_info("%d",percent); */
    /* log_xprint("", "",  "%d", percent); */
    if (percent > 66) {
        if (0 == sync->pe5_cnt) {
            c = '+';
            step = sync->x_step;
            uac_inc_sync_pe_reset(sync);
        }
        sync->sync_cnt = 0;
    } else if (percent < 34) {
        if (0 == sync->pe5_cnt) {
            c = '-';
            step = 0 - sync->x_step;
            uac_inc_sync_pe_reset(sync);
        }
        sync->sync_cnt = 0;
    } else {
        if (sync->sync_cnt > 60) {
            sync->sync_cnt = 0;
            if (sync->x_step > 2) {
                /* u32 tmp  = x_step / 6; */
                /* x_step -= tmp ; */
            }
            /* c = '='; */
        }
        sync->sync_cnt++;
        sync->pe5_step = 2;
    }
    if (0 != sync->pe5_cnt) {
        sync->pe5_cnt--;
    }
#if 1
    /* pe_cnt++; */
    /* if (pe_cnt >= 100) { */
    /* pe_cnt = 0; */
    /* pe_inc_data = 0; */
    /* pe_sub_data = 0; */
    /* } */

    if (sync->last_pe <= 100) {
        if (percent < sync->last_pe) {
            sync->pe_sub_data += (sync->last_pe - percent);
        } else if (percent > sync->last_pe) {
            sync->pe_inc_data += (percent - sync->last_pe);
        }

        /* if (0 == (pe_cnt % 30)) { */
        /* pe_inc_data = 0; */
        /* pe_sub_data = 0; */

        /* } */
        if (sync->pe_sub_data > sync->pe_inc_data) {
            sync->pe_sub_data -= sync->pe_inc_data;
            sync->pe_inc_data = 0;
            if (sync->pe_sub_data > 14) {
                c = 's';
                step =  0 - sync->x_step;
                sync->pe_sub_data = 0;
            }
        } else {
            sync->pe_inc_data -= sync->pe_sub_data;
            sync->pe_sub_data = 0;
            if (sync->pe_inc_data > 14) {
                c = 'p';
                step = sync->x_step;
                sync->pe_inc_data = 0;
            }
        }

    }
#endif
    if (c != 0) {
        log_char(c);
        /* log_char( (u8)0xff & percent ); */
    }
    if (0 != step) {
        sync->uac_sync_parm = p_ops->config(
                                  p_src_si->p_dbuf,
                                  SRC_CMD_INSR_INC_SET,
                                  (void *)step
                              );
    }

    sync->last_pe = percent;
}


extern uac_sync uac_spk_sync;
extern uac_sync uac_mic_sync;
void uac_inc_sync(void)
{

    EFFECT_OBJ *e_obj = NULL;
    u32 percent;
    /* log_char('a'); */
#if ( TCFG_PC_ENABLE && (USB_DEVICE_CLASS_CONFIG & MIC_CLASS))
#if TCFG_MIC_SRC_ENABLE
    e_obj = uac_mic_percent(&percent);
    if (NULL != e_obj) {
        /* log_char('b'); */
        uac_inc_sync_one(e_obj, percent, &uac_mic_sync);
    }
    /* log_char('c'); */
#endif
#endif

#if TCFG_SPK_SRC_ENABLE
    e_obj = uac_spk_percent(&percent);
    if (NULL != e_obj) {
        /* log_char('d'); */
        uac_inc_sync_one(e_obj, percent, &uac_spk_sync);
    }
    /* log_char('e'); */
#endif
}

