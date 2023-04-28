#ifndef MSG_H
#define MSG_H

#include "typedef.h"
#include "uart.h"

enum {
//SYS_MSG_START_LINE
    MSG_0 = 0,
    MSG_1,
    MSG_2,
    MSG_3,
    MSG_4,
    MSG_5,
    MSG_6,
    MSG_7,
    MSG_8,
    MSG_9,
    MSG_PP_2,
    MSG_RECODE_START,
    MSG_RECODE_END,
    ///APP
    MSG_500MS,
    MSG_APP_SWITCH_ACTIVE,
    MSG_LOW_POWER,
    MSG_ENTER_IDLE,
    MSG_POWER_OFF,

    // MSG_VOL_UP,
    // MSG_VOL_DOWN,
    // MSG_VOL_KEY_UP,
    // MSG_MUTE,
    // MSG_POWER_ON,
    // MSG_POWER_OFF,
    MSG_PLAY_FILE1,
    MSG_PLAY_FILE2,

    ///音乐操作相关消息
    MSG_PP,
    MSG_NEXT_FILE,
    MSG_PREV_FILE,
    MSG_NEXT_DIR,
    MSG_A_PLAY,
    MSG_VOL_DOWN,
    MSG_VOL_UP,
    //录音操作相关信息
    MSG_REC_MODE_SWITCH,
    MSG_REC_SPEED_EN,
    //midi
    MSG_MIDI_MODE_SWITCH,
    MSG_MIDI_OKON_GOON,
    //midi ctrl
    MSG_MIDICTRL_NOTE_ON_DO,
    MSG_MIDICTRL_NOTE_ON_RE,
    MSG_MIDICTRL_NOTE_ON_MI,
    MSG_MIDICTRL_NOTE_ON_FA,
    MSG_MIDICTRL_NOTE_ON_SO,
    MSG_MIDICTRL_NOTE_ON_LA,
    MSG_MIDICTRL_NOTE_ON_XI,
    MSG_MIDICTRL_NOTE_OFF_DO,
    MSG_MIDICTRL_NOTE_OFF_RE,
    MSG_MIDICTRL_NOTE_OFF_MI,
    MSG_MIDICTRL_NOTE_OFF_FA,
    MSG_MIDICTRL_NOTE_OFF_SO,
    MSG_MIDICTRL_NOTE_OFF_LA,
    MSG_MIDICTRL_NOTE_OFF_XI,
    MSG_MIDICTRL_CHANNAL_PREV,
    MSG_MIDICTRL_CHANNAL_NEXT,
    MSG_MIDICTRL_PITCH_BEND_UP,
    MSG_MIDICTRL_PITCH_BEND_DOWN,
    //others
    MSG_NEXT_MODE,

    ///device message
    MSG_NEXT_DEV,
    MSG_SEL_NEW_DEVICE,
    MSG_FIND_LAST_DEVICE,
    MSG_AUTO_NEXT_DEVICE,

    //mbox msg
    MSG_CHANGE_WORK_MODE = 0x600,
    MSG_MUSIC_NEW_DEVICE_IN,
    MSG_MUSIC_SELECT_NEW_DEVICE,
    MSG_MUSIC_SELECT_NEW_FILE,
    MSG_MUSIC_PLAY_NEW_FILE,
    MSG_MUSIC_PLAY_NEW_FILE0,
    MSG_MUSIC_NEXT_FILE,
    MSG_AUTO_NEXT_FILE,
    MSG_MUSIC_PREV_FILE,
    MSG_AUTO_PREV_FILE,
    MSG_MUSIC_PP,
    MSG_MUSIC_PLAY,
    MSG_MUSIC_PAUSE,
    MSG_MUSIC_STOP,
    MSG_MUSIC_FF,
    MSG_MUSIC_FR,
    MSG_MUSIC_FFR_DONE,
    MSG_MUSIC_NEXT_EQ,
    MSG_MUSIC_PREV_EQ = 0x610,
    MSG_NEXT_PLAYMODE,
    MSG_PREV_PLAYMODE,
    MSG_NEXT_WORKMODE,                  /*0x20*/
    MSG_PREV_WORKMODE,
    MSG_PICK_SONG,
    MSG_PICK_DEVICE,
    MSG_NEXT_DEVICE,
    MSG_PREV_DEVICE,

    MSG_FM_NEXT_STEP,
    MSG_FM_PREV_STEP,
    MSG_FM_NEXT_STATION,
    MSG_FM_PREV_STATION,
    MSG_FM_PP,
    MSG_FM_SCAN_ALL_INIT,
    MSG_FM_SCAN_ALL,
    MSG_FM_SCAN_ALL_UP = 0x620,
    MSG_FM_SCAN_ALL_DOWN,
    MSG_STOP_SCAN,
    MSG_FM_SCAN_UP,
    MSG_FM_SCAN_DOWN,
    MSG_CH_SET,                     /*0x30*/
    MSG_CH_SAVE,
    MSG_KEY_CHANGE,

    MSG_MUTE,
    MSG_UNMUTE,
    MSG_MUTE_UNMUTE,
    MSG_10MS,
    MSG_100MS,                      //55
    MSG_200MS,
    MSG_DECODER_POWER,

