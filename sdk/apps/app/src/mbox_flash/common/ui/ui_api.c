
#pragma bss_seg(".led_ui.data.bss")
#pragma data_seg(".led_ui.data")
#pragma const_seg(".led_ui.text.const")
#pragma code_seg(".led_ui.text")
#pragma str_literal_override(".led_ui.text.const")
/*--------------------------------------------------------------------------*/
/**@file    LED_UI_API.c
   @brief   LED 显示界面接口函数
   @details
   @author  bingquan Cai
   @date    2012-8-30
   @note    AC109N
*/
/*----------------------------------------------------------------------------*/

#include "ui_api.h"
#include "ui_common.h"
#include "led5x7_driver.h"
#include "music_play.h"
#include "play_file.h"
#include "msg.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

UI_VAR UI_var;   /*UI 显示变量*/

/*----------------------------------------------------------------------------*/
/**@brief   UI 显示界面处理函数
   @param   menu：需要显示的界面
   @return  无
   @note    void UI_menu_api(u8 menu)
*/
/*----------------------------------------------------------------------------*/
void UI_menu_api(u8 menu)
{
    /*界面属性-非主界面自动返回*/
    if (menu == MENU_MAIN) {
        if (UI_var.bMenuReturnCnt < UI_RETURN) {
            UI_var.bMenuReturnCnt++;
            if (UI_var.bMenuReturnCnt == UI_RETURN) {
                LED5X7_clear_icon();
#if KEY_IR_EN
                if (UI_var.bCurMenu == MENU_INPUT_NUMBER) {
                    post_msg(1, MSG_INPUT_TIMEOUT);    //输入超时
                } else
#endif
                {
                    UI_var.bCurMenu = UI_var.bMainMenu;
                }
            }
        } else {
            /*等待界面不重复刷新界面*/
            if (UI_var.bCurMenu == UI_var.bMainMenu) {
                return;
            }
            UI_var.bCurMenu = UI_var.bMainMenu;
        }
    } else {
        if (menu > 0x80) {  //仅在当前界面为主界面时刷新界面,例如：在主界面刷新播放时间
            if (UI_var.bCurMenu != UI_var.bMainMenu) {
                return;
            }
        } else {
            LED5X7_clear_icon();
            /*非主界面需要启动返回计数器*/
            if (menu != UI_var.bMainMenu) {
                UI_var.bMenuReturnCnt = 0;
            }
            UI_var.bCurMenu = menu;
            /* if (menu != MENU_INPUT_NUMBER) { */
            /*     input_number = 0; */
            /* } */
        }
    }
    LED5X7_setX(0);

    switch (UI_var.bCurMenu) {
    /*-----System Power On UI*/
    case MENU_POWER_UP:
    case MENU_IDLE:
    case MENU_WAIT:
#ifdef USB_DEVICE_EN
    case MENU_PC_MAIN:
    case MENU_PC_VOL_UP:
    case MENU_PC_VOL_DOWN:
#endif
    case MENU_AUX_MAIN:
        LED5X7_show_string_menu(UI_var.bCurMenu);
        break;

    /*-----Common Info UI*/
    case MENU_MAIN_VOL:
        LED5X7_show_volume();
        break;
    /*-----Music Related UI*/
    case MENU_MUSIC_MAIN:
    case MENU_PAUSE:
        LED5X7_show_music_main();
        break;
    case MENU_FILENUM:
        LED5X7_show_filenumber();
        break;
#if AUDIO_EQ_ENABLE
    case MENU_EQ:
        LED5X7_show_eq();
        break;
#endif
#if KEY_IR_EN
    case MENU_INPUT_NUMBER:
        LED5X7_show_IR_number();
        break;
#endif
#if 1
    case MENU_PLAYMODE:
        LED5X7_show_playmode();
        break;
#endif

        /*-----FM Related UI*/
#if 0//FM_ENABLE
    case MENU_FM_MAIN:
    case MENU_FM_DISP_FRE:
        LED5X7_show_fm_main();
        break;
    case MENU_FM_FIND_STATION:
    case MENU_FM_CHANNEL:
        LED5X7_show_fm_station();
        break;
#endif

#if 0//RTC_EN
    case MENU_RTC_MAIN:
        RTC_setting_var.bMode = 0;    //模式与界面同步返回
    case MENU_RTC_SET:
        LED5X7_show_RTC_main();
        break;
#ifdef RTC_ALARM_EN
    case MENU_ALM_SET:
        LED5X7_show_alarm();
        break;
#endif
#endif

    default:
        break;
    }
}

void UI_init_api(void)
{
#if (LED_5X7)
    LED5X7_init();
#endif
}
