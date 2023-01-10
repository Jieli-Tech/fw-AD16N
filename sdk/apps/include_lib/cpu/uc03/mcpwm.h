#ifndef _MCPWM_H
#define _MCPWM_H

#include "typedef.h"

/* 对齐方式选择 */
typedef enum {
    pwm_edge_aligned,               //边沿对齐模式
    pwm_center_aligned,             //中心对齐模式
} pwm_aligned_mode_type;

/* pwm通道选择 */
typedef enum {
    pwm_ch0,
    pwm_ch1,
    pwm_ch_max,
} pwm_ch_num_type;

/* MCPWM TIMER寄存器 */
typedef struct _pwm_timer_reg {
    volatile u32 tmr_con;
    volatile u32 tmr_cnt;
    volatile u32 tmr_pr;
} PWM_TIMER_REG;


/* MCPWM通道寄存器*/
typedef struct _pwm_ch_reg {
    volatile u32 ch_con0;
    volatile u32 ch_con1;
    volatile u32 ch_cmph;
    volatile u32 ch_cmpl;
} PWM_CH_REG;

/* 初始化要用的参数结构体 */
struct pwm_platform_data {
    pwm_aligned_mode_type pwm_aligned_mode;//PWM对齐方式选择
    pwm_ch_num_type pwm_ch_num;//选择pwm通道号
    u32 frequency;//初始化共同频率，CH0,CH1,CH2.....

    u16 duty;//初始化占空比，0~10000 对应0%~100%
    u8 h_pin;//一个通道的H引脚
    u8 l_pin;//一个通道的L引脚
    u8 complementary_en;//该通道的两个引脚输出的波形。0:同步，1：互补，互补波形的占空比体现在H引脚上
};


#endif















