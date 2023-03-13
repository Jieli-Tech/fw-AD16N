#include "bsp_loop.h"
#include "msg.h"
#include "errno-base.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[bsp loop]"
#include "log.h"

unsigned int bsp_event;

void bsp_event_init(void)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    bsp_event = 0;
    OS_EXIT_CRITICAL();
}

int bsp_post_event(BSP_EVENT be)
{
    int err = 0;
    CPU_SR_ALLOC();
    /* log_info("evenr post : 0x%x\n", event); */
    OS_ENTER_CRITICAL();
    if (0  != (be / 32)) {
        err = E_BSP_EVENT;
    } else {
        bsp_event |= BIT(be);
    }
    OS_EXIT_CRITICAL();
    /* log_info("post_event :0x%x\n", bsp_event); */
    return err;
}
static BSP_EVENT bsp_get_event(void)
{
    u32 i;
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    u32 event_cls;
    BSP_EVENT event = B_NO_EVENT;
    __asm__ volatile("%0 = clz(%1)":"=r"(event_cls):"r"(bsp_event));
    if (event_cls != 32) {
        event = 31 - event_cls;
        bsp_event &= ~BIT(event);
    }
    OS_EXIT_CRITICAL();
    return event;
}

void _norflash_cache_sync_timer(u32 sync_step);
int bsp_loop(void)
{
    u32 event = bsp_get_event();

    switch (event) {
    case B_EVENT_100MS:
#if TFG_EXT_FLASH_EN
        _norflash_cache_sync_timer(5);
#endif
        break;
    default:
        break;
    }
    return 0;
}


