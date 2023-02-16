#ifndef _PL_CNT_H_
#define _PL_CNT_H_

#include "gpio.h"

struct pl_cnt_platform_data {
    u8 port_num;    //引脚个数
    u8 *port;       //放电计数的IO存储的地址
    u8 sum_num;     //重复计数的次数，用于求平均值
    u8 charge_time; //充电时间，要保证寄生电容能充满电
};

#define PL_CNT_PLATFORM_DATA_BEGIN(data) \
    static const struct pl_cnt_platform_data data = {

#define PL_CNT_PLATFORM_DATA_END() \
};

void pl_cnt_init(const struct pl_cnt_platform_data *pdata);
u32 get_pl_cnt_value(u8 ch);

#endif

