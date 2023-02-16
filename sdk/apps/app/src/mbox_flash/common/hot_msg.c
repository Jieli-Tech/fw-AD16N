#include "hot_msg.h"
#include "audio_dac_api.h"
#include "audio.h"
#include "msg.h"
#include "app.h"
#include "wdt.h"
#include "saradc.h"
#include "charge.h"
#include "power_api.h"
#include "ui_api.h"
#include "tick_timer_driver.h"
#include "vm.h"
#include "app_power_mg.h"

#include "usb/usb_config.h"
#include "usb/device/hid.h"
#include "usb/device/msd.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[hot_msg]"
#include "log.h"

void music_vol_update(void)
{
    u8 vol = 0;
    vm_read(VM_INDEX_VOL, &vol, sizeof(vol));
    if (vol != dac_vol('r', 0)) {
        vol = dac_vol('r', 0);
        vm_write(VM_INDEX_VOL, &vol, sizeof(vol));
        /* log_info("vol update to vm : %d\n", vol); */
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   公共消息集中处理函数
   @param   key： 需要处理的消息
   @return  void
   @author: Juntham
   @note    void ap_handle_hotkey(u8 key)
*/
/*----------------------------------------------------------------------------*/
void ap_handle_hotkey(u16 key)
{
    u8 vol = 0;
    switch (key) {
    case MSG_500MS:
        app_power_scan();
        wdt_clear();
        /* 模拟量的调节,电压不要急升 */
        audio_lookup();
#if TCFG_CHARGE_ENABLE
        charge_set_vbat_voltage(app_power_get_vbat());
#endif
        music_vol_update();
        break;
    //-------------音量调节
    case MSG_VOL_UP:
        vol = dac_vol('+', 255);
        goto __app_vol_deal;
    case MSG_VOL_DOWN:
        vol = dac_vol('-', 255);
__app_vol_deal:
        log_info("VOL:%d \n", vol);
        UI_menu(MENU_MAIN_VOL);
        break;
    //-------------设备上线
    case MSG_OTG_IN:
        if (0 != usb_host_mount(0, 3, 20, 200)) {
            log_info("mount err!\n");
            break;
        }
        post_event(EVENT_UDISK_IN);
        break;
    case MSG_OTG_OUT:
        usb_host_unmount(0);
        post_event(EVENT_UDISK_OUT);
        break;
    case MSG_PC_IN:
        log_info("MSG_PC_IN\n");
        work_mode = USB_SLAVE_MODE;
        post_msg(1, MSG_CHANGE_WORK_MODE);
        break;
    case MSG_PC_OUT:
        log_info("MSG_PC_OUT\n");
        usb_stop();
        break;
#if defined(TFG_DEV_UPGRADE_SUPPORT) && (1 == TFG_DEV_UPGRADE_SUPPORT)
    case MSG_USB_DISK_IN:
    case MSG_SDMMCA_IN:
        if (time_before(maskrom_get_jiffies(), 150)) {
            break;//上电1.5s内不响应设备上线消息
        }
        u8 update_dev = key - MSG_USB_DISK_IN;
        device_update(update_dev);
        break;
#endif

    //-------------模式退出处理
    case MSG_NEXT_WORKMODE:
        log_info("MSG_NEXT_WORKMODE\n");
        work_mode++;
        post_msg(1, MSG_CHANGE_WORK_MODE);
        break;
    case MSG_POWER_OFF:
        log_info("MSG_POWER_OFF\n");
        work_mode = SOFTOFF_MODE;
        post_msg(1, MSG_CHANGE_WORK_MODE);
        break;

#if KEY_IR_EN
    case MSG_0:
    case MSG_1:
    case MSG_2:
    case MSG_3:
    case MSG_4:
    case MSG_5:
    case MSG_6:
    case MSG_7:
    case MSG_8:
    case MSG_9:
        if (!Sys_IRInput) {
            break;
        }
        if (Input_Number > 999) {
            Input_Number = 0;
        }
        Input_Number = Input_Number * 10 + key;
        UI_menu(MENU_INPUT_NUMBER);
        break;
#endif
    }
}
