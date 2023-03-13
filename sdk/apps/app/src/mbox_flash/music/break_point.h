#ifndef __BREAK_POINT_H__
#define __BREAK_POINT_H__

#include "typedef.h"

#define MUSIC_BREAKPOINT_ENABLE 1

#if MUSIC_BREAKPOINT_ENABLE
void clear_music_break_point_phy(u8 dindex);
bool save_music_break_point_phy(u8 dindex, u8 mode);
bool load_music_break_point_phy(u8 dindex, u8 mode);
#define clear_music_break_point(n)  clear_music_break_point_phy(n)
#define save_music_break_point(n,m) save_music_break_point_phy(n,m)
#define load_music_break_point(n,m) load_music_break_point_phy(n,m)
#else
#define clear_music_break_point(...)
#define save_music_break_point(...)     0
#define load_music_break_point(...)     0
#endif

#endif
