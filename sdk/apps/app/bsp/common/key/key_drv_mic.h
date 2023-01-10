#ifndef __KEY_DRV_MIC_H__
#define __KEY_DRV_MIC_H__

#include "gpio.h"
#include "key.h"

extern const key_interface_t key_mic_info;
extern void mic_key_enable(u8 en);
extern void mic_key_set_vcmo(u8 vcmo);

void mic_key_init(void);
u8 get_mickey_value(void);
u8 mic_key_check_support_double_click(u8 key_value);

#endif/*__KEY_DRV_IO_H__*/

