#include "mio_api.h"
#include "vfs.h"
#include "gpio.h"
#include "clock.h"
#include "timer.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

#define MIO_EN				0
/* MIO最多支持4路PWM通道 */
#define MIO_API_PWM_PORT	IO_PORTA_15
#define PWM_FRE				3000

/* MIO最多支持(16 - 已使用PWM通道数)路IO通道 */
#define MIO_API_IO_PORT		JL_PORTA
#define MIO_API_IO_OFFSET	1

u32 mio_a_read(void *pfile, u8 *buff, u32 len)
{
    return vfs_read(pfile, buff, len);
}

void mio_a_pwm_init(u32 chl)
{
#if MIO_EN
    log_info("mio pwm init -> chl : %d\n", chl);
    gpio_set_pull_up(MIO_API_PWM_PORT, 0);
    gpio_set_pull_down(MIO_API_PWM_PORT, 0);
    gpio_set_direction(MIO_API_PWM_PORT, 0);
    gpio_set_die(MIO_API_PWM_PORT, 1);
    gpio_och_sel_output_signal(IO_PORTA_15, OUTPUT_CH_SIGNAL_TIMER0_PWM);
    JL_TIMER0->CON = 0;
    SFR(JL_TIMER0->CON, 10, 4, TIMER_SRC_STD_24M);      //时钟源选择std24m
    u32 timer_clk = 24000000;
    SFR(JL_TIMER0->CON, 4, 4, TIMER_PRESCALE_2);        //pset=2
    JL_TIMER0->CNT = 0;								    //清计数值
    JL_TIMER0->PRD = timer_clk / (2 * PWM_FRE);			//设置周期
    JL_TIMER0->CON |= BIT(8) | (0b01 << 0); 			//计数模式
#endif
}

/**
 *
 */
void mio_a_pwm_run(u32 chl, u32 duty)
{
#if MIO_EN
    local_irq_disable();
    JL_TIMER0->PWM = (JL_TIMER0->PRD * duty) / 255;	//0~255对应0~100%
    local_irq_enable();
#endif
}

void mio_a_io_init(u32 mask)
{
#if MIO_EN
    log_info("mio io init -> mask : 0x%x\n", mask);
    MIO_API_IO_PORT->PU0 &= ~(mask << MIO_API_IO_OFFSET);
    MIO_API_IO_PORT->PD0 |= (mask << MIO_API_IO_OFFSET);
    MIO_API_IO_PORT->DIR &= ~(mask << MIO_API_IO_OFFSET);
    MIO_API_IO_PORT->OUT &= ~(mask << MIO_API_IO_OFFSET);
#endif
}
void mio_a_io_run(u32 mask, u32 io_ver)
{
#if MIO_EN
    MIO_API_IO_PORT->OUT &= ~(mask << MIO_API_IO_OFFSET);
    MIO_API_IO_PORT->OUT |= (io_ver << MIO_API_IO_OFFSET);
#endif
}

void mio_a_hook_init(sound_mio_obj *obj)
{
    obj->read = mio_a_read;
    obj->pwm_init = mio_a_pwm_init;
    obj->pwm_run = mio_a_pwm_run;
    obj->io_init = mio_a_io_init;
    obj->io_run = mio_a_io_run;
}

