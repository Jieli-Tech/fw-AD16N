

#include "a_api.h"
#include "cpu.h"
#include "app_modules.h"
#if DECODER_A_EN

#include "config.h"
#include "typedef.h"
#include "hwi.h"
#include "decoder_api.h"
/* #include "dev_manage.h" */
#include "vfs.h"
#include "circular_buf.h"
/* #include "dac.h" */
#include "errno-base.h"
#include "msg.h"
#include "decoder_msg_tab.h"
#include "app_config.h"
#include "audio_dac_api.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[a_api]"
#include "log.h"


#define A_OBUF_SIZE DAC_DECODER_BUF_SIZE
#define A_OUTPUT_MAX_SIZE (32 * 2)
#define A_KICK_SIZE (A_OBUF_SIZE - (A_OUTPUT_MAX_SIZE * 2))

cbuffer_t cbuf_a AT(.a_data);
u16 obuf_a[A_OBUF_SIZE / 2] AT(.a_data) ;
dec_obj dec_a_hld ;
/* u32 a_decode_buff[228 / 4] AT(.a_data) ; */
u32 a_decode_buff[0xf4 / 4] AT(.a_data);
#define A_CAL_BUF ((void *)&a_decode_buff[0])

const char a_ext[] = {".a"};
const char b_ext[] = {".b"};
const char e_ext[] = {".e"};

const struct if_decoder_io a_dec_io0 = {
    &dec_a_hld,  //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
    mp_input,
    0,
    mp_output,
    decoder_get_flen,
    0
};

static u32 a_dp_buff[24 / 4] AT(.a_data) ;

u32 get_a_dp_buff_size(void)
{
    return sizeof(a_dp_buff);
}

u32 a_decode_api(void *p_file, void **p_dec, void *p_dp_buf)
{
    u32 buff_len, i;
    /* void *name; */
    /* char name[VFS_FILE_NAME_LEN] = {0}; */
    decoder_ops_t *ops;
    log_info("a_decode_api");
    memset(&dec_a_hld, 0, sizeof(dec_obj));
    dec_a_hld.type = D_TYPE_A;
    ops = get_ima_ops();
    buff_len = ops->need_dcbuf_size();
    if (buff_len > sizeof(a_decode_buff)) {
        log_info("afile dbuff : 0x%x 0x%x\n", buff_len, (int)sizeof(a_decode_buff));
        return E_A_DBUF;
    }
    /******************************************/
    cbuf_init(&cbuf_a, &obuf_a[0], sizeof(obuf_a));
    /* debug_puts("A\n"); */
    dec_a_hld.p_file       = p_file;
    dec_a_hld.sound.p_obuf = &cbuf_a;
    dec_a_hld.sound.para = A_KICK_SIZE;
    dec_a_hld.p_dbuf       = A_CAL_BUF;
    dec_a_hld.dec_ops      = ops;
    dec_a_hld.event_tab    = (u8 *)&a_evt[0];
    dec_a_hld.p_dp_buf     = &a_dp_buff[0];// p_dp_buf;
    /* debug_puts("B\n"); */
    //
    /* reg_dac_channel_api(&dec_a_hld.dac, &dec_a_hld, &cbuf_a, 255); */
    /******************************************/
    ops->open(A_CAL_BUF, &a_dec_io0, NULL);         //传入io接口，说明如下

    /* name = vfs_file_name(p_file); */
    int file_len = vfs_file_name(p_file, (void *)g_file_sname, sizeof(g_file_sname));
    log_info("file g_file_sname : %s", g_file_sname);
    if (check_ext_api(g_file_sname, a_ext, 2)) {
        log_info("file is a a_file");
        //ctype = 'a';
        i = 8000;
    } else if (check_ext_api(g_file_sname, b_ext, 2)) {
        log_info("file is a b_file");
        //ctype = 'b';
        i = 16000;
    } else { //if (check_ext_api(name,e_ext,2))
        log_info("file is a e_file");
        //ctype = 'c';
        if (ops->format_check(A_CAL_BUF)) {                  //格式检查
            log_error("e_file format check err\n");
            return E_A_FORMAT;
        }
        i = ops->get_dec_inf(A_CAL_BUF)->sr;                //获取采样率
    }
    memcpy(&a_dp_buff[0], (void *)ops->get_bp_inf(A_CAL_BUF), sizeof(a_dp_buff));
    /* regist_dac_channel(&dec_a_hld.sound, kick_decoder); //注册到DAC; */
    /* [>*******************************************************<] */
    /* log_info("file sr : %d\n", i); */
    dec_a_hld.sr = i;
    *p_dec = (void *)&dec_a_hld;
    return 0;
}

extern const u8 a_buf_start[];
extern const u8 a_buf_end[];
u32 a_buff_api(dec_buf *p_dec_buf)
{
    p_dec_buf->start = (u32)&a_buf_start[0];
    p_dec_buf->end   = (u32)&a_buf_end[0];
    return 0;
}





#endif
