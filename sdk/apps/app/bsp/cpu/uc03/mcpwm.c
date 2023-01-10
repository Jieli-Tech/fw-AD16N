#include "config.h"
#include "common.h"
#include "gpio.h"
#include "mcpwm.h"
#include "log.h"
#include "clock.h"

#define LOG_TAG     "[mcpwm]"
#define MCPWM_CLK   clk_get("lsb")

PWM_CH_REG *get_pwm_ch_reg(pwm_ch_num_type index)
{
    PWM_CH_REG *reg = NULL;
    switch (index) {
    case pwm_ch0:
        reg = (PWM_CH_REG *)(&(JL_MCPWM->CH0_CON0));
        break;
    case pwm_ch1:
        reg = (PWM_CH_REG *)(&(JL_MCPWM->CH1_CON0));
        break;
    default:
        break;
    }
    return reg;

}
PWM_TIMER_REG *get_pwm_timer_reg(pwm_ch_num_type index)
{
    PWM_TIMER_REG *reg = NULL;
    switch (index) {
    case pwm_ch0:
        reg = (PWM_TIMER_REG *)(&(JL_MCPWM->TMR0_CON));
        break;
    case pwm_ch1:
        reg = (PWM_TIMER_REG *)(&(JL_MCPWM->TMR1_CON));
        break;
    default:
        break;
    }
    return reg;
}




/*
 * @brief 更改MCPWM的频率
 * @parm frequency 频率
 */
void mcpwm_set_frequency(pwm_ch_num_type ch, pwm_aligned_mode_type align, u32 frequency)
{
    PWM_TIMER_REG *reg = get_pwm_timer_reg(ch);
    if (reg == NULL) {
        return;
    }

    reg->tmr_con = 0;
    reg->tmr_cnt = 0;
    reg->tmr_pr = 0;

    u32 i = 0;
    u32 mcpwm_div_clk = 0;
    u32 mcpwm_tmr_pr = 0;
    u32 mcpwm_fre_min = 0;
    u32 clk = MCPWM_CLK;
    for (i = 0; i < 16; i++) {
        mcpwm_fre_min = clk / (65536 * (1 << i));
        if ((frequency >= mcpwm_fre_min) || (i == 15)) {
            break;
        }
    }
    reg->tmr_con |= (i << 3); //div 2^i
    mcpwm_div_clk = clk / (1 << i);
    if (frequency == 0) {
        mcpwm_tmr_pr = 0;
    } else {
        if (align == pwm_center_aligned) { //中心对齐
            mcpwm_tmr_pr = mcpwm_div_clk / (frequency * 2) - 1;
        } else {
            mcpwm_tmr_pr = mcpwm_div_clk / frequency - 1;
        }
    }
    reg->tmr_pr = mcpwm_tmr_pr;
    //timer mode
    if (align == pwm_center_aligned) { //中心对齐
        reg->tmr_con |= 0b10;
    } else {
        reg->tmr_con |= 0b01;
    }
}

/*
 * @brief 设置一个通道的占空比
 * @parm pwm_ch_num 通道号：pwm_ch0，pwm_ch1
 * @parm duty 占空比：0 ~ 10000 对应 0% ~ 100%
 */
void mcpwm_set_duty(pwm_ch_num_type pwm_ch, u16 duty)
{
    PWM_TIMER_REG *timer_reg = get_pwm_timer_reg(pwm_ch);
    PWM_CH_REG *pwm_reg = get_pwm_ch_reg(pwm_ch);

    if (pwm_reg && timer_reg) {
        pwm_reg->ch_cmpl = timer_reg->tmr_pr * duty / 10000;
        /* pwm_reg->ch_cmph = timer_reg->tmr_pr * duty / 10000; */
        pwm_reg->ch_cmph = pwm_reg->ch_cmpl;
        timer_reg->tmr_cnt = 0;
        timer_reg->tmr_con |= 0b01;
        if (duty == 10000) {
            timer_reg->tmr_cnt = 0;
            timer_reg->tmr_con &= ~(0b11);
        } else if (duty == 0) {
            timer_reg->tmr_cnt = pwm_reg->ch_cmpl;
            timer_reg->tmr_con &= ~(0b11);
        }
    }
}

