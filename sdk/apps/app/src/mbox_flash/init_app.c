
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

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[init]"
#include "log.h"

void app_system_init(void)
{
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

