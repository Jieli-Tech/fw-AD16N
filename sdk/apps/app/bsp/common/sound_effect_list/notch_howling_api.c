#include "app_modules.h"
#include "decoder_api.h"
#include "circular_buf.h"
#if NOTCH_HOWLING_EN
#include "howling_api.h"
#include "notch_howling_api.h"

#define LOG_TAG_CONST         NORM
#define LOG_TAG               "[notch_howling]"
#include "log.h"

void *notch_howling_api(void *obuf, u32 sr, void **ppsound)
{
    NotchHowlingParam nhparam = {0};
    nhparam.gain              = (int)(-10.0 * (1 << 20)); //陷波器压制程度，越大防啸叫越好，但发声啸叫频点误检时音质会更差
    nhparam.Q                 = (int)(2.0 * (1 << 24));   //陷波器带宽，越小防啸叫越好，但发声啸叫频点误检时音质会更差
    nhparam.fade_time         = 10;                       //启动时间与释放时间，越小启动与释放越快，可能导致杂音出现且音质变差
    nhparam.threshold         = (int)(25.0 * (1 << 15));  //频点啸叫判断阈值，越小越容易判定啸叫频点，但可能误检导致音质音质变差
    nhparam.SampleRate        = sr;
    return notch_howling_phy(obuf, &nhparam, ppsound);
}

void *link_notch_howling_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr)
{
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    p_curr_sound->effect = notch_howling_api(p_curr_sound->p_obuf, sr, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
        log_info("notch howling link success!\n");
    } else {
        log_error("notch howling link fail\n");
    }
    return p_curr_sound;
}

typedef struct _NH_HOWLING_HDL {
    EFFECT_OBJ obj;
    sound_in_obj si;
    NH_IO_CONTEXT io;
    NotchHowlingParam parm;
    u32 update;
    cbuffer_t cbuf;
} NH_HOWLING_HDL;

u8 notch_howling_work_buf[3472] AT(.howling_data);
NH_HOWLING_HDL notch_howling_hdl_save AT(.howling_data);

static void notch_howling_parm_debug(NH_HOWLING_HDL *howling_hdl)
{
    log_info("howling->parm.gain %d", howling_hdl->parm.gain);
    log_info("howling->parm.Q %d", howling_hdl->parm.Q);
    log_info("howling->parm.fade_time %d", howling_hdl->parm.fade_time);
    log_info("howling->parm.threshold %d", howling_hdl->parm.threshold);
    log_info("howling->parm.SampleRate %d", howling_hdl->parm.SampleRate);
}

static int notch_howling_run(void *hld, short *inbuf, int len)
{
    NH_HOWLING_HDL *howling_hdl = &notch_howling_hdl_save;
    if (!howling_hdl) {
        return 0;
    }
    NH_STRUCT_API *ops;
    int res = 0;
    sound_in_obj *p_si = hld;
    ops = (NH_STRUCT_API *)p_si->ops;
    res = ops->run(p_si->p_dbuf, inbuf, len);
    /* putchar('R'); */
    if (howling_hdl->update) {
        howling_hdl->update = 0;
        ops->update(p_si->p_dbuf, &howling_hdl->parm);
    }
    return res;
}

void *notch_howling_phy(void *obuf, NotchHowlingParam *parm, void **ppsound)
{
    u32 buf_len;
    NH_STRUCT_API *ops;
    EFFECT_OBJ *howling_obj;
    sound_in_obj *howling_si;
    if (!parm) {
        log_error("notch howling parm NULL\n");
        return NULL;
    }
    ops = get_notchHowling_ops();
    buf_len = ops->need_buf(parm);
    if (buf_len > sizeof(notch_howling_work_buf)) {
        log_error("notch howling buf need : %d\n", buf_len);
        return NULL;
    }
    u32 *howling_hdl_ptr = (u32 *)notch_howling_work_buf;
    log_info("notch howling work buf len %d\n", buf_len);
    memset(&notch_howling_hdl_save, 0x0, sizeof(NH_HOWLING_HDL));
    NH_HOWLING_HDL *howling_hdl = &notch_howling_hdl_save;
    if (howling_hdl && parm) {
        memcpy(&howling_hdl->parm, parm, sizeof(NotchHowlingParam));
        notch_howling_parm_debug(howling_hdl);
    }
    howling_obj = &howling_hdl->obj;
    howling_hdl->io.priv = &howling_obj->sound;
    howling_hdl->io.output = sound_output;

    howling_si = &howling_hdl->si;
    howling_si->ops = ops;
    howling_si->p_dbuf = howling_hdl_ptr;

    howling_obj->p_si = howling_si;
    howling_obj->run = notch_howling_run;
    howling_obj->sound.p_obuf = obuf;
    *ppsound = &howling_obj->sound;

    ops->open(howling_hdl_ptr, &howling_hdl->parm, &howling_hdl->io);
    return howling_obj;
}

#endif
