#include "cpu.h"
#include "config.h"
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
#define LOG_TAG             "[normal]"
#include "log.h"


#define ump3_OBUF_SIZE (DAC_DECODER_BUF_SIZE * 2)
#define UMP3_OUTPUT_MAX_SIZE    (32 * 2)
#define UMP3_KICK_SIZE  (ump3_OBUF_SIZE - (UMP3_OUTPUT_MAX_SIZE * 2))
cbuffer_t cbuf_ump3 AT(.ump3_data);
u16 obuf_ump3[ump3_OBUF_SIZE / 2] AT(.ump3_data) ;
dec_obj dec_ump3_hld;
/* u32 ump3_decode_buff[1601] AT(.ump3_data) ; */
u32 ump3_decode_buff[0x1a18 / 4] AT(.ump3_data) ;
#define ump3_CAL_BUF ((void *)&ump3_decode_buff[0])


const struct if_decoder_io ump3_dec_io0 = {
    &dec_ump3_hld,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
    mp_input,
    0,
    mp_output,
    decoder_get_flen,
    0
};

u32 ump3_decode_api(void *p_file, void **p_dec, void *p_dp_buf)
{
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
    cbuf_init(&cbuf_ump3, &obuf_ump3[0], sizeof(obuf_ump3));
    dec_ump3_hld.p_file       = p_file;
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
    int file_len = vfs_file_name(p_file, (void *)g_file_sname, sizeof(g_file_sname));
    log_info("file name : %s\n", g_file_sname);
    log_info(" -ump3 open\n");
    ops->open(ump3_CAL_BUF, &ump3_dec_io0, p_dp_buf);         //传入io接口，说明如下
    log_info(" -ump3 open over\n");
    if (ops->format_check(ump3_CAL_BUF)) {                  //格式检查
        log_info(" ump3 format err : %s\n", g_file_sname);
        return E_UMP3_FORMAT;
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

extern const u8 ump3_buf_start[];
extern const u8 ump3_buf_end[];
u32 ump3_buff_api(dec_buf *p_dec_buf)
{
    p_dec_buf->start = (u32)&ump3_buf_start[0];
    p_dec_buf->end   = (u32)&ump3_buf_end[0];
    return 0;
}


