#ifndef  __GPTIMER_H__
#define  __GPTIMER_H__

#include "cpu.h"
#include "gptimer_hw.h"
#include "gpio.h"

#define TIMER_LOG_ENABLE    0
#if TIMER_LOG_ENABLE
#define timer_print(arg, ...)     printf(arg, ##__VA_ARGS__)
#else
#define timer_print(arg, ...)
#endif

enum GPTIMER_MODE {
    GPTIMER_MODE_TIMER,
    GPTIMER_MODE_PWM,
    GPTIMER_CAPTRUE_EDGE_RISE, //上升沿
    GPTIMER_CAPTRUE_EDGE_FAILL, //下降沿
    GPTIMER_CAPTRUE_EDGE_ANYEDGE, //双边沿
    // GPTIMER_MODE_COUNTER,
};
typedef void (*timer_irq_callback)(int tid); //回调函数
struct gptimer_config { //计时（计数）器配置
    u32 resolution_us;
    timer_irq_callback irq_cb;
    int tid;
    u16 irq_priority;
};
struct gptimer_pwm_config { //pwm配置
    enum gpio_port port;
    u32 pin;
    u32 freq;
    u32 pwm_duty_X10000;
    int tid;
};

struct gptimer_capture_config { //捕获配置
    enum gpio_port port;
    u32 pin;
    enum GPTIMER_MODE edge_type;
    u32 filter; //需要滤除的频率，单位 Hz
    timer_irq_callback irq_cb;
    int tid;
    u16 irq_priority;
};

struct gptimer_info_t {
    enum GPTIMER_MODE mode; //工作模式
    struct gptimer_config gt_tim_cfg;
    struct gptimer_pwm_config gt_pwm_cfg;
    struct gptimer_capture_config gt_cap_cfg;
    u32 timerx; //存放 JL_TIMERx 寄存器地址
};

//以下为对外 api 接口---------------------------------------------------------
int gptimer_init(const struct gptimer_config *gptimer);
int gptimer_pwm_init(const struct gptimer_pwm_config *gptimer);
int gptimer_capture_init(const struct gptimer_capture_config *gptimer);
int gptimer_deinit(int tid);
int gptimer_start(int tid);
int gptimer_pause(int tid);
int gptimer_resume(int tid);
u32 gptimer_set_resolution(int tid, u32 resolution_us);
u32 gptimer_get_resolution(int tid);
u32 gptimer_set_pwm_freq(int tid, u32 freq);
u32 gptimer_get_pwm_freq(int tid);
u32 gptimer_set_pwm_duty(int tid, u32 duty);
u32 gptimer_get_pwm_duty(int tid);
u32 gptimer_pwm_flip(int tid);
u32 gptimer_pwm_enable(int tid);
u32 gptimer_pwm_disable(int tid);
u32 gptimer_set_capture_edge_type(int tid, enum GPTIMER_MODE edge_type);
enum GPTIMER_MODE gptimer_get_capture_edge_type(int tid);
u32 gptimer_set_irq_callback(int tid, void (*irq_cb)(int tid));
enum GPTIMER_MODE gptimer_get_work_mode(int tid);
u32 gptimer_set_work_mode(int tid, enum GPTIMER_MODE type);
u32 gptimer_set_capture_filter(int tid, u32 filter);
u32 gptimer_get_capture_filter(int tid);
u32 gptimer_get_capture_count(int tid);
u32 gptimer_tick2us(int tid, u32 tick);
u32 gptimer_get_count(int tid);
u32 gptimer_set_count(int tid, u32 cnt);
u32 gptimer_get_prd(int tid);
void gptimer_dump();



//以下为底层驱动接口，直接操作寄存器---------------------------------------------------------
u32 timer_hw_init(JL_TIMER_TypeDef *JL_TIMERx, u32 us, u16 priority, void (*irq_cb)(int tid));
u32 timer_hw_pwm_init(JL_TIMER_TypeDef *JL_TIMERx, enum gpio_port port, u32 pin, u32 freq, u32 duty);
u32 timer_hw_capture_init(JL_TIMER_TypeDef *JL_TIMERx, enum gpio_port port, u32 pin, u32 edge_type, u32 filter, u16 priority, void (*irq_cb)(int tid));
void timer_hw_deinit(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_start(JL_TIMER_TypeDef *JL_TIMERx, u16 type);
void timer_hw_pause(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_resume(JL_TIMER_TypeDef *JL_TIMERx);
u32 timer_hw_set_resolution(JL_TIMER_TypeDef *JL_TIMERx, u32 us);
u32 timer_hw_get_resolution(JL_TIMER_TypeDef *JL_TIMERx, u32 tick);
u32 timer_hw_set_pwm_freq(JL_TIMER_TypeDef *JL_TIMERx, u32 freq, u32 duty);
u32 timer_hw_get_pwm_freq(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_set_pwm_duty(JL_TIMER_TypeDef *JL_TIMERx, u32 freq, u32 duty);
u32 timer_hw_get_pwm_duty(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_pwm_flip(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_pwm_enable(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_pwm_disable(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_set_capture_edge_type(JL_TIMER_TypeDef *JL_TIMERx, enum GPTIMER_MODE edge_type);
enum GPTIMER_MODE timer_hw_get_capture_edge_type(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_set_irq_cb(JL_TIMER_TypeDef *JL_TIMERx, void (*irq_cb)(int tid));
enum GPTIMER_MODE timer_hw_get_work_mode(JL_TIMER_TypeDef *JL_TIMERx);
u32 timer_hw_get_capture_count(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_set_count(JL_TIMER_TypeDef *JL_TIMERx, int cnt);
u32 timer_hw_set_capture_filter(JL_TIMER_TypeDef *JL_TIMERx, u32 filter, enum GPTIMER_MODE mode);
u32 timer_hw_get_capture_filter(JL_TIMER_TypeDef *JL_TIMERx);
void timer_hw_disable_filter(JL_TIMER_TypeDef *JL_TIMERx);
u32 timer_hw_get_count(JL_TIMER_TypeDef *JL_TIMERx);
u32 timer_hw_get_prd(JL_TIMER_TypeDef *JL_TIMERx);

u32 timer_hw_set_clk(u8 *clk_src_set, u8 *clk_div_set, u32 para, enum GPTIMER_MODE mode);
u32 hw_irflt_set_clk(u8 *clk_src_set, u8 *clk_div_set, u32 filter);
u32 timer_hw_tid2timer(int tid);
void timer_hw_clr_pnd(int tid);
void timer_hw_set_work_mode(JL_TIMER_TypeDef *JL_TIMERx, enum GPTIMER_MODE type);


void Youbaibai_timer_test(void);

#endif

