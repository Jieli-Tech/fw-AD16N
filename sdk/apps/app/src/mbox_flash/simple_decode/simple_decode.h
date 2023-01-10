#ifndef __SIMPLE_DECODE_H__
#define __SIMPLE_DECODE_H__

#include "typedef.h"
#include "key.h"
#include "simple_play_file.h"

#define MUSIC_BREAK_POINT_EN    1

void simple_decode_app(void);
extern u16 simple_dec_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
static bool simple_switch_device(play_control *ppctl);
static bool simple_next_dir(play_control *ppctl);
#endif
