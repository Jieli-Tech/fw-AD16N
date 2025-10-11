#ifndef __IIR_H__
#define __IIR_H__

// #include <stdio.h>
#include "typedef.h"

// 一阶低通IIR滤波器结构体
typedef struct {
    int alpha;   // 截止频率控制参数
    int y_prev;  // 上一时刻的输出
} LowPassFilter;

// 初始化一阶低通IIR滤波器
void initLowPassFilter(LowPassFilter *filter, int alpha);
// 应用一阶低通IIR滤波器
int applyLowPassFilter(LowPassFilter *filter, int input);



#endif
