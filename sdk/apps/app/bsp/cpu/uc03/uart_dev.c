#pragma bss_seg(".uart.data.bss")
#pragma data_seg(".uart.data")
#pragma const_seg(".uart.text.const")
#pragma code_seg(".uart.text")
#pragma str_literal_override(".uart.text.const")

#include "clock.h"
#include "uart_dev.h"
#include "cpu.h"
#include "gpio.h"
/* #include "spinlock.h" */


// #define LOG_TAG_CONST       UART
#define LOG_TAG             "[uart_dev]"
#include "log.h"

#define UART_CLK_GET  clk_get("lsb")//clk_get("uart")
#define UART_OT_CLK   UART_CLK_GET


static uart_bus_t uart0;
static uart_bus_t uart1;

/* _WEAK_ */
/* extern */
const u32 CONFIG_UART0_ENABLE = 1;

/* _WEAK_ */
/* extern */
const u32 CONFIG_UART1_ENABLE  = 1;

/* _WEAK_ */
/* extern */
const u32 CONFIG_UART0_ENABLE_TX_DMA =  0;//uart0 没有DMA

/* _WEAK_ */
/* extern */
const u32 CONFIG_UART1_ENABLE_TX_DMA =  1;

static u32 kfifo_get(KFIFO *kfifo, u8 *buffer, u32 len)
{
    unsigned int i;
    len = MIN(len, kfifo->buf_in - kfifo->buf_out);

    i = MIN(len, kfifo->buf_size - (kfifo->buf_out & (kfifo->buf_size - 1)));

    memcpy(buffer, kfifo->buffer + (kfifo->buf_out & (kfifo->buf_size - 1)), i);

    memcpy(buffer + i, kfifo->buffer, len - i);

    kfifo->buf_out += len;
    return len;

}
static u32 kfifo_length(KFIFO *kfifo)
{
    return kfifo->buf_in - kfifo->buf_out;
}
/**
 * @brief ut0发送一个byte
 *
 * @param a 要发送的字节
 */
static void UT0_putbyte(char a)
{
    if (JL_UART0->CON0 & BIT(0)) {
        JL_UART0->BUF = a;
        __asm__ volatile("csync");
        while ((JL_UART0->CON0 & BIT(15)) == 0);
        JL_UART0->CON0 |= BIT(13);
    }
}
/**
 * @brief ut0接收一个byte
 *
 * @param buf 字节存放地址
 * @param timeout 接收超时时间，单位1ms
 * @return 返回0：接收失败；返回1：接收成功
 */
static u8 UT0_getbyte(u8 *buf, u32 timeout)
{
    u32 _timeout, _t_sleep;
    timeout = ut_msecs_to_jiffies(timeout);
    if (JL_UART0->CON0 & BIT(6)) {
        // uart0 no DMA_MODE
        if (!kfifo_length(&uart0.kfifo)) {
            UT_OSSemPend(&uart0.sem_rx, timeout);
        }
        UT_OSSemSet(&uart0.sem_rx, 0);

        return kfifo_get(&uart0.kfifo, buf, 1);
    } else {
        _timeout = timeout + ut_get_jiffies();
        _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
        while (!(JL_UART0->CON0 & BIT(14))) {
            if (timeout && time_before(_timeout, ut_get_jiffies())) {
                return 0;
            }
            if (time_before(_t_sleep, ut_get_jiffies())) {
                ut_sleep();
                _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
            }
        }
        *buf = JL_UART0->BUF;
        JL_UART0->CON0 |= BIT(12);
        __asm__ volatile("csync");  //make RX_PND_CLR taking effect
    }
    return 1;
}
/**
 * @brief ut0中断函数
 */
