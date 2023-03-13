#include "echo_api.h"
#include "decoder_api.h"
#include "config.h"
#if ECHO_EN
#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

#define MAX_RATE 48000

#if (MAX_RATE == 16000)
#define echo_work_buf_len   3692
#elif (MAX_RATE == 24000)
#define echo_work_buf_len   5292
#elif (MAX_RATE == 48000)
#define echo_work_buf_len    10092
#endif

/*1代表下采，0代表不下采。下采的话，申请的Buf会减少，但是回声部分的高频也会丢失*/
const  int DOWN_S_FLAG = 0;


u32 echo_work_buf[(echo_work_buf_len + 3) / 4] AT(.echo_data);//echo混响buf长度，采样率改变，buf长度会变
/* u32 echo_work_buf[(echo_work_buf_len + 3) / 4] AT(.echo_data);//echo混响buf长度，采样率改变，buf长度会变 */

void *echo_api(void *obuf, u32 sr, void **ppsound)
{
    ECHO_PARM eparm;
    memset(&eparm, 0, sizeof(ECHO_PARM));
    log_info("echo_api\n");
    eparm.echo_parm_obj.decayval            = 60;  //decay(0~70)回声衰减比
    eparm.echo_parm_obj.delay               = 200; //回声延时0~300ms :范围0到max_ms，超过max_ms的话，会当成max_ms的
    eparm.echo_parm_obj.energy_vad_threshold = 512; //mute阈值
    eparm.echo_parm_obj.direct_sound_enable  = 1;   //如果运算结果需要叠加干声，则这个置成1，否则置0
    eparm.echo_fix_parm.max_ms               = 100; //最大延时100ms
    eparm.echo_fix_parm.sr                   = sr;  //采样率
    eparm.echo_fix_parm.wetgain              = 3000;//4096;//湿声增益
    eparm.echo_fix_parm.drygain              = 4096;//干声增益，如果direct_sound_enable是0，则这个参数无效，因为不叠加干声了
    eparm.ptr = echo_work_buf;
    eparm.ptr_len = sizeof(echo_work_buf);

    return echo_phy(obuf, &eparm, ppsound);
}

void update_echo_parm(void *priv, ECHO_PARM_SET *new_echo_parm)
{
    if ((NULL == priv) || (NULL == new_echo_parm)) {
        return;
    }
    log_info("set echo parm\n");

    EFFECT_OBJ *e_obj = (EFFECT_OBJ *)priv;
    sound_in_obj *p_si = e_obj->p_si;
    ECHO_FUNC_API *ops = p_si->ops;
    OS_ENTER_CRITICAL();
    ops->init(p_si->p_dbuf, new_echo_parm);
    OS_EXIT_CRITICAL();
}

void update_echo_fix_parm(void *priv, EF_REVERB_FIX_PARM *new_echo_fix_parm)
{
    if ((NULL == priv) || (NULL == new_echo_fix_parm)) {
        return;
    }
    log_info("update echo fix parm\n");

    EFFECT_OBJ *e_obj = (EFFECT_OBJ *)priv;
    sound_in_obj *p_si = e_obj->p_si;
    ECHO_FUNC_API *ops = p_si->ops;
    OS_ENTER_CRITICAL();
    ops->reset_wetdry(p_si->p_dbuf, new_echo_fix_parm->wetgain, new_echo_fix_parm->drygain);
    OS_EXIT_CRITICAL();
}

void *link_echo_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr)
{
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    p_curr_sound->effect = echo_api(p_curr_sound->p_obuf, sr, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;

        log_info("echo init succ\n");
    } else {
        log_error("echo init fail\n");
    }
    return p_curr_sound;
}
typedef struct _ECHO_HDL {
    EFFECT_OBJ obj;//必须在第一个
    sound_in_obj si;
    ECHO_IO_CONTEXT io;
    ECHO_API_STRUCT echo;
    u32 update;
} ECHO_HDL;
#if 0
u32 echo_work_buf[(3692 + 3) / 4] AT(.echo_data);
#endif
ECHO_HDL echo_hdl_save AT(.echo_data);

void echo_parm_update(ECHO_PARM_SET *parm)
{
    ECHO_HDL *echo_hdl = &echo_hdl_save;
    if (echo_hdl && parm) {
        memcpy(&echo_hdl->echo.echo_parm_obj, parm, sizeof(ECHO_PARM_SET));
        echo_hdl->update = 1;
    }
}


