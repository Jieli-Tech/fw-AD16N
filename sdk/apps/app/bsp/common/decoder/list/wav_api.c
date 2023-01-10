
#pragma bss_seg(".wav_data")
#pragma data_seg(".wav_data")
#pragma const_seg(".wav_api.text.const")
#pragma code_seg(".wav_api.text")
#pragma str_literal_override(".wav_api.text.const")

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
#include "audio_dac_api.h"
#include "decoder_msg_tab.h"
#include "app_config.h"

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[wav]"
#include "log.h"

#if DECODER_WAV_EN

dec_obj dec_wav_hld AT(.wav.data.bss);


#define WAV_OBUF_SIZE 		DAC_DECODER_BUF_SIZE*4
/* #define WAV_KICK_SIZE		(WAV_OBUF_SIZE - 2*DAC_PACKET_SIZE) */
#define WAV_OUTPUT_MAX_SIZE  (160 * 2 * 2)
#define WAV_KICK_SIZE		(WAV_OBUF_SIZE - (WAV_OUTPUT_MAX_SIZE * 2))


cbuffer_t cbuf_wav;
u16 obuf_wav[WAV_OBUF_SIZE / 2]AT(.wav_data);
u32 wav_decode_buff[0x5a4 / 4]AT(.wav_data);
static AUDIO_DECODE_PARA modevalue AT(.wav_data);



#define WAV_CAL_BUF ((void *)&wav_decode_buff[0])



const struct if_decoder_io wav_dec_io0 = {
    &dec_wav_hld,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
    mp_input,
    0,
    mp_output,
    decoder_get_flen,
    0
};

u32 wav_decode_api(void *p_file, void **p_dec, void *p_dp_buf)
{
    u32 buff_len, i;
    /* void *name; */
    /* char name[VFS_FILE_NAME_LEN] = {0}; */
    decoder_ops_t *ops;
    /* log_info("wav_decode_api\n"); */
    memset(&dec_wav_hld, 0, sizeof(dec_obj));
    memset(&wav_decode_buff, 0, sizeof(wav_decode_buff));


    dec_wav_hld.type = D_TYPE_WAV;
    dec_wav_hld.function = DEC_FUNCTION_FF_FR;
    ops = get_wav_ops();
    buff_len = ops->need_dcbuf_size();
    if (buff_len > sizeof(wav_decode_buff)) {
        log_info("wav file dbuff : 0x%x 0x%x\n", buff_len, (u32)sizeof(wav_decode_buff));
        return E_WAV_DBUF;
    }
    /* log_info("wav file dbuff : 0x%x 0x%x\n", buff_len, (u32)sizeof(wav_decode_buff)); */
    /******************************************/
    cbuf_init(&cbuf_wav, &obuf_wav[0], sizeof(obuf_wav));
    dec_wav_hld.p_file       = p_file;
    dec_wav_hld.sound.p_obuf = &cbuf_wav;
    dec_wav_hld.sound.para = WAV_KICK_SIZE;
    dec_wav_hld.sound.info |= B_STEREO;
    dec_wav_hld.p_dbuf       = WAV_CAL_BUF;
    dec_wav_hld.dec_ops      = ops;
    dec_wav_hld.event_tab    = (u8 *)&wav_evt[0];
    dec_wav_hld.p_dp_buf     = p_dp_buf;
    //dac reg
    // dec_wav_hld.dac.obuf = &cbuf_wav;
    // dec_wav_hld.dac.vol = 255;
    // dec_wav_hld.dac.index = reg_channel2dac(&dec_wav_hld.dac);
    /******************************************/

    /* name = vfs_file_name(p_file); */
    /* int file_len = vfs_file_name(p_file, (void *)g_file_sname, sizeof(g_file_sname)); */
    /* log_info("file name : %s\n", g_file_sname); */
    /* log_info(" -wav open\n"); */
    ops->open(WAV_CAL_BUF, &wav_dec_io0, p_dp_buf);         //传入io接口，说明如下
    /* log_info(" -wav open over\n"); */
    if (ops->format_check(WAV_CAL_BUF)) {                  //格式检查
        log_info(" wav format err : %s\n", g_file_sname);
        return E_WAV_FORMAT;
    }

    modevalue.mode = 1;          //output是否判断返回值
    ops->dec_confing(WAV_CAL_BUF, SET_DECODE_MODE, &modevalue);
    /* parm_nchv.ch_value = FAST_LR_OUT; */
    /* ops->dec_confing(WAV_CAL_BUF, CMD_SET_DECODE_CH, &parm_nchv);  //配置解码输出通道 */

    /* regist_dac_channel(&dec_wav_hld.sound, kick_decoder);//注册到DAC; */
    /* i = ops->get_dec_inf(WAV_CAL_BUF)->sr;                //获取采样率 */
    /* dec_wav_hld.sr = i; */
    /* log_info("file sr : %d\n", i); */
    *p_dec = (void *)&dec_wav_hld;
    return 0;
}

extern const u8 wav_buf_start[];
extern const u8 wav_buf_end[];
u32 wav_buff_api(dec_buf *p_dec_buf)
{
    p_dec_buf->start = (u32)&wav_buf_start[0];
    p_dec_buf->end   = (u32)&wav_buf_end[0];
    return 0;
}


#endif
