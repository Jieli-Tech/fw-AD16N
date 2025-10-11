#ifndef MIDI_DEC_API_h__
#define MIDI_DEC_API_h__

#include "PlatformDefine.h"
#define  CTRL_CHANNEL_NUM             16

#include "if_decoder_ctrl.h"

typedef struct _MIDI_CONFIG_PARM_ {

    unsigned int spi_pos;		//	音色文件地址
    unsigned char sample_rate;	//	采样率0 - 8 对应采样率表{48000,44100,32000,24000,22050,16000,12000,11025,8000}
    unsigned char bitwidth;
    unsigned char OutdataBit;
    unsigned char out_channel;
    short player_t;				//  最多同时发声的key数
} MIDI_CONFIG_PARM;

typedef struct _EX_CH_VOL_PARM_ {
    unsigned short  cc_vol[CTRL_CHANNEL_NUM];                 //16个通道或轨道的音量      <=>4096等于原音量
    unsigned char	ex_vol_use_chn;							  //ex_vol_use_chn = 0 轨道音量；ex_vol_use_chn = 1 通道音量
} EX_CH_VOL_PARM;


typedef  struct _EX_INFO_STRUCT_ {
    void *priv;
    u32(*mark_trigger)(void *priv, u8 *val, u8 len);			//标记回调，val 标记名称  len 字节数
} EX_INFO_STRUCT;


typedef  struct _EX_TmDIV_STRUCT_ {					 //小节回调
    void *priv;
    u32(*timeDiv_trigger)(void *priv);
} EX_TmDIV_STRUCT;

typedef  struct _EX_BeatTrig_STRUCT_ {				//节拍回调/*val1 一节多少拍,  val2每拍多少分音符*/
    void *priv;
    u32(*beat_trigger)(void *priv, u8 val1, u8 val2);
} EX_BeatTrig_STRUCT;

typedef  struct _EX_MELODY_STRUCT_ {				//主旋律音符回调 vel 为按键力度
    void *priv;
    u32(*melody_trigger)(void *priv, u8 key, u8 vel);
} EX_MELODY_STRUCT;

typedef struct _EX_MELODY_STOP_STRUCT_ {			//主旋律音符停止回调
    void *priv;
    u8 main_chn_enable;
    u32(*melody_stop_trigger)(void *priv, u8 key, u8 chnl);
} EX_MELODY_STOP_STRUCT;

typedef struct _WDT_CLEAR_ {					//清狗回调，okon 只播主旋时需要清狗
    void *priv;
    u32 count;									//副旋count key回调一次
    u32(*wdt_clear_trigger)(void *priv);
} WDT_CLEAR;

typedef struct _MIDI_Limiter {
    void *limiter_buf;
    void (*limiter_run)(void *limiter_buf, void *indata, void *outdata, int per_channel_npoint);
} MIDI_Limiter;

typedef struct _MIDI_NOTE_ON_ {
    u8 Enable;
    void (*midi_note_on_trigger)(void *priv, u8 trk_num, u8 chnl, u8 *key, u8 *vel, u8 *note_bypass);
} MIDI_NOTE_ON;

typedef struct _MIDI_NOTE_OFF_ {
    u8 Enable;
    void (*midi_note_off_trigger)(void *priv, u8 trk_num, u8 chnl, u8 *key);
} MIDI_NOTE_OFF;

typedef struct _MIDI_ADSR_ {
    u8 Enable;
    void (*midi_adsr_trigger)(void *priv, u16 *attack, int *decay, int *release, s8 stero_channel);
} MIDI_ADSR;

typedef struct __MIDI_CTRL_EVENT_ {
    void (*midi_event_protect)();
    void (*midi_event_disprotect)();
} MIDI_CTRL_EVENT;


enum {
    CMD_MIDI_CTRL_MODE_0 = 0X00,		 // 正常解码
    CMD_MIDI_CTRL_MODE_1 = 0X01,	//OKON 模式
    CMD_MIDI_CTRL_MODE_2,					// 只推消息，不出声
    CMD_MIDI_CTRL_MODE_W2S                   //外部音源
};

