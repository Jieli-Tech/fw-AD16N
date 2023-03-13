#ifndef echo_api_h__
#define echo_api_h__

#include "typedef.h"
#include "reverb_api.h"

typedef struct _EHCO_PARM_ {
    ECHO_PARM_SET echo_parm_obj;  //参数
    EF_REVERB_FIX_PARM echo_fix_parm;
    void *ptr;
    u32 ptr_len;
} ECHO_PARM;

void *echo_phy(void *obuf, ECHO_PARM *parm, void **ppsound);
void echo_parm_update(ECHO_PARM_SET *parm);

void *link_echo_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr);

void update_echo_parm(void *priv, ECHO_PARM_SET *new_echo_parm);

void update_echo_fix_parm(void *priv, EF_REVERB_FIX_PARM *new_echo_fix_parm);

#endif // reverb_api_h__

