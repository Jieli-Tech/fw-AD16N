#ifndef __VBAT_DET_TRIM_H__
#define __VBAT_DET_TRIM_H__

u8 get_vbat_det_level();

void vbat_det_enable();

void vbat_det_disable();

u8 check_vbat_det_trim();

void update_vbat_det_trim_level(u8 level);

#endif
