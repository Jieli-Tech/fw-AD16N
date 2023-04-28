#pragma bss_seg(".rtc_mode.data.bss")
#pragma data_seg(".rtc_mode.data")
#pragma const_seg(".rtc_mode.text.const")
#pragma code_seg(".rtc_mode.text")
#pragma str_literal_override(".rtc_mode.text.const")

#include "rtc_mode.h"
#include "hot_msg.h"
#include "sys_memory.h"
#include "rtc.h"
#include "cpu.h"
#include "common.h"
#include "msg.h"
#include "audio.h"
#include "ui_api.h"
#include "asm/power/p33_app.h"
#include "power_api.h"
#include "audio_dac.h"
#include "usb/host/usb_host.h"
#include "usb/device/usb_stack.h"
#include "usb/otg.h"
#include "lcd_seg4x8_driver.h"
#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[RTC]"
#include "log.h"

#define RTC_LCD 0
extern const u8 powerdown_lcd_on;
void lcd_seg4x8_wakeupshow(u8 flag);
static void alarm_callback(u8 priv)
{
    log_info("alarm time up!\n");
}

const static struct sys_time clock_time = {  //时钟
    .year = 2021,
    .month = 1,
    .day = 1,
    .hour = 0,
    .min = 0,
    .sec = 0,
};

const static struct sys_time clock_alarm = {  //闹钟
    .year = 2021,
    .month = 1,
    .day = 1,
    .hour = 0,
    .min = 0,
    .sec = 10,
};
const static struct rtc_dev_platform_data rtc_config = {
    .default_sys_time = (struct sys_time *) &clock_time,
    .default_alarm = (struct sys_time *) &clock_alarm,
    .cbfun = alarm_callback,//闹钟中断的回调函数,此回调在中断运行，不可执行时间过长
#if RTC_LCD
    .timefun = lcd_seg4x8_wakeupshow,//时基唤醒中断的回调函数,此回调在中断运行，不可执行时间过长
#else
    .timefun = NULL,
#endif
    .clk_sel = RTC_CLK_SEL,//时钟源选择,只能选择32k时钟或者LRC时钟
};


#if RTC_LCD
struct sys_time new_time;
const struct lcd_seg4x8_platform_data lcd_config = {
    /* .vlcd = LCD_VOLTAGE_3_3V, */
    /* .bias = LCD_BIAS_1_3, */
    .vlcd = LCD_SEG4X8_VOLTAGE_3_0V,
    .bias = LCD_SEG4X8_BIAS_1_3,
    .hd_isel = LCD_SEG4X8_HD_ISEL_200NA,
    .ctu_en = 0, //0:断续推屏(en ie) 1:连续推屏
    .pin_cfg.pin_com[0] = IO_PORTC_05,
    .pin_cfg.pin_com[1] = IO_PORTC_04,
    .pin_cfg.pin_com[2] = IO_PORTC_03,
    .pin_cfg.pin_com[3] = IO_PORTC_02,
    .pin_cfg.pin_seg[0] = IO_PORTA_00,
    .pin_cfg.pin_seg[1] = IO_PORTA_01,
    .pin_cfg.pin_seg[2] = IO_PORTA_02,
    .pin_cfg.pin_seg[3] = IO_PORTA_03,
    .pin_cfg.pin_seg[4] = IO_PORTA_04,
    .pin_cfg.pin_seg[5] = IO_PORTA_05,
    .pin_cfg.pin_seg[6] = IO_PORTA_06,
    .pin_cfg.pin_seg[7] = IO_PORTA_07,
    /* .pin_cfg.pin_seg[8] = IO_PORTA_09, */
};
void lcd_seg4x8_wakeupshow(u8 flag)
{
    read_sys_time(&new_time);
    u16 show_time = new_time.min * 100; //获取分钟
    show_time += new_time.sec;          //获取秒钟
    lcd_seg4x8_show_number(show_time);
}
#endif

