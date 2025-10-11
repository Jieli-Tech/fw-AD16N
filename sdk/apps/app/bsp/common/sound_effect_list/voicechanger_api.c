#include "app_modules.h"
#if VO_CHANGER_EN
#include "voicechanger_api.h"
#include "sound_mge.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[voice_changer]"
#include "log.h"

VOICECHANGER_AV_PARM vc_parm   AT(.voicechanger_data);
VOICESYN_AV_PARM vs_parm	   AT(.voicechanger_data);
EFFECT_OBJ vchange_obj         AT(.voicechanger_data);
static sound_in_obj vchange_si AT(.voicechanger_data);
static u32 vc_sr;
const int VC_NG_THRES = 712;      //噪声门限，建议用于底噪较大的方案
u32 vc_buflen[0x2010 / 4] 	   AT(.voicechanger_data);

void *voice_changer_api(void *obuf, u32 sr, void **ppsound)
{
    vc_parm.shiftv = 65;
    vc_parm.formant_shift = 100;
    vc_parm.speedv = 80;
    vc_parm.effect_v = EFFECT_VC_AV_BIRD6;

    vs_parm.randpercent = 100;
    vs_parm.vibrate_lenCtrol = 30;
    vs_parm.vibrate_rate_u = 0;
    vs_parm.vibrate_rate_d = 100;

    log_info("vc_parm.shiftv %d\n", vc_parm.shiftv);
    log_info("vc_parm.formant_shift %d\n", vc_parm.formant_shift);
    log_info("vc_parm.speedv %d\n", vc_parm.speedv);
    log_info("vc_parm.effect_v 0x%x\n", vc_parm.effect_v);

    log_info("vs_parm.randpercent %d\n", vs_parm.randpercent);
    log_info("vs_parm.vibrate_lenCtrol %d\n", vs_parm.vibrate_lenCtrol);
    log_info("vs_parm.vibrate_rate_u %d\n", vs_parm.vibrate_rate_u);
    log_info("vs_parm.vibrate_rate_d %d\n", vs_parm.vibrate_rate_d);
    return voice_changer_phy(obuf, sr, &vc_parm, &vs_parm, ppsound);
}

const struct _VCAD_IO_CONTEXT_ vc_pitch_io = {
    &vchange_obj.sound,
    sound_output,
};

int voice_changer_run(void *hld, short *inbuf, int len)
{
    VOICECHANGER_A_FUNC_API *ops;
    int res = 0;
    sound_in_obj *p_si = hld;
    ops = p_si->ops;
    res = ops->run(p_si->p_dbuf, inbuf, len);
    return res;
}

void update_voice_changer_parm(VOICECHANGER_AV_PARM *new_vc_parm, VOICESYN_AV_PARM *new_vsyn_ctrol)
{
    if ((NULL == new_vc_parm) || (NULL == new_vsyn_ctrol)) {
        return;
    }

    /* log_info("new_vc_parm->shiftv %d\n", new_vc_parm->shiftv); */
    /* log_info("new_vc_parm->formant_shift %d\n", new_vc_parm->formant_shift); */
    /* log_info("new_vc_parm->speedv %d\n", new_vc_parm->speedv); */
    /* log_info("new_vc_parm->effect_v 0x%x\n", new_vc_parm->effect_v); */

    /* log_info("new_vsyn_ctrol->vibrate_rate_u %d\n", new_vsyn_ctrol->vibrate_rate_u); */
    /* log_info("new_vsyn_ctrol->vibrate_rate_d %d\n", new_vsyn_ctrol->vibrate_rate_d); */
    /* log_info("new_vsyn_ctrol->vibrate_lenCtrol %d\n", new_vsyn_ctrol->vibrate_lenCtrol); */
    /* log_info("new_vsyn_ctrol->randpercent 0x%x\n", new_vsyn_ctrol->randpercent); */

    VOICECHANGER_A_FUNC_API *ops;
    ops = get_voiceChangerA_func_api();
    OS_ENTER_CRITICAL();
    ops->open(&vc_buflen[0], vc_sr, new_vc_parm, new_vsyn_ctrol, NULL);
    OS_EXIT_CRITICAL();
}

void *link_voice_changer_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 in_sr)
{
    log_info("linking voice changer sound\n");
    log_info("sample rate %d\n", in_sr);
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    p_curr_sound->effect = voice_changer_api(p_curr_sound->p_obuf, in_sr, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
        log_info("voice change init succ\n");
    } else {
        log_info("voice change init fail\n");
    }
    return p_curr_sound;
}


void *voice_changer_phy(void *obuf, u32 sr, VOICECHANGER_AV_PARM *pvc_parm, VOICESYN_AV_PARM *pvs_parm, void **ppsound)
{
    u32 need_buff_len;
    VOICECHANGER_A_FUNC_API *ops;
    ops = get_voiceChangerA_func_api();
    need_buff_len = ops->need_buf(sr, pvc_parm);
    if (need_buff_len > sizeof(vc_buflen)) {
        log_error("buff_len not enough, need 0x%x\n", need_buff_len);
        return 0;
    }
    log_info("need buff len 0x%x, use len 0x%x\n", need_buff_len, sizeof(vc_buflen));
    ops->open(&vc_buflen[0], sr, pvc_parm, pvs_parm, (void *)&vc_pitch_io);
    vc_sr = sr;
    memset(&vchange_obj, 0, sizeof(vchange_obj));
    vchange_si.ops = ops;
    vchange_si.p_dbuf = &vc_buflen[0];

    vchange_obj.p_si = &vchange_si;
    vchange_obj.run = voice_changer_run;
    vchange_obj.sound.p_obuf = obuf;
    *ppsound = &vchange_obj.sound;
    return &vchange_obj;
}

#endif
