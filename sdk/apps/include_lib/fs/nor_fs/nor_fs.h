/***********************************Jieli tech************************************************
  File : nor_fs.h
  By   : Huxi
  Email: xi_hu@zh-jieli.com
  date : 2016-11-30 14:30
********************************************************************************************/
#ifndef _NOR_FS_H_
#define _NOR_FS_H_

// #include "sdk_cfg.h"
#include "typedef.h"



#define NORFS_MAGIC_NUM		0X11223344
#define REC_FILE_END 0xFE


//文件索引
typedef struct __RECF_INDEX_INFO {
    u32 index;  //文件索引号
    u16 sector; //文件所在扇区
} RECF_INDEX_INFO ;

#define FLASH_PAGE_SIZE 256
#define NORF_SECTOR_SIZE   FLASH_PAGE_SIZE

//文件系统句柄
typedef struct __RECFILESYSTEM {
    RECF_INDEX_INFO index;
    // u8 buf[FLASH_PAGE_SIZE];
    u16 total_file;
    u16 first_sector;
    u16 last_sector;
    // u8 *buf;
    u8 sector_bits;
    //void (*eraser)(u32 address);
    //s32(*read)(u8 *buf, u32 addr, u32 len);
    //s32(*write)(u8 *buf, u32 addr, u32 len);
    // s32(*ctrl_api)(void *dev, u32 cmd, void *parm);
    // u32(*disk_read)(void *hdev, u8 _xdata *buf, u32 lba);		[> device read function <]
    // u32(*disk_write)(void *hdev, u8 _xdata *buf, u32 lba);		[> device write function <]
    void	*hdev;
} NORFS, RECFILESYSTEM, *PRECFILESYSTEM ;




//文件句柄
typedef struct __REC_FILE {
    RECF_INDEX_INFO index;
    RECFILESYSTEM *pfs;
    u32 len;
    u32 w_len;
    u32 rw_p;
    u16 sr;
    char name[16];
} NORFILE, REC_FILE;

typedef struct {
    u32 norfs_magic_num;
    u16 first_sector;
    u16 last_sector;
    u8 sector_size;
} norfs_flash_info_t;


void norfs_init_api(void);

void norfs_init(u32 sector_start, u32 sector_end, u32 sector_bit);
u32 norfs_seek(REC_FILE *pfile, u32 offsize, u32 type);
u16 norfs_read(REC_FILE *pfile, u8 *buff, u16 btr);
u16 norfs_write(REC_FILE *pfile, u8 *buff, u16 btw);
u32 norfs_createfile(RECFILESYSTEM *pfs, REC_FILE **ppfile, u32 *pindex);
u32 norfs_closefile(REC_FILE **ppfile);
void norfs_save_sr(REC_FILE *pfile, u16 sr);
u32 norfs_mount(RECFILESYSTEM **ppfs, void *p_device);
u32 norfs_openbyindex(RECFILESYSTEM *pfs, REC_FILE **ppfile, u32 index);
u32 norfs_filelen(REC_FILE *pfile, u32 *plen);
int norfs_ioctl(REC_FILE *pfile, int cmd, int arg);
u32 norfs_name(REC_FILE *pfile, char *name, u32 len);


#endif
