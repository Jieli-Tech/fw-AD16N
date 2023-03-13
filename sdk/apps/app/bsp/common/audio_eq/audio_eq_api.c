#include "audio_eq.h"
#include "typedef.h"
#include "config.h"
#include "sound_mge.h"
#include "decoder_api.h"
#include "my_malloc.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[eq]"
#include "log.h"

//仅使用一路EQ，需要344字节ram；多路EQ时，每一路需要(344+64*最大段数)字节ram
const int config_hw_single_eq = 1;
const int config_eq_fade_enbale = 1;
const int config_eq_fade_step = 128;

const struct eq_seg_info eq_tab_normal[] = {
    {0, EQ_IIR_TYPE_BAND_PASS, 31,    0, 0.7f},
    {1, EQ_IIR_TYPE_BAND_PASS, 62,    0, 0.7f},
    {2, EQ_IIR_TYPE_BAND_PASS, 125,   0, 0.7f},
    {3, EQ_IIR_TYPE_BAND_PASS, 250,   0, 0.7f},
    {4, EQ_IIR_TYPE_BAND_PASS, 500,   0, 0.7f},
    {5, EQ_IIR_TYPE_BAND_PASS, 1000,  0, 0.7f},
    {6, EQ_IIR_TYPE_BAND_PASS, 2000,  0, 0.7f},
    {7, EQ_IIR_TYPE_BAND_PASS, 4000,  0, 0.7f},
    {8, EQ_IIR_TYPE_BAND_PASS, 8000,  0, 0.7f},
    {9, EQ_IIR_TYPE_BAND_PASS, 16000, 0, 0.7f},
};
const struct eq_seg_info eq_tab_rock[] = {
    {0, EQ_IIR_TYPE_BAND_PASS, 31,    -2, 0.7f},
    {1, EQ_IIR_TYPE_BAND_PASS, 62,     0, 0.7f},
    {2, EQ_IIR_TYPE_BAND_PASS, 125,    2, 0.7f},
    {3, EQ_IIR_TYPE_BAND_PASS, 250,    4, 0.7f},
    {4, EQ_IIR_TYPE_BAND_PASS, 500,   -2, 0.7f},
    {5, EQ_IIR_TYPE_BAND_PASS, 1000,  -2, 0.7f},
    {6, EQ_IIR_TYPE_BAND_PASS, 2000,   0, 0.7f},
    {7, EQ_IIR_TYPE_BAND_PASS, 4000,   0, 0.7f},
    {8, EQ_IIR_TYPE_BAND_PASS, 8000,   4, 0.7f},
    {9, EQ_IIR_TYPE_BAND_PASS, 16000,  4, 0.7f},
};
const struct eq_seg_info eq_tab_pop[] = {
    {0, EQ_IIR_TYPE_BAND_PASS, 31,     3, 0.7f},
    {1, EQ_IIR_TYPE_BAND_PASS, 62,     1, 0.7f},
    {2, EQ_IIR_TYPE_BAND_PASS, 125,    0, 0.7f},
    {3, EQ_IIR_TYPE_BAND_PASS, 250,   -2, 0.7f},
    {4, EQ_IIR_TYPE_BAND_PASS, 500,   -4, 0.7f},
    {5, EQ_IIR_TYPE_BAND_PASS, 1000,  -4, 0.7f},
    {6, EQ_IIR_TYPE_BAND_PASS, 2000,  -2, 0.7f},
    {7, EQ_IIR_TYPE_BAND_PASS, 4000,   0, 0.7f},
    {8, EQ_IIR_TYPE_BAND_PASS, 8000,   1, 0.7f},
    {9, EQ_IIR_TYPE_BAND_PASS, 16000,  2, 0.7f},
};
const struct eq_seg_info eq_tab_classic[] = {
    {0, EQ_IIR_TYPE_BAND_PASS, 31,     0, 0.7f},
    {1, EQ_IIR_TYPE_BAND_PASS, 62,     8, 0.7f},
    {2, EQ_IIR_TYPE_BAND_PASS, 125,    8, 0.7f},
    {3, EQ_IIR_TYPE_BAND_PASS, 250,    4, 0.7f},
    {4, EQ_IIR_TYPE_BAND_PASS, 500,    0, 0.7f},
    {5, EQ_IIR_TYPE_BAND_PASS, 1000,   0, 0.7f},
    {6, EQ_IIR_TYPE_BAND_PASS, 2000,   0, 0.7f},
    {7, EQ_IIR_TYPE_BAND_PASS, 4000,   0, 0.7f},
    {8, EQ_IIR_TYPE_BAND_PASS, 8000,   2, 0.7f},
    {9, EQ_IIR_TYPE_BAND_PASS, 16000,  2, 0.7f},
};
const struct eq_seg_info eq_tab_country[] = {
    {0, EQ_IIR_TYPE_BAND_PASS, 31,    -2, 0.7f},
    {1, EQ_IIR_TYPE_BAND_PASS, 62,     0, 0.7f},
    {2, EQ_IIR_TYPE_BAND_PASS, 125,    0, 0.7f},
    {3, EQ_IIR_TYPE_BAND_PASS, 250,    2, 0.7f},
    {4, EQ_IIR_TYPE_BAND_PASS, 500,    2, 0.7f},
    {5, EQ_IIR_TYPE_BAND_PASS, 1000,   0, 0.7f},
    {6, EQ_IIR_TYPE_BAND_PASS, 2000,   0, 0.7f},
    {7, EQ_IIR_TYPE_BAND_PASS, 4000,   0, 0.7f},
    {8, EQ_IIR_TYPE_BAND_PASS, 8000,   4, 0.7f},
    {9, EQ_IIR_TYPE_BAND_PASS, 16000,  4, 0.7f},
};
const struct eq_seg_info eq_tab_jazz[] = {
    {0, EQ_IIR_TYPE_BAND_PASS, 31,     0, 0.7f},
    {1, EQ_IIR_TYPE_BAND_PASS, 62,     0, 0.7f},
    {2, EQ_IIR_TYPE_BAND_PASS, 125,    0, 0.7f},
    {3, EQ_IIR_TYPE_BAND_PASS, 250,    4, 0.7f},
    {4, EQ_IIR_TYPE_BAND_PASS, 500,    4, 0.7f},
    {5, EQ_IIR_TYPE_BAND_PASS, 1000,   4, 0.7f},
    {6, EQ_IIR_TYPE_BAND_PASS, 2000,   0, 0.7f},
    {7, EQ_IIR_TYPE_BAND_PASS, 4000,   2, 0.7f},
    {8, EQ_IIR_TYPE_BAND_PASS, 8000,   3, 0.7f},
    {9, EQ_IIR_TYPE_BAND_PASS, 16000,  4, 0.7f},
};

