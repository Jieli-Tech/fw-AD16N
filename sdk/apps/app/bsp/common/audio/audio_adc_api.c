/***********************************Jieli tech************************************************
  File : adc_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2019-1-14
********************************************************************************************/
/* #include "adc_api.h" */
/* #include "adc.h" */
#include "circular_buf.h"
#include "string.h"
#include "uart.h"
#include "config.h"
#include "audio.h"
#include "audio_adc.h"
#include "audio_adc_api.h"
#if defined (AUDIO_ADC_TYPE) && (AUDIO_ADC_TYPE == AUIN_USE_ALINK)
#include "audio_link/audio_link_sync.h"
#include "audio_link/audio_link_api.h"
#endif
/* #include "audio_analog.h" */
#include "sound_mge.h"
#include "uac_sync.h"
#include "app_config.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[audio_adc_api]"
#include "log.h"



#define AUADC_CHANNEL_TOTAL 2

typedef struct _AUDIO_ADC_MANAGE {
    /*多路数据流句柄*/
    sound_out_obj *sound_pre[AUDIO_ADC_CHANNEL_TOTAL];
    sound_out_obj *sound_later[AUDIO_ADC_CHANNEL_TOTAL];
    /*多路数据流句柄所对应的触发函数*/
    void (*kick[AUDIO_ADC_CHANNEL_TOTAL])(void *);
    u8 ch;          /*通路标记*/
    u8 track;       /*声道数目*/
} AUDIO_ADC_MANAGE;

AUDIO_ADC_MANAGE audio_adc_mge ;//AT(.AUDIO_ADC_BUFFER);



u32 audio_adc_buf[AUDIO_ADC_PACKET_SIZE * AUDIO_ADC_SP_SIZE  / 4 ] ;//AT(.AUDIO_ADC_BUFFER);

_OTP_CONST_ AUDIO_ADC_CTRL_HDL c_audio_adc_hdl = {
    .buf      = (void *) &audio_adc_buf[0],
    .pns      = AUDIO_ADC_SP_PNS,
    .sp_total = sizeof(audio_adc_buf) / AUDIO_ADC_SP_SIZE,
    .sp_size  = AUDIO_ADC_SP_SIZE,
};

/*----------------------------------------------------------------------------*/
/**@brief   adc模块初始化
   @param   无
   @return  无
   @author  liujie
   @note    void adc_init_api(void)
*/
/*----------------------------------------------------------------------------*/
void audio_adc_mode_init(void)
{
    memset(&audio_adc_mge, 0, sizeof(audio_adc_mge));
    /* audio_adc_resource_init(audio_adc_buf, sizeof(audio_adc_buf), 25); */
}


u32 audio_adc_init_api(u32 sr, AUDIO_ADC_MODE mode, u32 throw_sp_num)
{
    if (sr == 0) {
        return E_ADC_SR;
    }
    u32 res = 0;
    if (AUDIO_ADC_MIC == mode) {
        audio_adc_mge.track = 1;
        auadc_open_mic();
    } else if (AUDIO_ADC_LINEIN == mode) {
        auadc_open_linein();
        audio_adc_mge.track = 1;
    } else if (AUDIO_LINK == mode) {
#if defined (AUDIO_ADC_TYPE) && (AUDIO_ADC_TYPE == AUIN_USE_ALINK)
        auadc_open_alink();
        audio_adc_mge.track = 2;
#endif
    } else {
        audio_adc_mge.track = 1;
    }

    res = auin_init((void *)&c_audio_adc_hdl, sr, AUDIO_ADC_CON_DEFAULT, throw_sp_num);
    return res;

}
void audio_adc_off_api(void)
{
    audio_adc_phy_off();
    audio_adc_analog_off();
    /* adc_analog_close(); */
}

/* extern void kick_encode_api(void); */
/* //弱定义函数，定义不可修改，由扩音模式重新实现 */
/* __attribute__((weak))void kick_loudsperaker(void *_sound)AT(.adc_oput_code) */
/* { */

