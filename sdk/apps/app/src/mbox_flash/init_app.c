
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
#if TCFG_CHARGE_ENABLE
#include "charge.h"
#endif
#include "audio.h"
#include "audio_dac_api.h"
#include "adc_api.h"
#include "pa_mute.h"
#include "src_api.h"
#include "app_config.h"
#include "flash_init.h"
#include "sine_play.h"
#include "app_power_mg.h"
#include "gpio.h"
#include "power_interface.h"
#include "power_api.h"
#include "audio_adc.h"
#if HAS_NORFS_EN
#include "nor_fs.h"
#endif

#if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN)
#include "effects_adj.h"
#endif

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
    UI_menu(MENU_POWER_UP, 0);


    /* audio */
    audio_variate_init();
    pa_mute(1);
    src_mode_init();
    audio_init();
    dac_mode_init(16);
    dac_init_api(SR_DEFAULT);
    auin_mode_init();

    /* key */
    adc_init();
    key_init();
    d_key_voice_init();

    /* power_scan */
    app_power_init();

#if TCFG_CHARGE_ENABLE
    charge_init();
#endif

    /* flash_system_init(); */

#if HAS_NORFS_EN
    norfs_init_api();
#endif

    /* EQ */
#if defined(AUDIO_HW_EQ_EN) && (AUDIO_HW_EQ_EN)
    effect_eq_parm_init();
#endif
}

static u8 get_power_on_status(void)
{
    u8 on_status = 0;
    gpio_hw_set_dieh(IO_PORT_SPILT(POWER_WAKEUP_IO), 1);
    if (POWER_WAKEUP_EDGE == FALLING_EDGE) {
        gpio_set_mode(IO_PORT_SPILT(POWER_WAKEUP_IO), PORT_INPUT_PULLUP_10K);
        on_status = !(!!gpio_read(POWER_WAKEUP_IO));
    } else if (POWER_WAKEUP_EDGE == RISING_EDGE) {
        gpio_set_mode(IO_PORT_SPILT(POWER_WAKEUP_IO), PORT_INPUT_PULLDOWN_10K);
        on_status = !!gpio_read(POWER_WAKEUP_IO);
    } else {
        on_status = 1;
    }

    return on_status;
}

void check_power_on_key(void)
{
    extern bool is_port_edge_wkup_source(void);
    if (0 == is_port_edge_wkup_source()) {
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
