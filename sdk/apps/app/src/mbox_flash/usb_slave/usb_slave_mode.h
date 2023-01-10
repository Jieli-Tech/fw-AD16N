#ifndef __USB_DEVICE_MODE__
#define __USB_DEVICE_MODE__
#include "typedef.h"
#include "app.h"
#include "key.h"

void usb_slave_app(void);
extern u16 usb_slave_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
#endif
