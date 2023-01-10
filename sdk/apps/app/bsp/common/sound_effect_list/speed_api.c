
#include "app_modules.h"
#if HAS_SPEED_EN
#include "lib_speed_api.h"
#include "speed_api.h"
#include "typedef.h"
#include "decoder_api.h"
#include "config.h"
#include "sound_mge.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"


SPEED_PITCH_PARA_STRUCT sp_parm AT(.sp_data);

void *speed_api(void *obuf, u32 insample, void **ppsound)
{
    log_info("speed_api\n");

    sp_parm.insample = insample;             //输入音频采样率
    sp_parm.pitchrate = 100;           //变调比例，128:为不变调，<128:音调变高，>128:音调变低，声音更沉
    //speedout/speedin 的结果大于0.6小于1.8.变快变慢最好都不要超过2倍
    sp_parm.speedin = 80;
    sp_parm.speedout = 144;          //sp_parm.speedin：sp_parm.speedout为变速比例，例如speedin=1;speedout=2;则为变慢1倍
    sp_parm.quality = 4;            //变调运算的运算质量，跟速度成正比，配置范围3-8
    sp_parm.pitchflag = 1;     //如果pitchflag=1,就是输入输出速度只由speedin：speedout决定,调节音调的时候，速度比例保持不变。例如设置speedout：speedin=1:2 ，这时候，去调节pitchrate，都是只变调，速度一直是保持放慢1倍。
    //如果pitchflag=0，就是变速变调比例分开控制 ，这样实际变速的比例是 （speedout/speedin)*pitchrate了。

    return speed_phy(obuf, &sp_parm, ppsound);
}

void *link_speed_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 in_sr)
{
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    p_curr_sound->effect = speed_api(p_curr_sound->p_obuf, in_sr, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
    } else {
        log_info("echo init fail\n");
    }
    return p_curr_sound;
}



/***************************phy***************************************************************/
EFFECT_OBJ speed_obj AT(.sp_data);
u32  SP_BUFLEN[0xbb4 / 4] AT(.sp_data);          //处理32k音频变速变调需要的空间

static sound_in_obj speed_si AT(.sp_data);

const struct _RS_IO_CONTEXT_ sppitch_io = {
    &speed_obj.sound,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
    sound_output,
};

static int speed_run(void *hld, short *inbuf, int len)
{
    SPEEDPITCH_STUCT_API *ops;
    int res = 0;
    sound_in_obj *p_si = hld;
    ops = p_si->ops;
    res = ops->run(p_si->p_dbuf, inbuf, len);
    return res;
}

void *speed_phy(void *obuf, SPEED_PITCH_PARA_STRUCT *psp_parm, void **ppsound)
{
    u32 buff_len, i;
    SPEEDPITCH_STUCT_API *ops;
    log_info("speed_api\n");

    ops = get_sppitch_context();           //获取变采样函数接口
    buff_len = ops->need_buf(psp_parm->insample);                          //运算空间获取
    if (buff_len > sizeof(SP_BUFLEN)) {
        log_info("speed buff need : 0x%x\n", buff_len);
        return 0;
    }
    log_info("speed buff need : 0x%x, 0x%x ", buff_len, sizeof(SP_BUFLEN));
    /******************************************/
    //初始化：rs_buf：运算Buf; rs_parm：参数指针，传完可以释放的，里面不会记录这个指针的。sppitch_io:output接口，说明如下
    ops->open(&SP_BUFLEN[0], psp_parm, (void *)&sppitch_io);
    /*************************************************/
    memset(&speed_obj, 0, sizeof(speed_obj));
    speed_si.ops = ops;
    speed_si.p_dbuf = &SP_BUFLEN[0];
    /*************************************************/
    speed_obj.p_si = &speed_si;
    speed_obj.run = speed_run;
    speed_obj.sound.p_obuf = obuf;
    *ppsound = &speed_obj.sound;
    log_info("speed_succ\n");
    return &speed_obj;
}

#endif