void rtc_app(void)
{
    sysmem_write_api(SYSMEM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    key_table_sel(rtc_key_msg_filter);

    rtc_init(&rtc_config);				    //初始化rtc
    read_current_time();

    int msg[2];
    u32 err;
    u8 switch_alarm = 0;
    while (1) {
        err = get_msg(2, &msg[0]);
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }

        switch (msg[0]) {
        case MSG_READ_CLOCK:
            read_current_time();
            break;

        case MSG_WRITE_CLOCK:
            write_clock_time(2023, 3, 1, 16, 28, 12);
            break;

        case MSG_READ_ALARM:
            read_alarm_time();
            break;

        case MSG_WRITE_ALARM:
            write_alarm_time(2023, 3, 1, 16, 28, 17);
            break;

        case MSG_TIME_WAKEUP:
            log_info("timer  wakeup");
            time_wakeup_set(RTC_WKUP_SRC_1HZ, 1); //使能时钟1s定时唤醒
            break;

        case MSG_ALARM:
            if (switch_alarm == 0) {
                log_info("close alram");
                switch_alarm = 1;
                set_alarm_ctrl(0);      //关闭闹钟
            } else {
                log_info("open alram");
                switch_alarm = 0;
                set_alarm_ctrl(1);      //开启闹钟
            }
            break;
        case MSG_CHANGE_WORK_MODE:
            goto __rtc_app_exit;
        case MSG_500MS:
            UI_menu(MENU_MAIN);
            wdt_clear();
            break;
        default:
            ap_handle_hotkey(msg[0]);
            break;
        }
    }
__rtc_app_exit:
    /* rtc_disable(); */
    key_table_sel(NULL);
}

static void rtc_powerdown(void)
{
    u32 sr = dac_sr_read();

    OS_ENTER_CRITICAL();
    UI_init();//关闭数码管
#if LCD_4X8_EN
    u8 lcd_ctu_en = 0;
    if (powerdown_lcd_on) {
        if (JL_LCDC->CON0 & BIT(0)) {         //lcd_en
            /* LCD推屏模式断续变成连续 */
            if (!(JL_LCDC->CON1 & BIT(1))) {  //ctu_en
                lcd_ctu_en = 1;
                JL_LCDC->CON1 &= ~BIT(2);	  //lcd_ie
                JL_LCDC->CON1 |= BIT(1);	  //ctu_en
            }
        }
    }
#endif
    sys_power_down(-2);//进入powerdown
#if LCD_4X8_EN
    if (powerdown_lcd_on) {
        if (lcd_ctu_en) {
            JL_LCDC->CON1 |= BIT(2);
            JL_LCDC->CON1 &= ~BIT(1);
            lcd_ctu_en = 0;
        }
    }
#endif
    OS_EXIT_CRITICAL();

    /* powerdown应用恢复 */
    if (usb_otg_online(0) == HOST_MODE) {
        usb_host_resume(0);
        usb_write_faddr(0, 8);
    }

    dac_power_on(sr);
}

void rtc_timed_wakeup_app()
{
    sysmem_write_api(SYSMEM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    key_table_sel(rtc_key_msg_filter);
    rtc_init(&rtc_config);				    //初始化rtc
#if RTC_LCD
    lcd_seg4x8_init(&lcd_config);
    memset(&new_time, 0, sizeof(new_time));
    lcd_seg4x8_show_number(8888);
#endif
    time_wakeup_set(RTC_WKUP_SRC_1HZ, 1); //使能时钟1s定时唤醒
    int msg[2];
    u32 err;
    log_info("rtc_timed_wakeup_app------------");
    while (1) {
        err = get_msg(2, &msg[0]);
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }

        switch (msg[0]) {
        case MSG_CHANGE_WORK_MODE:
            goto __rtc_timed_wakeup_app_exit;
        default:
            UI_menu(MENU_MAIN);
            ap_handle_hotkey(msg[0]);
            rtc_powerdown();
            read_current_time();
            break;

        }
    }
__rtc_timed_wakeup_app_exit:
    log_info("rtc out ");
    time_wakeup_set(RTC_WKUP_SRC_1HZ, 0); //失能时钟1s定时唤醒
    /* rtc_disable(); */
    key_table_sel(NULL);
}