SET_INTERRUPT
static void UT0_isr_fun(void)
{
    u32 rx_len = 0;
    u32 rx_err_len = 0;
    if ((JL_UART0->CON0 & BIT(2)) && (JL_UART0->CON0 & BIT(15))) {
        JL_UART0->CON0 |= BIT(13);
        UT_OSSemPost(&uart0.sem_tx);
        if (uart0.isr_cbfun) {
            uart0.isr_cbfun(&uart0, UT_TX);
        }
    }
    if ((JL_UART0->CON0 & BIT(3)) && (JL_UART0->CON0 & BIT(14))) {
        JL_UART0->CON0 |= BIT(7);                     //DMA模式
        JL_UART0->CON0 |= BIT(12);           //清RX PND
        __asm__ volatile("csync");
        while (!(JL_UART0->CON0 & BIT(8))) {
            __asm__ volatile("nop");
        }

        rx_len = JL_UART0->HRXCNT;             //读当前串口接收数据的个数
        /* uart0.kfifo.buf_in += uart0.frame_length; //每满frame_length字节则产生一次中断 */
        uart0.kfifo.buf_in += rx_len;
        UT_OSSemPost(&uart0.sem_rx);
        if (uart0.isr_cbfun) {
            uart0.isr_cbfun(&uart0, UT_RX);
        }
    }
    if ((JL_UART0->CON0 & BIT(5)) && (JL_UART0->CON0 & BIT(11))) {
        //OTCNT PND
        JL_UART0->CON0 |= BIT(7);                     //DMA模式
        JL_UART0->CON0 |= BIT(10);                    //清OTCNT PND
        JL_UART0->CON0 |= BIT(12);                    //清RX PND(这里的顺序不能改变，这里要清一次)
        __asm__ volatile("csync");
        while (!(JL_UART0->CON0 & BIT(8))) {
            __asm__ volatile("nop");
        }

        rx_len = JL_UART0->HRXCNT;             //读当前串口接收数据的个数

        if (rx_len) {
            uart0.kfifo.buf_in += rx_len;
            /* printf("%s() %d\n", __func__, __LINE__); */
            UT_OSSemPost(&uart0.sem_rx);
            if (uart0.isr_cbfun) {
                uart0.isr_cbfun(&uart0, UT_RX_OT);
            }
        }
    }
    if ((JL_UART0->CON2 & BIT(6)) && (JL_UART0->CON2 & BIT(8))) {
        rx_err_len = JL_UART0->RX_ERR_CNT;
        JL_UART0->CON2 |= BIT(7);             //清CHK PND
        log_error("-------uart0 parity fail! RB8:%d,rx num:%d", !!(JL_UART0->CON2 & BIT(2)), rx_err_len);
    }
}
/**
 * @brief ut0接收字符串
 *
 * @param buf 字符串存放首地址
 * @param len 预接收长度
 * @param timeout 接收超时时间，单位1ms
 * @return 返回实际接收长度
 */
static u32 UT0_read_buf(u8 *buf, u32 len, u32 timeout)
{
    u32 i;
    u32 _timeout, _t_sleep;
    if (len == 0) {
        return 0;
    }
    timeout = ut_msecs_to_jiffies(timeout);
    if (JL_UART0->CON0 & BIT(6)) {
        // uart0 no DMA_MODE
        if (!kfifo_length(&uart0.kfifo)) {
            UT_OSSemPend(&uart0.sem_rx, timeout);
        }
        UT_OSSemSet(&uart0.sem_rx, 0);

        return kfifo_get(&uart0.kfifo, buf, len);
    } else {
        _timeout = timeout + ut_get_jiffies();
        _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
        for (i = 0; i < len; i++) {
            while (!(JL_UART0->CON0 & BIT(14))) {
                if (timeout && time_before(_timeout, ut_get_jiffies())) {
                    return i;
                }
                if (time_before(_t_sleep, ut_get_jiffies())) {
                    ut_sleep();
                    _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
                }
            }
            *(buf + i) = JL_UART0->BUF;
            JL_UART0->CON0 |= BIT(12);
            __asm__ volatile("csync");  //make RX_PND_CLR taking effect
        }
    }
    return len;
}
/**
 * @brief ut0发送字符串
 *
 * @param buf 字符串首地址
 * @param len 发送的字符串长度
 */
static void UT0_write_buf(const u8 *buf, u32 len)
{
    u32 i;
    if (len == 0) {
        return;
    }
    if (CONFIG_UART0_ENABLE_TX_DMA) {
        // uart0 no DMA_MODE
        UT_OSSemSet(&uart0.sem_tx, 0);
        JL_UART0->CON0 |= BIT(13);
        JL_UART0->CON0 |= BIT(2);
        JL_UART0->TXADR = (u32)buf;
        JL_UART0->TXCNT = len;
        UT_OSSemPend(&uart0.sem_tx, 0);
        JL_UART0->CON0 &= ~BIT(2);
    } else {
        for (i = 0; i < len; i ++) {
            UT0_putbyte(*(buf + i));
        }
    }
}
/**
 * @brief ut0配置波特率
 *
 * @param baud 波特率值
 */
