#ifndef __USB_MIC_INTERFACE_H__
#define __USB_MIC_INTERFACE_H__


#include "typedef.h"
#include "src.h"
#include "src_api.h"
#include "audio_adc_api.h"
#include "sound_mge.h"
#include "circular_buf.h"
#include "app_config.h"
//#include "audio_adc.h"

u32 usb_slave_mic_open(u32 sr, u32 frame_len, u32 ch);
void usb_slave_mic_close(void);
int usb_slave_mic_read(u8 *buf, u32 len);
EFFECT_OBJ *uac_mic_percent(u32 *p_percent);

#endif
