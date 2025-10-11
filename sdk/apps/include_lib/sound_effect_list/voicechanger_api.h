#ifndef __VOICECHANGER_API_H__
#define __VOICECHANGER_API_H__
#include "app_modules.h"
#include "voiceChanger_av_api.h"

void *link_voice_changer_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 in_sr);
void *voice_changer_api(void *obuf, u32 sr, void **ppsound);
int voice_changer_run(void *hld, short *inbuf, int len);
void *voice_changer_phy(void *obuf, u32 sr, VOICECHANGER_AV_PARM *pvc_parm, VOICESYN_AV_PARM *vsyn_ctrol, void **ppsound);

#endif
