#include "msg.h"
#include "common.h"
#include "string.h"
#include "circular_buf.h"
#include <stdarg.h>
#include "config.h"
#include "asm/power_interface.h"
#include "power_api.h"
#include "ui_api.h"
#include "audio_dac.h"
#include "pa_mute.h"
#include "usb/host/usb_host.h"
#include "usb/device/usb_stack.h"
#include "usb/otg.h"
#include "charge.h"
#include "audio.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[idle]"
#include "log.h"

#define IDLE_CHECK_EN       1
#define IDLE_CNT_MAX        2//CNT值不低于2,idle时间=IDLE_CNT_MAX*500ms
static u8 app_idle_cnt;

/*----------------------------------------------------------------------------*/
/**@brief   应用进入power down模式
  @param   is_busy 1:系统繁忙中
                   0:系统空闲
  @note    power_down定时唤醒时，睡眠时间不可超过看门狗唤醒时间的一半
 **/
/*----------------------------------------------------------------------------*/
void app_powerdown_deal(u8 is_busy)
{
#if IDLE_CHECK_EN
    if ((0 == charge_check_is_idle()) || is_busy) {
        app_idle_cnt = 0;
        return;
    }

    app_idle_cnt++;

    if (IDLE_CNT_MAX == app_idle_cnt) {
        app_idle_cnt = 0;
        u32 sr = dac_sr_read();
        pa_mute(1);

        OS_ENTER_CRITICAL();
        UI_init();//关闭数码管
        sys_power_down(-2);//进入powerdown
        OS_EXIT_CRITICAL();

        /* powerdown应用恢复 */
        if (usb_otg_online(0) == HOST_MODE) {
            usb_host_resume(0);
            usb_write_faddr(0, 8);
        }

        dac_power_on(sr);
        pa_mute(0);
    }

#endif
}
