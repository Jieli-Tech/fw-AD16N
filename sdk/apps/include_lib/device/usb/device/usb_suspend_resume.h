#ifndef  __USB_SUSPEND_RESUME_H__
#define  __USB_SUSPEND_RESUME_H__

#include "typedef.h"
#include "usb.h"

#define USB_SUSPEND_RESUME      0   //usb从机休眠使能:TODO
#define USB_REMOTE_WAKEUP_EN    0
#if USB_REMOTE_WAKEUP_EN
#undef USB_SUSPEND_RESUME
#define USB_SUSPEND_RESUME      1
#endif

typedef enum {
    USB_SLAVE_RESUME = 0,
    USB_SLAVE_SUSPEND,
} usb_slave_status;

void usb_slave_suspend_resume_init(const usb_dev usb_id);
u8 usb_slave_status_get();
// void usb_slave_status_set(const usb_dev usb_id, usb_slave_status status);
void usb_slave_reset(const usb_dev usb_id);
void usb_slave_suspend(const usb_dev usb_id);
void usb_slave_resume(const usb_dev usb_id);
void usb_remote_wakeup(const usb_dev usb_id);


#endif  /*USB_STACK_H*/
