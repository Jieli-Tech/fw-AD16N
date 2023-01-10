#ifndef _MBR_H_
#define _MBR_H_

#include "typedef.h"

#define MBR_DRV_MAX				4		//扫描最大的分区数
#define MBR_MALLOC				0		//允许使用malloc

typedef enum {
    MBR_OK = 0,
    MBR_FAIL = 0x80,
    MBR_DISK_ERROR,
    MBR_OK_REACH_FULL,
    MBR_OK_NO_SEL_DRIVE,
    MBR_OK_MBR_MABE,
} MRESULT;

typedef struct _MBR_WIN_BUF {
    u8  start[512];
    u32  sector;
    struct _MBR_FS  *mbr;
    u8   flag;
} MBR_WIN_BUF;

typedef struct _MBR_DRIVE_INFO {
    u32 drive_max;						//配置最大分区数量
    u32 drive_cnt;						//获取实际分区数量
    u32 drive_boot_sect[MBR_DRV_MAX];	//获取实际分区的boot_sect
} MBR_DRIVE_INFO;

typedef struct _MBR_FS {
    u32(*disk_read)(void *hdev, u8 *buf, u32 lba);		/* device read function */
    u32(*disk_write)(void *hdev, u8 *buf, u32 lba);		/* device write function */
    void 	*hdev;

    MBR_WIN_BUF win;		        /* Disk access window for Directory/FAT/File */
    u8		sector_512size;			/* size of per sector */

    MBR_DRIVE_INFO *inf;
} MBR_FS;

MRESULT mbr_scan(MBR_FS *mbr, u32 bootsect, u8 drive_sel);
u32 __dev_read(void *p, u8 *buf, u32 addr);
u32 __dev_write(void *p, u8 *buf, u32 addr);
int mbr_scan_parition(MBR_DRIVE_INFO *mbr_inf, void *device);

#endif

