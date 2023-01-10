

#pragma bss_seg(".decoder_api.data.bss")
#pragma data_seg(".decoder_api.data")
#pragma const_seg(".decoder_api.text.const")
#pragma code_seg(".decoder_api.text")
#pragma str_literal_override(".decoder_api.text.const")


#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "hwi.h"
#include "vfs.h"
#include "circular_buf.h"
#include "audio.h"
#include "audio_dac.h"
#include "audio_dac_api.h"
#include "app_modules.h"

#if DECODER_F1A_EN
#include "f1a_api.h"
#endif
#if DECODER_UMP3_EN
#include "ump3_api.h"
#endif
#if DECODER_A_EN
#include "a_api.h"
#endif

#if HAS_SPEED_EN
#include "speed_api.h"
#endif
#if DECODER_MIDI_EN
#include "midi_api.h"
#endif
#if DECODER_MIDI_KEYBOARD_EN
#include "midi_ctrl.h"
#endif
/* #include "a_api.h" */
/* #include "mp3_standard_api.h" */
/* #include "wav_api.h" */
/* #include "msg.h" */
/* #include "src_api.h" */
#include "decoder_mge.h"
#include "decoder_api.h"
#include "string.h"
#include "errno-base.h"
#include "decoder_msg_tab.h"
#include "audio_eq_api.h"
/* #include "eq.h" */
#if HAS_MIO_EN
#include "mio_api.h"
#endif
#include "app_config.h"

#if HAS_SRC_EN
#include "src_api.h"
#endif





#define LOG_TAG_CONST       NORM
/* #define LOG_TAG_CONST       OFF */
#define LOG_TAG             "[dec_api]"
#include "log.h"


AT(.docoder_mge.text.cache.L1)
u32 dec_hld_tab[] = {
    F1A1_LST
    F1A2_LST
    UMP3_LST
    A_LST
    MIDI_LST
    MIDI_CTRL_LST
    WAV_LST
    MP3_ST_LST
    /* 0, */
};


const u32 decoder_tab[] = {
    F1A1_API
    F1A2_API
    UMP3_API
    A_API
    MIDI_API
    MIDI_CTRL_API
    WAV_API
    MP3_ST_API
    /* 0, */
};


const u32 decoder_mutual[] = {
    F1A1_MUT_TAB
    F1A2_MUT_TAB
    UMP3_MUT_TAB
    A_MUT_TAB
    MIDI_MUT_TAB
    MIDI_CTRL_MUT_TAB
    WAV_MUT_TAB
    MP3_ST_MUT_TAB
    /* 0, */
};
const u8 decoder_channel = sizeof(dec_hld_tab) / 4;

void decoder_init(void)
{
    u32 i;
    dec_obj *obj;
    u8 dc;
    dc = sizeof(dec_hld_tab) / 4;

    for (i = 0; i < dc; i++) {
        obj = (void *)dec_hld_tab[i];
        memset(obj, 0, sizeof(dec_obj));
    }
    /* decoder_channel_set(dc); */
    HWI_Install(IRQ_SOFT0_IDX, (u32)decoder_soft0_isr, IRQ_DECODER_IP) ;
}

