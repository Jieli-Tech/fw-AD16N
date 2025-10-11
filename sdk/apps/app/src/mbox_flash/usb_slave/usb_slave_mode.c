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

#include "usb/usb_config.h"
#include "usb/device/hid.h"
#include "usb/device/msd.h"

#include "audio_dac_api.h"
#include "app_config.h"
#include "stream_frame.h"

#include "clock.h"

#if TCFG_CFG_TOOL_ENABLE
#include "cfg_tools.h"
#endif

#if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN)
#include "audio_eq.h"
#include "effects_adj.h"
#endif

#include "usb_audio_interface.h"

#if TCFG_PC_ENABLE

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[pc]"
#include "log.h"

#if (USB_DEVICE_CLASS_CONFIG & HID_CLASS)
static void hid_key_api(u16 key, u8 flag)
{
    u8 key_buf[1 + sizeof(key)];
    memset(&key_buf[0], 0, sizeof(key_buf));

    key_buf[0] = usb_get_hidkey_report_id();//配置report id
    memcpy(&key_buf[1], &key, sizeof(key)); //配置hidkey键值
    usb_write_hid_key(key_buf, sizeof(key_buf));
    if (flag) {
        memset(&key_buf[1], 0, sizeof(key));//抬起hidkey
        usb_write_hid_key(key_buf, sizeof(key_buf));
    }
}
#endif

extern void USB_MassStorage(const struct usb_device_t *usb_device);
void usb_slave_app(void)
{
    int sys_clk = clk_get("sys");   //备份系统时钟
    clk_set("sys", 120000000);
    u32 dac_sr = dac_sr_read();     //备份dac采样率
    u8 temp_vol = dac_vol('r', 0);  //备份音量
    key_table_sel(usb_slave_key_msg_filter);

    /* #if TCFG_CFG_TOOL_ENABLE */

    stream_frame_init(IRQ_STREAM_IP);
    SET_UI_MAIN(MENU_PC_MAIN);
    UI_menu(MENU_PC_MAIN, 0);

    usb_device_mode(0, 0);
#if TCFG_USB_EXFLASH_UDISK_ENABLE
    void *device = dev_open("ext_flsh", 0);
    if (device != NULL) {
        dev_ioctl(device, IOCTL_SET_READ_USE_CACHE, 1);
        dev_ioctl(device, IOCTL_SET_CACHE_SYNC_ISR_EN, 0);
    } else {
        log_info("dev open err\n");
    }
#endif
    usb_start();

    int msg[2];
    u32 err;
    while (1) {
#if (USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS)
        USB_MassStorage(NULL);
#endif
        err = get_msg(2, &msg[0]);
        bsp_loop();
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

#if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN)
        case MSG_HW_EQ_SW:
            eq_mode_sw(get_usb_eq_handle());
            break;
#endif
            /* #if TCFG_CFG_TOOL_ENABLE */
#if (USB_DEVICE_CLASS_CONFIG & CDC_CLASS) && TCFG_CFG_TOOL_ENABLE
        case MSG_CFG_RX_DATA:
            cfg_tool_rx_data();
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
            UI_menu(MENU_PC_VOL_DOWN, 0);
            break;
        case MSG_VOL_UP:
            log_info("UP\n");
            hid_key_api(USB_AUDIO_VOLUP, 1);
            UI_menu(MENU_PC_VOL_UP, 0);
            break;
        case MSG_MUTE_UNMUTE:
            log_info("MU\n");
            hid_key_api(USB_AUDIO_MUTE, 1);
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
            goto __out_t_usb_slave;
        case MSG_CHANGE_WORK_MODE:
            usb_pause();
            goto __out_t_usb_slave;
        case MSG_500MS:
            UI_menu(MENU_MAIN, 0);
        /* UI_menu(MENU_HALF_SEC_REFRESH); */
        default:
            ap_handle_hotkey(msg[0]);
            break;
        }
    }
__out_t_usb_slave:
#if TCFG_USB_EXFLASH_UDISK_ENABLE
    if (device != NULL) {
        dev_close(device);
    }
#endif
    dac_vol(0, temp_vol);   //还原音量
    dac_sr_api(dac_sr);     //还原dac采样率
    clk_set("sys", sys_clk);//还原系统时钟
    SET_UI_MAIN(MENU_POWER_UP);
    UI_menu(MENU_POWER_UP, 0);
    key_table_sel(NULL);
    stream_frame_uninit();
}
#endif
