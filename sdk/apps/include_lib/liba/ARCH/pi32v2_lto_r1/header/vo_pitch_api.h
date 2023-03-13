#ifndef __VOPITCH_API_H__
#define __VOPITCH_API_H__
#include "typedef.h"
typedef struct _VP_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} VP_IO_CONTEXT;

enum {
    HARMO_ROBORT = 0x1,
    HARMO_PITCHSHIFT = 0x2,
    HARMO_RAP = 0x4,
    HARMO_PITCHSHIFT2 = 0x8,
    HARMO_ECHO = 0x10,
    HARMO_RAP_REALTIME = 0x20,
    HARMO_ROBORT2 = 0x40,
    HARMO_CARTOON = 0x80,
    HARMO_NULL = 0x80,
};

enum {
    FILT_NORH = 1,      //陷波滤波器
    FILT_LOWSELF = 2,   //处理中心频率以下频段
    FILT_HIGHSELF = 3,  //处理中心频率以上频段
    FILT_BAND = 4       //处理某个频段需要压制多少
};

enum {
    RAP_PREPARE = 0x0,
    RAP_START = 0x01,
    RAP_END
};

typedef struct _VOICE_PITCH_PARA_STRUCT_ {
    unsigned short do_flag;                //选择变声效果
    unsigned short samplerate;             //输入音频采样率，支持16K/24K
    unsigned short noise_dc;   //2048      //能量阈值
    unsigned short pitchrate;              //音调高低，128代表原音调，<128音调升高，>128音调降低
    unsigned short hamorrate;  //64-256,   //音色调整。<128音色变尖，>128音色变沉
    const unsigned char *midi_file;        //RAP模式的音源输入
    unsigned short midifile_len;           //RAP模式的音源大小
    void (*callback)(void *priv, int pos); //RAP模式的状态回调
    void *priv;
} VOICE_PITCH_PARA_STRUCT;

typedef struct  _VOICEPITCH_STUCT_API_ {
    unsigned int(*need_buf)();
    int(*open)(unsigned int *ptr, VOICE_PITCH_PARA_STRUCT *speedpitch_para, VP_IO_CONTEXT *rs_io);
    int(*run)(unsigned int *ptr, short *inbuf, int len);
} VOICEPITCH_STUCT_API;

VOICEPITCH_STUCT_API *get_vopitch_context();

typedef enum {
    VP_CMD_PITCHSHIFT = 0,  //变调音效
    VP_CMD_PITCHSHIFT2,     //变调音效2
    VP_CMD_ROBOT,           //机器人音效
    VP_CMD_ROBOT2,          //机器人音效2
    VP_CMD_RAP,             //RAP音效
    VP_CMD_RAP_REALTIME,    //RAP音效2
    VP_CMD_CARTOON,         //卡通音效
    VP_CMD_ECHO,            //混响音效
} VP_CMD;

extern void no_filt_init(void *ptr, int sr, VP_IO_CONTEXT *vp_io);
extern int  no_filt_do(void *ptr, void *data, int len);
extern int  no_filt_needbuf();


sound_out_obj *link_voice_pitch_sound(sound_out_obj *p_curr_sound, void *p_dac_cbuf, void **pp_effect, u32 sr, u32 cmd);
void *vp_phy(void *obuf, VOICE_PITCH_PARA_STRUCT *pvp_parm, void **ppsound, u32 sr);

void rap_callback(void *priv, int pos);
bool vp_cmd_case(VP_CMD cmd, u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm);
void vp_cmd_rap(u32 sr, VOICE_PITCH_PARA_STRUCT *p_vc_parm);
void rap_reopen(u32 sr);
void voice_pitch_debug(VOICE_PITCH_PARA_STRUCT *p_vp_parm);
#endif

