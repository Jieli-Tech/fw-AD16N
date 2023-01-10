
#pragma bss_seg(".f1a_data")
#pragma data_seg(".f1a_data")
#pragma const_seg(".f1a_api.text.const")
#pragma code_seg(".f1a_api.text")
#pragma str_literal_override(".f1a_api.text.const")

#include "f1a_api.h"
#include "cpu.h"
#include "config.h"
#include "typedef.h"
//#include "hwi.h"
#include "decoder_api.h"
//#include "dev_manage.h"
#include "vfs.h"
//#include "fs.h"
#include "circular_buf.h"
#include "audio_dac_api.h"
/* #include "resample.h" */
/* #include "speed_api.h" */
#include "msg.h"
#include "errno-base.h"
#include "decoder_msg_tab.h"
#include "app_config.h"
//#include "avio.h"
//#include "bitstream.h"
//#include "wmadata.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[f1a_api]"
#include "log.h"

#define F1A_OBUF_SIZE   (DAC_DECODER_BUF_SIZE * 2)
#define F1A_DBUF_SIZE   (0xc70)
#define F1A_OUTPUT_MAX_SIZE (32 * 2)
#define F1A_KICK_SIZE   (F1A_OBUF_SIZE - (F1A_OUTPUT_MAX_SIZE * 2))
dec_obj dec_f1a_hld[MAX_F1A_CHANNEL] AT(.f1a.data.bss);

#ifdef CPU_SH57
const int cos_tab_split = 1300 / 4;
/* const int wma_cos_maskrom_tab[1] = {0}; */
#else
const int cos_tab_split = 0;
const int wma_cos_maskrom_tab[1] = {0};
#endif

/*************************************************************/
/* cbuffer_t cbuf_f1a[MAX_F1A_CHANNEL] AT(.f1a_buf); */
/* u8 obuf_f1a[MAX_F1A_CHANNEL][F1A_OBUF_SIZE] AT(.f1a_buf); */
/* u8 f1a_decode_buff[MAX_F1A_CHANNEL][F1A_DBUF_SIZE] AT(.f1a_buf); */

typedef struct _f1x_data {
    cbuffer_t cbuf;
    u16 o_buf[F1A_OBUF_SIZE / 2];
    u16 d_buff[F1A_DBUF_SIZE / 2];
} f1x_data;

f1x_data g_f1x1_data AT(.f1a_1_buf);
f1x_data g_f1x2_data AT(.f1a_2_buf);

const struct if_decoder_io f1a_dec_io[MAX_F1A_CHANNEL] = {
    {
        &dec_f1a_hld[0],      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
        mp_input,
        0,
        mp_output,
        decoder_get_flen,
        0
    },
#if (MAX_F1A_CHANNEL > 1)
    {
        &dec_f1a_hld[1],      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
        mp_input,
        0,
        mp_output,
        decoder_get_flen,
        0
    }
#endif
};

const char f1a_ext[] = {".f1a"};
const char f1b_ext[] = {".f1b"};
const char f1c_ext[] = {".f1c"};
const char f1x_ext[] = {".f1x"};
extern audio_decoder_ops *get_f1a_ops();

const u8 g_loop_tab[10] = {
    0, 1, 2, 0, 0,
    1, 2, 3, 2, 1
};

