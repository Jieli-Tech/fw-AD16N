
#ifndef __ANS_API_H__
#define __ANS_API_H__
#include "typedef.h"

// ..note ::
// ANS降噪算法最低系统时钟需要跑107M以上
// 只支持16k 和 8k采样率, 8k采样率暂不建议使用
//

void *link_ans_sound(void *p_sound_out, void *p_ans_obuf, u32 sr);
#endif

