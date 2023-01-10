#ifndef _FAT_RESOURCE_H
#define _FAT_RESOURCE_H
#include "fat/ff_opr.h"

FATFS *fat_fshdl_alloc(void);
FATFS *fat_fshdl_free(FATFS *fshdl);
FIL *fat_fhdl_alloc(void);
FIL *fat_fhdl_free(FIL *pfile);

void *fat_lfn_alloc(void);
void fat_lfn_free(void *lfn_buffer);

void *fat_tmp_alloc(void);
void *fat_tmp_free(void *tmp_buf);

FF_APIS *fat_ff_apis_alloc(void);
FF_APIS *fat_ff_apis_free(FF_APIS *ff_apis_buf);

SWIN_BUF *fat_wbuf_alloc(void);
SWIN_BUF *fat_wbuf_free(SWIN_BUF *tmp_wbuf);

VFSCAN *fat_vfscan_alloc(void);
VFSCAN *fat_vfscan_free(VFSCAN *fsn_wbuf);

FAT_SCANDEV *fat_scan_alloc(void);
FAT_SCANDEV *fat_scan_free(FAT_SCANDEV *scan_buf);


#endif
