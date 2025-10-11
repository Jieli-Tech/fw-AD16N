#ifndef __LIB_WAV_ARCH_H__
#define __LIB_WAV_ARCH_H__
//uc03

#include "if_decoder_ctrl.h"

#define WAV_DBUF_SIZE   0x5a4

#define WAV_TRACK B_STEREO



static AUDIO_DECODE_PARA modevalue AT(.wav_data);

void wav_dconfig(decoder_ops_t *ops, void *dbuf)
{
    modevalue.mode = 1;          //output是否判断返回值
    ops->dec_confing(dbuf, SET_DECODE_MODE, &modevalue);
}

#define WAV_DECODER_CONFIG(n)	wav_dconfig(n)



#endif
