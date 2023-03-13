#include "app_modules.h"
#include "decoder_api.h"
#if VO_PITCH_EN
#include "vo_pitch_api.h"
#include "midi_file0_h.h"
#include "audio_adc.h"

#define LOG_TAG_CONST         NORM
#define LOG_TAG               "[vo_pitch]"
#include "log.h"

#define NEED_FILTER           0
u32 VP_BUFLEN[7304 / 4] AT(.vp_data);

const int VC_ENABLE_FLAG = 1;             //变声模块使能
const int EXTRA_DATA_SIZE = 500;          //rap模式所可以包含的音源长度
const int VO_RAP_LOOPEN = 1;              //rap模式是否repeat音源
const int VO_RAP_COMPRESS_RATE = 460;     //0到460：越大音源时间压缩越多
const int VP_DECAY_VAL = 70;              //ECHO模式的decay速度
const int VP_HIS_LEN = 2000;              //ECHO模式的decay：复用其他模式的Buf

#if NEED_FILTER
u32 NOFILT_BUFLEN[512 / 4] AT(.vp_data);
const int no_filtnum = 3;                 //滤波器个数
const int vol_out = 170;                  //输出音量。配置成128，则输出不放大，否则倍数vol_out/128

const short filtCONFIG[] = {              //参数依次代表：滤波器类型，中心频率，带宽，增益
    FILT_NORH, 3000, 724, 0,              //陷3K频点，带宽配得越大影响范围越宽
    FILT_HIGHSELF, 1000, 1024, -5 * 1024, //1k以上频率削弱5db
    FILT_NORH, 2200, 700, 0,
    FILT_BAND, 1000, 100, -8384,          //1k频点压一半，带宽配得越大, 带宽建议范围30到2000
};
#endif

VOICE_PITCH_PARA_STRUCT vp_parm AT(.vp_data);
EFFECT_OBJ vp_obj AT(.vp_data);
static sound_in_obj vp_si AT(.vp_data);

void *voice_pitch_api(void *obuf, u32 cmd, u32 sr, void **ppsound)
{
    bool flag = 1;
    VOICE_PITCH_PARA_STRUCT vc_parm;
    log_info("voice pitch api\n");

    flag = vp_cmd_case(cmd, sr, &vc_parm);
    if (!flag) {
        log_error("negative found voice pitch effect!\n");
        return NULL;
    }
    return vp_phy(obuf, &vc_parm, ppsound, sr);
}
void voice_pitch_debug(VOICE_PITCH_PARA_STRUCT *p_vp_parm)
{
    log_info("p_vp_parm->do_flag 0x%x\n", p_vp_parm->do_flag);
    log_info("p_vp_parm->samplerate %d\n", p_vp_parm->samplerate);
    log_info("p_vp_parm->noise_dc %d\n", p_vp_parm->noise_dc);
    log_info("p_vp_parm->pitchrate %d\n", p_vp_parm->pitchrate);
    log_info("p_vp_parm->hamorrate %d\n", p_vp_parm->hamorrate);
}

sound_out_obj *link_voice_pitch_sound(sound_out_obj *p_curr_sound, void *p_dac_cbuf, void **pp_effect, u32 sr, u32 cmd)
{
    sound_out_obj *p_next_sound = 0;
    p_curr_sound->effect = voice_pitch_api(p_curr_sound->p_obuf, cmd, sr, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
        log_info("voice pitch link success\n");
    } else {
        log_error("voice pitch link fail\n");
    }
    return p_curr_sound;
}

u8 startindexflag;
void rap_callback(void *priv, int pos)
{
    u32 sr = (u32)priv;
    if (pos == RAP_PREPARE) {
        startindexflag = 0;   //进入不会出数阶段，只会消耗来数，生成音源
    } else if (pos == RAP_START) {
        startindexflag = 1;   //进入run会出数，但是跟输入的数据无关，根据midi表来生成的阶段
    } else if (pos == RAP_END) {
        startindexflag = 0;   //输出完毕
        /* rap_reopen(sr); */
    }
}
/************ 动态修改变音参数 **************/
void update_voice_pitch_parm(void *priv, VOICE_PITCH_PARA_STRUCT *new_vp_parm)
{
    if ((NULL == priv) || (NULL == new_vp_parm)) {
        return;
    }
    log_info("Update voice pitch parm\n");

    EFFECT_OBJ *e_obj = (EFFECT_OBJ *)priv;
    sound_in_obj *p_si = e_obj->p_si;
    VOICEPITCH_STUCT_API *ops = p_si->ops;
    OS_ENTER_CRITICAL();
    ops->open(p_si->p_dbuf, new_vp_parm, NULL);
    OS_EXIT_CRITICAL();
}
/******************************************/
const struct _VP_IO_CONTEXT_ vp_pitch_io = {
    &vp_obj.sound,
    sound_output,
};

