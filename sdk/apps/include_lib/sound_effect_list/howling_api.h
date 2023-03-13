#ifndef __HOWLING_API_H_
#define __HOWLING_API_H_
#if NOTCH_HOWLING_EN
#include "notch_howling_api.h"
void *notch_howling_api(void *obuf, u32 sr, void **ppsound);
void *notch_howling_phy(void *obuf, NotchHowlingParam *parm, void **ppsound);
#endif
#if PITCHSHIFT_HOWLING_EN
#include "howling_pitchshifter_api.h"
void *pitchshift_howling_api(void *obuf, u32 sr, void **ppsound);
void *howling_phy(void *obuf, void *dbuf, HOWLING_PITCHSHIFT_PARM *parm, u32 sr, void **ppsound);
#endif
#endif
