#include "app_modules.h"

#if HAS_SONG_SPEED_EN
#include "ps_for69_api.h"
#include "song_speed_api.h"
#include "sound_mge.h"
#include "typedef.h"

#define LOG_TAG_CONST        NORM
#define LOG_TAG              "[song_speed]"
#include "log.h"

#define SONG_SPEED_FAST      1
#if SONG_SPEED_FAST
const int PSFAST_FLAG = 1;   //0:普通模式       1:快速模式(需要增加buf大小)
const int PSADD_BUF = 1;     //0:不增加buf大小  1:增加256个byte，用于快速模式
u8 buflen[0x147c] AT(.song_sp_data);
#else
const int PSFAST_FLAG = 0;
const int PSADD_BUF = 0;
u8 buflen[0x137c] AT(.song_sp_data);
#endif

PS69_CONTEXT_CONF ps_parm AT(.song_sp_data);

void *song_speed_api(void *obuf, u16 ch, u32 insample, void **ppsound)
{
    ps_parm.chn    = ch;         //输入音源的声道信息
    ps_parm.sr     = insample;   //输入音源的采样率信息
    ps_parm.speedV = 120;        //>80是变快，<80 是变慢 ，建议范围：40到130
    ps_parm.pitchV = 45000;      //>32768是音调变高，<32768音调变低， 建议范围20000到50000
    return song_speed_phy(obuf, &ps_parm, ppsound);
}

void *link_song_speed_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 in_sr, u8 ch)
{
    log_info("speed init \n");
    log_info("in_sr %d, ch %d\n", in_sr, ch);
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    p_curr_sound->effect = song_speed_api(p_curr_sound->p_obuf, ch, in_sr, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
    } else {
        log_info("speed init fail\n");
    }
    return p_curr_sound;
}

void update_song_speed_parm(void *priv, PS69_CONTEXT_CONF *new_ps_parm)
{
    if ((NULL == priv) || (NULL == new_ps_parm)) {
        return;
    }

    log_info("Update Song Speed Parm\n");
    EFFECT_OBJ *e_obj = (EFFECT_OBJ *)priv;
    sound_in_obj *p_si = e_obj->p_si;
    PS69_API_CONTEXT *ops = p_si->ops;
    OS_ENTER_CRITICAL();
    ops->dconfig(p_si->p_dbuf, new_ps_parm);
    OS_EXIT_CRITICAL();
}

EFFECT_OBJ song_speed_obj AT(.song_sp_data);
static sound_in_obj speed_si;

const struct _PS69_CONTEXT_IO_ song_sppitch_io = {
    (u8 *) &song_speed_obj.sound,
    (void *)sound_output,
};

static int song_speed_run(void *hld, short *inbuf, int len)
{
    PS69_API_CONTEXT *ops;
    int res = 0;
    sound_in_obj *p_si = hld;
    ops = p_si->ops;
    res = ops->run(p_si->p_dbuf, inbuf, len);
    return res;
}

void *song_speed_phy(void *obuf, PS69_CONTEXT_CONF *psp_parm, void **ppsound)
{
    u32 buff_len;
    PS69_API_CONTEXT *ops;
    log_info("speed api\n");

    ops = get_ps_cal_api();       //获取接口
    buff_len = ops->need_size();  //获取运算空间
    if (buff_len > sizeof(buflen)) {
        log_error("buff need : 0x%x\n", buff_len);
        return 0;
    }
    log_info("buff need : 0x%x  0x%x\n", buff_len, sizeof(buflen));
    ops->open(&buflen[0], (void *)&song_sppitch_io);
    ops->dconfig(&buflen[0], psp_parm);

    memset(&song_speed_obj, 0, sizeof(song_speed_obj));
    speed_si.ops    = ops;
    speed_si.p_dbuf = &buflen[0];

    song_speed_obj.p_si = &speed_si;
    song_speed_obj.run = song_speed_run;
    song_speed_obj.sound.p_obuf = obuf;
    *ppsound = &song_speed_obj.sound;
    log_info("speed succ\n");
    return &song_speed_obj;
}
#endif

