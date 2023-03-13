
#pragma bss_seg(".dev_app.data.bss")
#pragma data_seg(".dev_app.data")
#pragma const_seg(".dev_app.text.const")
#pragma code_seg(".dev_app.text")
#pragma str_literal_override(".dev_app.text.const")

#include "config.h"
#include "common.h"
#include "app_config.h"
#include "device.h"
#include "device_app.h"
#include "errno-base.h"
#include "update/update.h"

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[device_app]"
#include "log.h"


static const char device_name[MAX_DEVICE][9] = {
    {"udisk0"},
    {"sd0"},
#ifdef D_SFC_DEVICE_EN
    {"sfc"},
    {"sfc"},
#endif
#if TFG_EXT_FLASH_EN
    {"ext_flsh"},
#endif
};
static void *p_device[MAX_DEVICE];

void *device_open(u32 index)
{
    if (index >= MAX_DEVICE) {
        return NULL;
    }

#ifdef D_SFC_DEVICE_EN
    if (INNER_FLASH_RO == index) {
        return NULL;
    }
#endif
    if (NULL == p_device[index]) {
        p_device[index] = dev_open((void *)&device_name[index][0], 0);
    }

    return p_device[index];
}

u32 device_close(u32 index)
{
    void *p_curr_device = NULL;
    if (index > MAX_DEVICE) {
        log_info("illegality device index\n");
        return E_IDEV_ILL;
    }
#ifdef D_SFC_DEVICE_EN
    if (INNER_FLASH_RO == index) {
        return 0;
    }
#endif

    p_curr_device = p_device[index];

    u32 res = 0;
    if (NULL != p_curr_device) {
        log_info("device:%d need close\n", index);
        u32 retry = 20;
        do {
            res = dev_close(p_curr_device);
            if (0 == res) {
                p_device[index] = NULL;
            }
            retry--;
        } while (res && (0 != retry));
    } else {
        log_info("device:%d had been close!!", index);
    }
    if (0 != res) {
        log_info("close_device FAIL\n");
        return E_PDEV_FAIL;
    }
    log_info("close_device:%d SUCC\n", index);
    return 0;
}
void *device_obj(u32 index)
{
    if (index > MAX_DEVICE) {
        return 0;
    }
#ifdef D_SFC_DEVICE_EN
    if (INNER_FLASH_RO == index) {
        return NULL;
    }
#endif
    return p_device[index];

}
u32 device_online(void)
{
    u32 online = 0;
    for (u32 i = 0; i < MAX_DEVICE; i++) {
#ifdef D_SFC_DEVICE_EN
        if (INNER_FLASH_RO == i) {
            online |= BIT(i);
            continue;
        }
#endif
        if (E_DEV_OFFLINE != device_status(i, 1)) {
            online |= BIT(i);
        }
    }
    return online;
}
u32 device_status(u32 index, bool mode)
{
    if (index > MAX_DEVICE) {
        return E_IDEV_ILL;
    }
#ifdef D_SFC_DEVICE_EN
    if (INNER_FLASH_RO == index) {
        return 0;
    }
#endif
    bool lost = 0;
    if (!dev_online((void *)&device_name[index][0])) {
        log_info("Ask device:%d is't online ", index);
        if (0 != mode) {
            device_close(index);
            lost = 1;
        }
    } else {
        log_info("device:%d status is ok\n", index);
        return 0;
    }
    bool bres = dev_online(&device_name[index][0]);
    if (bres) {
        if (lost) {
            return E_DEV_LOST;
        } else {
            return 0;
        }
    } else {
        return E_DEV_OFFLINE;
    }
}

#if defined(TFG_DEV_UPGRADE_SUPPORT) && (1 == TFG_DEV_UPGRADE_SUPPORT)
void device_update(u8 update_dev)
{
    try_to_upgrade((char *)device_name[update_dev], TFG_UPGRADE_FILE_NAME);
}
#endif
