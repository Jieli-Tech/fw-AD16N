
#include "rtc.h"
#include "cpu.h"
#include "asm/power/p33.h"
#include "asm/power/p33_app.h"
#include "asm/power/power_reset.h"
#include "asm/power/power_wakeup.h"
#include "gpio.h"
#include "irq.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[RTC]"
#include "log.h"

void read_current_time()
{
    struct sys_time tmp_time;
    memset((u8 *)&tmp_time, 0, sizeof(tmp_time));
    read_sys_time(&tmp_time); 				//读rtc时间
    log_info("current_rtc_time: %d-%d-%d %d:%d:%d", tmp_time.year, \
             tmp_time.month, \
             tmp_time.day, \
             tmp_time.hour, \
             tmp_time.min, \
             tmp_time.sec);

}
void write_clock_time(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
    struct sys_time tmp_time;
    memset((u8 *)&tmp_time, 0, sizeof(tmp_time));
    tmp_time.year = year;
    tmp_time.month = month;
    tmp_time.day = day;
    tmp_time.hour = hour;
    tmp_time.min = min;
    tmp_time.sec = sec;
    write_sys_time(&tmp_time); 		//修改rtc时间
    log_info("modification time : %d-%d-%d %d:%d:%d", tmp_time.year, \
             tmp_time.month, \
             tmp_time.day, \
             tmp_time.hour, \
             tmp_time.min, \
             tmp_time.sec);
    read_current_time();
}

void read_alarm_time()
{
    struct sys_time tmp_time;
    memset((u8 *)&tmp_time, 0, sizeof(tmp_time));
    read_alarm(&tmp_time); 				//读rtc时间
    log_info("current_rtc_time: %d-%d-%d %d:%d:%d", tmp_time.year, \
             tmp_time.month, \
             tmp_time.day, \
             tmp_time.hour, \
             tmp_time.min, \
             tmp_time.sec);

}
void write_alarm_time(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
    struct sys_time tmp_time;
    memset((u8 *)&tmp_time, 0, sizeof(tmp_time));
    tmp_time.year = year;
    tmp_time.month = month;
    tmp_time.day = day;
    tmp_time.hour = hour;
    tmp_time.min = min;
    tmp_time.sec = sec;
    write_alarm(&tmp_time); 		//修改rtc时间
    log_info("modification time : %d-%d-%d %d:%d:%d", tmp_time.year, \
             tmp_time.month, \
             tmp_time.day, \
             tmp_time.hour, \
             tmp_time.min, \
             tmp_time.sec);
    read_alarm_time();
}

