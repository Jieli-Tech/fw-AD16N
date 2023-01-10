#ifndef __DECODER_API_H__
#define __DECODER_API_H__
#include "if_decoder_ctrl.h"
#include "decoder_mge.h"
// #include "dac_api.h"
#include "sound_mge.h"
#include "app_modules.h"
// #include "decoder_msg_tab.h"




typedef enum {
    D_TYPE_F1A_1 = 0,
#if (MAX_F1A_CHANNEL > 1)
    D_TYPE_F1A_2 = 1,
#endif
    D_TYPE_UMP3 = 2,
    D_TYPE_A,
    D_TYPE_MIDI,
    D_TYPE_MIDI_CTRL,
    D_TYPE_WAV,
    D_TYPE_MP3_ST = 7,
} DECOER_TYPE ;


typedef struct _dp_buff {

    union {
        u32 findex;
        u32 sclust;
    };
    u16 crc;
    u16 len;
    union {
        u8 buff[1];
#if DECODER_UMP3_EN
        u8 ump3[20];
#endif
#if DECODER_MP3_ST_EN
        u8 mp3[0x10];
#endif
#if DECODER_F1A_EN
        u8 f1a[60];
#endif
#if DECODER_WAV_EN
        u8 wav[12];
#endif
    };
} dp_buff;


#define DEC_FUNCTION_FF_FR      BIT(0)  // 快进快退功能
#define DEC_FORMAT_CHECK_FIX    BIT(1)  // MP3格式检查


// extern dec_obj dec_ump3_hld;
// extern dec_obj dec_midi_hld;
// extern dec_obj dec_midi_ctrl_hld;
// extern dec_obj dec_f1a_hld[];
// extern dec_obj dec_a_hld;
// extern dec_obj dec_mp3_st_hld;
// extern dec_obj dec_wav_hld;

//断点函数
bool get_dp(dec_obj *obj, dp_buff *dbuff);
void *check_dp(dp_buff *dbuff);
void clear_dp(dp_buff *dbuff);
void clear_dp_buff(void *buff);


int mp_input(void *priv, u32 addr, void *buf, int len, u8 type);
u32 mp_output(void *priv, void *data, int len);
// int effect_output(void *priv, void *data, int len);
void decoder_init(void);
u32 decoder_set_sr(dec_obj *d_obj);

typedef enum  {//停止解码时，是否需要将DAC中剩余的样点消耗完
    NO_WAIT = 0,
    NEED_WAIT = 1,
} DEC_STOP_WAIT;




extern u32 dec_hld_tab[];
extern const u32 decoder_tab[];


void decoder_test_fun(void);
u32 mp_read_2_dac(void);
bool check_ext_api(char _xdata *fname, char const *ext, u32 len);


void irq_decoder_ret(dec_obj *obj, u32 ret);

void decoder_soft0_isr();
void decoder_channel_set(u8 dc);

u32 if_decoder_is_run(dec_obj *obj);
bool decoder_pause(dec_obj *obj);
bool decoder_stop(dec_obj *obj, DEC_STOP_WAIT wait, void *p_dp);
int decoder_fun(void *pfile, u32 dec_ctl, s32 *dec_index);
// dec_obj *decoder_io(void *pfile, u32 dec_ctl, dp_buff * dbuff);
dec_obj *decoder_io(void *pfile, u32 dec_ctl, dp_buff *dbuff, u8 loop);
int decoder_time(dec_obj *p_dec);
u32 decoder_status(dec_obj *obj);
// void decoder_fun(u32(*fun)(void *, void **), void *pfs, void **ppfile, u32 file_index);

bool decoder_ff(dec_obj *obj, u8 step);	// 快进。step单位-秒
bool decoder_fr(dec_obj *obj, u8 step);	// 快退。step单位-秒

void decoder_set_file_size(dec_obj *obj, u32 size);	// 设置解码文件长度
void decoder_soft_hook(void);


//需要解码以外的部分实现的函数
extern u32 decoder_get_flen(void *priv);

#endif


