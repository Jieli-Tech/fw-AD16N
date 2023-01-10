#ifndef _FAT_IO_H_
#define _FAT_IO_H_

#include "typedef.h"
#include "stdarg.h"
#include "vfs.h"

#define FAT_IO_DEBUG

#ifdef FAT_IO_DEBUG
#define fat_io_deg             log_info
#define fat_io_deg_puts        log_info
#define fat_io_deg_buf         log_info
#else
#define fat_io_deg(...)
#define fat_io_deg_puts(...)
#define fat_io_deg_buf(...)
#endif

/*----------------------------------------------------------------------------*/
/**
  								FILE_SYSTEM_PARM
*/
/*----------------------------------------------------------------------------*/
typedef struct __fs_dev_info {
    void *hdl;			//device handle
    void *read_p;		//
    void *write_p;	//
    void *io_ctrl_p;
    u32 drive_base;		//patition info
    u32 block_size;	//device block info
    void *eraser_p;
    u32 drive_end;
} _FS_DEV_INFO;

/*----------------------------------------------------------------------------*/
/**
  								FILE_SYSTEM_STRUCT
*/
/*----------------------------------------------------------------------------*/
typedef enum {
    FS_IO_MKDIR = 0,
    FS_IO_GET_FOLDER_FILE,
    FS_IO_CHANGE_FOLDER,

    FS_IO_GET_FILE_NAME,
    FS_IO_GET_FILE_SIZE,
    FS_IO_GET_FILE_SCLUST,
    FS_IO_GET_FILE_NUMBER,
    FS_IO_GET_FILE_INFO,
} FS_IO_CMD;


//init
s32 fat_drive_open(void **p_fs_hdl, void *p_fs_dev_info);
s32 fat_drive_close(void **p_fs_hdl);

/******io_function******/
//file_function
s32 fat_open(void *p_fs_hdl, void **p_f_hdl, char *path, const char *mode);
s32 fat_close(void **pp_f_hdl);
u32 fat_seek(void *p_f_hdl, u8 type, u32 offsize);
u32 fat_read(void *p_f_hdl, u8 _xdata *buff, u16 len);
u32 fat_write(void *p_f_hdl, u8 _xdata *buff, u16 len);
s32 fat_delete(void *p_f_hdl);

//folder_function
s32 fat_mk_dir(void *p_fs_hdl, char *path, u32 mode);
// u32 fat_folder_file(void *p_f_hdl, int *start_file, int *end_file);
u32 fat_folder_file(struct vfscan *fscan, int *start_file, int *end_file);
s32 fat_get_filenum_byfolder(void *p_f_hdl, u8 mode, u32 *total_file, u32 *curr_file);

//file_function
// u32 fat_get_file_byindex(void *p_fs_hdl, void **p_f_hdl, u32 file_number);
// s32 fat_get_file_bypath(void *p_fs_hdl, void **p_f_hdl, u8 *path);
// u32 fat_get_file_bysclust(void *p_fs_hdl, void **p_f_hdl, u32 sclust);
u32 fat_get_file_byfile(void *pcvfile, void **p_f_hdl, void *ext_name);

//file_info_function
int fat_fscan(struct vfscan *fscan, void *p_fs_hdl, const char *path, u8 max_deepth, int (*callback)(void));
void fat_fscan_release(struct vfscan *fscan);
u32 fat_fsel(struct vfscan *fsn, void *p_fs_hdl, int sel_mode, void **p_f_hdl, int arg);

u32 fat_get_name(void *p_f_hdl, char *name, u32 len);
u32 fat_get_fsize(void *p_f_hdl, u32 *parm);
u32 fat_get_ftell(void *p_f_hdl, u32 *parm);
u32 fat_get_attrs(void *p_f_hdl, struct vfs_attr *attr);

u32 fat_io_ctrl(void *f_p, u32 cmd, int arg);

void fs_ext_setting(const char *str);  //AT(FF_API_C_API)

//base init
void fat_init(void);
// void fat_set_lfn_buf(void *buf);
//0成功  n:需要的ram大小, 如果membuf不符合条件将使用默认的RAM
// u32 fat_mem_init(u8 *membuf, u32 size);
#endif
