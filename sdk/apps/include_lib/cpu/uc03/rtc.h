#ifndef __RTC_H__
#define __RTC_H__

#include "typedef.h"


struct sys_time {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
};

enum {
    RTC_WKUP_SRC_256HZ,
    RTC_WKUP_SRC_64HZ,
    RTC_WKUP_SRC_2HZ,
    RTC_WKUP_SRC_1HZ,
};

struct rtc_dev_platform_data {
    struct sys_time *default_sys_time;
    struct sys_time *default_alarm;
    void (*cbfun)(u8);
    void (*timefun)(u8);
    u8 clk_sel;
    u8 x32xs;
};

#define RTC_DEV_PLATFORM_DATA_BEGIN(data) \
	const struct rtc_dev_platform_data data = {

#define RTC_DEV_PLATFORM_DATA_END()  \
    .x32xs = 0, \
};


int rtc_init(const struct rtc_dev_platform_data *arg);
void rtc_disable(void);
void set_alarm_ctrl(u8 set_alarm);
void write_sys_time(struct sys_time *curr_time);
void read_sys_time(struct sys_time *curr_time);
void write_alarm(struct sys_time *alarm_time);
void read_alarm(struct sys_time *alarm_time);
void rtc_wakup_source(void);
void time_wakeup_set(u8 index, u8 en);

u16 month_to_day(u16 year, u8 month);
void day_to_ymd(u16 day, struct sys_time *sys_time);
u16 ymd_to_day(struct sys_time *time);
u8 caculate_weekday_by_time(struct sys_time *r_time);

/**********************************************************
 * 应用接口
 ******/
void read_current_time();   //读取当前时间
void write_clock_time(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec); //修改当前时间
void read_alarm_time();     //读取闹钟设置时间
void write_alarm_time(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec); //修改闹钟时间


#endif // __RTC_API_H__