static void UT0_set_baud(u32 baud)
{
    JL_UART0->CON0 &= ~(BIT(0) | BIT(1));
    JL_UART0->CON0 |= BIT(13) | BIT(12) | BIT(10);
    /* JL_UART0->BAUD = ((UART_CLK_GET + baud / 2) / baud) / 4 - 1; */
    /* log_info("uart0 calculate baud:0x%x",((UART_CLK_GET) / baud) / 4 - 1); */
    JL_UART0->BAUD = ((UART_CLK_GET) / baud) / 4 - 1;
    if (JL_UART0->CON0 & BIT(5)) {
        if (uart0.rx_timeout > 10) {
            JL_UART0->OTCNT = (uart0.rx_timeout / 10) * (UART_OT_CLK / 10) / 10;
        } else {
            JL_UART0->OTCNT = uart0.rx_timeout * UART_OT_CLK / 1000;
        }
    }
    JL_UART0->CON0 |= BIT(13) | BIT(12) | BIT(10) | BIT(0) | BIT(1);
}

/**
*获取串口数据长度
*/
static u32 uart1_get_data_len()
{
    return kfifo_length(&uart1.kfifo);
}

/**
 * @brief ut0使能
 */
static void UT0_open(u32 baud, void *cbuf, u32 cbuf_size, u32 rx_cnt, u32 ot, u32 is_9bit, enum uart_parity_type parity_type)
{
    JL_UART0->CON0 = BIT(13) | BIT(12) | BIT(10);
    UT_OSSemCreate(&uart0.sem_rx, 0);
    UT_OSSemCreate(&uart0.sem_tx, 0);
    request_irq(IRQ_UART0_IDX, 3, UT0_isr_fun, 0);
    if (cbuf) {
        uart0.kfifo.buffer = cbuf;
        uart0.kfifo.buf_size = cbuf_size;
        uart0.kfifo.buf_in = 0;
        uart0.kfifo.buf_out = 0;
        uart0.frame_length = rx_cnt;
        uart0.rx_timeout = ot;
        JL_UART0->RXSADR = (u32)uart0.kfifo.buffer;
        JL_UART0->RXEADR = (u32)(uart0.kfifo.buffer + uart0.kfifo.buf_size);
        JL_UART0->RXCNT = uart0.frame_length;
        JL_UART0->CON0 |= BIT(6) | BIT(5) | BIT(3);
        log_error("uart0 no DMA capability");
    }
    if (is_9bit) {
        /* SFR(JL_UART1->CON0, 18, 2, 2); //滤波 */
        /* SFR(JL_UART1->CON0, 16, 2, 2); //滤波 */
        SFR(JL_UART0->CON2, 4, 2, parity_type); //校验
        JL_UART0->CON2 |= BIT(6);//校验出错中断使能
        JL_UART0->CON2 |= BIT(3);//parity enable
        JL_UART0->CON2 |= BIT(0);//9bit enable
    } else {
        JL_UART0->CON2 &= ~BIT(3);
        JL_UART0->CON2 &= ~BIT(0);
    }
    UT0_set_baud(baud);
}
/**
 * @brief ut0关闭，注销
 */
static void UT0_close(void)
{
    UT_OSSemClose(&uart0.sem_rx);
    UT_OSSemClose(&uart0.sem_tx);
    irq_disable(IRQ_UART0_IDX);
    JL_UART0->CON0 = BIT(13) | BIT(12) | BIT(10);
}
/**
 * @brief ut1发送一个byte
 *
 * @param a 要发送的字节
 */
static void UT1_putbyte(char a)
{
    if (JL_UART1->CON0 & BIT(0)) {
        JL_UART1->BUF = a;
        __asm__ volatile("csync");
        while ((JL_UART1->CON0 & BIT(15)) == 0);
        JL_UART1->CON0 |= BIT(13);
    }
}
/**
 * @brief ut1接收一个byte
 *
 * @param buf 字节存放地址
 * @param timeout 接收超时时间，单位1ms
10ms¥；返回1：接收成功
 */