static int echo_run(void *hld, short *inbuf, int len)
{
    ECHO_FUNC_API *ops;
    int res = 0;
    sound_in_obj *p_si = hld;
    ops = (ECHO_FUNC_API *)p_si->ops;
    res = ops->run(p_si->p_dbuf, inbuf, len);
    /* ECHO_HDL *echo_hdl = &echo_hdl_save; */
    /* if (echo_hdl && echo_hdl->update) { */
    /* echo_hdl->update = 0; */
    /* ops->init(p_si->p_dbuf, &echo_hdl->echo.echo_parm_obj); */
    /* } */
    return res;
}


void echo_parm_debug(ECHO_HDL *echo_hdl)
{
    log_info("parm.echo_parm_obj.decayval %d\n", echo_hdl->echo.echo_parm_obj.decayval);
    log_info("parm.echo_parm_obj.delay %d\n", echo_hdl->echo.echo_parm_obj.delay);
    log_info("parm.echo_parm_obj.energy_vad_threshold %d\n", echo_hdl->echo.echo_parm_obj.energy_vad_threshold);
    log_info("parm.echo_parm_obj.direct_sound_enable %d\n", echo_hdl->echo.echo_parm_obj.direct_sound_enable);

    log_info("parm.echo_fix_parm.max_ms %d\n", echo_hdl->echo.echo_fix_parm.max_ms);
    log_info("parm.echo_fix_parm.sr %d\n", echo_hdl->echo.echo_fix_parm.sr);
    log_info("parm.echo_fix_parm.wetgain %d\n", echo_hdl->echo.echo_fix_parm.wetgain);
    log_info("parm.echo_fix_parm.drygain %d\n", echo_hdl->echo.echo_fix_parm.drygain);
}
void *echo_phy(void *obuf, ECHO_PARM *parm, void **ppsound)
{
    u32 buf_len, i;
    ECHO_FUNC_API *ops;
    EFFECT_OBJ *echo_obj;
    sound_in_obj *echo_si;
    if (!parm) {
        log_error("echo parm NULL\n");
        return NULL;
    }

    ops = (ECHO_FUNC_API *)get_echo_func_api(); //接口获取
    buf_len = ops->need_buf(NULL, (EF_REVERB_FIX_PARM *)&parm->echo_fix_parm);           //运算空间获取
    log_info("echo work_buf_len %d\n", buf_len);
    memset(&echo_hdl_save, 0x0, sizeof(ECHO_HDL));
    ECHO_HDL *echo_hdl = (ECHO_HDL *)&echo_hdl_save;
    if (echo_hdl && parm) {
        memcpy(&echo_hdl->echo.echo_parm_obj, &parm->echo_parm_obj, sizeof(ECHO_PARM_SET));
        memcpy(&echo_hdl->echo.echo_fix_parm, &parm->echo_fix_parm, sizeof(EF_REVERB_FIX_PARM));
        echo_parm_debug(echo_hdl);
    }
#if 0
    echo_hdl->echo.ptr = (unsigned int *)echo_work_buf;
    log_info("echo_hdl->echo.ptr %x\n", echo_hdl->echo.ptr);
    if (sizeof(echo_work_buf) < buf_len) {
        ¦   log_error("echo work buf less %d, need len %d", sizeof(echo_work_buf), buf_len);
        ¦   return NULL;
    }
#else
    echo_hdl->echo.ptr = (unsigned int *)parm->ptr;;
    if (parm->ptr_len < buf_len) {
        log_error("echo work buf less %d, need len %d", parm->ptr_len, buf_len);
        return NULL;
    }

#endif

    echo_obj = &echo_hdl->obj;
    echo_hdl->io.priv = &echo_obj->sound;
    echo_hdl->io.output = sound_output;


    /*************************************************/
    echo_si =  &echo_hdl->si;
    echo_si->ops = ops;
    echo_si->p_dbuf = echo_hdl->echo.ptr;
    /*************************************************/
    echo_obj->p_si = echo_si;
    echo_obj->run = echo_run;
    echo_obj->sound.p_obuf = obuf;
    *ppsound = &echo_obj->sound;

    ops->open(echo_hdl->echo.ptr, &echo_hdl->echo.echo_parm_obj, &echo_hdl->echo.echo_fix_parm, &echo_hdl->io);

    return echo_obj;
}
#endif