/*
 * @brief 打开或者关闭一个时基
 * @parm pwm_ch_num 通道号：pwm_ch0，pwm_ch1
 * @parm enable 1：打开  0：关闭
 */
void mctimer_ch_open_or_close(pwm_ch_num_type pwm_ch, u8 enable)
{
    if (pwm_ch > pwm_ch_max) {
        return;
    }
    if (enable) {
        JL_MCPWM->MCPWM_CON0 |= BIT(pwm_ch + 8); //TnEN
    } else {
        JL_MCPWM->MCPWM_CON0 &= (~BIT(pwm_ch + 8)); //TnDIS
    }
}


/*
 * @brief 打开或者关闭一个通道
 * @parm pwm_ch_num 通道号：pwm_ch0，pwm_ch1
 * @parm enable 1：打开  0：关闭
 */
void mcpwm_ch_open_or_close(pwm_ch_num_type pwm_ch, u8 enable)
{
    if (pwm_ch >= pwm_ch_max) {
        return;
    }
    if (enable) {
        JL_MCPWM->MCPWM_CON0 |= BIT(pwm_ch); //PWMnEN
    } else {
        JL_MCPWM->MCPWM_CON0 &= (~BIT(pwm_ch)); //PWMnDIS
    }
}

/*
 * @brief 关闭MCPWM模块
 */
void mcpwm_open(pwm_ch_num_type pwm_ch)
{
    if (pwm_ch >= pwm_ch_max) {
        return;
    }
    PWM_CH_REG *pwm_reg = get_pwm_ch_reg(pwm_ch);
    pwm_reg->ch_con1 &= ~(0b111 << 8);
    pwm_reg->ch_con1 |= (pwm_ch << 8); //sel mctmr
    mcpwm_ch_open_or_close(pwm_ch, 1);
    mctimer_ch_open_or_close(pwm_ch, 1);
}


/*
 * @brief 关闭MCPWM模块
 */
void mcpwm_close(pwm_ch_num_type pwm_ch)
{
    mctimer_ch_open_or_close(pwm_ch, 0);
    mcpwm_ch_open_or_close(pwm_ch, 0);
}


void log_pwm_info(pwm_ch_num_type pwm_ch)
{
    PWM_CH_REG *pwm_reg = get_pwm_ch_reg(pwm_ch);
    PWM_TIMER_REG *timer_reg = get_pwm_timer_reg(pwm_ch);
    log_debug("tmr%d con0 = 0x%x", pwm_ch, timer_reg->tmr_con);
    log_debug("tmr%d pr = 0x%x", pwm_ch, timer_reg->tmr_pr);
    log_debug("pwm ch%d_con0 = 0x%x", pwm_ch, pwm_reg->ch_con0);
    log_debug("pwm ch%d_con1 = 0x%x", pwm_ch, pwm_reg->ch_con1);
    log_debug("pwm ch%d_cmph = 0x%x, pwm ch%d_cmpl = 0x%x", pwm_ch, pwm_reg->ch_cmph, pwm_ch, pwm_reg->ch_cmpl);
    log_debug("MCPWM_CON0 = 0x%x", JL_MCPWM->MCPWM_CON0);
    log_debug("mcpwm clk = %d", MCPWM_CLK);
}