static u8 UT1_getbyte(u8 *buf, u32 timeout)
{
    u32 _timeout, _t_sleep;
    timeout = ut_msecs_to_jiffies(timeout);
    if (JL_UART1->CON0 & BIT(6)) {
        if (!kfifo_length(&uart1.kfifo)) {
            UT_OSSemPend(&uart1.sem_rx, timeout);
        }
        UT_OSSemSet(&uart1.sem_rx, 0);

        return kfifo_get(&uart1.kfifo, buf, 1);
    } else {
        _timeout = timeout + ut_get_jiffies();
        _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
        while (!(JL_UART1->CON0 & BIT(14))) {
            if (timeout && time_before(_timeout, ut_get_jiffies())) {
                return 0;
            }
            if (time_before(_t_sleep, ut_get_jiffies())) {
                ut_sleep();
                _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
            }
        }
        *buf = JL_UART1->BUF;
        JL_UART1->CON0 |= BIT(12);
        __asm__ volatile("csync");  //make RX_PND_CLR taking effect
    }
    return 1;
}
/**
 * @brief ut1中断函数
 */
SET_INTERRUPT
static void UT1_isr_fun(void)
{
    u32 rx_len = 0;
    u32 rx_err_len = 0;
    if ((JL_UART1->CON0 & BIT(2)) && (JL_UART1->CON0 & BIT(15))) {
        JL_UART1->CON0 |= BIT(13);
        UT_OSSemPost(&uart1.sem_tx);
        if (uart1.isr_cbfun) {
            uart1.isr_cbfun(&uart1, UT_TX);
        }
    }
    if ((JL_UART1->CON0 & BIT(3)) && (JL_UART1->CON0 & BIT(14))) {
        JL_UART1->CON0 |= BIT(7);                     //DMA模式
        JL_UART1->CON0 |= BIT(12);           //清RX PND
        __asm__ volatile("csync");
        while (!(JL_UART1->CON0 & BIT(8))) {
            __asm__ volatile("nop");
        }
        rx_len = JL_UART1->HRXCNT;             //读当前串口接收数据的个数
        /* uart1.kfifo.buf_in += uart1.frame_length; //每满32字节则产生一次中断 */
        uart1.kfifo.buf_in += rx_len;
        UT_OSSemPost(&uart1.sem_rx);
        if (uart1.isr_cbfun) {
            uart1.isr_cbfun(&uart1, UT_RX);
        }
    }

    if ((JL_UART1->CON0 & BIT(5)) && (JL_UART1->CON0 & BIT(11))) {
        //OTCNT PND
        JL_UART1->CON0 |= BIT(7);                     //DMA模式
        JL_UART1->CON0 |= BIT(10);                    //清OTCNT PND
        JL_UART1->CON0 |= BIT(12);                    //清RX PND(这里的顺序不能改变，这里要清一次)
        __asm__ volatile("csync");
        while (!(JL_UART1->CON0 & BIT(8))) {
            __asm__ volatile("nop");
        }
        rx_len = JL_UART1->HRXCNT;             //读当前串口接收数据的个数

        if (rx_len) {
            uart1.kfifo.buf_in += rx_len;
            /* printf("%s() %d\n", __func__, __LINE__); */
            UT_OSSemPost(&uart1.sem_rx);
            if (uart1.isr_cbfun) {
                uart1.isr_cbfun(&uart1, UT_RX_OT);
            }
        }
    }
    if ((JL_UART1->CON2 & BIT(6)) && (JL_UART1->CON2 & BIT(8))) {
        rx_err_len = JL_UART1->RX_ERR_CNT;
        JL_UART1->CON2 |= BIT(7);             //清CHK PND
        log_error("-------uart1 parity fail! RB8:%d,rx num:%d", !!(JL_UART1->CON2 & BIT(2)), rx_err_len);
    }
}
/**
 * @brief ut1接收字符串
 *
 * @param buf 字符串存放首地址
 * @param len 预接收长度
 * @param timeout 接收超时时间，单位1ms
 * @return 返回实际接收的长度
 */
static u32 UT1_read_buf(u8 *buf, u32 len, u32 timeout)
{
    u32 i;
    u32 _timeout, _t_sleep;
    if (len == 0) {
        return 0;
    }
    timeout = ut_msecs_to_jiffies(timeout);
    if (JL_UART1->CON0 & BIT(6)) {
        if (!kfifo_length(&uart1.kfifo)) {
            UT_OSSemPend(&uart1.sem_rx, timeout);
        }
        UT_OSSemSet(&uart1.sem_rx, 0);

        return kfifo_get(&uart1.kfifo, buf, len);
    } else {
        _timeout = timeout + ut_get_jiffies();
        _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
        for (i = 0; i < len; i++) {
            while (!(JL_UART1->CON0 & BIT(14))) {
                if (timeout && time_before(_timeout, ut_get_jiffies())) {
                    return i;
                }
                if (time_before(_t_sleep, ut_get_jiffies())) {
                    ut_sleep();
                    _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
                }
            }
            *(buf + i) = JL_UART1->BUF;
            JL_UART1->CON0 |= BIT(12);
            __asm__ volatile("csync");  //make RX_PND_CLR taking effect
        }
    }
    return len;
}
/**
 * @brief ut1发送字符串
 *
 * @param buf 字符串首地址
 * @param len 发送的字符串长度
 * @param timeout 发送超时时间，单位10ms
 */
