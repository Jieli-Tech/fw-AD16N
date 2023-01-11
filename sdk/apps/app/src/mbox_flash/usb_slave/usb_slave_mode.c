#pragma bss_seg(".usb_slave_mode.data.bss")
#pragma data_seg(".usb_slave_mode.data")
#pragma const_seg(".usb_slave_mode.text.const")
#pragma code_seg(".usb_slave_mode.text")
#pragma str_literal_override(".usb_slave_mode.text.const")

#include "usb_slave_mode.h"
#include "common.h"
#include "msg.h"
#include "ui_api.h"
#include "hot_msg.h"
#include "stream_frame.h"

#include "usb/usb_config.h"
#include "usb/device/hid.h"
#include "usb/device/msd.h"

#include "audio_dac_api.h"

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[pc]"
#include "log.h"

#if (USB_DEVICE_CLASS_CONFIG & HID_CLASS)
static void hid_key_api(u32 key, u8 flag)
{
    usb_hid_control(key);
    if (flag) {
        usb_hid_control(0);
    }
}
#endif

extern void USB_MassStorage(const struct usb_device_t *usb_device);
void usb_slave_app(void)
{
    u32 dac_sr = dac_sr_read();
    u8 temp_vol = dac_vol('r', 0);
    key_table_sel(usb_slave_key_msg_filter);

    SET_UI_MAIN(MENU_PC_MAIN);
    UI_menu(MENU_PC_MAIN);

    stream_frame_init();
    usb_device_mode(0, 0);
    usb_start();

    int msg[2];
    u32 err;
    while (1) {
#if (USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS)
        USB_MassStorage(NULL);
#endif
        err = get_msg(2, &msg[0]);
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }

        switch (msg[0]) {
#if (USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS)
        case MSG_SDMMCA_IN:
            log_info("MSG_SDMMCA_IN");
            break;
        case MSG_SDMMCA_OUT:
            log_info("MSG_SDMMCA_OUT");
            break;
#endif
#if (USB_DEVICE_CLASS_CONFIG & HID_CLASS)
        case MSG_PREV_FILE:
            log_info("PR\n");
            hid_key_api(USB_AUDIO_PREFILE, 1);
            break;
        case MSG_NEXT_FILE:
            log_info("NE\n");
            hid_key_api(USB_AUDIO_NEXTFILE, 1);
            break;
        case MSG_PP:
            log_info("PP\n");
            hid_key_api(USB_AUDIO_PP, 1);
            break;
        case MSG_VOL_DOWN:
            log_info("DO\n");
            hid_key_api(USB_AUDIO_VOLDOWN, 1);
            UI_menu(MENU_PC_VOL_DOWN);
            break;
        case MSG_VOL_UP:
            log_info("UP\n");
            hid_key_api(USB_AUDIO_VOLUP, 1);
            UI_menu(MENU_PC_VOL_UP);
            break;
#else
        case MSG_VOL_DOWN:
        case MSG_VOL_UP:
            break;
#endif
        case MSG_PC_OUT:
            log_info("MSG_PC_OUT\n");
            usb_stop();
            work_mode++;
        case MSG_CHANGE_WORK_MODE:
            usb_pause();
            goto __out_t_usb_slave;
        case MSG_500MS:
            UI_menu(MENU_MAIN);
        /* UI_menu(MENU_HALF_SEC_REFRESH); */
        default:
            ap_handle_hotkey(msg[0]);
            break;
        }
    }
__out_t_usb_slave:

    dac_vol(0, temp_vol);
    dac_sr_api(dac_sr);
    SET_UI_MAIN(MENU_POWER_UP);
    UI_menu(MENU_POWER_UP);
    key_table_sel(NULL);
}
