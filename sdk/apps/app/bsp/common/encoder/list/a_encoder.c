
#include "app_modules.h"

#if ENCODER_A_EN

#include "encoder_mge.h"
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "hwi.h"
#include "a_encode_lib.h"

#include "circular_buf.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[a_enc]"
#include "log.h"




cbuffer_t cbuf_ima_o AT(.enc_a_data);
u8 obuf_ima_o[1024] AT(.enc_a_data) ;
u32 a_encode_buff[368 / 4] AT(.enc_a_data) ;

enc_obj enc_a_hdl;

const EN_FILE_IO a_enc_io = {
    &enc_a_hdl,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
    enc_input,
    enc_output,
};

u32 a_encode_api(void *p_file)
{
    u32 buff_len, i;
    ENC_OPS *ops;
    log_info("a_encode_api\n");
    ops = get_ima_code_ops();
    buff_len = ops->need_buf();
    if (buff_len > sizeof(a_encode_buff)) {
        log_info("a buff_len : %d", buff_len);
        log_info("a a_encode_buff len : %d", sizeof(a_encode_buff));
        return 0;
    }
    /******************************************/
    cbuf_init(&cbuf_ima_o, &obuf_ima_o[0], sizeof(obuf_ima_o));
    /* debug_puts("A\n"); */
    // debug_puts("B\n");
    enc_a_hdl.p_file = p_file;
    /* debug_u32hex((u32)p_file); */
    enc_a_hdl.p_ibuf = REC_ADC_CBUF; //adc_hdl.p_adc_cbuf;//&cbuf_ima_i;
    enc_a_hdl.p_obuf = &cbuf_ima_o;
    enc_a_hdl.p_dbuf = &a_encode_buff[0];
    enc_a_hdl.enc_ops = ops;
    enc_a_hdl.info.sr = read_audio_adc_sr();
    enc_a_hdl.info.br = 256;
    enc_a_hdl.info.nch = 1;


    /* debug_puts("D\n"); */
    /******************************************/
    ops->open((void *)&a_encode_buff[0], (void *)&a_enc_io);          //传入io接口，说明如下
    ops->set_info((void *)&a_encode_buff[0], &enc_a_hdl.info);
    ops->init((void *)&a_encode_buff[0]);
    return (u32)&enc_a_hdl;
}

#endif
