#ifndef __RTC_MODE_H__
#define __RTC_MODE_H__
#include "typedef.h"
#include "app.h"
#include "key.h"

void rtc_app(void);         //模式1，展示功能
void rtc_timed_wakeup_app();//模式2，定时唤醒进入powerdown
extern u16 rtc_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
#endif

