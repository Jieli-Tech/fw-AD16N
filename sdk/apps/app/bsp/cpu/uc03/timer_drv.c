#include "cpu.h"
#include "config.h"
#include "gpio.h"
#include "clock.h"
#include "mcpwm.h"
#include "timer_drv.h"
#include "hwi.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

#define TIMER_SFR(ch) JL_TIMER##ch

/*
 *timer
 * */
#define _timer_init(ch,us)  \
    request_irq(IRQ_TIME##ch##_IDX, 7, timer##ch##_isr, 0); \
	SFR(TIMER_SFR(ch)->CON, 10, 4, 7);                      \
    SFR(TIMER_SFR(ch)->CON, 4, 4, 1);                       \
	TIMER_SFR(ch)->CNT = 0;                                     \
	TIMER_SFR(ch)->PRD = 12000000 / 1000000 / 4 * us;	       \
	TIMER_SFR(ch)->CON |= BIT(14);                              \
	TIMER_SFR(ch)->CON |= BIT(0);


SET(interrupt(""))
static void timer0_isr(void)
{
    TIMER_SFR(0)->CON |= BIT(14);
    log_char('0');
}
SET(interrupt(""))
void timer1_isr(void)
{
    JL_PORTA->DIR &= ~BIT(4);
    JL_PORTA->OUT |= BIT(4);
    TIMER_SFR(1)->CON |= BIT(14);
    log_char('1');
    JL_PORTA->OUT &= ~BIT(4);
}
SET(interrupt(""))
static void timer2_isr(void)
{
    TIMER_SFR(2)->CON |= BIT(14);
    log_char('2');
}

void timer_init(u8 timer_ch, u32 us)
{
    switch (timer_ch) {
    case 0:
        _timer_init(0, us);
        break;
    case 1:
        _timer_init(1, us);
        break;
    case 2:
        _timer_init(2, us);
        break;
    default:
        break;
    }
}