static int vp_run(void *hld, short *inbuf, int len)
{
    VOICEPITCH_STUCT_API *ops;
    int res = 0;
    sound_in_obj *p_si = hld;
    ops = p_si->ops;
    res = ops->run(p_si->p_dbuf, inbuf, len);
    return res;
}

void *vp_phy(void *obuf, VOICE_PITCH_PARA_STRUCT *pvp_parm, void **ppsound, u32 sr)
{
    u32 buf_len;
    VOICEPITCH_STUCT_API *ops;
    log_info("voice pitch api\n");

    ops = get_vopitch_context();
    buf_len = ops->need_buf();
    if (buf_len > sizeof(VP_BUFLEN)) {
        log_error("vo pitch buff need 0x%x\n", buf_len);
        return NULL;
    }
#if NEED_FILTER
    void *filt_ptr = (void *)NOFILT_BUFLEN;
    no_filt_init(filt_ptr, sr, (VP_IO_CONTEXT *)&vp_pitch_io);
    struct _VP_IO_CONTEXT_ vp_io2;
    vp_io2.priv = filt_ptr;
    vp_io2.output = no_filt_do;
    ops->open(&VP_BUFLEN[0], pvp_parm, (void *)&vp_io2);
    log_info("vo_pitch use filter\n");
    log_info("vo pitch buff 0x%x\n", sizeof(VP_BUFLEN) + sizeof(NOFILT_BUFLEN));

#else
    log_info("vo pitch buff 0x%x\n", sizeof(VP_BUFLEN));
    ops->open(&VP_BUFLEN[0], pvp_parm, (void *)&vp_pitch_io);
#endif
    memset(&vp_obj, 0, sizeof(vp_obj));
    vp_si.ops = ops;
    vp_si.p_dbuf = &VP_BUFLEN[0];

    vp_obj.p_si = &vp_si;
    vp_obj.run = vp_run;
    vp_obj.sound.p_obuf = obuf;
    *ppsound = &vp_obj.sound;
    return &vp_obj;
}

/******* rap模式结束时调用该函数重新开始rap模式 ******/
void rap_reopen(u32 sr)
{
    log_info("rap reopen!\n");
    VOICE_PITCH_PARA_STRUCT p_vc_parm;
    u32 tmp_sr = sr;
    /* log_info("reopen sr %d\n", sr); */
    vp_cmd_rap(tmp_sr, &p_vc_parm);

    VOICEPITCH_STUCT_API *ops;
    ops = get_vopitch_context();
    ops->open(&VP_BUFLEN[0], &p_vc_parm, NULL);
}


/************ 变调模式参数*********/
void vp_cmd_pitchshift(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    log_info("voice pitch pitchshift\n");
    p_vc_parm->do_flag = HARMO_PITCHSHIFT;
    p_vc_parm->samplerate = sr;
    p_vc_parm->noise_dc = 2048;
    p_vc_parm->pitchrate = 80;    //>128音调升高，<128音调变低
    //无效参数
    p_vc_parm->hamorrate = 128;
    p_vc_parm->midi_file = midifile_file0_tab;
    p_vc_parm->midifile_len = sizeof(midifile_file0_tab);
    p_vc_parm->callback = rap_callback;
    p_vc_parm->priv = NULL;
}

/************ 平调机器音模式参数*********/
void vp_cmd_robot(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    log_info("voice pitch robot\n");
    p_vc_parm->do_flag = HARMO_ROBORT;
    p_vc_parm->samplerate = sr;
    p_vc_parm->noise_dc = 2048;
    p_vc_parm->pitchrate = 80;     //机器音音高，50到250
    p_vc_parm->hamorrate = 100;    //机器音音色，50到250
    //无效参数
    p_vc_parm->midi_file = midifile_file0_tab;
    p_vc_parm->midifile_len = sizeof(midifile_file0_tab);
    p_vc_parm->callback = rap_callback;
    p_vc_parm->priv = NULL;
}

/************ 变调机器音模式参数 *********/
void vp_cmd_robot2(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    log_info("voice pitch robot2\n");
    p_vc_parm->do_flag = HARMO_ROBORT2;
    p_vc_parm->samplerate = sr;
    p_vc_parm->noise_dc = 2048;
    p_vc_parm->pitchrate = 180;    //调出不同的声音
    //无效参数
    p_vc_parm->hamorrate = 100;
    p_vc_parm->midi_file = midifile_file0_tab;
    p_vc_parm->midifile_len = sizeof(midifile_file0_tab);
    p_vc_parm->callback = rap_callback;
    p_vc_parm->priv = NULL;
}

