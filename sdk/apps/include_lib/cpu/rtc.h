#ifndef __RTC_H__
#define __RTC_H__

#include "typedef.h"

#define CLK_SEL_LRC     1
#define CLK_SEL_32K     2
#define CLK_SEL_BTOSC   3

enum KEEP_RTC_CLK {
    KEEP_CLK_LRC = 1,
    KEEP_CLK_32K,
    KEEP_CLK_BTOSC,
};

enum RTC_SEL {
    HW_RTC,
    VIR_RTC,
};


struct rtc_config_init {
    struct sys_time *default_sys_time;
    struct sys_time *default_alarm;
    u32 rtc_clk;
    void (*cbfun)(void);
    void (*timefun)(u8);    //仅uc03使用
    enum RTC_SEL rtc_sel;   //仅bd49使用
    u8 timer_wkup_en;       //仅bd49使用
    u8 alm_en;
};

struct sys_time {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
} _GNU_PACKED_;

//API


void rtc_dev_init(const struct rtc_config_init *rtc);
void rtc_dev_disable(void);
void rtc_read_time(struct sys_time *time);
void rtc_write_time(struct sys_time *time);
void rtc_read_alarm(struct sys_time *time);
void rtc_write_alarm(struct sys_time *time);
void rtc_debug_dump(void);
void rtc_alarm_en(u32 en);
u32 rtc_get_alarm_en(void);


bool leapyear(u32 year); //判断是否为闰年
u32 year_to_day(u32 year);
u32 month_to_day(u32 year, u32 month);
void day_to_ymd(u32 day, struct sys_time *sys_time);
u32 ymd_to_day(struct sys_time *time);
u32 caculate_weekday_by_time(struct sys_time *r_time); //计算当天为星期几
u32 get_day_of_month(u32 year, u32 month); //返回每月的天数

//bd49使用
void set_time_wakeup_soff(u32 clk_sel, u32 sec);//softoff定时唤醒配置，clk_sel为时钟源，sec单位:ms
void time_wakeup_soff_disable(void);
u32 rtc_get_alm_wkup(void); //获取是否为闹钟唤醒



//uc03 使用
#define MEM_OLD_RTC_TIME         0
#define MEM_OLD_REAL_TIME        1
#define MEM_RTC_ALARM_TIME       2

enum {
    RTC_WKUP_SRC_256HZ,
    RTC_WKUP_SRC_64HZ,
    RTC_WKUP_SRC_2HZ,
    RTC_WKUP_SRC_1HZ,
};

void rtc_wakup_source(void);
void time_wakeup_set(u8 index, u8 en);


/**********************************************************
 * 应用接口
 ******/
void read_current_time();   //读取当前时间
void write_clock_time(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec); //修改当前时间
void read_alarm_time();     //读取闹钟设置时间
void write_alarm_time(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec); //修改闹钟时间


#endif // __RTC_API_H__
