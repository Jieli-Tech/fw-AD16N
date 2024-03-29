#pragma bss_seg(".app.data.bss")
#pragma data_seg(".app.data")
#pragma const_seg(".app.text.const")
#pragma code_seg(".app.text")
#pragma str_literal_override(".app.text.const")

#include "config.h"
#include "common.h"
#include "app.h"
#include "msg.h"
#include "sys_memory.h"

#include "music_play.h"
#include "usb_slave_mode.h"
#include "linein_mode.h"
#include "record_mode.h"
#include "simple_decode.h"
#include "midi_dec_mode.h"
#include "midi_keyboard_mode.h"
#include "loudspk_mode.h"
#include "rtc_mode.h"
#include "softoff_mode.h"

#include "jiffies.h"
#include "ui_api.h"
#include "key.h"
#include "audio_dac_api.h"
#include "usb_audio_interface.h"
#include "device.h"
#include "otg.h"
#include "charge.h"
#include "pa_mute.h"
#include "device_app.h"
#include "saradc.h"
#include "usb/host/usb_host.h"
#include "usb/device/usb_stack.h"
#include "usb/otg.h"
#include "wdt.h"
#include "bsp_loop.h"

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[mbox_app]"
#include "log.h"

u8 work_mode;
#if KEY_IR_EN
bool Sys_IRInput;
u16 Input_Number;
#endif

AT(.tick_timer.text.cache.L2)
void tick_timer_ram_loop(void)
{
#if LED_5X7
    LED5X7_scan();
#endif
}

#if (FLASH_CACHE_ENABLE == 1)
#if TFG_EXT_FLASH_EN
extern u8 get_flash_cache_timer(void);
#endif
#endif
extern u8 tick_cnt;
void app_timer_loop(void)
{
    saradc_scan();

#if (TCFG_PC_ENABLE || TCFG_UDISK_ENABLE)
    if (0 == (tick_cnt % 25)) {
        void usb_hotplug_detect_plus(void *argv);
        usb_hotplug_detect_plus(NULL);
    }
#endif
#if (FLASH_CACHE_ENABLE == 1)
#if TFG_EXT_FLASH_EN
    if (get_flash_cache_timer()) {
        if (0 == (tick_cnt % 50)) {
            bsp_post_event(B_EVENT_100MS);
            /* putchar('t'); */
        }
    }
#endif
#endif
#if TCFG_PC_ENABLE
    static u16 cnt = 0;
    cnt++;
    if (0 == (cnt % 10)) {
        uac_inc_sync();
    }
    if (cnt >= 500) {
        uac_1s_sync();
        cnt = 0;
    }
#endif
#if TFG_SD_EN
    if (0 == (tick_cnt % 100)) {
        extern void sd0_dev_detect(void *p);
        sd0_dev_detect(NULL);
    }
#endif

#if TCFG_CHARGE_ENABLE
    charge_timer_handle();
#endif
}
u32 get_up_suc_flag();
void mbox_flash_main(void)
{
    log_info("Mbox-Flash App\n");
    if (get_up_suc_flag()) {
        log_info("----device update end----\n");
        wdt_close();
        while (1);
    }

    vm_isr_response_index_register(IRQ_TICKTMR_IDX);//vm擦写flash时响应tick_timer_ram_loop()函数
    delay_10ms(50);//等待系统稳定
    pa_mute(0);

    u8 vol = 0;
    u32 res = sysmem_read_api(SYSMEM_INDEX_VOL, &vol, sizeof(vol));
    if ((vol <= 31) && (res == sizeof(vol))) {
        dac_vol(0, vol);
        log_info("powerup set vol : %d\n", vol);
    }

    sysmem_read_api(SYSMEM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    /* work_mode = MUSIC_MODE; */
    /* work_mode = RECORD_MODE; */
    /* work_mode = AUX_MODE; */
    /* work_mode = MIDI_DEC_MODE; */
    /* work_mode = MIDI_KEYBOARD_MODE; */
    /* work_mode = SIMPLE_DEC_MODE; */
    /* work_mode = LOUDSPEAKER_MODE; */
    /* work_mode = RTC_MODE; */
    while (1) {
        clear_all_message();
        sysmem_pre_erase_api();
        switch (work_mode) {
#if MUSIC_MODE_EN
        case MUSIC_MODE:
            log_info("-Music Mode\n");
            music_app();
            break;
#endif
#if TCFG_PC_ENABLE
        case USB_SLAVE_MODE:
            if (SLAVE_MODE == usb_otg_online(0)) {
                log_info("-PC Mode\n");
                usb_slave_app();
            } else {
                /* work_mode = MUSIC_MODE; */
                work_mode++;
            }
            break;
#endif
#if LINEIN_MODE_EN
        case AUX_MODE:
            log_info("-Linein Mode\n");
            linein_app();
            break;
#endif
#if RECORD_MODE_EN
        case RECORD_MODE:
            log_info("-Record Mode\n");
            record_app();
            break;
#endif
#if DECODER_MIDI_EN
        case MIDI_DEC_MODE:
            log_info("-Midi Dec Mode\n");
            midi_decode_app();
            break;
#endif
#if DECODER_MIDI_KEYBOARD_EN
        case MIDI_KEYBOARD_MODE:
            log_info("-Midi Keyboard Mode\n");
            midi_keyboard_app();
            break;
#endif
#if SIMPLE_DEC_EN
        case SIMPLE_DEC_MODE:
            log_info("-Simple Dec Mode\n");
            simple_decode_app();
            break;
#endif
#if LOUDSPEAKER_EN
        case LOUDSPEAKER_MODE:
            log_info("-Loud Speaker Mode\n");
            loudspeaker_app();
            break;
#endif
#if RTC_EN
        case RTC_MODE:
            log_info("-Rtc Mode\n");
            rtc_app();
            /* rtc_timed_wakeup_app(); */
            break;
#endif

        case SOFTOFF_MODE:
            log_info("-SoftOff Mode\n");
            softoff_app();
            break;
        default:
            work_mode++;
            if (work_mode >= MAX_WORK_MODE) {
                work_mode = MUSIC_MODE;
            }
            break;
        }
    }
}

void app(void)
{
    mbox_flash_main();
}
