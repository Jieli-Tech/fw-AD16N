

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
#include "audio_dac_fade.h"

#if DECODER_F1A_EN
#include "f1a_api.h"
#endif
#if DECODER_UMP3_EN
#include "ump3_api.h"
#endif
#if DECODER_A_EN
#include "a_api.h"
#endif

#if HAS_SONG_SPEED_EN
#include "song_speed_api.h"
#endif
#if DECODER_MIDI_EN
#include "midi_api.h"
#endif
#if DECODER_MIDI_KEYBOARD_EN
#include "midi_ctrl.h"
#endif
#if DECODER_MP3_ST_EN
#include "mp3_standard_api.h"
#endif
#if DECODER_WAV_EN
#include "wav_api.h"
#endif
#if DECODER_OPUS_EN
#include "opus_api.h"
#endif
#if DECODER_IMA_EN
#include "ima_api.h"
#endif
#if DECODER_SPEEX_EN
#include "speex_api.h"
#endif
#if DECODER_SBC_EN
#include "sbc_api.h"
#endif
#if DECODER_JLA_LW_EN
#include "jla_lw_api.h"
#endif
/* #include "msg.h" */
/* #include "src_api.h" */
#include "decoder_mge.h"
#include "decoder_api.h"
#include "string.h"
#include "errno-base.h"
#include "decoder_msg_tab.h"
#if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN)
#include "audio_eq.h"
#endif
#if defined(PCM_SW_EQ_EN) && (PCM_SW_EQ_EN)
#include "pcm_eq_api.h"
#endif
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
    OPUS_LST
    IMA_LST
    SPEEX_LST
    SBC_LST
    JLA_LW_LST
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
    OPUS_API
    IMA_API
    SPEEX_API
    SBC_API
    JLA_LW_API
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
    OPUS_MUT_TAB
    IMA_MUT_TAB
    SPEEX_MUT_TAB
    SBC_MUT_TAB
    JLA_LW_MUT_TAB
    /* 0, */
};


