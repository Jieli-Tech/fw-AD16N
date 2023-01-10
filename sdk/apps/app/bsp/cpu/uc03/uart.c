
#pragma bss_seg(".uart.data.bss")
#pragma data_seg(".uart.data")
#pragma const_seg(".uart.text.const")
#pragma code_seg(".uart.text")
#pragma str_literal_override(".uart.text.const")

#include "config.h"
#include "clock.h"
#include "uart.h"
#include "app_config.h"
#include "gpio.h"


#if UART_DEBUG

#define     DEBUG_UART  JL_UART1

AT(.log_ut_text)
void ut_putchar(char a)
{
    u32 i = 0x10000;
    if (!(DEBUG_UART->CON0 & BIT(0))) {
        return;
    }
    while (((DEBUG_UART->CON0 & BIT(15)) == 0) && (0 != i)) {  //TX IDLE
        i--;
    }
    DEBUG_UART->CON0 |= BIT(13);  //清Tx pending

    DEBUG_UART->BUF = a;
    __asm__ volatile("csync");
}

char ut_getchar(void)
{
    char c;
    c = 0;
    if (DEBUG_UART->CON0 & BIT(14)) {
        c = DEBUG_UART->BUF;
        DEBUG_UART->CON0 |=  BIT(12);

    }
    return c;
}


void uart_init(u32 fre)
{
    if (TCFG_UART_TX_PORT < IO_PORT_MAX) {
        //crossbar
        gpio_direction_output(TCFG_UART_TX_PORT, 1);
        gpio_set_fun_output_port(TCFG_UART_TX_PORT, FO_UART1_TX, 1, 1);
    }
#ifdef TCFG_UART_RX_PORT
    if (TCFG_UART_RX_PORT < IO_PORT_MAX) {
        //crossbar
        gpio_direction_input(TCFG_UART_RX_PORT);
        gpio_set_pull_up(TCFG_UART_RX_PORT, 1);
        gpio_set_die(TCFG_UART_RX_PORT, 1);
        gpio_set_fun_input_port(TCFG_UART_RX_PORT, PFI_UART1_RX);
        /* #<{(| request_irq(IRQ_UART1_IDX, 0, uart_irq_handler, 0); |)}># */
    }
#endif
    DEBUG_UART->BAUD = (48000000 / fre) / 4 - 1;
    DEBUG_UART->CON0 = BIT(13) | BIT(12) | BIT(10) | BIT(1) | BIT(0);
    /* DEBUG_UART->CON0 = BIT(13) | BIT(12) | BIT(0); */
    DEBUG_UART->CON1 = 0;  //清Tx pending
    DEBUG_UART->CON0 |= BIT(13) | BIT(12);  //清Tx pending
}


AT(.log_ut_text)
void putchar(char a)
{
    ut_putchar(a);
}
char getchar(void)
{
    return ut_getchar();
}

#else

AT(.log_ut_text)
void putchar(char a)
{

}
char getchar(void)
{
    return 0;
}

#endif
