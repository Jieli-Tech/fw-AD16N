#ifndef __SOUND_KICK_H__
#define __SOUND_KICK_H__

#include "typedef.h"

void sound_kick(void *sound_hld, void *pkick);

u32 audio_kick_size(sound_out_obj *psound);
void if_kick_decoder(sound_stream_obj *p_stream_in, sound_out_obj *psound, void *pkick);

#endif