/************ 你说我唱模式参数*********/
void vp_cmd_rap_realtime(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    log_info("voice pitch rap_realtime\n");
    p_vc_parm->do_flag = HARMO_RAP_REALTIME;
    p_vc_parm->samplerate = sr;
    p_vc_parm->noise_dc = 2048;
    p_vc_parm->pitchrate = 149;    //改变音高，50到250
    p_vc_parm->hamorrate = 100;    //改变音色，50到250
    p_vc_parm->midi_file = midifile2_file1_tab; //改变旋律
    p_vc_parm->midifile_len = sizeof(midifile2_file1_tab);
    //无效参数
    p_vc_parm->callback = rap_callback;
    p_vc_parm->priv = NULL;
}

/************ CARTOON模式参数*********/
void vp_cmd_cartoon(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    log_info("voice pitch cartoon\n");
    p_vc_parm->do_flag = HARMO_CARTOON;
    p_vc_parm->samplerate = sr;
    p_vc_parm->noise_dc = 2048;
    p_vc_parm->pitchrate = 70;    //50到250
    p_vc_parm->hamorrate = 400;   //-800到800
    //无效参数
    p_vc_parm->midi_file = NULL;
    p_vc_parm->midifile_len = 0;
    p_vc_parm->callback = NULL;
    p_vc_parm->priv = NULL;
}

/*********** 变调模式2 ***********/
void vp_cmd_pitchshift2(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    log_info("voice pitch pitchshift2\n");
    p_vc_parm->do_flag = HARMO_PITCHSHIFT2;
    p_vc_parm->samplerate = sr;
    p_vc_parm->noise_dc = 2048;
    p_vc_parm->pitchrate = 60;    //50到250
    p_vc_parm->hamorrate = 90;    //50到250
    //无效参数
    p_vc_parm->midi_file = midifile_file0_tab;
    p_vc_parm->midifile_len = sizeof(midifile_file0_tab);
    p_vc_parm->callback = rap_callback;
    p_vc_parm->priv = NULL;
}

/************ RAP 模式参数[所有参数都有效]*********/
void vp_cmd_rap(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    log_info("voice pitch rap\n");
    p_vc_parm->do_flag = HARMO_RAP;
    p_vc_parm->samplerate = sr;
    p_vc_parm->noise_dc = 2048;
    p_vc_parm->pitchrate = 100;
    p_vc_parm->hamorrate = 128;
    p_vc_parm->midi_file = midifile_file0_tab;
    p_vc_parm->midifile_len = sizeof(midifile_file0_tab);
    p_vc_parm->callback = rap_callback;
    p_vc_parm->priv = (void *)sr;
}

/*********** ECHO模式 ***********/
void vp_cmd_echo(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    log_info("voice pitch echo\n");
    p_vc_parm->do_flag = HARMO_ECHO;
    p_vc_parm->samplerate = sr;
    //无效参数
    p_vc_parm->noise_dc = 2048;
    p_vc_parm->pitchrate = 80;
    p_vc_parm->hamorrate = 100;
    p_vc_parm->midi_file = midifile_file0_tab;
    p_vc_parm->midifile_len = sizeof(midifile_file0_tab);
    p_vc_parm->callback = rap_callback;
    p_vc_parm->priv = NULL;
}

bool vp_cmd_case(VP_CMD cmd, u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm)
{
    switch (cmd) {
    case VP_CMD_PITCHSHIFT:
        vp_cmd_pitchshift(sr, p_vc_parm);
        break;
    case VP_CMD_ROBOT:
        vp_cmd_robot(sr, p_vc_parm);
        break;
    case VP_CMD_ROBOT2:
        vp_cmd_robot2(sr, p_vc_parm);
        break;
    case VP_CMD_RAP_REALTIME:
        vp_cmd_rap_realtime(sr, p_vc_parm);
        break;
    case VP_CMD_CARTOON:
        vp_cmd_cartoon(sr, p_vc_parm);
        break;
    case VP_CMD_PITCHSHIFT2:
        vp_cmd_pitchshift2(sr, p_vc_parm);
        break;
    case VP_CMD_RAP:
        vp_cmd_rap(sr, p_vc_parm);
        break;
    case VP_CMD_ECHO:
        vp_cmd_echo(sr, p_vc_parm);
        break;
    default:
        return false;
    }
    voice_pitch_debug(p_vc_parm);
    return true;
}
#endif
