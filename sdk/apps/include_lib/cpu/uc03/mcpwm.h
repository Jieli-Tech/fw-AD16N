#ifndef _MCPWM_H_
#define _MCPWM_H_

#include "typedef.h"

#define MCPWM_NUM_MAX           2
#define MCPWM_CH_MAX            2
#define MCPWM_TMR_BASE_ADDR     (&JL_MCPWM->TMR0_CON)
#define MCPWM_TMR_OFFSET        (&JL_MCPWM->TMR1_CON - &JL_MCPWM->TMR0_CON)
#define MCPWM_CH_BASE_ADDR      (&JL_MCPWM->CH0_CON0)
#define MCPWM_CH_OFFSET         (&JL_MCPWM->CH1_CON0 - &JL_MCPWM->CH0_CON0)

//TMRx_CON reg
#define MCPWM_TMR_INCF  15
// #define MCPWM_TMR_RESERVE  14
#define MCPWM_TMR_UFPND 13
#define MCPWM_TMR_OFPN  12
#define MCPWM_TMR_UFCLR 11
#define MCPWM_TMR_OFCLR 10
#define MCPWM_TMR_UFIE  9
#define MCPWM_TMR_OFTE  8
#define MCPWM_TMR_CKSRC 7
#define MCPWM_TMR_CKPS  3 //4bit
// #define MCPWM_TMR_RESERVE  2
#define MCPWM_TMR_MODE  0 //2bit

//CHx_CON0 reg
#define MCPWM_CH_DTCKPS 12 //4bit
#define MCPWM_CH_DTPR   7 //5bit
#define MCPWM_CH_DTEN   6
#define MCPWM_CH_L_INV  5
#define MCPWM_CH_H_INV  4
#define MCPWM_CH_L_EN   3
#define MCPWM_CH_H_EN   2
#define MCPWM_CH_CMP_LD 0 //2bit

//CHx_CON1 reg
#define MCPWM_CH_FPND   15
#define MCPWM_CH_FCLR   14
// #define MCPWM_CH_RESERVE   12 //2bit
#define MCPWM_CH_INTEN  11
#define MCPWM_CH_TMRSEL 8 //3bit
// #define MCPWM_CH_reserve 5 //3bit
#define MCPWM_CH_FPINEN 4
#define MCPWM_CH_FPINAUTO   3
#define MCPWM_CH_FPINSEL    0 //3bit

//FPIN_CON rg
#define MCPWM_FPIN_EDGE  16 //8bit
#define MCPWM_FPIN_FLT_EN  8 //8bit
// #define MCPWM_CH_reserve 6 //2bit
#define MCPWM_FPIN_FLT_PR  8 //5bit

//MCPWM_CON reg
#define MCPWM_CON_CLK_EN    16
#define MCPWM_CON_TMR_EN    8 //8bit
#define MCPWM_CON_PWM_EN    0 //8bit2


/* pwm通道选择 */
typedef enum {
    MCPWM_CH0 = 0,
    MCPWM_CH1,
} mcpwm_ch_type;

/* 对齐方式选择 */
typedef enum {
    MCPWM_EDGE_ALIGNED,  ///< 边沿对齐模式
    MCPWM_CENTER_ALIGNED, ///< 中心对齐模式
} mcpwm_aligned_mode_type;

/* 故障保护触发边沿 */
typedef enum {
    MCPWM_EDGE_FAILL = 0, //下降沿触发
    MCPWM_EDGE_RISE,  //上升沿触发
    MCPWM_EDGE_DEFAULT = 0xff, //默认会忽略
} mcpwm_edge;

/* MCPWM通道寄存器 */
typedef struct _mcpwm_ch_reg {
    volatile u32 ch_con0;
    volatile u32 ch_con1;
    volatile u32 ch_cmph;
    volatile u32 ch_cmpl;
} MCPWM_CHx_REG;

/* MCPWM TIMER寄存器 */
typedef struct _mcpwm_timer_reg {
    volatile u32 tmr_con;
    volatile u32 tmr_cnt;
    volatile u32 tmr_pr;
} MCPWM_TIMERx_REG;


/* 初始化要用的参数结构体 */
typedef void (*mcpwm_detect_irq_callback)(u32 ch); //回调函数
struct mcpwm_config {
    mcpwm_ch_type ch;                         ///< 选择pwm通道号
    mcpwm_aligned_mode_type aligned_mode;             ///< PWM对齐方式选择
    u32 frequency;                               		///< 初始共同频率，CH0, CH, CH2,,,,,,
    u16 duty;                                           ///< 初始占空比，0~10000 对应 0%~100% 。每个通道可以有不同的占空比。互补模式的占空比体现在高引脚的波形上。
    u16 h_pin;                                           ///< 一个通道的H引脚。
    u16 l_pin;                                           ///< 一个通道的L引脚，不需要则填-1
    u8 complementary_en;                                ///< 该通道的两个引脚输出的波形。0: 同步， 1: 互补，互补波形的占空比体现在H引脚上
    u16 detect_port;
    mcpwm_edge edge;
    mcpwm_detect_irq_callback irq_cb;
    u16 irq_priority; //默认值优先级1
};

struct mcpwm_info_t {
    MCPWM_CHx_REG *ch_reg;
    MCPWM_TIMERx_REG *timer_reg;
    struct mcpwm_config cfg;
};


int mcpwm_init(struct mcpwm_config *mcpwm_cfg);
void mcpwm_deinit(int mcpwm_cfg_id);
void mcpwm_start(int mcpwm_cfg_id);
void mcpwm_pause(int mcpwm_cfg_id);
void mcpwm_resume(int mcpwm_cfg_id);
void mcpwm_set_frequency(int mcpwm_cfg_id, mcpwm_aligned_mode_type align, u32 frequency);
void mcpwm_set_duty(int mcpwm_cfg_id, u16 duty);
void mcpwm_fpnd_clr(u32 ch);


#endif


