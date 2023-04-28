#ifndef _PORT_WKUP_H_
#define _PORT_WKUP_H_
#include "typedef.h"
/**
 * 注意：JL_WAKEUP 区别于PMU管理的唤醒。可理解为一个独立的模块使用。但在低功耗的情况下，中断无效。
 */

/*JL_WAKEUP->CON0:
 *                0：IO-wkup
 *                1：IRFLT-wkup 与红外功能复用
 *                2：uart0_rxas 与uart0_rx功能复用
 *                3：uart1_rxas 与uart1_rx功能复用
 * */

enum WAKEUP_SOURCE {
    WKUP_S_ICH_WKUP = 0,
    WKUP_S_IRFLT,
    WKUP_S_UART0_RXAS,
    WKUP_S_UART1_RXAS,
};
enum WAKEUP_EDGE {
    PORT_WKUP_EDGE_RISE,
    PORT_WKUP_EDGE_FALL,
};
int port_wkup_interrupt_init(u32 port, enum WAKEUP_SOURCE wkup_s, enum WAKEUP_EDGE trigger_edge, void (*cbfun)(u32 port, u32 edge)); //注意功能复用
int port_wkup_interrupt_deinit(u32 port, enum WAKEUP_SOURCE wkup_s);

int port_wkup_interrupt_change_edge(enum WAKEUP_SOURCE wkup_s, enum WAKEUP_EDGE trigger_edge);
int port_wkup_interrupt_change_cbfun(enum WAKEUP_SOURCE wkup_s, void (*cbfun)(u32 port, u32 edge));

#endif

