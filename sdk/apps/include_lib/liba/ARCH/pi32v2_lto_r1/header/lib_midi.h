#ifndef __LIB_MIDI_H__
#define __LIB_MIDI_H__

#include "app_modules.h"
// midi解码
#define MIDI_DEC_2_BYTE_NORMARK_DBUF_SIZE   (4448)
#define MIDI_DEC_4_BYTE_NORMARK_DBUF_SIZE   (5232)
#define MIDI_DEC_DBUF_SIZE (5304)
#define MIDI_DEC_2_BYTE_POS_TYPE   unsigned short *
#define MIDI_DEC_4_BYTE_POS_TYPE   unsigned int

// midi琴
#define MIDI_DEC_2_BYTE_DBUF_SIZE   (4576)
#define MIDI_DEC_4_BYTE_DBUF_SIZE   (5240)
#define MIDI_CTRL_CHANNEL           (1)
#define MIDI_CTRL_2_BYTE_POS_TYPE   unsigned short *
#define MIDI_CTRL_4_BYTE_POS_TYPE   unsigned int

#if defined(MIDI_VER_4BYTE) && (MIDI_VER_SELECT == MIDI_VER_4BYTE)
// midi解码
#define MIDI_DEC_TRACK              (B_STEREO)  //双声道
#define MIDI_DEC_NOMARK_NEED_BUF_SIZE   (MIDI_DEC_4_BYTE_NORMARK_DBUF_SIZE)
#define MIDI_DEC_POS_TYPE           MIDI_DEC_4_BYTE_POS_TYPE

// midi琴
#define MIDI_CTRL_TRACK             (B_STEREO)
#define MIDI_CTRL_POS_TYPE          MIDI_CTRL_4_BYTE_POS_TYPE
#define MIDI_CTRL_DBUF_SIZE         (MIDI_DEC_4_BYTE_DBUF_SIZE)



#else //2byte
// midi解码
#define MIDI_DEC_TRACK              (0)   //单声道
#define MIDI_DEC_NOMARK_NEED_BUF_SIZE   (MIDI_DEC_2_BYTE_NORMARK_DBUF_SIZE)
#define MIDI_DEC_POS_TYPE           MIDI_DEC_2_BYTE_POS_TYPE

// midi琴
#define MIDI_CTRL_TRACK             (0)
#define MIDI_CTRL_POS_TYPE          MIDI_CTRL_2_BYTE_POS_TYPE
#define MIDI_CTRL_DBUF_SIZE         (MIDI_DEC_2_BYTE_DBUF_SIZE)

#endif

#endif
