#include "app_modules.h"
#include "decoder_api.h"
#include "circular_buf.h"
#if PITCHSHIFT_HOWLING_EN
#include "howling_pitchshifter_api.h"
#include "howling_api.h"

#define LOG_TAG_CONST        NORM
#define LOG_TAG              "[pitchshift_howling]"
#include "log.h"

typedef struct _HOWLING_HDL {
    EFFECT_OBJ obj;
    sound_in_obj si;
    HOWL_PS_IO_CONTEXT io;
    HOWLING_PITCHSHIFT_PARM parm;
    u32 update;
} HOWLING_HDL;
const int FRESHIFT_SPEED_MODE_QUALITY = 2; //滤波器阶数参数：2<=>8;阶数越大，抑制效果越好，但对速度要求更高
HOWLING_HDL howling_hdl_save AT(.howling_data);
u32 howling_work_buf[3612 / 4] AT(.howling_data);

void *pitchshift_howling_api(void *obuf, u32 sr, void **ppsound)
{
    HOWLING_PITCHSHIFT_PARM phparm = {0};
    phparm.ps_parm                 = -200;   //等比移频参数，-350到350，归一化系数为8192
    phparm.fs_parm                 = 10;      //线性移频参数，-10到10，单位Hz
    phparm.effect_v                = EFFECT_HOWLING_PS; //开关位，可以为 EFFECT_HOWLING_PS或者 EFFECT_HOWLING_FS,或者 EFFECT_HOWLING_FS | EFFECT_HOWLING_PS
    HOWLING_PITCHSHIFT_FUNC_API *ops;
    ops = (HOWLING_PITCHSHIFT_FUNC_API *)get_howling_ps_func_api();
    u32 buf_len = ops->need_buf(phparm.effect_v);
    if (buf_len > sizeof(howling_work_buf)) {
        log_error("pitchshift howling need buf %d\n", buf_len);
        return NULL;
    }
    log_info("pitchshift howling work buf %d\n", buf_len);
    return howling_phy(obuf, &howling_work_buf[0], &phparm, sr, ppsound);
}

void *link_pitchshift_howling_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr)
{
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    p_curr_sound->effect = pitchshift_howling_api(p_curr_sound->p_obuf, sr, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
        log_info("pitchshift howling link success\n");
    } else {
        log_error("pitchshift howling link fail\n");
    }
    return p_curr_sound;
}

static int howling_run(void *hld, short *inbuf, int len)
{
    HOWLING_PITCHSHIFT_FUNC_API *ops;
    int res = 0;
    sound_in_obj *p_si = hld;
    ops = p_si->ops;
    res = ops->run(p_si->p_dbuf, inbuf, len);
    return res;
}

static void howling_parm_debug(HOWLING_HDL *howling_hdl)
{
    log_info("howling->parm.ps_parm %d\n", howling_hdl->parm.ps_parm);
    log_info("howling->parm.fs_parm %d\n", howling_hdl->parm.fs_parm);
    log_info("howling->parm.effect_v %d\n", howling_hdl->parm.effect_v);
}

void *howling_phy(void *obuf, void *dbuf, HOWLING_PITCHSHIFT_PARM *parm, u32 sr, void **ppsound)
{
    HOWLING_PITCHSHIFT_FUNC_API *ops;
    EFFECT_OBJ *howling_obj;
    sound_in_obj *howling_si;
    if (!parm) {
        log_error("howling parm NULL\n");
        return NULL;
    }
    ops = (HOWLING_PITCHSHIFT_FUNC_API *)get_howling_ps_func_api();
    memset(&howling_hdl_save, 0x0, sizeof(HOWLING_HDL));
    HOWLING_HDL *howling_hdl = &howling_hdl_save;
    if (howling_hdl && parm) {
        memcpy(&howling_hdl->parm, parm, sizeof(HOWLING_PITCHSHIFT_PARM));
        howling_parm_debug(howling_hdl);
    }
    u32 *howling_hdl_ptr = dbuf;

    howling_obj = &howling_hdl->obj;
    howling_hdl->io.priv = &howling_obj->sound;
    howling_hdl->io.output = sound_output;

    howling_si = &howling_hdl->si;
    howling_si->ops = ops;
    howling_si->p_dbuf = howling_hdl_ptr;

    howling_obj->p_si = howling_si;
    howling_obj->run = howling_run;
    howling_obj->sound.p_obuf = obuf;
    *ppsound = &howling_obj->sound;

    ops->open(howling_hdl_ptr, sr, &howling_hdl->parm, &howling_hdl->io);
    return howling_obj;
}

#endif
