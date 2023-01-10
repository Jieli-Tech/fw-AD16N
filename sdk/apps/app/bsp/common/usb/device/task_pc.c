#include "app_config.h"
#include "msg.h"
#include "otg.h"

#if TCFG_PC_ENABLE
#include "usb/usb_config.h"
#include "usb/device/usb_stack.h"
#if USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS
#include "usb/device/msd.h"
#endif
#if USB_DEVICE_CLASS_CONFIG & HID_CLASS
#include "usb/device/hid.h"
#endif
#if USB_DEVICE_CLASS_CONFIG & AUDIO_CLASS
#include "usb/device/uac_audio.h"
#endif

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[TASK_PC]"
#include "log.h"

void usb_start()
{
    log_info("USB Start");
    usb_device_mode(0, USB_DEVICE_CLASS_CONFIG);
#if (USB_DEVICE_CLASS_CONFIG & HID_CLASS)
    hid_init();
#endif
#if USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS
    msd_init();
#endif
#if USB_DEVICE_CLASS_CONFIG & AUDIO_CLASS
    uac_init();
#endif
}

void usb_pause()
{
    log_info("USB Pause");
    usb_sie_disable(0);
#if USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS
    msd_unregister_all();
#endif
    usb_device_mode(0, 0);
}

void usb_stop()
{
    log_info("USB Stop");
    usb_pause();
    if (usb_otg_online(0) == DISCONN_MODE) {
        usb_sie_close(0);
    }
}

#endif
