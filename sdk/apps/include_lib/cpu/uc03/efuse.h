#ifndef  __EFUSE_H__
#define  __EFUSE_H__

#include "cpu.h"

bool is_boot_otp();
u32 efuse_get_vbat_trim_420(void);
u32 efuse_get_vbat_trim_435(void);
u32 efuse_get_charge_cur_trim(void);
u32 efuse_get_lighting_mode(void);
u32 efuse_get_lighting_2_typec(void);
u32 efuse_get_audio_rdac_trim(void);
u32 efuse_get_audio_vbg_trim(void);

u32 efuse_get_lrc_pll_nr();
u32 efuse_get_wvdd_trim();

#endif  /* EFUSE_H */
