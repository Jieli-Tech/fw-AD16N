
#ifndef __REMAIN_OUTPUT_H__
#define __REMAIN_OUTPUT_H__

#include "typedef.h"

typedef struct __remain_ops {
    u32(*output)(void *priv, void *data, int len);
    void *obuf;     //outdata
    u32 len;        //outdata大小
    u32 remain_len; //剩余输出长度
    u32 output_len; //已经输出长度
} remain_ops;

u32 init_remain_obuf(remain_ops *p_ops, void *output, void *obuf, u32 len);
u32 remain_output(void *priv, remain_ops *p_ops);
u32 set_remain_len(remain_ops *p_ops, u32 len);
#endif




