#include "gpio.h"
#include "port_wkup.h"
#include "hwi.h"

/* #define LOG_TAG_CONST       WKUP */
#define LOG_TAG             "[PORT-WKUP]"
#include "log.h"
/**
 * 注意：JL_WAKEUP 区别于PMU管理的唤醒。可理解为一个独立的模块使用。但在低功耗的情况下，中断无效。
 */

/*JL_WAKEUP->CON0:
 *                0：IO-wkup
 *                1：IRFLT-wkup 与红外功能复用
 *                2：uart0_rxas 与uart0_rx功能复用
 *                3：uart1_rxas 与uart1_rx功能复用
 * */

static u32 port_wkup_source_pfi[] = {
    INPUT_CH_SIGNAL_IRFLT,
    INPUT_CH_SIGNAL_IRFLT,
    PFI_UART0_RX,
    PFI_UART1_RX,
};
static void (*port_wkup_irq_cbfun[4])(u32 port, u32 edge) = {NULL};
static u8 user_port[4] = {0};
/**
 * @brief 引脚中断函数
 */
___interrupt
void port_wkup_irq_fun(void)
{
    u8 wakeup_index = 0;
    //printf("png:0x%x\n", JL_WAKEUP->CON3);
    for (; wakeup_index < 4; wakeup_index++) {
        if (JL_WAKEUP->CON3 & BIT(wakeup_index)) {
            JL_WAKEUP->CON2 |= BIT(wakeup_index);
            if (port_wkup_irq_cbfun[wakeup_index]) {
                //printf(">");
                port_wkup_irq_cbfun[wakeup_index](user_port[wakeup_index], (!!(JL_WAKEUP->CON1 & BIT(wakeup_index))));
            }
        }
    }
}
/**
 * @brief 引脚中断初始化//注意功能复用
 * @param port 任意引脚的引脚号：IO_PORTA_00......
 * @param trigger_edge 触发边沿。 0：上升沿触发。 1；下降沿触发
 * @param cbfun 相应的中断回调函数
 */
void port_wkup_info_init()
{
    static u8 init_flag = 1;
    if (init_flag) {
        memset((u8 *)user_port, 0xff, sizeof(user_port));
        memset((u8 *)port_wkup_irq_cbfun, 0, sizeof(port_wkup_irq_cbfun));
        init_flag = 0;
    }
}
int port_wkup_interrupt_init(u32 port, enum WAKEUP_SOURCE wkup_s, enum WAKEUP_EDGE trigger_edge, void (*cbfun)(u32 port, u32 edge)) //注意功能复用
{
    port_wkup_info_init();
    if (JL_WAKEUP->CON0 & BIT(wkup_s)) {
        log_error("port wkup(%d) has been open.please close!\n", wkup_s);
        return -1;
    }
    if (trigger_edge == PORT_WKUP_EDGE_RISE) {
        JL_WAKEUP->CON1 &= ~BIT(wkup_s);//0:上升沿
        gpio_set_direction(port, 1);
        gpio_set_die(port, 1);
        gpio_set_pull_up(port, 0);
        gpio_set_pull_down(port, 1);
    } else {
        JL_WAKEUP->CON1 |= BIT(wkup_s);
        gpio_set_direction(port, 1);
        gpio_set_die(port, 1);
        gpio_set_pull_up(port, 1);
        gpio_set_pull_down(port, 0);
    }
    if (wkup_s <= WKUP_S_IRFLT) {
        gpio_ich_sel_input_signal(port, port_wkup_source_pfi[wkup_s], INPUT_CH_TYPE_GP_ICH0);
    } else {
        gpio_set_fun_input_port(port, port_wkup_source_pfi[wkup_s]);
    }
    user_port[wkup_s] = port;
    if (cbfun) {
        port_wkup_irq_cbfun[wkup_s] = cbfun;
    }
    request_irq(IRQ_PORT_IDX, 3, port_wkup_irq_fun, 0); //注册中断函数
    JL_WAKEUP->CON2 |= BIT(wkup_s);                          //清一次pnd
    JL_WAKEUP->CON0 |= BIT(wkup_s);                          //引脚中断使能
    JL_WAKEUP->CON2 |= BIT(wkup_s);
    log_info("init ok:port:%d,wkup_s:%d\n", port, wkup_s);
    return 0;//ok
}

/**
 * @brief 关掉该引脚的中断功能
 * @param port 引脚号：IO_PORTA_00......
 */