const u32 decoder_parm_set[] = {
    F1A1_PARM_SET
    F1A2_PARM_SET
    UMP3_PARM_SET
    A_PARM_SET
    MIDI_PARM_SET
    MIDI_CTRL_PARM_SET
    WAV_PARM_SET
    MP3_ST_PARM_SET
    OPUS_PARM_SET
    IMA_PARM_SET
    SPEEX_PARM_SET
    SBC_PARM_SET
    JLA_LW_PARM_SET
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

void decoder_reset_file_stream(void *pfile)
{
    fs_seek(pfile, 0, SEEK_SET);
}

const struct if_decoder_io dec_io_for_file = {
    0,
    mp_input,
    0,
    mp_output,
    decoder_get_flen,
    0
};

dec_obj *decoder_list(dec_data_stream *p_strm, u32 dec_ctl, dp_buff *dbuff, u8 loop, u32 output_sr)
{
    u32(*fun)(void *, void **, void *);
    u32 res, dec_i, j;

#if HAS_MIO_EN
    u32 mio_res = -1;
    void *mio_pfile = NULL;
    if (0 == (p_strm->strm_ctl & B_DEC_IS_STRM)) {
        if (check_ext_api(g_file_sname, ".mio", 4)) {
            return NULL;
        }

        void *pfile = p_strm->strm_source;
        if (pfile) {
            mio_res = fs_openbyfile(pfile, &mio_pfile, "mio");
        }
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
        if (NULL != p_strm->strm_source) {
            if (NULL != p_strm->reset_stream) {
                p_strm->reset_stream(p_strm->strm_source);
            } else {
                log_error("file no seek");
            }
        }
        /* fs_seek(pfile, 0, SEEK_SET); */
        fun = (void *)decoder_tab[dec_i];
        p_dec = 0;
        log_info("file name:%s\n", g_file_sname);
        res = fun(p_strm, (void **)(&p_dec), check_dp(dbuff));
        /* res = fun(pfile, (void **)(&p_dec), 0); */
        if (0 == res) {
            if (p_strm->strm_ctl & B_DEC_NO_CHECK) {
                u32(*parm_set_fun)(u32, u32, int(*)(void *)) = (void *)decoder_parm_set[dec_i];
                if (0 != (u32)parm_set_fun) {
                    parm_set_fun(p_strm->sr, p_strm->br, (void *)p_strm->goon_callback);
                }
                p_dec->sr = p_strm->sr;
            } else {
                decoder_ops_t *ops = p_dec->dec_ops;
                dec_inf_t *p_dinfo = ops->get_dec_inf(p_dec->p_dbuf);
                log_info("long %d:%d", p_dinfo->total_time / 60, p_dinfo->total_time % 60);
#if DECODE_SR_IS_NEED_JUDIGMENT
                if (0 == p_dec->sr)
#endif
                {
                    log_info("need read sr");
                    if (0 != p_dinfo->sr) {
                        p_dec->sr = p_dinfo->sr;               //获取采样率
                    }
                }

                nch = p_dinfo->nch;
                log_info("sr:%d", p_dec->sr);
                log_info("br:%d", p_dinfo->br);
                log_info("nch:%d", nch);
            }
            break;
        }
    }

    if (0 == res) {
        // 设置解码参数
        p_curr_sound = &p_dec->sound;
        p_curr_sound->enable = 0;
        sound_out_obj *first_sound = p_curr_sound;
        void *cbuff_o = p_dec->sound.p_obuf;
        u8 output_ch = (first_sound->info & B_STEREO) ? 2 : 1;
#if defined(HAS_SONG_SPEED_EN) && HAS_SONG_SPEED_EN
        //变速变调
        if (dec_ctl & BIT_SPEED) {
            p_curr_sound = link_song_speed_sound(\
                                                 p_curr_sound,               \
                                                 cbuff_o,                    \
                                                 (void **) NULL,             \
                                                 p_dec->sr,                  \
                                                 output_ch);
        }
#endif

#if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN)
        //EQ
        if (dec_ctl & BIT_EQ) {
            p_curr_sound = link_eq_sound(\
                                         p_curr_sound,               \
                                         cbuff_o,                    \
                                         (void **) &p_dec->eq_effect, \
                                         p_dec->sr,                  \
                                         output_ch);
        }
#endif
        /* #if defined(PCM_SW_EQ_EN) && (PCM_SW_EQ_EN) */
        /* PCM_EQ 注意与解码资源复用 */
        /* if (dec_ctl & BIT_EQ) { */
        /* log_info(" BIT-EQ\n"); */
        /* p_curr_sound = link_pcm_eq_sound(\ */
        /* p_curr_sound,               \ */
        /* cbuff_o,                    \ */
        /* (void **) NULL,			 \ */
        /* p_dec->sr,                  \ */
        /* output_ch); */
        /* } */
        /* #endif */
        //硬件src
        /* u32 dac_sr = dac_sr_read(); */
        p_curr_sound->enable = 0;
#if defined(D_IS_FLASH_SYSTEM) && HAS_SRC_EN
        log_info("output_sr %d; input sr %d ", output_sr, p_dec->sr);
        if ((dec_ctl & BIT_SRC_FORCE) || (output_sr != p_dec->sr)) {
            log_info("need SRC, %d->%d %d", p_dec->sr, output_sr, output_ch);
            p_curr_sound = link_src_sound(p_curr_sound, cbuff_o, (void **) &p_dec->src_effect, p_dec->sr, output_sr, output_ch);
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
        /* #if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN) */
        /* #if defined(D_IS_FLASH_SYSTEM) && HAS_SRC_EN */
        /*         void *p_eq_obj = audio_eq_open_api(output_ch, dac_sr); */
        /* #else */
        /*         void *p_eq_obj = audio_eq_open_api(output_ch, p_dec->sr); */
        /* #endif */
        /*         if (NULL != p_eq_obj) { */
        /*             p_curr_sound->info |= B_EQ; */
        /*         } */
        /* #endif */
#if (1 == DAC_TRACK_NUMBER)
        /* DAC差分输出时双声道音源融合成单声道 */
        if (2 == output_ch) {
            p_curr_sound->info |= B_LR_COMB;
        }
#endif


        /* clear_dp(dbuff); */

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
        log_info("decode succ \n");
    } else {
        log_info("decode err : 0x%x\n", res);
#if HAS_MIO_EN
        if (0 == mio_res) {
            fs_file_close(&mio_pfile);
        }
#endif
    }
    /* dac_fade_in_api(); */
    //while(1)clear_wdt();
    return p_dec;
}
dec_obj *decoder_io(void *pfile, u32 dec_ctl, dp_buff *dbuff, u8 loop)
{
    u32(*fun)(void *, void **, void *);
    u32 res, dec_i, j;


    int file_len = fs_file_name(pfile, (void *)g_file_sname, sizeof(g_file_sname));

    dec_data_stream t_dstrm = {0};
    t_dstrm.strm_source = pfile;
    t_dstrm.io = (struct if_decoder_io *)&dec_io_for_file;
    t_dstrm.reset_stream = decoder_reset_file_stream;

    /* t_dstrm.strm_ctl = 0; */
    dec_obj *p_dec = decoder_list(&t_dstrm, dec_ctl, dbuff, loop, dac_sr_read());
    if (NULL != p_dec) {
        p_dec->p_kick = kick_decoder_api;
        regist_dac_channel(NULL, &p_dec->sound, p_dec->p_kick);
        p_dec->sound.enable |= B_DEC_ENABLE | B_DEC_KICK | B_DEC_FIRST;
        kick_decoder_api(NULL, &p_dec->sound);
        log_info("decode succ \n");
    } else {
        /* log_info("decode err : 0x%x\n", res); */
    }
    /* dac_fade_in_api(); */
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


AT(.audio_isr_text)
void kick_decoder(void)
{
    bit_set_swi(0);
}

AT(.audio_isr_text)
void kick_decoder_api(void *p_stream_in, void *psound)
{
    ((sound_out_obj *)psound)->enable |= B_DEC_KICK;
    kick_decoder();
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
    if ((0 != ret) && (0x60 != (ret & 0x60))) {
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
    case MAD_ERROR_FF_FR_FILE_END:
        obj->sound.enable |= B_DEC_ERR;
        break;
    case MAD_ERROR_STREAM_NODATA:
        log_info("run no data\n");
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

bool decoder_stop_phy(dec_obj *obj, IS_WAIT dec_stop_wait, void *p_dp, bool fade, bool(*unregist_func)(void *))
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
    /* dac_fade_out_api(200); */
    if (NO_WAIT != dec_stop_wait) {
        /* log_info("decode stop dec_stop_wait!\n"); */
        while (obj->sound.enable & B_DEC_OBUF_EN) {
            if (false == dac_cbuff_active(&obj->sound)) {
                break;
            }
        }
        /* log_info("decode stop dec_stop_wait ok!\n"); */
    } else {
        /* log_info("decode stop no dec_stop_wait!\n"); */
    }

    /* #if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN) */
    /*     audio_eq_close_api(); */
    /*     obj->sound.info &= ~B_EQ; */
    /* #endif */


#if HAS_MIO_EN
    d_mio_close(&obj->sound.mio);
#endif
    if (unregist_func) {
        unregist_func(&obj->sound);
    } else {
        log_error("dec_stop unregist_func null!\n");
    }
    if (NULL != obj->src_effect) {
#if HAS_SRC_EN
        src_reless(&obj->src_effect);
#endif
    }
#if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN)
    if (NULL != obj->eq_effect) {
        eq_reless(&obj->eq_effect);
    }
#endif
    return 1;
}

/*----------------------------------------------------------------------------*/
/*
  @brief    decoder_stop
  @param    *obj:解码器句柄
            dec_stop_wait:是否等待解码器消耗完剩余样点
            *p_dp:解码器断点buf
  @return   0:解码器已停止工作或句柄为NULL
            1:解码器停止成功
  @note     解码器停止成功后会保存断点信号到p_dp中
 */
/*----------------------------------------------------------------------------*/
bool decoder_stop(dec_obj *obj, IS_WAIT dec_stop_wait, void *p_dp)
{
    return decoder_stop_phy(obj, dec_stop_wait, p_dp, 1, unregist_dac_channel);
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
    struct vfs_attr fattr = {0};
    fs_get_attrs(obj->p_file, &fattr);
    log_info("flen:%d \n", fattr.fsize);
    flen = fattr.fsize;
    return flen;
}

