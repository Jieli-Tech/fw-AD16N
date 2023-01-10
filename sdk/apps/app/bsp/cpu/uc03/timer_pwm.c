#include "includes.h"
#include "gpio.h"
#include "timer.h"


static const u32 TIMERx_table[3] = {
    (u32)JL_TIMER0,
    (u32)JL_TIMER1,
    (u32)JL_TIMER2,
};

/**
 * @param JL_TIMERx : JL_TIMER0/1/2
 * @param pwm_io : JL_PORTA_01, JL_PORTB_02,,,等等，支持任意普通IO
 * @param fre : 频率，单位Hz
 * @param duty : 初始占空比，0~10000对应0~100%
 */
void timer_pwm_init(JL_TIMER_TypeDef *JL_TIMERx, u32 pwm_io, u32 fre, u32 duty)
{
    u8 tmr_num;
    for (tmr_num = 0; tmr_num < 3; tmr_num ++) {
        if ((u32)JL_TIMERx == TIMERx_table[tmr_num]) {
            break;
        }
        if (tmr_num == 2) {
            return;
        }
    }
    gpio_och_sel_output_signal(pwm_io, OUTPUT_CH_SIGNAL_TIMER0_PWM + tmr_num);
    gpio_set_die(pwm_io, 1);
    gpio_set_pull_up(pwm_io, 0);
    gpio_set_pull_down(pwm_io, 0);
    gpio_set_direction(pwm_io, 0);
    //初始化timer
    JL_TIMERx->CON = 0;
    SFR(JL_TIMERx->CON, 10, 4, TIMER_SRC_STD_24M); //时钟源选择std24m
    u32 timer_clk = 24000000;
    SFR(JL_TIMERx->CON, 4, 4, TIMER_PRESCALE_2); //pset=2
    JL_TIMERx->CNT = 0;								//清计数值
    JL_TIMERx->PRD = timer_clk / (2 * fre);			//设置周期
    //设置初始占空比
    JL_TIMERx->PWM = (JL_TIMERx->PRD * duty) / 10000;	//0~10000对应0~100%

    JL_TIMERx->CON |= BIT(8) | (0b01 << 0); 			//计数模式
}

/**
 * @param JL_TIMERx : JL_TIMER0/1/2/3/4/5
 * @param duty : 占空比，0~10000对应0~100%
 */
void set_timer_pwm_duty(JL_TIMER_TypeDef *JL_TIMERx, u32 duty)
{
    JL_TIMERx->PWM = (JL_TIMERx->PRD * duty) / 10000;	//0~10000对应0~100%
}


/********************************* 以下SDK的参考示例 ****************************/
#if 0
void timer_pwm_test(void)
{
    printf("*********** timer pwm test *************\n");

    timer_pwm_init(JL_TIMER0, IO_PORTC_02, 1000, 5000); //1KHz 50%
    timer_pwm_init(JL_TIMER2, IO_PORTC_00, 10000, 7500);//10KHz 75%

    extern void wdt_clear();
    while (1) {
        wdt_clear();
    }
}
#endif

