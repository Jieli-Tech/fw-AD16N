#ifndef __PLAY_FILE_H__
#define __PLAY_FILE_H__
#include "typedef.h"
#include "decoder_api.h"
#include "music_play.h"

#define  BIT_FDEC_EQ        BIT(1)
#define  BIT_FDEC_DP        BIT(0)

dec_obj *decoder_by_sclust(void *pvfs, play_control *ppctl);
dec_obj *decoder_by_index(void *pvfs, play_control *ppctl);
bool music_play_control(MBOX_MUSIC_CMD cmd, u32 index, DEC_STOP_WAIT mode);
// bool mbox_simple_play_file(u8 dindex, u32 findex);
bool mbox_simple_play_file(play_control *ppctl);
#endif
