
#pragma bss_seg(".uart.data.bss")
#pragma data_seg(".uart.data")
#pragma const_seg(".uart.text.const")
#pragma code_seg(".uart.text")
#pragma str_literal_override(".uart.text.const")

#include "config.h"
#include "clock.h"
#include "uart.h"
#include "uart_v2.h"
#include "app_config.h"
#include "gpio.h"


#if UART_DEBUG
#if 0
//old uart driver
/*{{{*/
#define     DEBUG_UART  JL_UART0
#define     DEBUG_UART_OUTMAP   PORT_FUNC_UART0_TX
#define     DEBUG_UART_INMAP    PORT_FUNC_UART0_RX

AT(.log_ut_text)
void ut_putchar(char a)
{
    if (FALSE == libs_debug) {
        return;
    }
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
    if (FALSE == libs_debug) {
        return 0;
    }
    char c;
    c = 0;
    if (DEBUG_UART->CON0 & BIT(14)) {
        c = DEBUG_UART->BUF;
        DEBUG_UART->CON0 |=  BIT(12);

    }
    return c;
}


void debug_uart_init(u32 fre)
{
    if (FALSE == libs_debug) {
        return;
    }
    if (TCFG_UART_TX_PORT < IO_PORT_MAX) {
        //crossbar
        gpio_set_mode(IO_PORT_SPILT(TCFG_UART_TX_PORT), PORT_OUTPUT_HIGH);
        gpio_set_function(IO_PORT_SPILT(TCFG_UART_TX_PORT), DEBUG_UART_OUTMAP);
    }
#ifdef TCFG_UART_RX_PORT
    if (TCFG_UART_RX_PORT < IO_PORT_MAX) {
        //crossbar
        gpio_set_mode(IO_PORT_SPILT(TCFG_UART_RX_PORT), PORT_INPUT_PULLUP_10K);
        gpio_set_function(IO_PORT_SPILT(TCFG_UART_RX_PORT), DEBUG_UART_INMAP);
        /* #<{(| request_irq(IRQ_UART0_IDX, 0, uart_irq_handler, 0); |)}># */
    }
#endif
    DEBUG_UART->BAUD = (clk_get("uart") / fre) / 4 - 1;
    DEBUG_UART->CON0 = BIT(13) | BIT(12) | BIT(10) | BIT(1) | BIT(0);
    /* DEBUG_UART->CON0 = BIT(13) | BIT(12) | BIT(0); */
    DEBUG_UART->CON1 = 0;  //清Tx pending
    DEBUG_UART->CON0 |= BIT(13) | BIT(12);  //清Tx pending
}


/*}}}*/
#else
//new uart driver
#define     DEBUG_UART_NUM  0
#define     DEBUG_UART_DMA_EN   0

/* #define     TCFG_UART_BAUDRATE  1000000 */

#if DEBUG_UART_DMA_EN
#define     MAX_DEBUG_FIFO  256
static u8 debug_uart_buf[2][MAX_DEBUG_FIFO];
static u32 tx_jiffies = 0;
static u16 pos = 0;
static u8 uart_buffer_index = 0;

static void uart_irq(uart_dev uart_num, enum uart_event event)
{
    tx_jiffies = jiffies;
    uart_send_bytes(DEBUG_UART_NUM, debug_uart_buf[uart_buffer_index], pos);
    uart_buffer_index = !uart_buffer_index;
    pos = 0;
}
#endif
void debug_uart_init(u32 freq)
{
    if (FALSE == libs_debug) {
        return;
    }
    const struct uart_config debug_uart_config = {
        .baud_rate = TCFG_UART_BAUDRATE,
        .tx_pin = TCFG_UART_TX_PORT,
#ifdef TCFG_UART_RX_PORT
        .rx_pin = TCFG_UART_RX_PORT,
#else
        .rx_pin = -1,
#endif
        .tx_wait_mutex = 0,//1:不支持中断调用,互斥,0:支持中断,不互斥
    };
    uart_init(DEBUG_UART_NUM, &debug_uart_config);

#if DEBUG_UART_DMA_EN
    struct uart_dma_config dma_config = {
        .event_mask = UART_EVENT_TX_DONE,
        .irq_priority = 0,
        .irq_callback = uart_irq,
    };
    uart_dma_init(DEBUG_UART_NUM, &dma_config);
#endif
}

AT(.log_ut_text)
static void ut_putchar(char a)
{
    if (FALSE == libs_debug) {
        return;
    }
#if DEBUG_UART_DMA_EN

    debug_uart_buf[uart_buffer_index][pos] = a;
    pos++;
    if ((jiffies - tx_jiffies > 10) || (pos == MAX_DEBUG_FIFO)) {
        tx_jiffies = jiffies;
        uart_wait_tx_idle(DEBUG_UART_NUM, 10);
        uart_send_bytes(DEBUG_UART_NUM, debug_uart_buf[uart_buffer_index], pos);
        uart_buffer_index = !uart_buffer_index;
        pos = 0;
    }
#else
    /* if(a == '\n'){                          */
    /*     uart_putbyte(DEBUG_UART_NUM, '\r'); */
    /* }                                       */
    uart_putbyte(DEBUG_UART_NUM, a);

#endif
}

static char ut_getchar(void)
{
    if (FALSE == libs_debug) {
        return 0;
    }
    return uart_getbyte(DEBUG_UART_NUM);
}

#endif


AT(.log_ut_text)
void putchar(char a)
{
    ut_putchar(a);
}

AT(.log_ut_text)
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

