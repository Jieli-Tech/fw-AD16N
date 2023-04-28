#ifndef __USB_AUDIO_INTERFACE_H__
#define __USB_AUDIO_INTERFACE_H__


#include "typedef.h"
#include "src.h"
#include "src_api.h"
#include "audio_dac_api.h"
#include "sound_mge.h"
#include "circular_buf.h"
#include "app_config.h"
//#include "audio_adc.h"

void uac_1s_sync(void);
void uac_inc_sync(void);
void usb_slave_sound_close(sound_out_obj *p_sound);
void usb_slave_sound_open(sound_out_obj *p_sound, u32 sr);
void usb_slave_sound_write(const u8 *obuf, u32 len);

void *get_usb_eq_handle(void);
EFFECT_OBJ *uac_spk_percent(u32 *p_percent);
#if  0//TCFG_PC_ENABLE

#include "usb_common_def.h"
#if USB_DEVICE_CLASS_CONFIG & MIC_CLASS
extern cbuffer_t cbuf_usb_mic_o;
void usb_mic_init(void);
void usb_mic_uninit(void);
#endif
#endif

#endif
