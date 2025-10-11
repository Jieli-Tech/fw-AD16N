#ifndef TICK_TIMER_H
#define TICK_TIMER_H
#include "config.h"

extern volatile u32 jiffies;
extern void tick_timer_init(void);
void tick_timer_sleep_init();
extern void maskrom_update_jiffies(void);
extern void maskrom_set_jiffies(u32 cnt);
extern u32 maskrom_get_jiffies(void);
void set_jiffies(u32 cnt);
u32 get_jiffies(void);
void delay_10ms(u32 tick);
void os_time_dly(u32 tick);
extern __attribute__((weak)) void tick_timer_set_on(void);
extern __attribute__((weak)) void tick_timer_set_off(void);
extern __attribute__((weak)) bool tick_timer_close(void);

#endif
