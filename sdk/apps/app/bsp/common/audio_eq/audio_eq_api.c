#include "audio_eq.h"
#include "typedef.h"
#include "config.h"
#include "sound_mge.h"
#include "decoder_api.h"
#include "my_malloc.h"
#include "app_config.h"
#include "effects_adj.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[eq]"
#include "log.h"

//仅使用一路EQ，需要344字节ram；多路EQ时，每一路需要(344+64*最大段数)字节ram
const int config_hw_single_eq = 1;
const int config_eq_fade_enbale = 1;
const int config_eq_fade_step = 128;

struct music_eq_tool music_eq_tool_online = {
    .global_gain = 0x0,
    .seg_num = EQ_SECTION_MAX,
    .enable_section = 0x0,
    .seg = {
        {0, EQ_IIR_TYPE_BAND_PASS, 31, 0, 0.7f},
        {1, EQ_IIR_TYPE_BAND_PASS, 62, 0, 0.7f},
        {2, EQ_IIR_TYPE_BAND_PASS, 125, 0, 0.7f},
        {3, EQ_IIR_TYPE_BAND_PASS, 250, 0, 0.7f},
        {4, EQ_IIR_TYPE_BAND_PASS, 500, 0, 0.7f},
        {5, EQ_IIR_TYPE_BAND_PASS, 1000, 0, 0.7f},
        {6, EQ_IIR_TYPE_BAND_PASS, 2000, 0, 0.7f},
        {7, EQ_IIR_TYPE_BAND_PASS, 4000, 0, 0.7f},
        {8, EQ_IIR_TYPE_BAND_PASS, 8000, 0, 0.7f},
        {9, EQ_IIR_TYPE_BAND_PASS, 16000, 0, 0.7f},
    },
};


u8 eq_mode = 0;		//EQ模式参数由应用层管理
void *link_eq_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr, u8 ch)
{
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    if (NULL == pp_effect) {
        log_info("eq init err\n");
        return p_curr_sound;
    }

    /* EQ参数配置 */
    EQ_PARA_STRUCT para;
    para.sample_rate = sr;
    para.ch = ch;
    /* #if TCFG_CFG_TOOL_ENABLE */
    para.seg = music_eq_tool_online.seg;
    para.global_gain = music_eq_tool_online.global_gain;
    para.max_nsection = music_eq_tool_online.seg_num;
    p_curr_sound->effect = eq_api(p_curr_sound->p_obuf, &para, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        *pp_effect = p_curr_sound->effect;
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
        /* log_info("eq init succ\n"); */
    } else {
        log_info("eq init fail\n");
    }
    return p_curr_sound;
}

/* 在线eq助手更改曲线时调用 */
void eq_online_sw(void *parm)
{
    EFFECT_OBJ *e_obj = (EFFECT_OBJ *)parm;
    if (NULL == e_obj) {
        return ;
    }

    EQ_UPDATE update_parm;
    update_parm.seg = music_eq_tool_online.seg;
    update_parm.global_gain = music_eq_tool_online.global_gain;

    sound_in_obj *p_src_si = e_obj->p_si;
    EQ_STUCT_API *p_ops =  p_src_si->ops;
    p_ops->config(p_src_si->p_dbuf, EQ_CMD_SWITCH_TAB, (void *)&update_parm);
}


/* 切换EQ参数组 */
void eq_mode_sw(void *parm)
{
    EFFECT_OBJ *e_obj = (EFFECT_OBJ *)parm;
    if (NULL == e_obj) {
        return ;
    }
    /* 切换eq参数组 */
    eq_mode++;
    if (eq_mode >= get_eq_mode_total_num()) {
        eq_mode = 0;
    }

    /* 获取名字 */
    void *ret_name = NULL;
    ret_name = get_eq_mode_name_api(eq_mode);
    log_info("now eq_mode is %d , eq_mode_name is  %s  ", eq_mode, ret_name);


    /* 读取eq_cfg.bin文件，获取对应eq参数组参数 */
    u32 ret = ext_eff_file_analyze_api(eq_mode, &music_eq_tool_online, sizeof(struct music_eq_tool));
    if (ret != 0) {
        log_info("ext_eff_file_analyze ret %d ", ret);
        return ;
    }

    /* 配置eq */
    EQ_UPDATE update_parm;
    memset(&update_parm, 0, sizeof(update_parm));

    update_parm.seg = music_eq_tool_online.seg;
    update_parm.global_gain = music_eq_tool_online.global_gain;

    sound_in_obj *p_src_si = e_obj->p_si;
    EQ_STUCT_API *p_ops =  p_src_si->ops;
    p_ops->config(p_src_si->p_dbuf, EQ_CMD_SWITCH_TAB, (void *)&update_parm);
}

/***************************phy***************************************************************/
void *eq_hld_malloc(u32 malloc_size)
{
    return my_malloc(malloc_size, MM_EQ);
}
void eq_reless(void **ppeffect)
{
    EFFECT_OBJ *p_eobj = *ppeffect;
    log_info("eq free : 0x%x", (u32) p_eobj);
    if (NULL != p_eobj) {
        *ppeffect = my_free(p_eobj);
    }
}
static int eq_run_api(void *hld, short *inbuf, int len)
{
    sound_in_obj *p_si = (sound_in_obj *)hld;
    EQ_STUCT_API *ops = (EQ_STUCT_API *)p_si->ops;
    return ops->run(p_si->p_dbuf, inbuf, len);
}

static void *eq_api(void *obuf, EQ_PARA_STRUCT *p_para, void **ppsound)
{
    u32 buff_len, i;
    EQ_STUCT_API *ops = (EQ_STUCT_API *)get_eq_context();
    buff_len = ops->need_buf(p_para->max_nsection, p_para->ch);
    EQ_HDL *p_eq_hdl = eq_hld_malloc(buff_len);
    if (NULL == p_eq_hdl) {
        log_error("eq malloc fail, need buff_len:%d\n", buff_len);
        return 0;
    }
    log_info("eq malloc succ, hdl:0x%x len:%d\n", (u32)p_eq_hdl, buff_len);

    memset((void *)p_eq_hdl, 0, buff_len);
    ops->open((unsigned int *)&p_eq_hdl->buff, p_para, (void *)&p_eq_hdl->io);

    EFFECT_OBJ *p_eq_obj = &p_eq_hdl->obj;
    p_eq_hdl->io.priv = &p_eq_obj->sound;
    p_eq_hdl->io.output = sound_output;
    p_eq_hdl->si.ops = ops;
    p_eq_hdl->si.p_dbuf = &p_eq_hdl->buff;
    p_eq_hdl->obj.p_si = &p_eq_hdl->si;
    p_eq_hdl->obj.run = eq_run_api;
    p_eq_hdl->obj.sound.p_obuf = obuf;

    *ppsound = &p_eq_obj->sound;
    return p_eq_obj;
}
