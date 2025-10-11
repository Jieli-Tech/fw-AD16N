#ifndef TICK_TIMER_H
#define TICK_TIMER_H
#include "config.h"

extern volatile u32 jiffies;
#define maskrom_get_jiffies() jiffies
#define maskrom_set_jiffies(n) jiffies = n

extern volatile u32 jiffies_2ms;
#define maskrom_get_jiffies_2ms() jiffies_2ms
#define maskrom_set_jiffies_2ms(n) jiffies_2ms = n
extern void tick_timer_init(void);
// void tick_timer_sleep_init();
// extern void maskrom_update_jiffies(void);
// extern void maskrom_set_jiffies(u32 cnt);
// extern u32 maskrom_get_jiffies(void);
// void set_jiffies(u32 cnt);
// u32 get_jiffies(void);
// void delay_10ms(u32 tick);
// void os_time_dly(u32 tick);
// extern __attribute__((weak)) void tick_timer_set_on(void);
// extern __attribute__((weak)) void tick_timer_set_off(void);
// extern __attribute__((weak)) bool tick_timer_close(void);

#define COUNTDOWN_BY_JIFFIES(last_time, count_down)  \
	do { \
		u32 cur_jiffies = maskrom_get_jiffies(); \
		u32 pass_jiffies = cur_jiffies - last_time; \
		last_time = cur_jiffies; \
		if (count_down <= pass_jiffies) {  \
			count_down = 0; \
		} else {   \
			count_down -= pass_jiffies; \
		} \
	} while(0);

extern __attribute__((weak)) void tick_timer_set_on(void);
extern __attribute__((weak)) void tick_timer_set_off(void);
extern __attribute__((weak)) bool tick_timer_close(void);

#endif