static void UT1_write_buf(const u8 *buf, u32 len)
{
    u32 i;
    if (len == 0) {
        return;
    }
    if (CONFIG_UART1_ENABLE_TX_DMA) {
        UT_OSSemSet(&uart1.sem_tx, 0);
        JL_UART1->CON0 |= BIT(13);
        JL_UART1->CON0 |= BIT(2);
        JL_UART1->TXADR = (u32)buf;
        JL_UART1->TXCNT = len;
        UT_OSSemPend(&uart1.sem_tx, 0);
        JL_UART1->CON0 &= ~BIT(2);
    } else {
        for (i = 0; i < len; i ++) {
            UT1_putbyte(*(buf + i));
        }
    }
}
/**
 * @brief ut1配置波特率
 *
 * @param baud 波特率值
 */
static void UT1_set_baud(u32 baud)
{
    JL_UART1->CON0 &= ~(BIT(0) | BIT(1));
    JL_UART1->CON0 |= BIT(13) | BIT(12) | BIT(10);
    /* JL_UART1->BAUD = ((UART_CLK_GET + baud / 2) / baud) / 4 - 1; */
    /* log_info("uart1 calculate baud:0x%x",((UART_CLK_GET) / baud) / 4 - 1); */
    JL_UART1->BAUD = ((UART_CLK_GET) / baud) / 4 - 1;
    if (JL_UART1->CON0 & BIT(5)) {
        if (uart1.rx_timeout > 10) {
            JL_UART1->OTCNT = (uart1.rx_timeout / 10) * (UART_OT_CLK / 10) / 10;
        } else {
            JL_UART1->OTCNT = uart1.rx_timeout * UART_OT_CLK / 1000;
        }
    }
    JL_UART1->CON0 |= BIT(13) | BIT(12) | BIT(10) | BIT(0) | BIT(1);
}
/**
 * @brief ut1使能
 */
static void UT1_open(u32 baud, void *cbuf, u32 cbuf_size, u32 rx_cnt, u32 ot, u32 is_9bit, enum uart_parity_type parity_type)
{
    JL_UART1->CON0 = BIT(13) | BIT(12) | BIT(10);
    UT_OSSemCreate(&uart1.sem_rx, 0);
    UT_OSSemCreate(&uart1.sem_tx, 0);
    request_irq(IRQ_UART1_IDX, 3, UT1_isr_fun, 0);
    if (cbuf) {
        uart1.kfifo.buffer = cbuf;
        uart1.kfifo.buf_size = cbuf_size;
        uart1.kfifo.buf_in = 0;
        uart1.kfifo.buf_out = 0;
        uart1.frame_length = rx_cnt;
        uart1.rx_timeout = ot;
        JL_UART1->RXSADR = (u32)uart1.kfifo.buffer;
        JL_UART1->RXEADR = (u32)(uart1.kfifo.buffer + uart1.kfifo.buf_size);
        JL_UART1->RXCNT = uart1.frame_length;
        JL_UART1->CON0 |= BIT(6) | BIT(5) | BIT(3);
        log_info("uart1 DMA en");
    }
    if (is_9bit) {
        /* SFR(JL_UART1->CON0, 18, 2, 2); //滤波 */
        /* SFR(JL_UART1->CON0, 16, 2, 2); //滤波 */
        SFR(JL_UART1->CON2, 4, 2, parity_type); //校验
        JL_UART1->CON2 |= BIT(6);//校验出错中断使能
        JL_UART1->CON2 |= BIT(3);//parity enable
        JL_UART1->CON2 |= BIT(0);//9bit enable
    } else {
        JL_UART1->CON2 &= ~BIT(3);
        JL_UART1->CON2 &= ~BIT(0);
    }

    UT1_set_baud(baud);
}
/**
 * @brief ut1关闭，注销
 */
