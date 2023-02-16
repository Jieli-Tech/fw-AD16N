#ifndef  __DEVICE_APP_H__
#define  __DEVICE_APP_H__

#include "typedef.h"
#include "app_config.h"


void *device_open(u32 index);
u32 device_close(u32 index);
void *device_obj(u32 index);
u32 device_online(void);
u32 device_status(u32 index, bool mode);
void device_update(u8 update_dev);


enum {
    UDISK_INDEX = 0,
    SD0_INDEX,
#ifdef D_SFC_DEVICE_EN
    INNER_FLASH_RO,
    INNER_FLASH_RW,
#endif
#if TFG_EXT_FLASH_EN
    EXT_FLASH_RW,
#endif
    MAX_DEVICE,

    NO_DEVICE = 0xff,
};

#endif
