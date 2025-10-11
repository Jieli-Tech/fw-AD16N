#ifndef __CUSTOM_HID_H__
#define __CUSTOM_HID_H__

#include "typedef.h"
#include "usb.h"

extern struct hid_device_var_t _custom_hid_var;

void custom_hid_init(void);
u32 custom_hid_tx_data(const usb_dev usb_id, const u8 *buffer, u32 len);
int custom_hid_get_ready(const usb_dev usb_id);
void custom_hid_set_rx_hook(void *priv, void (*rx_hook)(void *priv, u8 *buf, u32 len));
u32 custom_hid_desc_config(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num);
void custom_hid_register(usb_dev usb_id, void *p);
void custom_hid_release();

#endif