u32 f1a_decode_index(void *p_file, u32 index, dec_obj **p_dec, void *p_dp_buf, f1x_data *p_f1x_data)
{
    u32 buff_len, i;
    decoder_ops_t *ops;
    /* void *name; */
    /* char name[16] = {0}; */
    char ctype = 0;
    log_info("f1a_decode_api");

    void *p_cbuf;
    dec_obj *p_dec_hld;
    void *p_obuf;
    void *p_dbuf;
    void *p_dec_io;

    if (index >= MAX_F1A_CHANNEL) {
        return E_F1A_INDEX;
    }

    /* p_cbuf = &cbuf_f1a[index]; */
    /* p_obuf = &obuf_f1a[index][0]; */
    /* p_dbuf = &f1a_decode_buff[index][0]; */
    p_cbuf = &p_f1x_data->cbuf;
    p_obuf = &p_f1x_data->o_buf[0];
    p_dbuf = &p_f1x_data->d_buff[0];

    p_dec_hld = &dec_f1a_hld[index];
    p_dec_io = (void *)&f1a_dec_io[index];

    local_irq_disable();
    memset(p_dec_hld, 0, sizeof(dec_obj));
    local_irq_enable();


    dec_f1a_hld[0].type = D_TYPE_F1A_1;
#if (MAX_F1A_CHANNEL > 1)
    dec_f1a_hld[1].type = D_TYPE_F1A_2;
#endif
    ops = get_f1a_ops();
    buff_len = ops->need_dcbuf_size();
    if (buff_len > F1A_DBUF_SIZE) {
        log_info("f1a buff need : 0x%x\n", buff_len);
        return E_F1A_DBUF;
    }
    /******************************************/
    cbuf_init(p_cbuf, p_obuf, F1A_OBUF_SIZE);
    /* log_info("A\n"); */
    /* log_info("c buffrt 0x%x\n",p_cbuf); */
    /* log_info("B\n"); */
    p_dec_hld->p_file = p_file;
    p_dec_hld->sound.p_obuf = p_cbuf;
    p_dec_hld->sound.para = F1A_KICK_SIZE;
    p_dec_hld->p_dbuf = p_dbuf;
    p_dec_hld->dec_ops = ops;
    p_dec_hld->event_tab = (u8 *)&f1a_evt[index][0];
    p_dec_hld->p_dp_buf = p_dp_buf;
    /* log_info("C\n"); */
    //


    /******************************************/
    /* name = vfs_file_name(p_file); */
    int file_len = vfs_file_name(p_file, (void *)g_file_sname, sizeof(g_file_sname));
    log_info("file name : %s", g_file_sname);
    u32 t_par = ops->open(p_dbuf, p_dec_io, p_dp_buf);         //传入io接口，说明如下
    if (check_ext_api(g_file_sname, f1a_ext, 4)) {
        ctype = 'a';
    } else if (check_ext_api(g_file_sname, f1b_ext, 4)) {
        ctype = 'b';
        /* ops->get_dec_inf(p_dbuf)->sr = 32000; */
        /* i = 32000; */
        /* ops->dec_confing(p_dbuf, CMD_SET_SAMPLE, &i); */
    } else if (check_ext_api(g_file_sname, f1c_ext, 4)) {
        ctype = 'c';
        if (ops->format_check(p_dbuf)) {                  //格式检查
            log_error("f1c format err\n");
            return E_F1A_FORMAT;
        }
    } else if (check_ext_api(g_file_sname, f1x_ext, 4)) {
        ctype = 'x';
        void *pf1x = f1x_play_api(p_file, (u8 *)g_loop_tab, sizeof(g_loop_tab), index, t_par);
        if (NULL == pf1x) {
            log_error("f1x null\n");
            return E_F1X_FORMAT;
        }
        ops->dec_confing(p_dbuf, CMD_SET_PLAY_FILE, pf1x);
        if (ops->format_check(p_dbuf)) {                  //格式检查
            log_error("f1x format err\n");
            return E_F1A_FORMAT;
        }
    } else {
        log_error("f1a type err\n");
        return E_F1A_TYPE;
    }
    log_info("file is a f1%c", ctype);

    if ('b' == ctype) {
        i = 32000;
        ops->dec_confing(p_dbuf, CMD_SET_SAMPLE, &i);
    } else {
        i = ops->get_dec_inf(p_dbuf)->sr;                //获取采样率
    }
    log_info("file sr : %d\n", i);

    /* dac_sr_api(i); */
    p_dec_hld->sr = i;

    *p_dec = p_dec_hld;
    return 0;
    regist_dac_channel(&p_dec_hld->sound, kick_decoder);//注册到DAC;

    /*************************************************/
    /* log_info("E\n"); */
    if ('b' == ctype) {
        i = 32000;
    } else {
        i = ops->get_dec_inf(p_dbuf)->sr;                //获取采样率
    }
    log_info("file sr : %d\n", i);

    /* dac_sr_api(i); */
    p_dec_hld->sr = i;
    *p_dec = p_dec_hld;
    return 0;
    /* p_dec_hld->sound.enable = B_DEC_ENABLE | B_DEC_KICK; */
    /* kick_decoder(); */
    /* return 0; */
}



//--------------------------------
u32 f1a_decode_api_1(void *p_file, void **p_pdec, void *t_dp_buf)
{
    return f1a_decode_index(p_file, 0, (dec_obj **)p_pdec, t_dp_buf, &g_f1x1_data);
}

u32 f1a_decode_api_2(void *p_file, void **p_pdec, void *t_dp_buf)
{
#if (MAX_F1A_CHANNEL > 1)
    return f1a_decode_index(p_file, 1, (dec_obj **)p_pdec, t_dp_buf, &g_f1x2_data);
#else
    return E_F1A_OUTRGE;
#endif
}

/* extern const u8 _buf_start[]; */
/* extern const u8 _buf_end[]; */
extern const u8 f1a_1_buf_start[];
extern const u8 f1a_1_buf_end[];
extern const u8 f1a_2_buf_start[];
extern const u8 f1a_2_buf_end[];
u32 f1a_1_buff_api(dec_buf *p_dec_buf)
{
    p_dec_buf->start = (u32)&f1a_1_buf_start[0];
    p_dec_buf->end   = (u32)&f1a_1_buf_end[0];
    return 0;
}

u32 f1a_2_buff_api(dec_buf *p_dec_buf)
{
    p_dec_buf->start = (u32)&f1a_2_buf_start[0];
    p_dec_buf->end   = (u32)&f1a_2_buf_end[0];
    return 0;
}
