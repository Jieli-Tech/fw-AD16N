#ifndef __SONG_SPEED_API_H__
#define __SONG_SPEED_API_H__
#include "typedef.h"
#include "app_modules.h"
#include "ps_for69_api.h"

void *song_speed_api(void *obuf, u16 ch, u32 insample, void **ppsound);
void *link_song_speed_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 in_sr, u8 ch);
void *song_speed_phy(void *obuf, PS69_CONTEXT_CONF *psp_parm, void **ppsound);
void set_song_speed_parm(void *priv, PS69_CONTEXT_CONF *new_ps_parm);
#endif

