#ifndef __MUSIC_PLAY_H__
#define __MUSIC_PLAY_H__
#include "typedef.h"
#include "decoder_api.h"
#include "key.h"
#include "app.h"
#include "simple_play_file.h"

#define RANDOM_PLAY_EN      //是否支持随机播放功能
#define FOLDER_PLAY_EN      //是否支持文件夹播放功能,代码消耗约1.1k

typedef enum {
    DEV_CMD_NULL = 0,
    DEV_CMD_SEL_NEW_DEV,
    DEV_CMD_PREV,
    DEV_CMD_NEXT,
    DEV_CMD_AUTO_PREV,//用于设备内部切换,外部不可调用
    DEV_CMD_AUTO_NEXT,//用于设备内部切换,外部不可调用

    FILE_CMD_PLAY_BY_INDEX = 0x80,
    FILE_CMD_PREV,
    FILE_CMD_NEXT,
    FILE_CMD_AUTO_PREV,
    FILE_CMD_AUTO_NEXT,
} MBOX_MUSIC_CMD;

typedef enum {
    SEL_FIRST_FILE = 1,
    SEL_LAST_FILE,
} SEL_FILE_MODE;

typedef enum {
    REPEAT_ALL = 0,
#ifdef FOLDER_PLAY_EN
    REPEAT_FOLDER,
#endif
    REPEAT_ONE,
#ifdef RANDOM_PLAY_EN
    REPEAT_RANDOM,
#endif
    MAX_PLAY_MODE,
} FILE_PLAY_MODE;

extern play_control pctl[1];
extern u8 err_device;
extern u8 last_dev;
void music_app(void);
extern u16 music_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
#endif