static void UT1_close(void)
{
    UT_OSSemClose(&uart1.sem_rx);
    UT_OSSemClose(&uart1.sem_tx);
    irq_disable(IRQ_UART1_IDX);
    JL_UART1->CON0 = BIT(13) | BIT(12) | BIT(10);
}

static u32 uart_is_idle(u32 ut_num)
{
    switch (ut_num) {
    case 0 :
        return !(JL_UART0->CON0 & BIT(0));
    case 1 :
        return !(JL_UART1->CON0 & BIT(0));
    default :
        break;
    }
    return 0;
}

static u8 uart_config(const struct uart_platform_data_t *arg, u8 tx_ch, enum PFI_TABLE rx_pfun)
{
    if (!(arg->tx_pin < IO_PORT_MAX || arg->rx_pin < IO_PORT_MAX)) {
        return -1;
    }

    if (arg->tx_pin < IO_PORT_MAX) {
        gpio_direction_output(arg->tx_pin, 1);
        gpio_set_fun_output_port(arg->tx_pin, tx_ch, 1, 1);
    }
    if (arg->rx_pin < IO_PORT_MAX) {
        gpio_direction_input(arg->rx_pin);
        gpio_set_pull_up(arg->rx_pin, 1);
        gpio_set_die(arg->rx_pin, 1);
        gpio_set_fun_input_port(arg->rx_pin, rx_pfun);
    }
    return 0;
}
static u8 uart_gpio_close(const struct uart_platform_data_t *arg, u8 tx_ch, enum PFI_TABLE rx_pfun)
{
    if (!(arg->tx_pin < IO_PORT_MAX || arg->rx_pin < IO_PORT_MAX)) {
        return -1;
    }

    if (arg->tx_pin < IO_PORT_MAX) {
        gpio_direction_input(arg->tx_pin);
        gpio_disable_fun_output_port(arg->tx_pin);
    }
    if (arg->rx_pin < IO_PORT_MAX) {
        gpio_direction_input(arg->rx_pin);
        gpio_set_pull_up(arg->rx_pin, 0);
        gpio_set_die(arg->rx_pin, 0);
        gpio_disable_fun_input_port(rx_pfun);
    }
    return 0;
}

/**
 * @brief ut模块初始const struct uart_platform_data_t *arg化函数，供外部调用
 *
 * @param arg 传入uartconst struct uart_platform_data_t *arg_argment型结构体指针
 * @return 返回uart_buconst struct uart_platform_data_t *args_t型结构体指针
 */
__attribute__((noinline))
const uart_bus_t *uart_dev_open(const struct uart_platform_data_t *arg)
{
    u8 ut_num;
    uart_bus_t *ut = NULL;
    u8 gpio_input_channle_flag = 0;

    if (uart_is_idle(0)) {
        ut_num = 0;
    } else if (uart_is_idle(1)) {
        ut_num = 1;
    } else {
        return NULL;
    }
    if (arg->rx_cbuf) {
        if ((arg->rx_cbuf_size == 0) || (arg->rx_cbuf_size & (arg->rx_cbuf_size - 1))) {
            return NULL;
        }
    }

    if (CONFIG_UART0_ENABLE && ut_num == 0) {
        if (uart_config(arg, FO_UART0_TX, PFI_UART0_RX)) {
            return NULL;
        }

        uart0.argv = arg->argv;
        uart0.isr_cbfun = arg->isr_cbfun;
        uart0.putbyte = UT0_putbyte;
        uart0.getbyte = UT0_getbyte;
        uart0.read = UT0_read_buf;
        uart0.write = UT0_write_buf;
        uart0.set_baud = UT0_set_baud;
        UT0_open(arg->baud,
                 arg->rx_cbuf, arg->rx_cbuf_size,
                 arg->frame_length, arg->rx_timeout,
                 arg->is_9bit, arg->parity_type);
        ut = &uart0;
    } else if (CONFIG_UART1_ENABLE && ut_num == 1) {
        if (uart_config(arg, FO_UART1_TX, PFI_UART1_RX)) {
            return NULL;
        }
        uart1.argv = arg->argv;
        uart1.isr_cbfun = arg->isr_cbfun;
        uart1.putbyte = UT1_putbyte;
        uart1.getbyte = UT1_getbyte;
        uart1.read    = UT1_read_buf;
        uart1.write   = UT1_write_buf;
        uart1.set_baud = UT1_set_baud;
        uart1.get_data_len = uart1_get_data_len;
        UT1_open(arg->baud,
                 arg->rx_cbuf, arg->rx_cbuf_size,
                 arg->frame_length, arg->rx_timeout,
                 arg->is_9bit, arg->parity_type);
        ut = &uart1;
    } else {
        return NULL;
    }
    return ut;
}
u32 uart_dev_close(uart_bus_t *ut)
{
    UT_OSSemClose(&ut->sem_rx);
    UT_OSSemClose(&ut->sem_tx);
    if (&uart0 == ut) {
        UT0_close();
        /* return gpio_close_uart0(); */
        return 1;
    } else if (&uart1 == ut) {
        UT1_close();
        /* return gpio_close_uart1(); */
        return 1;
    }
    return 0;
}

