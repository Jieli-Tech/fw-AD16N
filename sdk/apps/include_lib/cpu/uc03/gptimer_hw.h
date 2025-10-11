#ifndef  __GPTIMER_HW_H__
#define  __GPTIMER_HW_H__

#define TIMER_MAX_NUM   3
#define TIMER_BASE_ADDR JL_TIMER0
#define TIMER_OFFSET    (JL_TIMER1 - JL_TIMER0)
#define TIMER_PRD_SIZE  0xFFFFFFFF

/* JL_TIMERx->CON */
#define TIMER_MODE              0b11
//工作模式选择
#define TIMER_MODE_DISABLE      (0b00 << 0) //关闭TIMER功能
#define TIMER_MODE_TIME         (0b01 << 0) //定时、计数模式
#define TIMER_MODE_RISE         (0b10 << 0) //上升沿捕获
#define TIMER_MODE_FALL         (0b11 << 0) //下降沿捕获
//捕获模式端口选择
#define TIMER_CESL_IO           (0b00 << 2) //IO口输入
#define TIMER_CESL_IRFLT        (0b01 << 2) //IRFLT滤波
//时钟源和分频系数偏移
#define TIMER_PSET              4 //分频系数
#define TIMER_SSEL              10 //时钟源
//时钟源和分频系数所占位
#define TIMER_PSET_             0xf0 //分频系数
#define TIMER_SSEL_             0x3c00 //时钟源
//PWM使能
#define TIMER_PWM_EN		    (0b1 << 8)	//PWM输出使能
//PWM输出反向使能
#define TIMER_PWM_INV		    (0b1 << 9)	//PWM输出反向使能
//清PND
#define TIMER_PCLR              (0b1 << 14) //清PND 只写位
//PND
#define TIMER_PND               (0b1 << 15) //清PND 只读位
//双边沿捕获使能
#define TIMER_DUAL_EDGE_EN      (0b1 << 16) //使能双边沿捕获模式,上升沿或下降沿捕获模式使能时有效
//PWM输出默认值使能
// #define TIMER_PWM_CFG           (0b1 << 17) //使能PWM输出默认值
// #define TIMER_PWM_INIT          (0b1 << 18) //输出默认值, 先写 TIMER_PWM_CFG,再写TIMER_PWM_INIT

/* JL_IR->RFLT_CON */
//IRFLT模块使能
#define IRFLT_EN          (0b1 << 0)  //使能红外滤波功能
//时钟源和分频系数偏移
#define IRFLT_TSRC              2 //时钟源
#define IRFLT_PSEL              4 //分频系数
//时钟源和分频系数所占位
#define IRFLT_TSRC_             0xc //时钟源
#define IRFLT_PSEL_             0xf0 //分频系数

#endif