enum {
    CMD_MIDI_OKON_MODE_0 = 0x00,	//主旋 okon 副旋正常播放
    CMD_MIDI_OKON_MODE_1,			//主副旋一起 okon
    CMD_MIDI_OKON_MODE_2			//只播放主旋 okon
};

enum {
    CMD_MIDI_MELODY_KEY_0 = 0x00,	//melody_trigger 回调当前音符
    CMD_MIDI_MELODY_KEY_1			// melody_trigger 回调下一个音符
};


typedef struct _MIDI_MARK_PARAM_ {
    short mark_start;
    short mark_end;
    u8 loop_enable;
    u8 mark_enable;
} MIDI_MARK_PARAM;

typedef  struct _MIDI_PLAY_CTRL_MODE_ {
    u8 mode;
} MIDI_PLAY_CTRL_MODE;				//用于配置midi模式

typedef struct _MIDI_OKON_MODE_ {
    u8 Melody_Key_Mode;
    u8 OKON_Mode;
} MIDI_OKON_MODE;					//用于配置OKON模式

typedef  struct _MIDI_PLAY_CTRL_TEMPO_ {
    u16 tempo_val;								 //配置节奏
    u16 decay_val[CTRL_CHANNEL_NUM];             //1024 低11bit有效
    u32 mute_threshold;
} MIDI_PLAY_CTRL_TEMPO;				//用于配置节奏及衰减



typedef struct _MIDI_CHNNEL_CTRL_STRUCT_ {
    u8 chn;
} MIDI_CHNNEL_CTRL_STRUCT;

typedef struct _MIDI_PROG_CTRL_STRUCT_ {
    u8 prog;				  //乐器号
    u8 replace_mode;         //replace_mode==1，就是 替换所有通道； 否则替换主通道
    u16 ex_vol;              //1024是跟原来一样大， 变化为原来的(ex_vol/1024)倍
} MIDI_PROG_CTRL_STRUCT;		 //用于配置主通道乐器或者替换所有的通道

typedef struct _MIDI_PROG_CHNL_CTRL {
    u8 prog;
    u8 chnl;
    u8 enable;
} MIDI_PROG_CHNL_CTRL;

typedef struct _MIDI_SEEK_BACK_STRUCT_ {
    s8 seek_back_n;
} MIDI_SEEK_BACK_STRUCT;    //回调多少小节

typedef struct _MIDI_SEMITONE_CTRL_STRUCT_ {
    char  key_diff[CTRL_CHANNEL_NUM];	// 配置每个通道移多少半音
} MIDI_SEMITONE_CTRL_STRUCT;

#define MAX_WORD   10

typedef struct _MIDI_W2S_STRUCT_ {
    unsigned int word_cnt;                           //多少个音
    unsigned int data_pos[MAX_WORD + 1];               //每个音起始位置
    unsigned int data_len[MAX_WORD + 1];               //每个音数据长度
    unsigned short *rec_data;						//pcm 数据
    char key_diff;									//与音高成反比 建议范围[-12,12]
} MIDI_W2S_STRUCT;


