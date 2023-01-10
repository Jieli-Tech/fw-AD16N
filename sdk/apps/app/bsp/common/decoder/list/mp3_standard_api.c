
#pragma bss_seg(".mp3_st_data")
#pragma data_seg(".mp3_st_data")
#pragma const_seg(".mp3_st_api.text.const")
#pragma code_seg(".mp3_st_api.text")
#pragma str_literal_override(".mp3_st_api.text.const")

#include "app_modules.h"
#include "cpu.h"

#if DECODER_MP3_ST_EN

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
/* #include "eq.h" */
#include "audio_dac_api.h"
#include "app_config.h"
#include "tick_timer_driver.h"

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[mp3_st]"
#include "log.h"

dec_obj dec_mp3_st_hld AT(.mp3_st.data.bss);
#define MP3_FORMAT_CHECK_TIMEOUT_10MS     400   //MP3格式检查超时时间，单位10ms

#define MP3_ST_OBUF_SIZE DAC_DECODER_BUF_SIZE * 2
/* #define MP3_ST_KICK_SIZE (MP3_ST_OBUF_SIZE - 2*DAC_PACKET_SIZE) */
#define MP3_ST_OUTPUT_MAX_SIZE  (32 * 2 * 2)
#define MP3_ST_KICK_SIZE (MP3_ST_OBUF_SIZE - (MP3_ST_OUTPUT_MAX_SIZE * 2))

/*****-----不同时解码时可与其他解码复用的缓存----*****/
cbuffer_t cbuf_mp3_st SEC(.mp3_st_data);
u16 obuf_mp3_st[MP3_ST_OBUF_SIZE / 2] SEC(.mp3_st_data);
u32 mp3_st_decode_buff[0x3cfc / 4] SEC(.mp3_st_data);
static AUDIO_DECODE_PARA modevalue;

#define MP3_ST_CAL_BUF ((void *)&mp3_st_decode_buff[0])

#define CMD_SET_DECODE_CH    0x91
#define SET_EQ_SET_CMD       0x95

#define  EQ_FRE_NUM      10
typedef struct _PARM_DECODE_EQV_ {
    u8  eq_enable;    //eq使能
    s8  *gainval;
} PARM_DECODE_EQV;

//读数接口:priv为传入的dec_mp3_hld，返回值是读到多少个byte
static u32 format_check_start_jiffies;
int fix_mp3_format_check_input(void *priv, u32 addr, void *buf, int len, u8 type)
{
    dec_obj *obj = priv;
    if (DEC_FORMAT_CHECK_FIX & obj->function) {
        u32 tmp_jiffies = maskrom_get_jiffies();
        /* printf("time pass : %d ms\n", (tmp_jiffies - format_check_start_jiffies) * 10); */
        if (tmp_jiffies - format_check_start_jiffies >= MP3_FORMAT_CHECK_TIMEOUT_10MS) {
            wdt_clear();
            return 0;
        }
    }

    return mp_input(priv, addr, buf, len, type);
}


const struct if_decoder_io mp3_st_dec_io0 = {
    &dec_mp3_st_hld,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
    fix_mp3_format_check_input,
    0,
    mp_output,
    decoder_get_flen,
    0

};
u32 mp3_st_decode_api(void *p_file, void **p_dec, void *p_dp_buf)
{
    u32 buff_len, i;
    /* void *name; */
    /* char name[VFS_FILE_NAME_LEN] = {0}; */
    decoder_ops_t *ops;
    /* log_info("mp3_st_decode_api\n"); */
    memset(&dec_mp3_st_hld, 0, sizeof(dec_obj));
    memset(&mp3_st_decode_buff, 0, sizeof(mp3_st_decode_buff));


    dec_mp3_st_hld.type = D_TYPE_MP3_ST;
    dec_mp3_st_hld.function = DEC_FUNCTION_FF_FR;
    ops = get_mp3_ops();
    buff_len = ops->need_dcbuf_size();
    if (buff_len > sizeof(mp3_st_decode_buff)) {
        /* log_info("mp3 file dbuff : 0x%x 0x%x\n", buff_len, (u32)sizeof(mp3_st_decode_buff)); */
        return E_MP3_ST_DBUF;
    }
    /******************************************/
    cbuf_init(&cbuf_mp3_st, &obuf_mp3_st[0], sizeof(obuf_mp3_st));
    dec_mp3_st_hld.p_file       = p_file;
    dec_mp3_st_hld.sound.p_obuf = &cbuf_mp3_st;
    dec_mp3_st_hld.sound.info |= B_STEREO;
    dec_mp3_st_hld.sound.para = MP3_ST_KICK_SIZE;
    dec_mp3_st_hld.p_dbuf       = MP3_ST_CAL_BUF;
    dec_mp3_st_hld.dec_ops      = ops;
    dec_mp3_st_hld.event_tab    = (u8 *)&mp3_st_evt[0];
    dec_mp3_st_hld.p_dp_buf     = p_dp_buf;
    /* dec_mp3_st_hld.eq           = 0; */
    /* log_info("mp3 st obuf 0x%x\n", dec_mp3_st_hld.sound.p_obuf); */
    //dac reg
    // dec_mp3_st_hld.dac.obuf = &cbuf_mp3;
    // dec_mp3_st_hld.dac.vol = 255;
    // dec_mp3_st_hld.dac.index = reg_channel2dac(&dec_mp3_st_hld.dac);
    /******************************************/

    /* name = vfs_file_name(p_file); */
    /* int file_len = vfs_file_name(p_file, (void *)g_file_sname, sizeof(g_file_sname)); */
    /* log_info("file name : %s\n", g_file_sname); */
    /* log_info(" -mp3 open\n"); */
    ops->open(MP3_ST_CAL_BUF, &mp3_st_dec_io0, p_dp_buf);         //传入io接口，说明如下
    /* log_info(" -mp3 open over\n"); */
    dec_mp3_st_hld.function |= DEC_FORMAT_CHECK_FIX;
    format_check_start_jiffies = maskrom_get_jiffies();
    u32 err;
    err = ops->format_check(MP3_ST_CAL_BUF);
    if (0 != err) {                  //格式检查
        dec_mp3_st_hld.function &= ~DEC_FORMAT_CHECK_FIX;
        log_info(" mp3 format err : %s - 0x%x\n", g_file_sname, err);
        return E_MP3_ST_FORMAT;
    }
    dec_mp3_st_hld.function &= ~DEC_FORMAT_CHECK_FIX;

    modevalue.mode = 1;          //output是否判断返回值
    ops->dec_confing(MP3_ST_CAL_BUF, SET_DECODE_MODE, &modevalue);

    /* regist_dac_channel(&dec_mp3_st_hld.sound, kick_decoder);//注册到DAC; */
    /* i = ops->get_dec_inf(MP3_ST_CAL_BUF)->sr;                //获取采样率 */
    /* dec_mp3_st_hld.sr = i; */
    /* log_info("file sr : %d\n", i); */
    *p_dec = (void *)&dec_mp3_st_hld;
    return 0;
}

extern const u8 mp3_st_buf_start[];
extern const u8 mp3_st_buf_end[];
u32 mp3_st_buff_api(dec_buf *p_dec_buf)
{
    p_dec_buf->start = (u32)&mp3_st_buf_start[0];
    p_dec_buf->end   = (u32)&mp3_st_buf_end[0];
    return 0;
}

#endif