extern bool const config_decoder_auto_mutex;
void decoder_mutex(u32 index)
{
    dec_buf cw;
    dec_buf cl;
    u32 max_loop = sizeof(decoder_mutual) / sizeof(decoder_mutual[0]);
    if (index >= max_loop) {
        return;
    }
    u32(*fun)(dec_buf * p) = (void *)decoder_mutual[index];
    fun(&cw);
    for (u32 i = 0; i < max_loop; i++) {
        if (i == index) {
            continue;
        }
        fun = (void *)decoder_mutual[i];
        fun(&cl);
        if ((cl.start >= cw.end) || (cw.start >= cl.end)) {
            continue;
        }
        /* log_info("start 0x%x; end  0x%x", cl.start, cl.end); */
        /* log_info("start 0x%x; end  0x%x", cw.start, cw.end); */
        /*  */
        /* log_info("decoder mutex : %d %d\n", index, i); */
        decoder_stop((void *)dec_hld_tab[i], NO_WAIT, 0);
    }
}
dec_obj *decoder_io(void *pfile, u32 dec_ctl, dp_buff *dbuff, u8 loop)
{
    u32(*fun)(void *, void **, void *);
    u32 res, dec_i, j;


    int file_len = fs_file_name(pfile, (void *)g_file_sname, sizeof(g_file_sname));

#if HAS_MIO_EN
    if (check_ext_api(g_file_sname, ".mio", 4)) {
        return NULL;
    }

    /* log_info("\n************\ndecoder_fun"); */


    u32 mio_res = -1;
    void *mio_pfile = NULL;
    if (pfile) {
        mio_res = fs_openbyfile(pfile, &mio_pfile, "mio");
    }
#endif

    dec_obj *p_dec = 0;
    sound_out_obj *p_curr_sound = 0;
    sound_out_obj *p_next_sound = 0;
    res = E_DECODER;
    u8 nch = 0;
    for (dec_i = 0; dec_i < (sizeof(decoder_tab) / 4); dec_i++) {
        if (0 == (dec_ctl & BIT(dec_i))) {
            continue;
        }
        //启动解码时，将其他与之互斥的解码停止
        if (0 != config_decoder_auto_mutex) {
            decoder_mutex(dec_i);
        }

        fs_seek(pfile, 0, SEEK_SET);
        fun = (void *)decoder_tab[dec_i];
        p_dec = 0;
        log_info("file name:%s\n", g_file_sname);
        res = fun(pfile, (void **)(&p_dec), check_dp(dbuff));
        /* res = fun(pfile, (void **)(&p_dec), 0); */
        if (0 == res) {
            regist_dac_channel(&p_dec->sound, kick_decoder);
            decoder_ops_t *ops = p_dec->dec_ops;
            dec_inf_t *p_dinfo = ops->get_dec_inf(p_dec->p_dbuf);
            log_info("long %d:%d", p_dinfo->total_time / 60, p_dinfo->total_time % 60);
#if DECODE_SR_IS_NEED_JUDIGMENT
            if (0 == p_dec->sr)
#endif
            {
                log_info("need read sr");
                p_dec->sr = p_dinfo->sr;               //获取采样率
            }

            nch = p_dinfo->nch;
            log_info("sr:%d", p_dec->sr);
            log_info("br:%d", p_dinfo->br);
            log_info("nch:%d", nch);

            break;
        }
    }

    if (0 == res) {
        // 设置解码参数
        p_curr_sound = &p_dec->sound;
        p_curr_sound->enable = 0;
        sound_out_obj *first_sound = p_curr_sound;
        void *cbuff_o = p_dec->sound.p_obuf;
#if HAS_SPEED_EN
        //变速变调
        if (dec_ctl & BIT_SPEED) {
            p_curr_sound = link_speed_sound(\
                                            p_curr_sound,               \
                                            cbuff_o,                    \
                                            (void **) NULL,             \
                                            p_dec->sr);
            /* p_curr_sound->effect = speed_api(cbuff_o, p_dec->sr, (void **) &p_next_sound); */
            /* if (NULL != p_curr_sound->effect) { */
            /* p_curr_sound->enable |= B_DEC_EFFECT; */
            /* p_curr_sound = p_next_sound; */
            /* p_curr_sound->p_obuf = cbuff_o; */
            /* p_next_sound = 0; */
            /* log_info("src init succ\n"); */
            /* } */
        }
#endif


        //硬件src
        u32 dac_sr = dac_sr_read();
        u8 output_ch = (first_sound->info & B_STEREO) ? 2 : 1;
        p_curr_sound->enable = 0;
#if defined(D_IS_FLASH_SYSTEM) && HAS_SRC_EN
        if (dac_sr != p_dec->sr) {
            log_info("need SRC, %d->%d %d", p_dec->sr, dac_sr, output_ch);
            p_curr_sound = link_src_sound(p_curr_sound, cbuff_o, (void **) &p_dec->src_effect, p_dec->sr, dac_sr, output_ch);
        } else {
            log_info("don't need SRC");
            void *src_tmp = src_hld_malloc();
            src_reless((void **)&src_tmp);
            //log_info("do't need src\n");
        }
#else
        void dac_sr_api(u32 sr);
        dac_sr_api(p_dec->sr);
#endif
#if HAS_MIO_EN
        if (0 == mio_res) {
            d_mio_open(&first_sound->mio, mio_pfile, (void *)mio_a_hook_init);
        }
        p_curr_sound->mio = p_dec->sound.mio;
#endif
#if AUDIO_EQ_ENABLE
#if defined(D_IS_FLASH_SYSTEM) && HAS_SRC_EN
        void *p_eq_obj = audio_eq_open_api(output_ch, dac_sr);
#else
        void *p_eq_obj = audio_eq_open_api(output_ch, p_dec->sr);
#endif
        if (NULL != p_eq_obj) {
            p_curr_sound->info |= B_EQ;
        }
#endif
#if (1 == DAC_TRACK_NUMBER)
        /* DAC差分输出时双声道音源融合成单声道 */
        if (2 == output_ch) {
            p_curr_sound->info |= B_LR_COMB;
        }
#endif


        /* clear_dp(dbuff); */

#if DECODER_LOOP_EN
        if (0 != loop) { // (dec_ctl & BIT_LOOP)
            p_dec->loop = loop;
            //log_info("get loop dp\n");
            if (true == get_dp(p_dec, dbuff)) {
                //log_info(" -loop save succ!\n");
                p_dec->p_dp_buf = check_dp(dbuff);
            } else {
                /* log_info(" -loop save fail!\n"); */
            }
        }
#endif
        p_dec->sound.enable |= B_DEC_ENABLE | B_DEC_KICK | B_DEC_FIRST;
        kick_decoder();
        log_info("decode succ \n");
    } else {
        log_info("decode err : 0x%x\n", res);
#if HAS_MIO_EN
        if (0 == mio_res) {
            fs_file_close(&mio_pfile);
        }
#endif
    }
    dac_fade_in_api();
    //while(1)clear_wdt();
    return p_dec;
}

