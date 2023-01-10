#ifndef __UAC_SYNC_H__
#define __UAC_SYNC_H__


#include "typedef.h"
#include "src.h"
#include "src_api.h"
#include "audio_dac_api.h"
#include "sound_mge.h"
#include "circular_buf.h"
#include "app_config.h"

#define TCFG_SPK_SRC_ENABLE                 1
#define TCFG_MIC_SRC_ENABLE                 1

typedef struct _uac_sync {
    u32 sync_cnt;
    u32 uac_sync_parm;
    // u32 sr_curr;
    s16 x_step;
    u16 pe_inc_data;
    u16 pe_sub_data;
    u16 last_sr;
    u8 pe5_cnt;
    u8 pe5_step;
    /* u8 pe_cnt; */
    u8 last_pe;
} uac_sync;
void uac_sync_init(uac_sync *sync, s32 sr);

#endif