enum {
    CMD_MIDI_SEEK_BACK_N = 0xa0,		//回退								  MIDI_SEEK_BACK_STRUCT 变量
    CMD_MIDI_SET_CHN_PROG,			//配置主通道乐器或者所有的通道乐器	  MIDI_PROG_CTRL_STRUCT 变量
    CMD_MIDI_CTRL_TEMPO,			//配置节奏及衰减					  MIDI_PLAY_CTRL_TEMPO 变量
    CMD_MIDI_GOON,					//okon 发声
    CMD_MIDI_CTRL_MODE,				//配置midi模式						  MIDI_PLAY_CTRL_MODE 变量
    CMD_MIDI_SET_SWITCH,			//配置使能		                      使能参数
    CMD_MIDI_SET_EX_VOL,			//配置外部音量						  EX_CH_VOL_PARM 变量
    CMD_INIT_CONFIG,				//初始化配置						  MIDI_INIT_STRUCT 变量
    CMD_INIT_CONFIGS,				//音色文件配置						  MIDI_CONFIG_PARM  变量
    CMD_MIDI_OKON_MODE,				//配置OKON模式						  MIDI_OKON_MODE 变量
    CMD_MIDI_SET_SEMITONE,				//配置移半音					  MIDI_SEMITONE_CTRL_STRUCT 变量
    CMD_MIDI_MELODY_TRIGGER,			//配置主旋律音符播放或者midi琴音符播放
    CMD_MIDI_STOP_MELODY_TRIGGER,		//配置主旋律音符停止播放或者midi琴音符停止播放
    CMD_MIDI_LIMITER_TRIGGER,			//配置限幅器
    CMD_MIDI_NOTE_ON_TRIGGER,
    CMD_MIDI_NOTE_OFF_TRIGGER,
    CMD_MIDI_ADSR_TRIGGER,
    CMD_MIDI_SET_PROG,
    CMD_MIDI_CTRL_EVENT,
    CMD_MIDI_SET_MARK,
};

enum {
    MARK_ENABLE = 0x0001,                  //mark回调的使能
    MELODY_ENABLE = 0x0002,                //主旋律音符回调的使能
    TIM_DIV_ENABLE = 0x0004,               //小节回调的使能
    MUTE_ENABLE = 0x0008,                  //mute住解码的使能
    SAVE_DIV_ENBALE = 0x0010,               //小节保存的使能
    EX_VOL_ENABLE = 0x0020,                 //外部音量控制使能
    SET_PROG_ENABLE = 0x0040,               //主轨道设置成固定乐器使能
    MELODY_PLAY_ENABLE = 0x0080,             //主轨道播放使能
    BEAT_TRIG_ENABLE = 0x0100,                //每拍回调的使能
    MELODY_STOP_ENABLE = 0x200,				//主旋律音符停止回调使能
    MARK_LOOP_ENABLE = 0x400,				//使用mark做循环播放使能
    SEMITONE_ENABLE = 0x800,					//移半音使能
    MAIN_INTERRUPT_ENABLE = 0x1000,			//OKON 只播主旋律时音符中断
    LIMITER_ENABLE = 0x2000,
};


typedef struct _MIDI_INIT_STRUCT_ {
    MIDI_CONFIG_PARM init_info;               //初始化参数
    MIDI_PLAY_CTRL_MODE mode_info;            //控制模式
    MIDI_PLAY_CTRL_TEMPO  tempo_info;         //节奏参数
    EX_CH_VOL_PARM  vol_info;                 //外部音量控制
    MIDI_PROG_CTRL_STRUCT prog_info;          //主轨道乐器参数
    MIDI_CHNNEL_CTRL_STRUCT  mainTrack_info;  //主轨道设置参数
    EX_INFO_STRUCT  mark_info;                //mark回调函数
    EX_MELODY_STRUCT moledy_info;             //melody回调函数
    EX_TmDIV_STRUCT  tmDiv_info;              //小节回调参数
    EX_BeatTrig_STRUCT beat_info;             //每拍回调参数
    WDT_CLEAR wdt_clear;
    MIDI_OKON_MODE okon_info;				  //OKON参数
    EX_MELODY_STOP_STRUCT moledy_stop_info;	 //melody_stop回调函数
    MIDI_SEMITONE_CTRL_STRUCT semitone_info;  //移半音参数
    MIDI_W2S_STRUCT    w2s_info;
    MIDI_NOTE_ON    note_on_trigger;		 //note on 回调
    MIDI_NOTE_OFF   note_off_trigger;		 //note off 回调
    MIDI_ADSR		adsr_trigger;			 //adsr 回调
    u32   switch_info;              //初始化一些使能位，默认为0
} MIDI_INIT_STRUCT;


extern audio_decoder_ops *get_midi_ops();
extern int get_midi_tone_compressor(void *work_buf);

#endif // MIDI_DEC_API_h__


