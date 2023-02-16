#ifndef _KEY_TOUCH_H
#define _KEY_TOUCH_H
#include "key.h"
#include "gpio.h"

#define TOUCH_KEY_INIT                  TOUCH_KEY_IO_SEL
//滤波算法用到
#define TOUCH_VAL_CALIBRATE_CYCLE       100     //标定常态值的时间，单位为key_scand的时间
#define TOUCH_DELDA                     100     //手按下时变化量的阈值，大于阈值说明被按下

void touch_key_init(void);
u8 key_filter(u8 key);
u8 get_touch_key_value(void);
#endif