/* } */
/* AT(.adc_oput_code) */
AT(.audio_isr_text)
void fill_audio_adc_fill(u8 *buf, u32 len)
{
    u32 i;
    for (i = 0; i < AUDIO_ADC_CHANNEL_TOTAL; i++) {
        if (0 == (audio_adc_mge.ch & BIT(i))) {
            continue;
        }
        if (0 == (audio_adc_mge.sound_pre[i]->enable & B_DEC_RUN_EN)) {
            continue;
        }
        u32 wlen;
        wlen = cbuf_write(audio_adc_mge.sound_pre[i]->p_obuf, buf, len);
        if (wlen != len) {
            log_char('3');
        } else {
            /* putchar('4'); */
        }
        if (NULL !=  audio_adc_mge.kick[i]) {
            audio_adc_mge.kick[i](audio_adc_mge.sound_pre[i]);
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   注册adc缓存函数
   @param   psound_pre	:注册给ADC的SOUND
			psound_later:ADC后级SOUND
			kick		:回调函数
   @return  TRUE:成功   false:失败
   @author  note    bool regist_audio_adc_channel(void *psound_pre, void *psound_later, void *kick)
*/
/*----------------------------------------------------------------------------*/
bool regist_audio_adc_channel(void *psound_pre, void *psound_later, void *kick)
{
    u8 i;
    for (i = 0; i < AUDIO_ADC_CHANNEL_TOTAL; i++) {
        if (audio_adc_mge.ch & BIT(i)) {
            if (audio_adc_mge.sound_pre[i] == psound_pre) {
                log_error("current sound have been regist the same %d\n", i);
            }
            continue;
        }
    }
    ///--------------------------
    for (i = 0; i < AUDIO_ADC_CHANNEL_TOTAL; i++) {
        if (audio_adc_mge.ch & BIT(i)) {
            continue;
        }
        if (1 == audio_adc_mge.track) {
            ((sound_out_obj *)psound_pre)->info &= ~B_STEREO;
        } else if (2 == audio_adc_mge.track) {
            ((sound_out_obj *)psound_pre)->info |= B_STEREO;
        }

        audio_adc_mge.sound_pre[i] = psound_pre;
        audio_adc_mge.sound_later[i] = psound_later;
        audio_adc_mge.kick[i] = kick;
        audio_adc_mge.ch |= BIT(i);
        return true;
    }
    return false;
}

/*----------------------------------------------------------------------------*/
/**@brief   注销adc缓存函数
   @param   psound	:注册给ADC的SOUND
   @return
   @author  note    bool unregist_audio_adc_channel(void *psound)
*/
/*----------------------------------------------------------------------------*/
bool unregist_audio_adc_channel(void *psound)
{
    u8 i;
    sound_out_obj *ps = psound;

    for (i = 0; i < AUDIO_ADC_CHANNEL_TOTAL; i++) {
        if (0 == (audio_adc_mge.ch & BIT(i))) {
            continue;
        }

        if (audio_adc_mge.sound_pre[i] == psound) {
            local_irq_disable();
            ps->enable &= ~B_DEC_OBUF_EN;
            audio_adc_mge.ch &= ~BIT(i);
            audio_adc_mge.sound_pre[i] = NULL;
            audio_adc_mge.sound_later[i] = NULL;
            audio_adc_mge.kick[i] = NULL;
            local_irq_enable();
            break;
        }
    }
    return true;
}




#if defined (AUDIO_ADC_TYPE) && (AUDIO_ADC_TYPE == AUIN_USE_ALINK)
/* sync同步>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

/* 统计ibuf和obuf的百分比 */
void uac_audio_adc_percent(u8 ch)
{
    u32 src_ibuf = 0;
    u32 src_ibuf_size = 0;
    u32 src_obuf = 0;
    u32 src_obuf_size = 0;
    u32 insample = 0;
    u32 outsample = 0;
    u32 percent = 0;
    if (0 == (audio_adc_mge.ch & BIT(ch))) {
        return;
    }
    if ((audio_adc_mge.sound_later[ch] == NULL) || (audio_adc_mge.sound_pre[ch] == NULL)) {
        return;
    }
    if (B_DEC_FIRST & audio_adc_mge.sound_later[ch]->enable) {	//是否首次开启
        return;
    }
    EFFECT_OBJ *p_effect = audio_adc_mge.sound_pre[ch]->effect;
    sound_in_obj *p_src_si = p_effect->p_si;
    if (NULL == p_src_si) {		//即没有src 或 其他音效
        return;
    }
    SRC_STUCT_API *p_ops = p_src_si->ops;

    /* 1.读ibuf 数据长度 */
    src_ibuf = cbuf_get_data_size(audio_adc_mge.sound_pre[ch]->p_obuf);
    src_ibuf_size = cbuf_get_space(audio_adc_mge.sound_pre[ch]->p_obuf);
    /* 2.前后级采样率	 */
    insample = p_ops->config(
                   p_src_si->p_dbuf,
                   SRC_CMD_GET_INSAMPLE,
                   (void *)NULL
               );
    insample /= 1000;

    outsample = p_ops->config(
                    p_src_si->p_dbuf,
                    SRC_CMD_GET_OUTSAMPLE,
                    (void *)NULL
                );
    outsample /= 1000;


    /* 3.读src_obuf 数据长度*/
    src_obuf = cbuf_get_data_size(audio_adc_mge.sound_later[ch]->p_obuf);
    src_obuf_size = cbuf_get_space(audio_adc_mge.sound_later[ch]->p_obuf);
    /* 4. 1和2的百分比*/
    percent = (((src_ibuf * outsample / insample) + src_obuf) * 100) / ((src_ibuf_size * outsample / insample) + src_obuf_size);
    audio_link_sync_accumulate(ch, percent);

}

void audio_adc_sync_once(void)
{
    u8 i;
    EFFECT_OBJ *p_effect = NULL;

    for (i = 0; i < AUDIO_LINK_CHANNEL_TOTAL; i++) {
        if ((audio_adc_mge.ch & BIT(i)) == 0) {
            continue;
        }
        if (NULL == audio_adc_mge.sound_later[i]) {
            continue;
        }
        if (B_DEC_FIRST & audio_adc_mge.sound_later[i]->enable) {	//是否首次开启
            continue;
        }
        p_effect = audio_adc_mge.sound_later[i]->effect;
        if (p_effect == NULL) {
            log_error("this %d channel no src\n", i);
            continue;
        }
        /* 取同步间隔内百分比平均值 */
        audio_link_sync_percent(i, p_effect);
        audio_link_sync_reset(i);

    }
}

/* sync同步<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#endif
