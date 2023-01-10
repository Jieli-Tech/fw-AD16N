#ifndef __SRC_API_H__
#define __SRC_API_H__
#include "typedef.h"
#include "src_para.h"
// #include "sound_effect_api.h"
// void *speed_api(void *obuf, u32 insample);
//void *speed_api(void *obuf,u32 insample, u32 outsample);
void src_mode_init(void);
void src_reless(void **pp_eobj);
// void *src_api(void *obuf, u32 insample, u32 outsample, void **ppsound);
void *src_api(void *obuf, SRC_PARA_API *p_src_para, void **ppsound);
void *link_src_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 insample, u32 outsample, u8 ch);
void *src_hld_malloc();

#endif

