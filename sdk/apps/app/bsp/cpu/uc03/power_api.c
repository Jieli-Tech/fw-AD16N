#include "includes.h"
#include "asm/power_interface.h"
#include "power_api.h"
#include "irq.h"
#include "gpio.h"
#include "uart.h"
#include "wdt.h"
#include "charge.h"
#include "app_config.h"
#include "audio.h"

#define LOG_TAG_CONST       PMU
#define LOG_TAG             "[pmu]"
#include "log.h"

#pragma bss_seg(".power_api.data.bss")
#pragma data_seg(".power_api.data")
#pragma const_seg(".power_api.text.const")
#pragma code_seg(".power_api.text")
#pragma str_literal_override(".power_api.text.const")


//===============pmu功能配置====================
#if defined(USE_FLASH_DEBUG) && (0 == USE_FLASH_DEBUG)
//otp方案

//使能pmu otp相关操作
const u8 config_otp_enable = 1;
//使能pmu sfc相关操作
const u8 config_flash_enable = 0;
//使能soft off相关操作
const u8 config_soff_enable = 1;
//使能powerdown相关操作
const u8 config_pdown_enable = 1;
//使能lptmr相关操作, 定时唤醒需要该功能
const u8 config_tmr_enable = 0;
//使能唤醒源记录
const u8 config_record_wkup = 0;
//使能复位源记录
const u8 config_record_reset = 0;
//数字io支持唤醒数量, otp方案可选择1/2，分别对应通道1和2(0分配给长按复位)
const u8 config_max_wakeup_port = 1;

#else
//flash方案

//使能pmu otp相关操作
const u8 config_otp_enable = 1;
//使能pmu sfc相关操作
const u8 config_flash_enable = 1;
//使能soft off相关操作
const u8 config_soff_enable = 1;
//使能powerdown相关操作
const u8 config_pdown_enable = 1;
//使能lptmr相关操作, 定时唤醒需要该功能
const u8 config_tmr_enable = 1;
//使能唤醒源记录
const u8 config_record_wkup = 1;
//使能复位源记录
const u8 config_record_reset = 1;
//数字io支持唤醒数量
const u8 config_max_wakeup_port = 1;
#endif

/**************************************************** power_param *********************************************************/
const struct low_power_param power_param = {
    .config         = SLEEP_EN,          				   //0：sniff时芯片不进入低功耗  1：sniff时芯片进入powerdown
    .btosc_hz       = 12000000,                   	   	   //外接晶振频率
    .vddiom_lev     = TCFG_VDDIOM_LEVEL,       			   //强VDDIO等级,可选：2.1v~3.6v
    .vddiow_lev     = VDDIOW_VOL_28V,          			   //弱VDDIO等级,可选：2.1v~3.6v
    .osc_type       = OSC_TYPE_LRC,						   //低功耗使用晶振
    .vdc13_cap_en   = 0,								   //根据vdc13引脚是否有外部电容来配置, 1.外挂电容 0.无外挂电容
    .flash_pg_keep = 0,									   //低功耗是否保持flash电源
};

/**************************************************** wk_param *********************************************************/
const struct port_wakeup port0 = {
    .pullup_down_enable = 1,                          //配置I/O 内部上下拉是否使能
    .edge       = FALLING_EDGE,                       //唤醒方式选择,可选：上升沿\下降沿
    .iomap      = POWER_WAKEUP_IO,                    	  //唤醒口选择
};

#if TCFG_CHARGE_ENABLE
//用于判满中断
const struct port_wakeup charge_port = {
    .edge       = RISING_EDGE,
    .filter     = PORT_FLT_16ms,
    .iomap      = IO_CHGFL_DET,
};

//VPWR和VBAT比较器边沿切换唤醒
const struct port_wakeup vbat_port = {
    .edge       = BOTH_EDGE,
    .filter     = PORT_FLT_16ms,
    .iomap      = IO_VBTCH_DET,
};

#endif

const struct wakeup_param wk_param = {
    .port[1] = &port0,

#if TCFG_CHARGE_ENABLE
    .aport[0] = &charge_port,
    .aport[1] = &vbat_port,
#endif
};

void sleep_exit_callback(u32 usec)
{
    sleep_exit_callback_common(NULL);

    putchar('>');
}

void sleep_enter_callback(u8  step)
{
    /* 此函数禁止添加打印 */
    putchar('<');

    audio_off();

    sleep_enter_callback_common(NULL);
}


void board_set_soft_poweroff(void)
{
    soff_gpio_protect(POWER_WAKEUP_IO);

    audio_off();

    board_set_soft_poweroff_common(NULL);
}

void wakeup_callback(u8 index)
{
    /* log_info("gpio: %d\n", wk_param.port[index]); */
}

void awakeup_callback(u8 index, u8 gpio, POWER_WKUP_EDGE edge)
{
#if TCFG_CHARGE_ENABLE
    switch (gpio) {
    case IO_CHGFL_DET://charge port
        charge_wakeup_isr();
        break;
    case IO_VBTCH_DET://vbat port
        vpwr_wakeup_isr();
        break;
    }
#endif
}


void sys_power_init()
{
    power_init(&power_param);

    if (MAX_WAKEUP_PORT <= 7) {
        ASSERT(config_max_wakeup_port <= (MAX_WAKEUP_PORT - 1), "config_max_wakeup_port > (MAX_WAKEUP_PORT-1)");
    } else {
        ASSERT(config_max_wakeup_port <= (MAX_WAKEUP_PORT), "config_max_wakeup_port > (MAX_WAKEUP_PORT)");
    }

    power_wakeup_init(&wk_param);
}

void sys_power_down(u32 usec)
{
    u8 temp_wdt_con = 0;

    OS_ENTER_CRITICAL();

    temp_wdt_con = P3_WDT_CON;
    if (usec == (u32) - 2) {
        wdt_close();
    }
    low_power_sys_request(usec);
    P3_WDT_CON = temp_wdt_con;
    wdt_clear();

    OS_EXIT_CRITICAL();
}

void sys_softoff()
{
    power_set_soft_poweroff();
}




