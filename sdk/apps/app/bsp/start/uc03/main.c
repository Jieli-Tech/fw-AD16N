/*********************************************************************************************
    *   Filename        : main.c

    *   Description     :

    *   Author          :

    *   Email           :

    *   Last modifiled  :

    *   Copyright:(c)JIELI  2011-2017  @ , All Rights Reserved.
*********************************************************************************************/

#pragma bss_seg(".main.data.bss")
#pragma data_seg(".main.data")
#pragma const_seg(".main.text.const")
#pragma code_seg(".main.text")
#pragma str_literal_override(".main.text.const")

#include "config.h"
#include "common.h"
#include "gpio.h"
#include "clock.h"
/* #include "maskrom.h" */
/* #include "asm/power/p33.h" */
#include "app_config.h"
#include "init.h"
#include "init_app.h"
#include "app.h"
#include "msg.h"
#include "device.h"
#include "asm/power_interface.h"
#include "power_api.h"
#include "asm/debug.h"
#include "wdt.h"

/* #include "vm.h" */
/* #include "asm/power_interface.h" */
/* #include "power_api.h" */

#define LOG_TAG_CONST       MAIN
#define LOG_TAG             "[main]"
#include "log.h"

#include "msg.h"
//#include "usb/host/usb_host.h"

/* void exception_analyze(void *sp) */
/* { */
/*  */
/* } */

//extern void dac_api_test_demo(void);
extern void app(void);
extern void mask_init(void *exp_hook, void *pchar, void *clk_hook, void *emit_hook);
extern void mask_init_for_app(void);
extern void debug_init();


/* AT("audio.text.cache.L3"); */
/* volatile int xxxxxx = 0x12345678; */
extern int data_addr[];
extern int data_begin[];
extern int data_size[];
void io_debug()
{
    JL_PORTC->OUT |=  BIT(0);
    JL_PORTC->DIR &= ~BIT(0);
}

char *strchr(const char *s, int c)
{
    const char ch = c;
    for (; *s != ch; ++s) {
        if (*s == '\0') {
            return (NULL);
        }
    }
    return ((char *) s);
}
#if 1
int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *su1, *su2;
    for (su1 = s1, su2 = s2; 0 < n; ++su1, ++su2, --n) {
        if (*su1 != *su2) {
            return ((*su1 < *su2) ? -1 : +1);
        }

    }
    return (0);
}
#endif
#if 1
int strncmp(const char *s1, const char *s2, size_t n)
{
    for (; 0 < n; ++s1, ++s2, --n) {
        if (*s1 != *s2) {
            return ((*(unsigned char *)s1 < * (unsigned char *)s2) ? -1 : +1);
        } else if (*s1 == '\0') {
            return (0);
        }
    }
    return (0);
}
#endif
size_t strlen(const char *s)
{
    const char *sc;
    for (sc = s; *sc != '\0'; ++sc) {
        ;
    }
    return sc - s;
}

__attribute__((noreturn))
void c_main(int cfg_addr)
{
    wdt_close();

    mask_init_for_app();
    irq_init();
    local_irq_disable();
    local_irq_enable();

    //上电初始化所有IO
    port_init();

    log_init(TCFG_UART_BAUDRATE);

    pll_sel(TCFG_PLL_SEL, TCFG_PLL_DIV, TCFG_HSB_DIV);

    dump_clock_info();

    debug_init();

    wdt_init(WDT_8S);

    P3_PINR_CON &= ~BIT(0); //关闭长按复位

    /* gpio_clk_out(IO_PORTC_00, CLK_OUT_HSB); */

    log_info("time & date %s %s \n  OTP c_main\n", __TIME__, __DATE__);

    power_reset_source_dump();
    power_wakeup_reason_dump();
    sys_power_init();

    system_init();
    app();
    while (1) {
        wdt_clear();
    }
}