void mcpwm_init(struct pwm_platform_data *arg)
{
    PWM_CH_REG *pwm_reg = get_pwm_ch_reg(arg->pwm_ch_num);
    if (pwm_reg == NULL) {
        return;
    }
    //set mctimer frequency
    mcpwm_set_frequency(arg->pwm_ch_num, arg->pwm_aligned_mode, arg->frequency);
    pwm_reg->ch_con0 = 0;

    if (arg->complementary_en) {            //是否互补
        pwm_reg->ch_con0 &= ~(BIT(5) | BIT(4));
        pwm_reg->ch_con0 |= BIT(5);         //L_INV
    } else {
        pwm_reg->ch_con0 &= ~(BIT(5) | BIT(4));
    }
    mcpwm_open(arg->pwm_ch_num);        //mcpwm enable
    //set duty
    mcpwm_set_duty(arg->pwm_ch_num, arg->duty);

    //H:
    if (arg->h_pin < IO_MAX_NUM) {      //任意引脚
        pwm_reg->ch_con0 |= BIT(2);     //H_EN
        gpio_set_fun_output_port(arg->h_pin, FO_MCPWM0_H + 1 * arg->pwm_ch_num, 1, 1);
        gpio_set_direction(arg->h_pin, 0); //DIR output
    }
    if (arg->l_pin < IO_MAX_NUM) {
        pwm_reg->ch_con0 |= BIT(3);
        gpio_set_fun_output_port(arg->l_pin, FO_MCPWM0_L + 1 * arg->pwm_ch_num, 1, 1);
        gpio_set_direction(arg->l_pin, 0); //DIR output
    }
    printf("JL_OMAP->PA2_OUT = 0x%x\n", JL_OMAP->PA2_OUT);
    printf("JL_OMAP->PA3_OUT = 0x%x\n", JL_OMAP->PA3_OUT);
    log_pwm_info(arg->pwm_ch_num);

}


///////////// for test code //////////////////
void mcpwm_test(void)
{
#define PWM_CH0_ENABLE 		1
#define PWM_CH1_ENABLE 		1

    struct pwm_platform_data pwm_p_data;

#if PWM_CH0_ENABLE
    pwm_p_data.pwm_aligned_mode = pwm_edge_aligned;         //边沿对齐
    pwm_p_data.pwm_ch_num = pwm_ch0;                        //通道号
    pwm_p_data.frequency = 1000;                            //1KHz
    pwm_p_data.duty = 5000;                                 //占空比50%
    pwm_p_data.h_pin = IO_PORTA_02;                         //任意引脚
    pwm_p_data.l_pin = IO_PORTA_03;                         //任意引脚,不需要就填-1
    pwm_p_data.complementary_en = 0;                        //两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&pwm_p_data);
#endif
#if PWM_CH1_ENABLE
    pwm_p_data.pwm_aligned_mode = pwm_edge_aligned;         //边沿对齐
    pwm_p_data.pwm_ch_num = pwm_ch1;                        //通道号
    pwm_p_data.frequency = 1000;                            //1KHz
    pwm_p_data.duty = 6000;                                 //占空比25%
    pwm_p_data.h_pin = IO_PORTA_01;                         //任意引脚
    pwm_p_data.l_pin = -1;                                  //任意引脚,不需要就填-1
    pwm_p_data.complementary_en = 1;                        //两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&pwm_p_data);
#endif
    /* while(1); */

}


/*******************************  外部引脚中断参考代码  ***************************/
void (*io_isr_cbfun)(u8 index) = NULL;
void set_io_ext_interrupt_cbfun(void (*cbfun)(u8 index))
{
    io_isr_cbfun = cbfun;
}

__attribute__((interrupt("")))
void io_interrupt()
{
    u32 io_index = -1;
    if (JL_MCPWM->CH0_CON1 & BIT(15)) {
        JL_MCPWM->CH0_CON1 |= BIT(14);
        delay(100000);
        io_index = 0;
    } else if (JL_MCPWM->CH1_CON1 & BIT(15)) {
        JL_MCPWM->CH1_CON1 |= BIT(14);
        io_index = 1;
    } else {
        return;
    }
    if (io_isr_cbfun) {
        io_isr_cbfun(io_index);
    }
}