void uart_disable_for_ota()
{
    JL_UART0->CON0 = BIT(13) | BIT(12) | BIT(10);
    JL_UART1->CON0 = BIT(13) | BIT(12) | BIT(10);
}

#if 0//UART_CTS_RTS_EN
static u8 _rts_io = -1;
static u8 _cts_io = -1;
void uart1_flow_ctl_init(u8 rts_io, u8 cts_io)
{
    JL_UART1->CON1 = 0;
    //RTS
    if (rts_io < IO_PORT_MAX) {
        _rts_io = rts_io;
        gpio_set_die(rts_io, 1);
        gpio_set_direction(rts_io, 0);
        gpio_set_pull_up(rts_io, 0);
        gpio_set_pull_down(rts_io, 0);
        gpio_write(rts_io, 0);
        gpio_set_fun_output_port(rts_io, FO_UART1_RTS, 1, 1);
        JL_UART1->CON1 |= BIT(13) | BIT(0);
    }
    //CTS
    if (cts_io < IO_PORT_MAX) {
        _cts_io = cts_io;
        gpio_set_die(cts_io, 1);
        gpio_set_direction(cts_io, 1);
        gpio_set_pull_up(cts_io, 0);
        gpio_set_pull_down(cts_io, 1);
        gpio_set_fun_input_port(cts_io, PFI_UART1_CTS);
        JL_UART1->CON1 |= BIT(14) | BIT(2) | BIT(3);
    }
}

void uart1_flow_ctl_rts_suspend(void)
{
    if (!(JL_UART1->CON1 & BIT(0))) {
        gpio_write(_rts_io, 1);      //告诉对方，自己忙碌
    }
    JL_UART1->CON1 |= BIT(4);   //硬件停止接收
}

void uart1_flow_ctl_rts_resume(void)
{
    JL_UART1->CON1 &= ~BIT(4);  //硬件可以接收
    if (JL_UART1->CON1 & BIT(0)) {
        JL_UART1->CON1 |= BIT(13);
    } else {
        gpio_write(_rts_io, 0);      //表示可以继续接收数据
    }
}
#endif



