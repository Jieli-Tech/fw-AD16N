#ifndef __EFFECTS_ADJ__H
#define __EFFECTS_ADJ__H

#include "typedef.h"
#include "audio_eq.h"

typedef struct {
    int cmd;     			///<EQ_ONLINE_CMD
    int data[64];       	///<data
} EFF_ONLINE_PACKET;

#define REPLY_TO_TOOL       (0)
#define EFF_CFG_FILE_ID     (0x3)//音效配置项源文件id

enum {
    EFF_CMD_INQUIRE = 0x4,
    EFF_CMD_GETVER = 0x5,
    EFF_CMD_FILE_SIZE = 0xB,
    EFF_CMD_FILE = 0xC,
    EFF_CMD_CHANGE_MODE = 0xE,//切模式
    EFF_CMD_RESYNC_PARM_END = 0x28,//参数重复结束
    EFF_CMD_RESYNC_PARM_BEGIN = 0x30,//参数重复开始

    EFF_MIC_EQ0 = 0x1001,
    EFF_MIC_EQ1 = 0x1002,
    EFF_MIC_EQ2 = 0x1003,
    EFF_MIC_EQ3 = 0x1004,
    EFF_MIC_EQ4 = 0x1005,
    EFF_MIC_DRC0 = 0x1006,
    EFF_MIC_DRC1 = 0x1007,
    EFF_MIC_DRC2 = 0x1008,
    EFF_MIC_DRC3 = 0x1009,
    EFF_MIC_DRC4 = 0x100a,

    EFF_MIC_PLATE_REVERB  = 0x100c,
    EFF_MIC_ECHO          = 0x100e,
    EFF_MIC_NOISEGATE     = 0x1014,
    EFF_MIC_HOWLINE_PS    = 0x1015,
    EFF_MIC_NOTCH_HOWLING = 0x1016,
    EFF_MIC_VOICE_CHANGER = 0x1017,
    EFF_MIC_MIX_GAIN      = 0x1018,
    EFF_MIC_TUNNING_EQ    = 0x1019,
    EFF_MUSIC_TUNNING_EQ  = 0x101a,

    EFF_PHONE_WIDEBAND_EQ   = 0x1101,
    EFF_PHONE_NARROWBAND_EQ = 0x1102,
    EFF_AEC_WIDEBAND_EQ     = 0x1103,
    EFF_AEC_NARROWBAND_EQ   = 0x1104,
    EFF_PHONE_WIDEBAND_DRC  = 0x1105,
    EFF_PHONE_NARROWBAND_DRC = 0x1106,
    EFF_AEC_WIDEBAND_DRC    = 0x1107,
    EFF_AEC_NARROWBAND_DRC  = 0x1108,


    EFF_MUSIC_EQ          = 0x2001,
    EFF_MUSIC_LOW_DRC     = 0x2002,
    EFF_MUSIC_FR_EQ       = 0x2003,
    EFF_MUSIC_MID_DRC     = 0x2011,
    EFF_MUSIC_HIGH_DRC    = 0x2012,
    EFF_MUSIC_WHOLE_DRC   = 0x2013,
    EFF_MUSIC_CROSSOVER   = 0x2014,
    EFF_MUSIC_DYNAMIC_EQ  = 0x2015,
    EFF_MUSIC_EQ2         = 0x2016,
    EFF_MUSIC_GAIN        = 0x2017,

    EFF_MIC_GAIN          = 0x2018,//混响输出位置的gain
    EFF_MUSIC_RL_GAIN     = 0x2019,//rl_rr通道gain


    EFF_MUSIC_RL_RR_LOW_PASS = 0x2020,//2.1/2.2声道低通滤波器
    EFF_MUSIC_RL_EQ          = 0x2005,
    EFF_MUSIC_RL_LOW_DRC     = 0x2006,
    EFF_MUSIC_RR_EQ          = 0x2007,
    EFF_MUSIC_RL_MID_DRC     = 0x2027,
    EFF_MUSIC_RL_HIGH_DRC    = 0x2028,
    EFF_MUSIC_RL_WHOLE_DRC   = 0x2029,
    EFF_MUSIC_RL_CROSSOVER   = 0x2029,

    EFF_AUX_EQ         = 0x2030,//linein 需要独立的音效时，使用的效果id
    EFF_AUX_DRC        = 0x2031,
    EFF_AUX_GAIN       = 0x2032,

