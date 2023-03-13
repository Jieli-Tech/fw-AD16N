#ifndef __STREAM_FRAME_H__
#define __STREAM_FRAME_H__
#include "typedef.h"


void stream_frame_init(u32 irq_ip);
void stream_frame_uninit(void);
void *regist_stream_channel(void *psound);
bool unregist_stream_channel(void *psound);
extern const int IRQ_STREAM_IP;
extern const int IRQ_SPEAKER_IP;

#endif

