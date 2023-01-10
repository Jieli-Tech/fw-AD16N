#ifndef __STREAM_FRAME_H__
#define __STREAM_FRAME_H__
#include "typedef.h"


void stream_frame_init(void);
void *regist_stream_channel(void *psound);
bool unregist_stream_channel(void *psound);

#endif