    MSG_CLOSE_ALARM,
    MSG_SYS_DOWN,
    MSG_REQUEST_Y = 0x630,  /*0x30*/
    MSG_REQUEST_N,
    MSG_INPUT_TIMEOUT,
    MSG_HALF_SECOND,                /*0x10*/
    MSG_NORMAL_POWER,
    MSG_DECODE_USER_END,
    MSG_DECODE_FILE_END,
    MSG_DECODE_DISK_ERR,

    /* rtc_msg */
    MSG_READ_CLOCK,
    MSG_WRITE_CLOCK,
    MSG_READ_ALARM,
    MSG_WRITE_ALARM,
    MSG_TIME_WAKEUP,
    MSG_ALARM,
    MSG_POWER_DOWN,

    MSG_USB_DISK_IN,          //以下顺序不可随意调整
    MSG_SDMMCA_IN,
    MSG_SDMMCB_IN,
    MSG_EXTFLSH_IN,
    MSG_USB_DISK_OUT,
    MSG_SDMMCA_OUT,
    MSG_SDMMCB_OUT,
    MSG_USB_PC_IN,
    MSG_USB_PC_OUT,
    MSG_PC_MUTE = 0x640,
    MSG_PC_UNMUTE,
    MSG_AUX_IN,
    MSG_AUX_OUT,
    MSG_ALM_ON,
    MSG_PC_AUDIO_VOL,

    MSG_PP_KEY_PRESS,
    MSG_UP_KEY_PRESS,
    MSG_DOWN_KEY_PRESS,

    MSG_PP_KEY_LIFT,
    MSG_UP_KEY_LIFT,
    MSG_DOWN_KEY_LIFT,

    //系统相关消息，库会使用到，不能更改
    MSG_F1A1_FILE_END = 0x800,
    MSG_F1A1_FILE_ERR,
    MSG_F1A1_LOOP,
    MSG_F1A2_FILE_END,
    MSG_F1A2_FILE_ERR,
    MSG_F1A2_LOOP,
    MSG_MIDI_FILE_END,
    MSG_MIDI_FILE_ERR,
    MSG_A_FILE_END,
    MSG_A_FILE_ERR,
    MSG_A_LOOP,
    MSG_MP3_FILE_END,
    MSG_MP3_FILE_ERR,
    MSG_MP3_LOOP,
    MSG_WAV_FILE_END,
    MSG_WAV_FILE_ERR,
    MSG_WAV_LOOP,
    MSG_WFILE_FULL,

    MSG_OTG_IN,
    MSG_OTG_OUT,
    MSG_PC_IN,
    MSG_PC_OUT,
    MSG_PC_SPK,
    MSG_PC_MIC,

    MSG_ECHO_EFF,
    MSG_EQ_SW,

    MSG_CFG_RX_DATA,
    MSG_COMMON_MAX,//common最大消息
    NO_MSG = 0x0fff,
};

enum {
    MSG_NO_ERROR = 0,
    MSG_NO_MSG = 0,
    MSG_EVENT_EXIST = -1,
    MSG_NOT_EVENT = -2,
    MSG_EVENT_PARAM_ERROR = -3,
    MSG_BUF_NOT_ENOUGH = -4,
    MSG_CBUF_ERROR = -5,
};

//DEV_ONLINE

#define MSG_HEADER_BYTE_LEN     2
#define MSG_HEADER_BIT_LEN     (MSG_HEADER_BYTE_LEN*8)
#define MSG_HEADER_ALL_BIT     ((1L<<MSG_HEADER_BIT_LEN) - 1)

#define MSG_TYPE_BIT_LEN        12
#define MSG_PARAM_BIT_LEN       (MSG_HEADER_BYTE_LEN*8-MSG_TYPE_BIT_LEN)

#define MAX_POOL			128

#define NO_EVENT			0xffff

#define EVENT_F1A1_END      0
#define EVENT_F1A1_ERR      1
#define EVENT_F1A1_LOOP     2
#define EVENT_F1A2_END      3
#define EVENT_F1A2_ERR      4
#define EVENT_F1A2_LOOP    5

#define EVENT_MIDI_END      6
#define EVENT_MIDI_ERR      7
//                        --8
#define EVENT_A_END         9
#define EVENT_A_ERR         10
#define EVENT_A_LOOP        11

#define EVENT_MP3_END       12
#define EVENT_MP3_ERR       13
#define EVENT_MP3_LOOP      14

#define EVENT_WAV_END       15
#define EVENT_WAV_ERR       16
#define EVENT_WAV_LOOP      17

#define EVENT_APP_SW_ACTIVE	18
#define EVENT_WFILE_FULL    19

#define         EVENT_OTG_IN   20
#define         EVENT_OTG_OUT  21
#define         EVENT_UDISK_IN 22
#define         EVENT_UDISK_OUT    23
#define         EVENT_PC_IN    24
#define         EVENT_PC_OUT   25
#define         EVENT_PC_SPK   26
#define         EVENT_PC_MIC   27
#define 		EVENT_SD0_IN	   28
#define 		EVENT_SD0_OUT   29
#define			EVENT_AUX_IN   30
#define         EVENT_AUX_OUT  31
#define 		EVENT_EXTFLSH_IN	32

#define EVENT_HALF_SECOND	31


bool get_event_status(u32 event);
void clear_one_event(u32 event);
int get_msg(int len, int *msg);
int post_event(int event);
int post_msg(int argc, ...);
void clear_all_message(void);
void message_init();


#endif



