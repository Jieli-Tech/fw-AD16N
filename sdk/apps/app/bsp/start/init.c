
#pragma bss_seg(".init.data.bss")
#pragma data_seg(".init.data")
#pragma const_seg(".init.text.const")
#pragma code_seg(".init.text")
#pragma str_literal_override(".init.text.const")

#include "init.h"
#include "tick_timer_driver.h"
#include "device.h"
#include "vfs.h"
#include "msg.h"
#include "my_malloc.h"
#include "init_app.h"
#include "audio.h"
#include "sys_timer.h"
#include "app_modules.h"
#include "flash_init.h"
#include "update.h"
#if defined(UPDATE_V2_EN) && (1 == UPDATE_V2_EN)
#include "code_v2/update.h"
#endif

#if defined(BLE_EN) && (1 == BLE_EN)
#include "bt_config_tool.h"
#include "user_cfg.h"
#endif

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[init]"
#include "log.h"

void system_init(void)
{
    /* my_malloc_init(); */
#if SYS_TIMER_EN
    /* sys_timer_init(); */
#endif
    tick_timer_init();
    message_init();
    /* app_system_init(); */
    devices_init_api();
    flash_system_init();

#if defined(BLE_EN) && (1 == BLE_EN)
    cfg_bin_init();
    cfg_file_parse(0);
#endif

#if defined(UPDATE_V2_EN) && (1 == UPDATE_V2_EN)
    //升级初始化
    app_update_init();
#endif
}

