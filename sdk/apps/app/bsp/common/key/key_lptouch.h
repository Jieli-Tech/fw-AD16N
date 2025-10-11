#ifndef _KEY_LPTOUCH_H
#define _KEY_LPTOUCH_H

#include "key.h"

//滤波算法用到
#define LPTOUCH_VAL_CALIBRATE_CYCLE         100//标定常态值的时间，单位为key_scand的时间
#define LPTOUCH_DELDA                       100//手按下时变化量的阈值，大于阈值说明被按下

void lptouch_key_init(void);

u8 get_lptouch_key_value(void);

extern const key_interface_t key_lptouch_info;

#endif

