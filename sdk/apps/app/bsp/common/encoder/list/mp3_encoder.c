
#include "app_modules.h"

#if ENCODER_MP3_EN

#include "encoder_mge.h"
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "hwi.h"
#include "mp2_encode_api.h"

#include "circular_buf.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"




static cbuffer_t cbuf_emp3_o AT(.enc_mp3_data);
static u32 obuf_emp3_o[1024 / 4] AT(.enc_mp3_data) ;

/* static u32 mp3_encode_buff[1501] AT(.enc_mp3_data) ; */

static enc_obj enc_mp3_hdl;

#if ENCODER_MP3_STEREO
const int mp2_encode_channel = 2;
static u32 mp3_encode_buff[9584 / 4] AT(.enc_mp3_data) ;
#else
const int mp2_encode_channel = 1;
static u32 mp3_encode_buff[6736 / 4] AT(.enc_mp3_data) ;
#endif

static const EN_FILE_IO mp3_enc_io = {
    &enc_mp3_hdl,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
    enc_input,
    enc_output,
};

u32 mp3_encode_api(void *p_file)
{
    u32 buff_len, i;
    ENC_OPS *ops;
    log_info("mp3_encode_api\n");
    ops = get_mp2standard_ops();
    buff_len = ops->need_buf();
    if (buff_len > sizeof(mp3_encode_buff)) {
        log_info("buff_len : %d", buff_len);
        log_info("mp3_encode_buff len : %d", sizeof(mp3_encode_buff));
        return 0;
    }

    memset(&mp3_encode_buff[0], 0x00, sizeof(mp3_encode_buff));
    /******************************************/
    cbuf_init(&cbuf_emp3_o, &obuf_emp3_o[0], sizeof(obuf_emp3_o));
    log_info("A\n");
    // log_info("B\n");
    enc_mp3_hdl.p_file = p_file;
    enc_mp3_hdl.p_ibuf = REC_ADC_CBUF;//adc_hdl.p_adc_cbuf;//&cbuf_emp3_i;
    enc_mp3_hdl.p_obuf = &cbuf_emp3_o;
    enc_mp3_hdl.p_dbuf = &mp3_encode_buff[0];
    enc_mp3_hdl.enc_ops = ops;
    enc_mp3_hdl.info.sr = read_audio_adc_sr();
    /*br取值表：{8,16,24,32,40,48,56,64,80,96,112,128,144,160}*/
    enc_mp3_hdl.info.br = 80;
    enc_mp3_hdl.info.nch = mp2_encode_channel;

    log_info("encoder sr %d\n", enc_mp3_hdl.info.sr);
    /******************************************/
    ops->open((void *)&mp3_encode_buff[0], (void *)&mp3_enc_io);  //传入io接口，说明如下
    ops->set_info((void *)&mp3_encode_buff[0], &enc_mp3_hdl.info);
    ops->init((void *)&mp3_encode_buff[0]);
    /* enc_mp3_hdl.enable = B_ENC_ENABLE; */
    //debug_u32hex(enc_mp3_hdl.enable);
    return (u32)&enc_mp3_hdl;
}

#endif