int decoder_time(dec_obj *p_dec)
{
    int timeii = -1;
    if (0 != p_dec) {
        if (p_dec->sound.enable & B_DEC_ENABLE) {
            decoder_ops_t *ops = p_dec->dec_ops;
            timeii = ops->get_playtime(p_dec->p_dbuf);
            /* log_info("play time  %d : %d", timeii / 60, timeii % 60); */
        }
    }
    return timeii;
}



extern void if_kick_decoder(sound_out_obj *psound, void *pkick);

AT(.audio_isr_text)
int dac_kick_api(void *psound, void *pkick)
{
    if_kick_decoder(psound, pkick);
    return 0;
}

AT(.audio_isr_text)
void kick_decoder(void)
{
    bit_set_swi(0);
}


__attribute__((weak))
void midi_error_play_end_cb(dec_obj *obj, u32 ret)
{

}

void irq_decoder_ret(dec_obj *obj, u32 ret)
{
    if (MAD_ERROR_PLAY_END == ret) {
        midi_error_play_end_cb(obj, ret);
        return;
    }
    if (0 != ret) {
        log_info("decoder ret : 0x%x\n", ret);
        if (MAD_ERROR_F1X_START_ADDR == ret) {
            /* ret = MAD_ERROR_PLAY_END;  */
            post_event(obj->event_tab[MAD_ERROR_PLAY_END & 0x0f]);
        } else {
            post_event(obj->event_tab[ret & 0x0f]);
        }
    }
#if 1
    switch (ret) {
    case MAD_ERROR_NODATA:
    case MAD_ERROR_FILE_END:
    case MAD_ERROR_SYNC_LIMIT:
    case MAD_ERROR_F1X_START_ADDR:
        obj->sound.enable |= B_DEC_ERR;
        break;
    default:
        break;
    }
#else
    switch (ret) {
    case MAD_ERROR_NODATA:
        /* log_info("\n************file err nodata******************\n\n"); */
        obj->sound.enable |= B_DEC_ERR;
        break;
    case MAD_ERROR_FILE_END:
        /* log_info("\n************file err file end******************\n\n"); */
        obj->sound.enable |= B_DEC_ERR;
        /* log_info("file end\n"); */
        break;
    case MAD_ERROR_SYNC_LIMIT:
        /* log_info("\n************file err sync limit******************\n\n"); */
        obj->sound.enable |= B_DEC_ERR;
        /* log_info("file sync limit\n"); */
        break;
    case MAD_ERROR_F1X_START_ADDR:
        /* log_info("\n************file err fix start addr******************\n\n"); */
        obj->sound.enable |= B_DEC_ERR;
        /* log_info("file fix start addr\n"); */
        break;
    default:
        /* log_info("\n************file err 0x%x******************\n\n", ret); */
        break;
    }
#endif
}

void decoder_soft_hook(void)
{
#if HAS_MIO_EN
    d_mio_run();
#endif
}

bool decoder_stop_phy(dec_obj *obj, DEC_STOP_WAIT wait, void *p_dp, bool fade)
{
    if (NULL == obj) {
        return 0;
    }
    if (obj->sound.enable & B_DEC_RUN_EN) {
    } else {
        /* log_info("decoder has been closed"); */
        return 0;
    }
    log_info("decode stop\n");
    obj->sound.enable &= ~B_DEC_RUN_EN;
    get_dp(obj, p_dp);
    dac_fade_out_api(200);
    if (NO_WAIT != wait) {
        /* log_info("decode stop wait!\n"); */
        while (obj->sound.enable & B_DEC_OBUF_EN) {
            if (false == dac_cbuff_active(&obj->sound)) {
                break;
            }
        }
        /* log_info("decode stop wait ok!\n"); */
    } else {
        /* log_info("decode stop no wait!\n"); */
    }

#if AUDIO_EQ_ENABLE
    audio_eq_close_api();
    obj->sound.info &= ~B_EQ;
#endif


#if HAS_MIO_EN
    d_mio_close(&obj->sound.mio);
#endif
    unregist_dac_channel(&obj->sound);
    if (NULL != obj->src_effect) {
#if HAS_SRC_EN
        src_reless(&obj->src_effect);
#endif
    }
    return 1;
}

