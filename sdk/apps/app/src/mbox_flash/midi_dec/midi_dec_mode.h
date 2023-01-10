#ifndef __MIDI_DEC_MODE_H__
#define __MIDI_DEC_MODE_H__

#include "typedef.h"
#include "key.h"

void midi_decode_app(void);
static int midi_decode_set_mode(u8 midi_mode);
static int midi_decode_okon_goon(void);
extern u16 midi_dec_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
#endif
