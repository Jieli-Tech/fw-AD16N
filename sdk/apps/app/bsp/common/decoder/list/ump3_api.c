#include "cpu.h"
#include "config.h"
#include "app_modules.h"

#if defined(DECODER_UMP3_EN) && (DECODER_UMP3_EN)

#include "typedef.h"
#include "hwi.h"
#include "decoder_api.h"
/* #include "dev_manage.h" */
#include "vfs.h"
#include "circular_buf.h"
#include "errno-base.h"
#include "msg.h"
#include "decoder_msg_tab.h"
#include "app_config.h"
#include "audio_dac_api.h"
#include "ump3_api.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[ump3_api]"
#include "log.h"


#define UMP3_OBUF_SIZE (DAC_DECODER_BUF_SIZE * 2)
#define UMP3_KICK_SIZE (UMP3_OBUF_SIZE - (UMP3_DEC_OUTPUT_MAX_SIZE * 2))

dec_obj dec_ump3_hld;
cbuffer_t cbuf_ump3                 AT(.ump3_data);
u16 obuf_ump3[UMP3_OBUF_SIZE / 2]   AT(.ump3_data);
u32 ump3_decode_buff[0x1a18 / 4]    AT(.ump3_data);
#define ump3_CAL_BUF ((void *)&ump3_decode_buff[0])


/* static u32(*ump3_mp_input_cb)(void *, u32, void *, int, u8) = NULL; */
/* static int ump3_mp_input(void *priv, u32 addr, void *buf, int len, u8 type) */
/* { */
/*     if (ump3_mp_input_cb) { */
/*         return ump3_mp_input_cb(priv, addr, buf, len, type); */
/*     } else { */
/*         return mp_input(priv, addr, buf, len, type); */
/*     } */
/* } */
/* void ump3_mp_input_cb_sel(u32 ump3_mp_input_func(void *, u32, void *, int, u8)) */
/* { */
/*     local_irq_disable(); */
/*     ump3_mp_input_cb = ump3_mp_input_func; */
/*     local_irq_enable(); */
/* } */
struct if_decoder_io ump3_dec_io0 AT(.ump3_data);
/* const struct if_decoder_io ump3_dec_io0 = { */
/*     &dec_ump3_hld,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL */
/*     ump3_mp_input, */
/*     0, */
/*     mp_output, */
/*     decoder_get_flen, */
/*     0 */
/* }; */

u32 ump3_decode_api(void *strm, void **p_dec, void *p_dp_buf)
{
    dec_data_stream *p_strm = strm;
    u32 buff_len, i;
    /* void *name; */
    /* char name[VFS_FILE_NAME_LEN] = {0}; */
    decoder_ops_t *ops;
    log_info("ump3_decode_api\n");
    memset(&dec_ump3_hld, 0, sizeof(dec_obj));
    memset(&ump3_decode_buff, 0, sizeof(ump3_decode_buff));

    dec_ump3_hld.type = D_TYPE_UMP3;
    ops = get_ump3_ops();
    buff_len = ops->need_dcbuf_size();
    if (buff_len > sizeof(ump3_decode_buff)) {
        log_info("ump3 file dbuff : 0x%x 0x%lx\n", buff_len, sizeof(ump3_decode_buff));
        return E_UMP3_DBUF;
    }
    /******************************************/
    memcpy(&ump3_dec_io0, p_strm->io, sizeof(struct if_decoder_io));
    ump3_dec_io0.priv      = &dec_ump3_hld;

    cbuf_init(&cbuf_ump3, &obuf_ump3[0], sizeof(obuf_ump3));
    sound_stream_obj *psound_strm = p_strm->strm_source;
    dec_ump3_hld.p_file       = psound_strm;
    dec_ump3_hld.sound.p_obuf = &cbuf_ump3;
    dec_ump3_hld.sound.para   = UMP3_KICK_SIZE;
    dec_ump3_hld.p_dbuf       = ump3_CAL_BUF;
    dec_ump3_hld.dec_ops      = ops;
    dec_ump3_hld.event_tab    = (u8 *)&ump3_evt[0];
    dec_ump3_hld.p_dp_buf     = p_dp_buf;
    //dac reg
    // dec_ump3_hld.dac.obuf = &cbuf_ump3;
    // dec_ump3_hld.dac.vol = 255;
    // dec_ump3_hld.dac.index = reg_channel2dac(&dec_ump3_hld.dac);
    /******************************************/

    /* name = vfs_file_name(p_file); */
    log_info(" -ump3 open\n");
    ops->open(ump3_CAL_BUF, &ump3_dec_io0, p_dp_buf);         //传入io接口，说明如下
    log_info(" -ump3 open over\n");

    /* int file_len = vfs_file_name(p_file, (void *)g_file_sname, sizeof(g_file_sname)); */
    /* log_info("file name : %s\n", g_file_sname); */
    if ((B_DEC_IS_STRM & p_strm->strm_ctl)) {
        psound_strm->kick_thr = 128;
        dec_ump3_hld.sr	   	  = p_strm->sr;
    }

    if (!(B_DEC_NO_CHECK & p_strm->strm_ctl)) {
        if (ops->format_check(ump3_CAL_BUF)) {                  //格式检查
            log_info(" ump3 format err : %s\n", g_file_sname);
            return E_UMP3_FORMAT;
        }
    }

    /* regist_dac_channel(&dec_ump3_hld.sound, kick_decoder);//注册到DAC; */
    /* i = ops->get_dec_inf(ump3_CAL_BUF)->sr;                //获取采样率 */
    /* dec_ump3_hld.sr = i; */
    /* log_info("file sr : %d\n", i); */
    *p_dec = (void *)&dec_ump3_hld;
    return 0;
    /* dec_ump3_hld.enable = B_DEC_ENABLE | B_DEC_KICK; */
    /* debug_u32hex(dec_ump3_hld.enable); */
    /* kick_decoder(); */
    /* return 0; */
}

u32 ump3_decode_init(void *strm, void **p_dec, void *p_dp_buf, void *input_func, void *output_func)
{
    return ump3_decode_api(strm, p_dec, p_dp_buf);
}

extern const u8 ump3_buf_start[];
extern const u8 ump3_buf_end[];
u32 ump3_buff_api(dec_buf *p_dec_buf)
{
    p_dec_buf->start = (u32)&ump3_buf_start[0];
    p_dec_buf->end   = (u32)&ump3_buf_end[0];
    return 0;
}

u32 set_ump2_Headerstate_api(u32 sr, u32 br, int (*ump2_goon_cb)(void *))
{
    set_ump2_Headerstate(ump3_CAL_BUF, sr, br);
    GoOn_DEC_CallBack gocio = {0};
    gocio.priv = &dec_ump3_hld;
    gocio.callback = ump2_goon_cb;
    decoder_ops_t *ops = get_ump3_ops();
    ops->dec_confing(ump3_CAL_BUF, CMD_SET_GOON_CALLBACK, &gocio);
    return UMP3_DEC_INPUT_MAX_SIZE;
}

#endif
