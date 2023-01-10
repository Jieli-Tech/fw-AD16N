#ifndef __MUSIC_DEVICE_H__
#define __MUSIC_DEVICE_H__
#include "typedef.h"
#include "device_app.h"
#include "music_play.h"
#include "vfs.h"

extern struct vfscan *fsn_music;//设备扫描句柄
extern struct vfscan_reset_info dev_scan_info[MAX_DEVICE];

void fs_dev_close(void *ppctl);
u32 device_switch(u32 *p_index, s32 dir);
bool music_new_disk_in(u32 index);
bool decoder_disk_out(u32 disconnect_index);
u32 device_mount(play_control *ppctl, void *scan_info);
int fs_get_folderinfo(int *start_num, int *end_num);
#endif