/********************************uart test*********************************/
#if 0
u8 read_buf[512] __attribute__((aligned(4)));
void uart01_test()
{
    //uart0无DMA。uart1支持DMA收发。
    //常变量CONFIG_UART1_ENABLE_TX_DMA控制DMA发送开关(1:开DMA；0:关DMA)
    //当开启DMA接收时(给rx_cbuf结构体成员赋值会打开DMA接收)，个数rx_cbuf_size 必须设为2的n次方
    u8  write_buf[64] = "1bcde2bcde3bcde4bcde5bcde6bcde7bcde8bcde9bcde0bcde12345678901234";
    u8 read_data[512] = {0}, rx_cnt = 0;
    u32 i = 0, time_out = 0;
    const uart_bus_t *ut = NULL;
    struct uart_platform_data_t arg;

#if 0//uart0

    arg.tx_pin = IO_PORTA_08;//不使用发送功能赋值0xff
    arg.rx_pin = IO_PORTA_07;//不使接收功能赋值0xff
    arg.rx_cbuf = NULL;  //uart0 不支持
    arg.rx_cbuf_size = 0;//uart0 不支持
    arg.frame_length = 0;//uart0 不支持
    arg.rx_timeout = 0;//uart0 不支持
    arg.isr_cbfun = NULL;
    arg.argv = JL_UART0;
    arg.parity_type = UT_PARITY_EVEN;
    arg.is_9bit = 0;//校验使能, 默认关闭滤波
    arg.baud = 1000000;//波特率
    log_info("----uart0 test!----\n");
    if (uart_dev_close(&uart0) == 0) {
        log_error("~~~~~~~~~uart0 close fail!~~~~~~~~~~\n");
    }
    ut = uart_dev_open(&arg);
    if (ut == 0) {
        log_error("------uart0 init fail!--------\n");
    } else {
        log_info("------uart0 init ok!--------\n");
        log_info("uart0: con0:0x%x,con1:0x%x,con2:0x%x", JL_UART0->CON0, JL_UART0->CON1, JL_UART0->CON2);
        log_info("uart0: baud:0x%x,buf:0x%x", JL_UART0->BAUD, JL_UART0->BUF);
        log_info("uart0: RX_ERR_CNT:0x%x", JL_UART0->RX_ERR_CNT);

        //发送
        UT0_putbyte(write_buf[0]);
        ut->putbyte('\n');
        UT0_write_buf(write_buf, sizeof(write_buf));
        ut->putbyte('\n');
        ut->write(write_buf, sizeof(write_buf));
        //接收
        log_info("uart0 wait for time !\n");
        time_out = 9000;
        rx_cnt = 2;
        while (rx_cnt) {
            if (UT0_getbyte(&read_data[0], time_out)) {
                log_info("get byte: %c\n", read_data[0]);
                rx_cnt--;
            } else {
                log_info("receive 1 byte fail!");
                log_info("please send 1 byte!\n");
            }
        }
        rx_cnt = 2;
        while (rx_cnt) {
            if (ut->read(read_data, sizeof(read_data) - 1, time_out)) {
                log_info("get buf: %s\n", read_data);
                for (i = 0; i < sizeof(read_data); i++) {
                    read_data[i] = 0;
                }
                rx_cnt--;
            } else {
                log_info("receive buf fail!");
                log_info("please send some characters!\n");
            }
        }
        log_info("**uart test end!**");
    }
#else//uart1

    u32 rx_len = 0;
    arg.tx_pin = IO_PORTA_08; //不使用发送功能赋值0xff
    arg.rx_pin = IO_PORTA_07; //不使接收功能赋值0xff
    arg.rx_cbuf = read_buf;
    arg.rx_cbuf_size = sizeof(read_buf);
    arg.frame_length = 370;//dma接收长(<=sizeof(read_buf))
    arg.rx_timeout = 30;//单位ms
    arg.isr_cbfun = NULL; //中断回调
    arg.argv = JL_UART1;
    arg.parity_type = UT_PARITY_EVEN;
    arg.is_9bit = 0;//校验使能, 默认关闭滤波
    arg.baud = 1000000;//波特率
    log_info("----uart1 test!----\n");
    if (uart_dev_close(&uart1) == 0) {
        log_error("~~~~~~~~~uart1 close fail!~~~~~~~~~~\n");
    }
    ut = uart_dev_open(&arg);
    if (ut == 0) {
        log_error("------uart1 init fail!--------\n");
    } else {
        log_info("------uart1 init ok!--------\n");
        log_info("uart1: con0:0x%x,con1:0x%x,con2:0x%x", JL_UART1->CON0, JL_UART1->CON1, JL_UART1->CON2);
        log_info("uart1: baud:0x%x,buf:0x%x", JL_UART1->BAUD, JL_UART1->BUF);
        log_info("uart1: RX_ERR_CNT:0x%x", JL_UART1->RX_ERR_CNT);
        //发送
        ut->putbyte(write_buf[0]);
        ut->putbyte('\n');
        ut->write(write_buf, sizeof(write_buf));
        u8 temp1[31] = "\nTwo serial ports are required!";
        ut->write(temp1, sizeof(temp1));
        //接收
        log_info("\nuart1 wait for time !\n");
        time_out = 1000;
        rx_cnt = 3;
        while (rx_cnt) {
            if (ut->getbyte(&read_data[0], time_out)) {
                log_info("get byte: %c\n", read_data[0]);
                rx_cnt--;
            } else {
                log_info("receive 1 byte fail!");
                log_info("please send 1 byte!\n");
            }
        }
        rx_cnt = 5;
        while (rx_cnt) {
            rx_len = ut->read(read_data, sizeof(read_data) - 1, time_out);
            if (rx_len) {
                log_info("get buf: %s,len:%d", read_data, rx_len);
                ut->write(read_data, rx_len);
                for (i = 0; i < sizeof(read_data); i++) {
                    read_data[i] = 0;
                }
                rx_cnt--;
            } else {
                log_info("receive buf fail!");
                log_info("please send some characters!\n");
            }
        }
        log_info("**uart test end!**");
    }
#endif
}
#endif