    EFF_MUSIC_VOICE_CHANGER = 0x2033,
    EFF_MUSIC_VBASS         = 0x2034,//音乐虚拟低音
    EFF_MUSIC_VBASS_PREV_GAIN = 0x2035,

    EFF_MUSIC_LPF_EQ         = 0x2036,
    EFF_MUSIC_HIGH_BASS_EQ       = 0x2037,
    EFF_MUSIC_HIGH_BASS_DRC       = 0x2038,

    EFF_MUSIC_NOISEGATE     = 0x2039,


    EFF_MUSIC_SURRROUND_EFF       = 0x2040,
    EFF_MUSIC_EXT_EQ              = 0x2041,//2.1声道全频音箱通路的后级eq

    EFF_MUSIC_CH_SWAP             = 0x2042,
    EFF_MUSIC_RL_CH_SWAP          = 0x2043,
    EFF_DAC_PGA                   = 0x2044,
    EFF_MUSIC_EXT_EQ2             = 0x2045,//2.1声道低音音箱通路的后级eq
    EFF_MUSIC_FR_EXT_EQ           = 0x2046,
    EFF_ADC_PGA                   = 0x2047,

    EFF_MUSIC_LAST_WHOLE_DRC      = 0x2050,

    EFF_MUSIC_FR_LOW_DRC         = 0x2060,
    EFF_MUSIC_FR_MID_DRC         = 0x2061,
    EFF_MUSIC_FR_HIGH_DRC        = 0x2062,
    EFF_MUSIC_FR_WHOLE_DRC       = 0x2063,
    EFF_MUSIC_FR_CROSSOVER       = 0x2064,


    EFF_MUSIC_RR_LOW_DRC         = 0x2065,
    EFF_MUSIC_RR_MID_DRC         = 0x2066,
    EFF_MUSIC_RR_HIGH_DRC        = 0x2067,
    EFF_MUSIC_RR_WHOLE_DRC       = 0x2068,
    EFF_MUSIC_RR_CROSSOVER       = 0x2069,


    EFF_MIC_LAST_CROSSOVER = 0x3000,
    EFF_MIC_LAST_LOW_DRC = 0x3001,
    EFF_MIC_LAST_MID_DRC = 0x3002,
    EFF_MIC_LAST_HIGH_DRC = 0x3003,
    EFF_MIC_LAST_WHOLE_DRC = 0x3004,

    MIC_DUT_EQ = 0x4000,
    EFF_CMD_MAX,//最后一个

};

typedef enum {
//模式id,效果文件解析时会用于定位相应模式下效果参数
    phone_mode_seq = 1,
    aec_mode_seq = 2,

    music_mode_seq0 = 0x4,

    mic_mode_seq0 = 0x5,
    mic_mode_seq1 = 0x6,
    mic_mode_seq2 = 0x7,
    mic_mode_seq3 = 0x8,
    mic_mode_seq4 = 0x9,
    mic_mode_seq5 = 0xa,
    mic_mode_seq6 = 0xb,
    mic_mode_seq7 = 0xc,

    linein_mode_seq = 0xe,

//add xx
    mic_dut_mode_seq = 0x10,

    max_seq,
} MODE_NUM;

extern void effect_eq_parm_init(void);
extern int effect_get_cfg_file_data(u32 offset, u8 *data, u32 len);
extern int write_eq_vm_data(u8 *buffer, u32 len);
extern int get_eq_file_len(void);
int ext_eff_file_analyze_api(u8 eq_mode, void *data_buf, u32 buf_len);
u8 get_eq_mode_total_num();
void *get_eq_mode_name_api(u8 eq_mode);

struct cmd_interface {
    u32 cmd;
    void (*eff_default_parm_init)(void);//音效参数默认初始化默认值
    void (*eff_file_analyze_init)(void);//解析效果文件内相应的音效参数到目标地址
    int (*cmd_deal)(EFF_ONLINE_PACKET *packet, u8 id, u8 sq);//在线调试处理函数
};

#define REGISTER_CMD_TARGET(cmd_eff) \
	const struct cmd_interface cmd_eff SEC_USED(.eff_cmd)

extern const struct cmd_interface cmd_interface_begin[];
extern const struct cmd_interface cmd_interface_end[];

#define list_for_each_cmd_interface(p) \
	    for (p = (struct cmd_interface *)cmd_interface_begin; p < (struct cmd_interface *)cmd_interface_end; p++)


#endif/*__EFFECTS_ADJ__H*/

