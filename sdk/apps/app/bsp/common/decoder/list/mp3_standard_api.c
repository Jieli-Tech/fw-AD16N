
#pragma bss_seg(".mp3_st_data")
#pragma data_seg(".mp3_st_data")
#pragma const_seg(".mp3_st_api.text.const")
#pragma code_seg(".mp3_st_api.text")
#pragma str_literal_override(".mp3_st_api.text.const")

#include "app_modules.h"
#include "cpu.h"

#if defined(DECODER_MP3_ST_EN) && (DECODER_MP3_ST_EN)

#include "lib_mp3_st.h"
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

#define MP3_ST_OBUF_SIZE (DAC_DECODER_BUF_SIZE * 2)
/* #define MP3_ST_KICK_SIZE (MP3_ST_OBUF_SIZE - 2*DAC_PACKET_SIZE) */
#define MP3_ST_KICK_SIZE (MP3_ST_OBUF_SIZE - (MP3_ST_OUTPUT_MAX_SIZE * 2))

/*****-----不同时解码时可与其他解码复用的缓存----*****/
cbuffer_t cbuf_mp3_st SEC(.mp3_st_data);
u16 obuf_mp3_st[MP3_ST_OBUF_SIZE / 2] SEC(.mp3_st_data);
u32 mp3_st_decode_buff[MP3_ST_DBUF_SIZE / 4] SEC(.mp3_st_data);
static AUDIO_DECODE_PARA modevalue SEC(.mp3_st_data);

#define MP3_ST_CAL_BUF ((void *)&mp3_st_decode_buff[0])

#define CMD_SET_DECODE_CH    0x91

typedef struct _PARM_DECODE_CHV_ {
    u32  ch_value;
} PARM_DECODE_CHV;
enum {
    FAST_L_OUT = 0x01,                  //输出左声道
    FAST_R_OUT = 0x02,                  //输出右声道
    FAST_LR_OUT = 0x04                 //输出左右声道混合
};
static PARM_DECODE_CHV parm_nchv SEC(.mp3_st_data);

const int MP3_REPEAT_USE_BP_CONTINUE = 1;


typedef enum {
    MP3_EQ_ZERO = 0X00,
    MP3_EQ_POP,
    MP3_EQ_ROCK,
    MP3_EQ_JAZZ,
    MP3_EQ_CLASSIC,
    MP3_EQ_MODEMAX
} DEC_EQ_MODE;

#define SET_EQ_SET_CMD       0x95
#define  EQ_FRE_NUM      10

typedef struct _PARM_DECODE_EQV_ {
    u8  eq_enable;    //eq使能
    s8  *gainval;
} PARM_DECODE_EQV;

PARM_DECODE_EQV eq_parm AT(.mp3_st_data);

const s8 eqtab[MP3_EQ_MODEMAX][EQ_FRE_NUM] = {
    /*fre:0,96,185,356,684,1316,2530,4866,9359,18000*/
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},     //5zero_eq
    { 8, 7, -5, -3, -4, -3, -2, 0, 2, 3}, //2pop_eq
    {-2, 1, 3, 5, -2, -2, 2, 4, 5, 5},  //1rock_eq
    {-4, -4, -3, 2, 4, 4, -4, 3, 4, 5},    //3jazz_eq
    {-3, 3, 3, 1, -2, -4, -4, -4, -3, -3}     //0classic_eq
};

u32 mp3_st_eq_set(u8 eq_mode)
{
    if (eq_mode >= MP3_EQ_MODEMAX) {
        return -1;
    }
    eq_parm.gainval = (s8 *)&eqtab[eq_mode][0];                      //配置对应的eq表
    //中途切模式的话，也是调用这个函数，解码库内部是copy走eq_parm的，调用结束可以释放该参数
    if ((0 != dec_mp3_st_hld.dec_ops) && (0 != dec_mp3_st_hld.p_dbuf)) {
        decoder_ops_t *ops;
        ops = dec_mp3_st_hld.dec_ops;
        ops->dec_confing(dec_mp3_st_hld.p_dbuf, SET_EQ_SET_CMD, &eq_parm);
        log_info("mp3_st_eq_set %d \n", eq_mode);
        return eq_mode;
    } else {
        log_info("mp3_st_eq_set fail\n");
        return -1;
    }
}


#define MP3_FORMAT_CHECK_TIMEOUT_10MS     400   //MP3格式检查超时时间，单位10ms
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


//读数接口:priv为传入的dec_mp3_hld，返回值是读到多少个byte
struct if_decoder_io mp3_st_dec_io0 AT(.mp3_st_data);
/* const struct if_decoder_io mp3_st_dec_io0 = { */
/*     &dec_mp3_st_hld,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL */
/*     MP3_ST_INPUT, */
/*     0, */
/*     mp_output, */
/*     decoder_get_flen, */
/*     0 */
/*  */
/* }; */
u32 mp3_st_decode_api(void *strm, void **p_dec, void *p_dp_buf)
{
    dec_data_stream *p_strm = strm;
    u32 buff_len, i;
    u32 err;
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
    memcpy(&mp3_st_dec_io0, p_strm->io, sizeof(struct if_decoder_io));
    mp3_st_dec_io0.priv      = &dec_mp3_st_hld;
    mp3_st_dec_io0.input     = MP3_ST_INPUT;

    cbuf_init(&cbuf_mp3_st, &obuf_mp3_st[0], sizeof(obuf_mp3_st));
    sound_stream_obj *psound_strm = p_strm->strm_source;
    dec_mp3_st_hld.p_file       = (void *)psound_strm;
    dec_mp3_st_hld.sound.p_obuf = &cbuf_mp3_st;
    dec_mp3_st_hld.sound.info |= MP3_ST_TRACK;
    dec_mp3_st_hld.sound.para = MP3_ST_KICK_SIZE;
    dec_mp3_st_hld.p_dbuf       = MP3_ST_CAL_BUF;
    dec_mp3_st_hld.dec_ops      = ops;
    dec_mp3_st_hld.event_tab    = (u8 *)&mp3_st_evt[0];
    dec_mp3_st_hld.p_dp_buf     = p_dp_buf;
    dec_mp3_st_hld.eq           = mp3_st_eq_set;
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
    /* dec_mp3_st_hld.function |= DEC_FORMAT_CHECK_FIX; */
    /* format_check_start_jiffies = maskrom_get_jiffies(); */
    if (!(B_DEC_NO_CHECK & p_strm->strm_ctl)) {
        MP3_ST_FORMAT_CHECK_ENTER();
        err = ops->format_check(MP3_ST_CAL_BUF);
        if (0 != err) {                  //格式检查
            /* dec_mp3_st_hld.function &= ~DEC_FORMAT_CHECK_FIX; */
            MP3_ST_FORMAT_CHECK_EXIT();
            log_info(" mp3 format err : %s - 0x%x\n", g_file_sname, err);
            return E_MP3_ST_FORMAT;
        }
        /* dec_mp3_st_hld.function &= ~DEC_FORMAT_CHECK_FIX; */
        MP3_ST_FORMAT_CHECK_EXIT();
    }

    /* modevalue.mode = 1;          //output是否判断返回值 */
    /* ops->dec_confing(MP3_ST_CAL_BUF, SET_DECODE_MODE, &modevalue); */
    MP3_ST_DEC_CONFING();
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
