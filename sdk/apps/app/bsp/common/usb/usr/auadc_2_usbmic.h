#ifndef __AUADC_2_USBMIC_H__
#define __AUADC_2_USBMIC_H__
#include "typedef.h"
#include "sound_mge.h"
#include "usb/usr/usb_mic_interface.h"

sound_out_obj *auadc_mic_open(uac_mic_read *p_uac_read);
void auadc_mic_close(void **ppsrc);
#endif
