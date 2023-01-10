#pragma bss_seg(".ttmr_sys.data.bss")
#pragma data_seg(".ttmr_sys.data")
#pragma const_seg(".ttmr_sys.text.const")
#pragma code_seg(".ttmr_sys.text")
#pragma str_literal_override(".ttmr_sys.text.const")

#include "tick_timer_driver.h"
#include "clock.h"
#include "printf.h"
#include "jiffies.h"
#include "msg.h"
#include "key.h"
#include "app_config.h"
#if KEY_IR_EN
#include "irflt.h"
#endif

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[tick]"
#include "log.h"

u8 tick_cnt;
extern void app_timer_loop(void);
/* ticktimer定时任务公共处理函数,tick_cnt只能由该函数改变 */
void tick_timer_loop(void)
{
    tick_cnt ++;

    if (0 == (tick_cnt % 5)) {
        maskrom_update_jiffies();
#if KEY_IR_EN
        ir_timeout();
#endif
        key_scan();
        /* printf("+"); */
    }

    app_timer_loop();

    if (0 == (tick_cnt % 250)) {
        post_msg(1, MSG_500MS);
        tick_cnt = 0;
    }
}

void delay_10ms(u32 tick)
{
    u32 jiff = maskrom_get_jiffies();
    tick = tick + jiff;
    while (1) {
        jiff = maskrom_get_jiffies();
        if (time_after(jiff, tick)) {
            break;
        }
    }
}

void os_time_dly(u32 tick)
{
    delay_10ms(tick);
}