int port_wkup_interrupt_deinit(u32 port, enum WAKEUP_SOURCE wkup_s)
{
    if (JL_WAKEUP->CON0 & BIT(wkup_s)) {
        if (port == user_port[wkup_s]) {
            JL_WAKEUP->CON0 &= ~BIT(wkup_s);
            gpio_set_direction(port, 1);
            gpio_set_die(port, 0);
            gpio_set_pull_up(port, 0);
            gpio_set_pull_down(port, 0);
            if (wkup_s <= WKUP_S_IRFLT) {
                gpio_ich_disable_input_signal(port, port_wkup_source_pfi[wkup_s], INPUT_CH_TYPE_GP_ICH0);
            } else {
                gpio_disable_fun_input_port(port_wkup_source_pfi[wkup_s]);
            }

            gpio_disable_fun_input_port(port_wkup_source_pfi[wkup_s]);
            user_port[wkup_s] = 0xff;
            port_wkup_irq_cbfun[wkup_s] = NULL;
        } else {
            log_error("%s parameter:port error!\n", __func__);
            return -2;
        }
    } else {
        log_error("port wkup source:%d has been closed!\n", wkup_s);
        return -3;
    }
    return 0;
}

//切换边沿
int port_wkup_interrupt_change_edge(enum WAKEUP_SOURCE wkup_s, enum WAKEUP_EDGE trigger_edge)
{
    if ((JL_WAKEUP->CON0 & BIT(wkup_s)) == 0) {
        log_error("port wkup(%d) has been close.please open!\n", wkup_s);
        return -1;
    }
    if (trigger_edge == PORT_WKUP_EDGE_RISE) {
        JL_WAKEUP->CON1 &= ~BIT(wkup_s);//0:上升沿
        gpio_set_direction(user_port[wkup_s], 1);
        gpio_set_die(user_port[wkup_s], 1);
        gpio_set_pull_up(user_port[wkup_s], 0);
        gpio_set_pull_down(user_port[wkup_s], 1);
    } else {
        JL_WAKEUP->CON1 |= BIT(wkup_s);
        gpio_set_direction(user_port[wkup_s], 1);
        gpio_set_die(user_port[wkup_s], 1);
        gpio_set_pull_up(user_port[wkup_s], 1);
        gpio_set_pull_down(user_port[wkup_s], 0);
    }
    return 0;
}
//切换回调函数
int port_wkup_interrupt_change_cbfun(enum WAKEUP_SOURCE wkup_s, void (*cbfun)(u32 port, u32 edge))
{
    if ((JL_WAKEUP->CON0 & BIT(wkup_s)) == 0) {
        log_error("port wkup(%d) has been close.please open!\n", wkup_s);
        return -1;
    }
    if (cbfun) {
        port_wkup_irq_cbfun[wkup_s] = cbfun;
    }
    return 0;
}
/*********************************************************************************************************
 * ******************************           使用举例如下           ***************************************
 * ******************************************************************************************************/
#if 0

void _ich_wkup(u32 port, u32 edge)
{
    printf("%s\n", __func__);
}
void _irflt_wkup(u32 port, u32 edge)
{
    printf("%s\n", __func__);
}
void _ut0_wkup(u32 port, u32 edge)
{
    printf("%s\n", __func__);
}
void _ut1_wkup(u32 port, u32 edge)
{
    printf("%s\n", __func__);
}

void wdt_clear(void);
void port_wkup_test()
{
    port_wkup_interrupt_init(IO_PORTA_03, WKUP_S_ICH_WKUP, PORT_WKUP_EDGE_RISE, _ich_wkup);//上升沿触发
    port_wkup_interrupt_init(IO_PORTA_04, WKUP_S_IRFLT, PORT_WKUP_EDGE_FALL, _irflt_wkup);//下升沿触发
    port_wkup_interrupt_init(IO_PORTA_05, WKUP_S_UART0_RXAS, PORT_WKUP_EDGE_FALL, _ut0_wkup);//下升沿触发
    port_wkup_interrupt_init(IO_PORTA_02, WKUP_S_UART1_RXAS, PORT_WKUP_EDGE_FALL, _ut1_wkup);//下升沿触发
    port_wkup_interrupt_deinit(IO_PORTC_05, WKUP_S_ICH_WKUP);
    port_wkup_interrupt_deinit(IO_PORTA_03, WKUP_S_ICH_WKUP);
    while (1) {
        wdt_clear();
    }
}
#endif

