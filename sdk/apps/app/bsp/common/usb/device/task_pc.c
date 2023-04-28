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
#if USB_DEVICE_CLASS_CONFIG & CDC_CLASS
#include "cfg_tools.h"
#include "usb/device/cdc.h"
#endif

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[TASK_PC]"
#include "log.h"

#if USB_DEVICE_CLASS_CONFIG & CDC_CLASS
#if TCFG_CFG_TOOL_ENABLE && (TCFG_COMM_TYPE == TCFG_USB_COMM)
static u8 buf_rx[256] __attribute__((aligned(32))) AT(.usb_cdc_data);
static u8 rx_len_total;
#endif

static void usb_cdc_wakeup(struct usb_device_t *usb_device)
{
    //回调函数在中断里，正式使用不要在这里加太多东西阻塞中断，
    //或者先post到任务，由任务调用cdc_read_data()读取再执行后续工作
    const usb_dev usb_id = usb_device2id(usb_device);
    u8 buf[64] = {0};
    u32 rlen;

    log_debug("cdc rx hook");
    rlen = cdc_read_data(usb_id, buf, 64);

    /* printf_buf(buf, rlen);//固件三部测试使用 */
    /* cdc_write_data(usb_id, buf, rlen);//固件三部测试使用 */

    /* 负责处理接收工具发来的消息 */
#if TCFG_CFG_TOOL_ENABLE && (TCFG_COMM_TYPE == TCFG_USB_COMM)
    if ((buf[0] == 0x5A) && (buf[1] == 0xAA) && (buf[2] == 0xA5)) {
        memset(buf_rx, 0, 256);
        memcpy(buf_rx, buf, rlen);
        /* log_info("need len = %d\n", buf_rx[5] + 6); */
        /* log_info("rx len = %d\n", rlen); */
        if ((buf_rx[5] + 6) == rlen) {
            rx_len_total = 0;
            /* put_buf(buf_rx, rlen); */
            online_cfg_tool_data_deal(buf_rx, rlen);
        } else {
            rx_len_total += rlen;
        }
    } else {
        if ((rx_len_total + rlen) > 256) {
            memset(buf_rx, 0, 256);
            rx_len_total = 0;
            return;
        }
        memcpy(buf_rx + rx_len_total, buf, rlen);
        /* log_info("need len = %d\n", buf_rx[5] + 6); */
        /* log_info("rx len = %d\n", rx_len_total + rlen); */
        if ((buf_rx[5] + 6) == (rx_len_total + rlen)) {
            /* put_buf(buf_rx, rx_len_total + rlen); */
            online_cfg_tool_data_deal(buf_rx, rx_len_total + rlen);
            rx_len_total = 0;
        } else {
            rx_len_total += rlen;
        }
    }
#endif
}
#endif

void usb_start()
{
    log_info("USB Start");
#if (USB_DEVICE_CLASS_CONFIG & HID_CLASS)
    hid_init();
#endif
#if USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS
    msd_init();
#endif
#if USB_DEVICE_CLASS_CONFIG & AUDIO_CLASS
    uac_init();
#endif
    usb_device_mode(0, USB_DEVICE_CLASS_CONFIG);
#if USB_DEVICE_CLASS_CONFIG & CDC_CLASS
    cdc_set_wakeup_handler(usb_cdc_wakeup);
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
