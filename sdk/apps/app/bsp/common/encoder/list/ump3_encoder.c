
#include "app_modules.h"

#if ENCODER_UMP3_EN

#include "encoder_mge.h"
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "hwi.h"

#include "circular_buf.h"
#include "ump2_encode_api.h"
#include "ump3_encoder.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[ump3_enc]"
#include "log.h"


/* ------------------------------------------------------------------------------- */
/* |UMP3编码 采样率（SR）/压缩比（CR）对应关系                                   | */
/* ------------------------------------------------------------------------------- */
/* |采样率(SR)  |   8000    12000   16000   22050   24000   32000   44100   48000| */
/* |压缩比(CR)  |                               3                                | */
/* |            |                               4                                | */
/* |            |                               5                                | */
/* |            |                               6                                | */
/* |            |                               7                                | */
/* |            |                               8                                | */
/* ------------------------------------------------------------------------------- */
/* |注：码率（BR）的范围是：采样率（SR）*16/压缩比（CR），压缩比的范围是3~8      | */
/* |例如SR = 24k，那么BR的范围是 (24*16/8) ~ (24*16/3)                           | */
/* ------------------------------------------------------------------------------- */

static cbuffer_t cbuf_eump3_o AT(.enc_ump3_data);
static u8 obuf_eump3_o[UMP3_ENC_OUT_CBUF_SIZE] AT(.enc_ump3_data) ;
static u32 ump3_encode_buff[UMP3_ENC_DBUF_SIZE / 4] AT(.enc_ump3_data) ;

static enc_obj enc_ump3_hdl;

/* static u32(*ump3_enc_output_cb)(void *, u8 *, u16) = NULL; */
/* static u32 ump3_enc_output(void *priv, u8 *data, u16 len) */
/* { */
/*     if (ump3_enc_output_cb) { */
/*         return ump3_enc_output_cb(priv, data, len); */
/*     } else { */
/*         return enc_output(priv, data, len); */
/*     } */
/* } */
/* void ump3_enc_output_cb_sel(u32 ump3_enc_output_func(void *, u8 *, u16)) */
/* { */
/*     local_irq_disable(); */
/*     ump3_enc_output_cb = ump3_enc_output_func; */
/*     local_irq_enable(); */
/* } */
/* static const EN_FILE_IO ump3_enc_io = { */
/*     &enc_ump3_hdl,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL */
/*     enc_input, */
/*     ump3_enc_output, */
/* }; */
static EN_FILE_IO ump3_enc_io AT(.enc_ump3_data);
int ump2_max_valid_br(int sr)
{
    int val = 17 * sr / 3200;
    return val;
}

u32 ump3_encode_api(void *p_file, void *input_func, void *output_func)
{
    u32 buff_len, i;
    ENC_OPS *ops;
    log_info("ump3_encode_api\n");
    ops = get_ump2_ops();
    buff_len = ops->need_buf();
    /* log_info("ump3 need buf:%d\n", buff_len); */
    if (buff_len > sizeof(ump3_encode_buff)) {
        return 0;
    }
    /******************************************/
    memset(&enc_ump3_hdl, 0, sizeof(enc_obj));
    memset(&obuf_eump3_o[0], 0x00, sizeof(obuf_eump3_o));
    memset(&ump3_encode_buff[0], 0x00, sizeof(ump3_encode_buff));
    cbuf_init(&cbuf_eump3_o, &obuf_eump3_o[0], sizeof(obuf_eump3_o));
    log_info("A\n");
    // log_info("B\n");
    ump3_enc_io.priv        = &enc_ump3_hdl;
    ump3_enc_io.input_data  = input_func;
    ump3_enc_io.output_data = output_func;
    enc_ump3_hdl.p_file = p_file;
    enc_ump3_hdl.p_ibuf = REC_ADC_CBUF;//adc_hdl.p_adc_cbuf;//&cbuf_emp3_i;
    enc_ump3_hdl.p_obuf = &cbuf_eump3_o;
    enc_ump3_hdl.p_dbuf = &ump3_encode_buff[0];
    enc_ump3_hdl.enc_ops = ops;
    enc_ump3_hdl.info.sr = read_audio_adc_sr();
    /*br的范围是：sr*16/压缩比，压缩比的范围是3~8
     * 例如sr = 24k，那么br的范围是 (24*16/8) ~ (24*16/3)*/
    enc_ump3_hdl.info.br = 80;
    /* 最大码率限制 */
    if (enc_ump3_hdl.info.br > ump2_max_valid_br(enc_ump3_hdl.info.sr)) {
        enc_ump3_hdl.info.br = ump2_max_valid_br(enc_ump3_hdl.info.sr);
    }
    enc_ump3_hdl.info.nch = 1;


    log_info("D\n");
    /******************************************/
    enc_ump3_hdl.enable |= B_ENC_INIT;
    ops->open((void *)&ump3_encode_buff[0], (void *)&ump3_enc_io);           //传入io接口，说明如下
    ops->set_info((void *)&ump3_encode_buff[0], &enc_ump3_hdl.info);
    ops->init((void *)&ump3_encode_buff[0]);
    enc_ump3_hdl.enable &= ~B_ENC_INIT;
    return (u32)&enc_ump3_hdl;
}
#endif