void io_ext_interrupt_init(u8 index, u8 port, u8 trigger_mode)
{
    if (port > IO_PORT_MAX) {
        return;
    }
    gpio_set_die(port, 1);
    gpio_set_direction(port, 1);
    if (trigger_mode) {
        gpio_set_pull_up(port, 1);
        gpio_set_pull_down(port, 0);
        JL_MCPWM->FPIN_CON &= ~BIT(16 + index);//下降沿触发
    } else {
        gpio_set_pull_up(port, 0);
        gpio_set_pull_down(port, 1);
        JL_MCPWM->FPIN_CON |=  BIT(16 + index);//上升沿触发
    }
    JL_MCPWM->FPIN_CON |=  BIT(8 + index);//开启滤波
    JL_MCPWM->FPIN_CON |= (0b111111 << 0); //滤波时间 = 16 * 64 / hsb_clk (单位：s)

    gpio_set_fun_input_port(port, PFI_MCPWM0_FP + index);
    request_irq(IRQ_MCPWM_CHX_IDX, 1, io_interrupt, 0);   //注册中断函数
    PWM_CH_REG *pwm_reg = get_pwm_ch_reg(index);
    pwm_reg->ch_con1 = BIT(14) | BIT(11) | BIT(4) | (index << 0);
    pwm_reg->ch_con1 |= BIT(3);
    /* JL_MCPWM->CH0_CON1 |= BIT(3); */
    /* JL_MCPWM->CH1_CON1 |= BIT(3); */

    JL_MCPWM->MCPWM_CON0 |= BIT(index);
    PWM_TIMER_REG *timer_reg = get_pwm_timer_reg(pwm_ch0);
    log_info("FI_MCPWM0_FP = 0x%x\n", JL_IMAP->FI_MCPWM0_FP);
    log_info("PU = 0x%x\n", JL_PORTA->PU0);
    log_info("PD = 0x%x\n", JL_PORTA->PD0);
    log_info("DIR = 0x%x\n", JL_PORTA->DIR);
    log_info("DIE = 0x%x\n", JL_PORTA->DIE);
    log_info("JL_MCPWM->TMR%d_CON = 0x%x\n", index, timer_reg->tmr_con);
    log_info("JL_MCPWM->TMR%d_CNT = 0x%x\n", index, timer_reg->tmr_cnt);
    log_info("JL_MCPWM->TMR%d_PRD = 0x%x\n", index, timer_reg->tmr_pr);
    log_info("JL_MCPWM->CH%d_CMPH  = 0x%x\n", index, pwm_reg->ch_cmph);
    log_info("JL_MCPWM->CH%d_CMPL  = 0x%x\n", index, pwm_reg->ch_cmpl);
    log_info("JL_MCPWM->CH%d_CON0   =0x%x\n", index, pwm_reg->ch_con0);
    log_info("JL_MCPWM->CH%d_CON1  = 0x%x\n", index, pwm_reg->ch_con1);
    log_info("JL_MCPWM->FPIN_CON   = 0x%x\n", JL_MCPWM->FPIN_CON);
    log_info("JL_MCPWM->MCPWM_CON0 = 0x%x\n", JL_MCPWM->MCPWM_CON0);
}


void io_ext_interrupt_close(u8 index, u8 port)
{
    if (port > IO_PORT_MAX) {
        return;
    }
    gpio_set_die(port, 0);
    gpio_set_direction(port, 1);
    gpio_set_pull_up(port, 0);
    gpio_set_pull_down(port, 0);
    gpio_disable_fun_input_port(PFI_MCPWM0_FP + index);
    PWM_CH_REG *pwm_reg = get_pwm_ch_reg(index);
    pwm_reg->ch_con1 = BIT(14);
}

///////////// 使用举例如下 //////////////////
void my_io_isr_cbfun(u8 index)
{
    log_info("io index --> %d  Hello world !\n", index);
}
void io_ext_interrupt_test()
{
    set_io_ext_interrupt_cbfun(my_io_isr_cbfun);
    io_ext_interrupt_init(0, IO_PORTA_04, 0);
    io_ext_interrupt_init(1, IO_PORTB_01, 0);
    while (1) {
    };
}

