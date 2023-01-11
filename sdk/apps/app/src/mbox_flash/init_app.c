
#pragma bss_seg(".init_app.data.bss")
#pragma data_seg(".init_app.data")
#pragma const_seg(".init_app.text.const")
#pragma code_seg(".init_app.text")
#pragma str_literal_override(".init_app.text.const")

#include "init.h"
#include "device.h"
#include "vfs.h"
#include "msg.h"
#include "clock.h"
#include "key.h"
#include "ui_api.h"
#include "charge.h"
#include "audio.h"
#include "audio_dac_api.h"
#include "saradc.h"
#include "pa_mute.h"
#include "src_api.h"
#include "app_config.h"
#include "flash_init.h"
#include "sine_play.h"
#include "app_power_mg.h"
#include "gpio.h"
#include "power_api.h"
#include "asm/power_interface.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[init]"
#include "log.h"

void app_system_init(void)
{
    /* 按键唤醒消抖 */
    /* check_power_on_key(); */

    /* UI */
    UI_init();
    SET_UI_MAIN(MENU_POWER_UP);
    UI_menu(MENU_POWER_UP);

    /* device & fs */
    devices_init_api();

    /* dac */
    pa_mute(1);
    dac_mode_init(16);
    dac_init_api(SR_DEFAULT);

    /* key */
    saradc_init();
    key_init();
    d_key_voice_init();

    /* power_scan */
    app_power_init();

    /* audio */
    src_mode_init();
    audio_init();

#if TCFG_CHARGE_ENABLE
    charge_init();
#endif

    flash_system_init();
}

void sd_debug(u32 idx)
{
    if (idx == E_SD_RECEIVE_DATA_TIMEOUT || idx == E_SD_SEND_ACMD41_TIMEOUT) {
        wdt_clear();
    }
    /* if (idx >= E_SD_STATUS) { */
    /*     log_noinfo("sd_status:0x%x\n", idx); */
    /* } else { */
    /*     log_noinfo("sd_info:0x%x\n", idx); */
    /* } */
}

void mask_init_for_app(void)
{
    extern void mask_init(void *exp_hook, void *pchar, void *clk_hook, void *emit_hook);
    mask_init(exception_analyze, putchar, clk_get, device_status_emit);

//--------------------------
#if TFG_SD_EN
    extern void sd_mask_init(u32 idle_cnt_max, void *notify_hook, void *get_buf_hook, void *deal_event_hook, void *user_hookfun_hook, void *debug_hook);
    sd_mask_init(5, NULL, NULL, NULL, NULL, sd_debug);
#endif
}

static u8 get_power_on_status(void)
{
    u8 on_status = 0;
    gpio_set_direction(POWER_WAKEUP_IO, 1);
    gpio_set_die(POWER_WAKEUP_IO, 1);
    gpio_set_dieh(POWER_WAKEUP_IO, 1);
    if (POWER_WAKEUP_EDGE == FALLING_EDGE) {
        gpio_set_pull_up(POWER_WAKEUP_IO, 1);
        gpio_set_pull_down(POWER_WAKEUP_IO, 0);
        on_status = !(!!gpio_read(POWER_WAKEUP_IO));
    } else if (POWER_WAKEUP_EDGE == RISING_EDGE) {
        gpio_set_pull_up(POWER_WAKEUP_IO, 0);
        gpio_set_pull_down(POWER_WAKEUP_IO, 1);
        on_status = !!gpio_read(POWER_WAKEUP_IO);
    } else {
        on_status = 1;
    }

    return on_status;
}

void check_power_on_key(void)
{
    extern u64 get_wkup_source_value(void);
    if (0 == (get_wkup_source_value() & BIT(P3_WKUP_SRC_PORT_EDGE))) {
        printf("not port edge wakeup!\n");
        return;
    }

    u32 delay_10ms_cnt = 0;
    u32 delay_10msp_cnt = 0;

    while (1) {
        wdt_clear();
        mdelay(10);

        if (get_power_on_status()) {
            log_char('+');
            delay_10msp_cnt = 0;
            delay_10ms_cnt++;
            if (delay_10ms_cnt > 10) {
                return;
            }
        } else {
            log_char('-');
            delay_10ms_cnt = 0;
            delay_10msp_cnt++;
            if (delay_10msp_cnt > 10) {
                log_info("enter softpoweroff\n");
                sys_softoff();
            }
        }
    }
}
