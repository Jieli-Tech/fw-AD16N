#ifndef __BREAK_POINT_H__
#define __BREAK_POINT_H__

#include "typedef.h"

#define MBOX_USE_VM_MEMORY  1

void clear_music_break_point(u8 dindex);
bool save_music_break_point(u8 dindex, u8 mode);
bool load_music_break_point(u8 dindex, u8 mode);
#endif
