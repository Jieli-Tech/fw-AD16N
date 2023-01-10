#ifndef __if_decoder_ctrl_h
#define __if_decoder_ctrl_h

#include "string.h"

#include "typedef.h"
//#define DECODE_NORMAL  0x00
//#define DECODE_FF      0x01
//#define DECODE_FR      0x02
//#define DECODE_STOP    0x03

#define SET_DECODE_MODE       0x80
#define CMD_SET_CONTINUE_BK   0x90
#define CMD_SET_PLAY_FILE     0x91
#define CMD_SET_SAMPLE        0x92
#define CMD_SET_FADEOUT       0x93

typedef struct _AUDIO_FADE_PARA {
    u32 mode;                 //0跟之前一样，1代表要fade到0
} AUDIO_FADE_PARA;


#define PLAY_MOD_NORMAL   0x00
#define PLAY_MOD_FF   0x01
#define PLAY_MOD_FB   0x02

enum {
    FAST_FREQ_restrict = 0x01,
    FAST_FILTER_restrict = 0x02,
    FAST_CHANNEL_restrict = 0x04
};

//play control
#define PLAY_FILE       0x80000000
#define PLAY_CONTINUE   0x80000001
#define PLAY_NEXT       0x80000002


#define REAPT_MOD_STARTA   0x01
#define REAPT_MOD_STARTB   0x02
#define REAPT_MOD_STARTN   0x03
#define REAPT_MOD_FREPT    0x04

enum {
    SET_BREAKPOINT_A = 0x08,
    SET_BREAKPOINT_B,
    SET_RECOVER_MODE
};

typedef struct _PARM_RECOVERMODE_ {
    u32  RECOVER_MODE_value;
} PARM_RECOVERMODE;


#define  AB_REPEAT_CB_ABCONIFG      11


enum {
    AB_REPEAT_MODE_BP_A = 0x01,
    AB_REPEAT_MODE_BP_B,
    AB_REPEAT_MODE_CUR,
};

typedef struct  _audio_ab_repeat_mode_param {
    u32 value;
    int (*callback)(void *priv, int mode);
    void *callback_priv;
} audio_ab_repeat_mode_param;


#define  SET_DEST_PLAYPOS        0x93
#define  GET_PLAYPOS             0x94

typedef struct _PARM_PLAYPOS_ {
    u32  time_v;
} PARM_PLAYPOS;

typedef struct _PARM_DESTTIME_ {
    u32  start_time;         //要跳转过去播放的起始时间：ms
    u32  dest_time;          //要跳转过去播放的目标时间: ms
    u32(*callbackfun)(void *priv);
    void *priv;
} PARM_DESTTIME;

typedef struct _AUDIO_DECODE_PARA {
    u32 mode;
} AUDIO_DECODE_PARA;
#define SET_DECODE_MODE   0x80

typedef  struct FIXPHASE_obj {
    short fifo_buf[18 + 12][32][2];
} FIXPHASE_obj;


#define  REPEAT_PLAY_ALWAYS    0x90
typedef  struct _repeat_mode_flag {
    int flag;                   //1代表循环模式开，0代表循环模式关
    int headcut_frame;     //循环模式的时候跳掉文件前面的n帧: wav解码器不会用这个参数的；
    int tailcut_frame;       //循环模式的时候跳掉文件末尾的n帧： wav解码器不会用这个参数的；
    int (*repeat_callback)(void *priv);            //每次播完一遍就回调。如果不需要callback，设成0.
    void *callback_priv;
    FIXPHASE_obj *fix_obj;
} repeat_mode_flag;

#define AUDIO_BK_EN

typedef struct _EX_PlayFile_STRUCT_ {
    void *priv;
    u32(*set_play_file)(void *priv, u32 *startpos, u32 *endpos);
} EX_PlayFile_STRUCT;

struct if_decoder_io {
    void *priv ;
    int (*input)(void *priv, u32 addr, void *buf, int len, u8 type);
    /*
    priv -- 私有结构体，由初始化函数提供。
    addr -- 文件位置
    buf  -- 读入地址
    len  -- 读入长度 512 的整数倍
    type -- 0 --同步读（等到数据读回来，函数才返回） ，1 -- 异步读（不等数据读回来，函数就放回）

    */
    int (*check_buf)(void *priv, u32 addr, void *buf);
    u32(*output)(void *priv, void *data, int len);
    u32(*get_lslen)(void *priv);
    u32(*store_rev_data)(void *priv, u32 addr, int len);
};

typedef struct if_decoder_io IF_DECODER_IO;
typedef struct decoder_inf {
    u16 sr ;            ///< sample rate
    u16 br ;            ///< bit rate
    u32 nch ;           ///<声道
    u32 total_time;     ///<总时间
} dec_inf_t ;


typedef struct __audio_decoder_ops {
    char *name ;                                                            ///< 解码器名称
    u32(*open)(void *work_buf, const struct if_decoder_io *decoder_io, u8 *bk_point_ptr);   ///<打开解码器

    u32(*format_check)(void *work_buf);					///<格式检查

    u32(*run)(void *work_buf, u32 type);					///<主循环

    dec_inf_t *(*get_dec_inf)(void *work_buf) ;				///<获取解码信息
    u32(*get_playtime)(void *work_buf) ;					///<获取播放时间
    u32(*get_bp_inf)(void *work_buf);						///<获取断点信息

    //u32 (*need_workbuf_size)() ;							///<获取整个解码所需的buffer
    u32(*need_dcbuf_size)() ;		                        ///<获取解码需要的buffer
    u32(*need_rdbuf_size)();                               ///<获取解码读数buf的读文件缓存buf的大小
    u32(*need_bpbuf_size)() ;				                 ///<获取保存断点信息需要的buffer

    //void (*set_dcbuf)(void* ptr);			                ///<设置解码buffer
    //void (*set_bpbuf)(void *work_buf,void* ptr);			///<设置断点保存buffer

    void (*set_step)(void *work_buf, u32 step);				///<设置快进快进步长。
    void (*set_err_info)(void *work_buf, u32 cmd, u8 *ptr, u32 size);		///<设置解码的错误条件
    u32(*dec_confing)(void *work_buf, u32 cmd, void *parm);
} audio_decoder_ops, decoder_ops_t;
extern audio_decoder_ops *get_ump3_ops();
extern audio_decoder_ops *get_f1a_ops();
extern audio_decoder_ops *get_ima_ops();
extern audio_decoder_ops *get_mp3_ops();
extern audio_decoder_ops *get_wav_ops();


///------------------
// #include "sound_effect_api.h"

typedef struct _dec_buff {
    u32 start;
    u32 end;
} dec_buf;



#endif



