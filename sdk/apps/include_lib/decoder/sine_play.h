#ifndef __SINE_PLAY_H__
#define __SINE_PLAY_H__

#include "typedef.h"
#include "tab_read.h"
#include "app_config.h"


typedef struct _sine_obj {
    rtab_obj obj;
    u32      sr;
    u8       enable;
} sine_obj;

void sine_voice_init(void);
void sine_voice_kick(void);
u32 sine_read(void *buff, u32 len);

#if (defined(D_HAS_KEY_VOICE) && D_HAS_KEY_VOICE)
#define d_key_voice_init  sine_voice_init
#define d_key_voice_kick  sine_voice_kick
#define d_key_voice_read  sine_read

#else
#define d_key_voice_init(...)
#define d_key_voice_kick(...)
#define d_key_voice_read(...)

#endif

#endif
