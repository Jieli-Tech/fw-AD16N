#ifndef __LOUDSPK_MODE_H__
#define __LOUDSPK_MODE_H__
#include "typedef.h"
#include "app.h"
#include "key.h"

void audio_adc_speaker_start(u32 sr);
void audio_adc_speaker_reless(void);
void loudspeaker_app(void);
extern u16 loudspk_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
#endif
