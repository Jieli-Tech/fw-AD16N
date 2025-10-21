#pragma bss_seg(".app.data.bss")
#pragma data_seg(".app.data")
#pragma const_seg(".app.text.const")
#pragma code_seg(".app.text")
#pragma str_literal_override(".app.text.const")

#include "app_modules.h"
#include "app_config.h"
#include "config.h"
#include "common.h"
#include "app.h"
#include "msg.h"
#include "sys_memory.h"
#include "vm_sfc.h"

#include "music_play.h"
#include "usb_slave_mode.h"
#include "linein_mode.h"
#include "record_mode.h"
#include "simple_decode.h"
#include "midi_dec_mode.h"
#include "midi_keyboard_mode.h"
#include "loudspk_mode.h"
#include "rtc_mode.h"
#if RF_RADIO_EN
#include "rf_radio_app.h"
#endif
#include "rc_app.h"
#include "update_app.h"
#include "softoff_mode.h"

#include "jiffies.h"
#include "ui_api.h"
#include "key.h"
#include "audio_dac_api.h"
#include "usb_audio_interface.h"
#include "audio.h"
#include "audio_adc.h"
#include "device.h"
#include "otg.h"
#if TCFG_CHARGE_ENABLE
#include "charge.h"
#endif
#include "pa_mute.h"
#include "device_app.h"
#include "adc_api.h"
#include "usb/host/usb_host.h"
#include "usb/device/usb_stack.h"
#include "usb/otg.h"
#include "wdt.h"
#include "bsp_loop.h"
#include "app_power_mg.h"
#include "sine_play.h"
#include "init_app.h"
#if defined (BLE_EN) && (BLE_EN)
#include "bt_ble.h"
#endif
#if SYS_TIMER_EN
#include "sys_timer.h"
#endif

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
#if defined (BLE_EN) && (BLE_EN)
//蓝牙连接超时处理
u16 ble_tick_cnt = 0;
#endif
void app_timer_loop(void)
{
    if ((0 == (tick_cnt % 2))) {
        adc_scan();
    }

#if (TCFG_PC_ENABLE || TCFG_UDISK_ENABLE)
    /* #if defined (BLE_EN) && (BLE_EN) */
#if (0 == (RUN_APP_RC & RUN_APP_DONGLE))
    /* 只有音箱应用需要调用，遥控器和dongle不需要 */
    if (0 == (tick_cnt % 25)) {
        void usb_hotplug_detect(void *argv);
        usb_hotplug_detect(NULL);
    }
#endif
    /* #endif */
#endif
#if SYS_TIMER_EN
    timer_task_scan();
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

#if (0 == RF_REMOTECONTROL_MODE_EN)
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

#if 1//volume memory
    u8 vol = 0;
    u32 res = sysmem_read_api(SYSMEM_INDEX_VOL, &vol, sizeof(vol));
    if ((vol <= 31) && (res == sizeof(vol))) {
        dac_vol(0, vol);
        log_info("powerup set vol : %d\n", vol);
    }
#endif

    sysmem_read_api(SYSMEM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    /* work_mode = MUSIC_MODE; */
    /* work_mode = RECORD_MODE; */
    /* work_mode = AUX_MODE; */
    /* work_mode = MIDI_DEC_MODE; */
    /* work_mode = MIDI_KEYBOARD_MODE; */
    /* work_mode = SIMPLE_DEC_MODE; */
    /* work_mode = REMOTECONTROL_MODE; */
    /* work_mode = LOUDSPEAKER_MODE; */
    /* work_mode = RTC_MODE; */
    /* work_mode = RF_RADIO_MODE; */
    while (1) {
        clear_all_message();
        //切换模式前做预擦除动作
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
#if defined (BLE_EN) && (BLE_EN)
#if RF_RADIO_EN & BLE_EN & TESTE_BLE_EN
        case RF_RADIO_MODE:
            log_info("-RF Radio Mode\n");
            rf_radio_app();
            break;
#endif
#endif
        /* #if RF_REMOTECONTROL_MODE_EN & BLE_EN */
        /*         case REMOTECONTROL_MODE: */
        /* #if TRANS_DATA_HID_EN */
        /*             log_info("-Remotecontrol Mode\n"); */
        /*             rf_controller_app(); */
        /* #elif TRANS_DATA_SPPLE_EN */
        /*             log_info("-Dongle Mode\n"); */
        /*             dongle_app(); */
        /* #endif */
        /*             break; */
        /* #endif */
        case UPDATE_MODE:
            update_app();
            break;
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
void app_custom(void)
{
    log_info("%s() %d\n", __func__, __LINE__);
    app_system_init();
    mbox_flash_main();
}
#endif


#if (RF_REMOTECONTROL_MODE_EN)
#if (TRANS_DATA_HID_EN)
void app_ble_rc_ctrl(void)
{
    log_info("%s() %d\n", __func__, __LINE__);
    /* key */
    adc_init();
    key_init();
    /* d_key_voice_init(); */
    /* power_scan */
    app_power_init();
    auadc_analog_variate_init();
    audio_init();
    rf_rc_app();
}
#endif


#if (TRANS_DATA_SPPLE_EN)
void app_ble_dongle(void)
{
    log_info("%s() %d\n", __func__, __LINE__);
    /* power_scan */
    /* app_power_init(); */
    /* audac_analog_variate_init(); */
    /* audio_init(); */
    /* dac_mode_init(31); */
    /* dac_init_api(SR_DEFAULT); */
    dongle_app();
}
#endif
#endif


void app(void)
{
#if (RUN_APP_RC)
    app_ble_rc_ctrl();
#elif (RUN_APP_DONGLE)

#if (RF_REMOTECONTROL_MODE_EN)
#if (TRANS_DATA_SPPLE_EN)
    app_ble_dongle();
#endif
#endif

#elif (RUN_APP_CUSTOM)
    app_custom();
#else
    log_error("No app running!!!\n");
    while (1) {
        wdt_clear();
    }
#endif
}