typedef struct _EQ_TYPE_TAB_ {
    u32 eq_seg;//EQ系数表指针
    float global_gain;//对应系数总增益
} EQ_TYPE_TAB;
/* 添加需要参与切换的系数表 */
const EQ_TYPE_TAB eq_type_tab[] = {
    /* EQ系数表          总增益 */
    {(u32)eq_tab_normal,    0},
    {(u32)eq_tab_rock,      0},
    {(u32)eq_tab_pop,       0},
    {(u32)eq_tab_classic,  -8},
    {(u32)eq_tab_country,   0},
    {(u32)eq_tab_jazz,     -2},
};

u8 eq_mode;//EQ模式参数由应用层管理
void *link_eq_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr, u8 ch)
{
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    if (NULL == pp_effect) {
        log_info("eq init err\n");
        return p_curr_sound;
    }

    /* EQ参数配置 */
    eq_mode = 0;
    EQ_PARA_STRUCT para;
    para.sample_rate = sr;
    para.ch = ch;
    para.seg = (struct eq_seg_info *)(eq_type_tab[eq_mode].eq_seg);
    para.global_gain = eq_type_tab[eq_mode].global_gain;
    para.max_nsection = EQ_SECTION_MAX;

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

int eq_mode_sw(void *parm)
{
    EFFECT_OBJ *e_obj = (EFFECT_OBJ *)parm;
    if (NULL == e_obj) {
        return -1;
    }

    eq_mode++;
    if (eq_mode >= ARRAY_SIZE(eq_type_tab)) {
        eq_mode = 0;
    }

    log_info("eq_mode : %d\n", eq_mode);

    EQ_UPDATE update_parm;
    update_parm.eq_mode = eq_mode;
    update_parm.seg = (void *)(eq_type_tab[eq_mode].eq_seg);
    update_parm.global_gain = (eq_type_tab[eq_mode].global_gain);

    sound_in_obj *p_src_si = e_obj->p_si;
    EQ_STUCT_API *p_ops =  p_src_si->ops;
    p_ops->config(p_src_si->p_dbuf, EQ_CMD_SWITCH_TAB, (void *)&update_parm);
    return eq_mode;
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