/*----------------------------------------------------------------------------*/
/*
  @brief    decoder_stop
  @param    *obj:解码器句柄
            wait:是否等待解码器消耗完剩余样点
            *p_dp:解码器断点buf
  @return   0:解码器已停止工作或句柄为NULL
            1:解码器停止成功
  @note     解码器停止成功后会保存断点信号到p_dp中
 */
/*----------------------------------------------------------------------------*/
bool decoder_stop(dec_obj *obj, DEC_STOP_WAIT wait, void *p_dp)
{
    return decoder_stop_phy(obj, wait, p_dp, 1);
}

/*----------------------------------------------------------------------------*/
/*
  @brief    if_decoder_is_run
  @param    *obj:解码器句柄
  @return   0:解码器已停止工作或句柄为NULL
            1:解码器正在工作
  @note     解码器在decoder_io()成功后开始工作，decoder_stop()成功后停止
 */
/*----------------------------------------------------------------------------*/
u32 if_decoder_is_run(dec_obj *obj)
{
    if (NULL == obj) {
        return 0;
    }
    return (obj->sound.enable & B_DEC_RUN_EN);
}

/*----------------------------------------------------------------------------*/
/*
  @brief    decoder_status
  @param    *obj:解码器句柄
  @return   0:解码器句柄为NULL
            obj->sound.enable:解码器状态
  @note
 */
/*----------------------------------------------------------------------------*/
u32 decoder_status(dec_obj *obj)
{
    if (NULL == obj) {
        return 0;
    }
    return obj->sound.enable;
}

/*----------------------------------------------------------------------------*/
/*
  @brief    decoder_pause
  @param    *obj:解码器句柄
  @return   0:解码器已停止工作
            1:暂停/播放成功
  @note
 */
/*----------------------------------------------------------------------------*/
bool decoder_pause(dec_obj *obj)
{
    if (!if_decoder_is_run(obj)) {
        return 0;
    }
    obj->sound.enable ^= B_DEC_PAUSE;
    if (0 == (obj->sound.enable & B_DEC_PAUSE)) {
        obj->sound.enable |= B_DEC_KICK;
        kick_decoder();
    }
    return 1;
}

/*----------------------------------------------------------------------------*/
/*
  @brief    decoder_ff
  @param    *obj:解码器句柄
            step:步进长度
  @return   0：解码器已停止工作
            1：快进成功
  @note     如果解码器是暂停状态，快进后会切换为播放状态
 */
/*----------------------------------------------------------------------------*/
bool decoder_ff(dec_obj *obj, u8 step)
{
    if ((!if_decoder_is_run(obj)) || (!(obj->function & DEC_FUNCTION_FF_FR))) {
        // 不支持快进快退
        return 0;
    }
    if (obj->sound.enable & B_DEC_PAUSE) {
        // 如果是暂停状态，改为播放
        decoder_pause(obj);
    }
    // 设置步长
    obj->ff_fr_step = step;
    return 1;
}

/*----------------------------------------------------------------------------*/
/*
  @brief    decoder_fr
  @param    *obj:解码器句柄
            step:步进长度
  @return   0：解码器已停止工作
            1：快退成功
  @note     如果解码器是暂停状态，快退后会切换为播放状态
 */
/*----------------------------------------------------------------------------*/
bool decoder_fr(dec_obj *obj, u8 step)
{
    if ((!if_decoder_is_run(obj)) || (!(obj->function & DEC_FUNCTION_FF_FR))) {
        // 不支持快进快退
        return 0;
    }
    if (obj->sound.enable & B_DEC_PAUSE) {
        // 如果是暂停状态，改为播放
        decoder_pause(obj);
    }
    // 设置步长
    obj->ff_fr_step = 0 - step;
    return 1;
}

u32 decoder_get_flen(void *priv)
{
    dec_obj *obj = priv;
    u32 flen = 0;
    fs_get_fsize(obj->p_file, &flen);
    log_info("flen:%d \n", flen);
    return flen;
}
