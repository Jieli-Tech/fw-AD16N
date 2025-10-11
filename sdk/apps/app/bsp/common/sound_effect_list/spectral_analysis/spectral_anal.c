#include "spectral_anal.h"
#include "config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[spectral_anal]"
#include "log.h"

static u32 spectral_anal_buff[2148 / 4];
#define MAX_POINT    (320)
#define MAX_EQ_SEG   (10)
static const struct eq_seg_info eq_seg[MAX_EQ_SEG] = {
    {0, EQ_IIR_TYPE_BAND_PASS_NEW, 31, 0, 10.0f},
    {1, EQ_IIR_TYPE_BAND_PASS_NEW, 62, 0, 10.0f},
    {2, EQ_IIR_TYPE_BAND_PASS_NEW, 125, 0, 10.0f},
    {3, EQ_IIR_TYPE_BAND_PASS_NEW, 250, 0, 10.0f},
    {4, EQ_IIR_TYPE_BAND_PASS_NEW, 500, 0, 10.0f},
    {5, EQ_IIR_TYPE_BAND_PASS_NEW, 1000, 0, 10.0f},
    {6, EQ_IIR_TYPE_BAND_PASS_NEW, 2000, 0, 10.0f},
    {7, EQ_IIR_TYPE_BAND_PASS_NEW, 4000, 0, 10.0f},
    {8, EQ_IIR_TYPE_BAND_PASS_NEW, 8000, 0, 10.0f},
    {9, EQ_IIR_TYPE_BAND_PASS_NEW, 16000, 0, 10.0f}
};

short follow_db[MAX_EQ_SEG][2];
void printf_follow_db(void)
{
    for (int i = 0; i < MAX_EQ_SEG; i++) {
        log_info("%d : L:%d R:%d", eq_seg[i].freq, (u32)follow_db[i][0], (u32)follow_db[i][1]);
    }
    putchar('\n');
}
void getFollowdB_api(void)
{
    _SPECTRAL_ANAL_HDL *p_sa_hdl = (_SPECTRAL_ANAL_HDL *)&spectral_anal_buff[0];
    u8 ch = p_sa_hdl->buff.para.ch;
    u32 offset = p_sa_hdl->buff.follow_buff_info.part_size;
    void *fo_workbuf = (void *)p_sa_hdl->buff.follow_buff_info.start_offset;
    /* 获取每一段频点的dB值（未归一化，范围0~90dB） */
    for (int i = 0; i < MAX_EQ_SEG; i++) {
        local_irq_disable();
        short *res_db = getFollowDB(fo_workbuf + i * offset);
        local_irq_enable();
        if (ch == 2) {
            follow_db[i][0] = res_db[0];
            follow_db[i][1] = res_db[1];
        } else {
            follow_db[i][0] = res_db[0];
        }
    }
    printf_follow_db();
}

static int spectral_anal_run_api(void *hld, short *inbuf, int len)
{
    sound_in_obj *p_si = (sound_in_obj *)hld;
    _SPECTRAL_ANAL_OPS *ops = (_SPECTRAL_ANAL_OPS *)p_si->ops;
    int output_len = ops->run(p_si->p_dbuf, inbuf, len);
    /* printf("sa_run %d %d\n", output, len); */
    return output_len;
}

static void *spectral_anal_api(void *obuf, void **ppsound, u32 sr, u8 ch)
{
    _SPECTRAL_ANAL_PARA_STRUCT eq_parm = {0};
    eq_parm.seg = (struct eq_seg_info *)&eq_seg[0];
    eq_parm.ch = ch;
    eq_parm.max_seg = MAX_EQ_SEG;
    eq_parm.max_point = MAX_POINT;
    eq_parm.sample_rate = sr;

    FollowParam fo_parm = {0};
    fo_parm.SampleRate = sr;
    fo_parm.channel = ch;
    fo_parm.attackTime = 5;     //跟随时间，单位ms
    fo_parm.releaseTime = 300;  //释放时间，单位ms
    fo_parm.rmsTime = 10;
    fo_parm.algorithm = PEAK;
    fo_parm.mode = TWOPOINT;    //间隔2个点运算一次

    u32 buff_len, i;
    _SPECTRAL_ANAL_OPS *ops = (_SPECTRAL_ANAL_OPS *)get_spectral_anal_context();
    buff_len = ops->need_buf(eq_parm.max_seg, &fo_parm, MAX_POINT);
    if (buff_len > sizeof(spectral_anal_buff)) {
        log_info("sa need buff size:%d, now buff size:%d is too small!\n", buff_len, sizeof(spectral_anal_buff));
        return NULL;
    }

    /* log_info("spectral_anal_buff 0x%x -> 0x%x %d\n", spectral_anal_buff, \ */
    /*         (u32)spectral_anal_buff + sizeof(spectral_anal_buff), \ */
    /*         sizeof(spectral_anal_buff)); */

    memset((void *)&spectral_anal_buff[0], 0, sizeof(spectral_anal_buff));
    _SPECTRAL_ANAL_HDL *p_sa_hdl = (_SPECTRAL_ANAL_HDL *)&spectral_anal_buff[0];
    ops->open((unsigned int *)&p_sa_hdl->buff, (void *)&p_sa_hdl->io, &eq_parm, &fo_parm);

    EFFECT_OBJ *p_sa_obj = &p_sa_hdl->obj;
    p_sa_hdl->io.priv = &p_sa_obj->sound;
    p_sa_hdl->io.output = sound_output;
    p_sa_hdl->si.ops = ops;
    p_sa_hdl->si.p_dbuf = &p_sa_hdl->buff;
    p_sa_hdl->obj.p_si = &p_sa_hdl->si;
    p_sa_hdl->obj.run = spectral_anal_run_api;
    p_sa_hdl->obj.sound.p_obuf = obuf;

    *ppsound = &p_sa_obj->sound;
    return p_sa_obj;
}

void *link_spectral_anal_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr, u8 ch)
{
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;

    p_curr_sound->effect = spectral_anal_api(p_curr_sound->p_obuf, (void **)&p_next_sound, sr, ch);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
        log_info("spectral_anal init succ, hdl:0x%x len:%d\n", (u32)spectral_anal_buff, sizeof(spectral_anal_buff));
    } else {
        log_info("spectral_anal init fail\n");
    }
    return p_curr_sound;
}
