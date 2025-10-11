
#pragma bss_seg(".dev_lst.data.bss")
#pragma data_seg(".dev_lst.data")
#pragma const_seg(".dev_lst.text.const")
#pragma code_seg(".dev_lst.text")
#pragma str_literal_override(".dev_lst.text.const")

#include "config.h"
#include "common.h"
#include "app_config.h"
#include "device.h"
/* #include "norflash.h" */
/* #include "sd.h" */
#include "msg.h"
#include "usb/host/usb_storage.h"
#include "usb/otg.h"
#include "usb/usb_config.h"

#define LOG_TAG_CONST       NORM
/* #define LOG_TAG_CONST       OFF */
#define LOG_TAG             "[cpu/device]"
#include "log.h"

// *INDENT-OFF*

int	devices_init_api()
{
#if (TCFG_PC_ENABLE || TCFG_UDISK_ENABLE)
    usb_dev_ops.init = usb_otg_init;
#endif
    set_device_node((struct dev_node *)device_node_begin, (struct dev_node *)device_node_end);

    return devices_init();
}

/* volatile u8 test_flag = 0;//固件三部测试使用 */
int device_status_emit(const char *device_name, const u8 status)
{
    int event = 0;
    log_info("device_name:%s status:%d \n", device_name, status);

    if (!strcmp(device_name, "sd0")) {
        log_info(">>>>>>>sd ");
        if (status) {
            post_event(EVENT_SD0_IN);
            log_noinfo("online \n");
        } else {
            post_event(EVENT_SD0_OUT);
            log_noinfo("offline \n");
        }
    } else if (!strncmp(device_name, "otg:h", 5)) {
        log_info(">>>>>>>udisk ");
        if (status == 1) {
            post_event(EVENT_OTG_IN);
            log_noinfo("online \n");
            /* test_flag = 1; */
        } else {
            post_event(EVENT_OTG_OUT);
            log_noinfo("offline \n");
            /* test_flag = 2; */
        }
    } else if (!strncmp(device_name, "otg:s", 5)) {
        log_info(">>>>>>>pc ");
        if (status) {
            post_event(EVENT_PC_IN);
            log_noinfo("online \n");
        } else {
            post_event(EVENT_PC_OUT);
            log_noinfo("offline \n");
        }
    }
    return 0;
}



