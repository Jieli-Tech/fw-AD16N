#ifndef _VFS_FAT_H_
#define _VFS_FAT_H_

#include "fat/ff_opr.h"
#include "fat/fat_resource.h"
#include "typedef.h"

typedef struct vfscan_reset_info {
    // struct vfscan *fsn; //扫描句柄
    u16 file_total; // 当前设备文件总数
    u16 dir_total;// 当前设备文件夹总数
    u8 active; //当前设备是否有效，无效需要扫描。
    u8 scan_over; //当前设备之前是否扫描过，没有需要扫描。
} VFSCAN_RESET_INFO;

int vfs_get_fsize(void *pvfile, void *parm);
int vfs_ftell(void *pvfile, void *parm);
u32 vfs_file_delete(void *pvfile);
struct vfscan *vfs_fscan_new(void *pvfs, const char *path, const char *arg, u8 max_deepth, int (*callback)(void), struct vfscan *fsn, struct vfscan_reset_info *info);
struct vfscan *vfs_fscan(void *pvfs, const char *path, const char *arg, u8 max_deepth, int (*callback)(void));
void vfs_fscan_release(void *pvfs, struct vfscan *fs);
int vfs_select(void *pvfs, void **ppvfile, struct vfscan *fs, int sel_mode, int arg);
int vfs_mk_dir(void *pvfs, char *folder, u8 mode);
int vfs_get_encfolder_info(void *pvfs, char *folder, char *ext, u32 *last_num, u32 *total_num);
int vfs_get_folderinfo(void *pvfile, struct vfscan *fs, int *start_num, int *end_num);
void __fscan_arg_handler(struct vfscan *fs, const char *arg);
/* --------------------------------------------------------------------------*/
/**
 * @brief  格式化接口
 *
 * @param ppvfs: vfs指针 , 使用格式化接口之后vfs指针会被free，需要重新mount后获取, 如果ppvfs == NULL， clust_size 不为0
 * @param dev_name : 设备名, 例如 “sd0”
 * @param type : 文件系统类型， 一般为“fat”
 * @param clust_size : 簇大小，如果填0 则使用原来簇大小，如果create_new == 1时，不能填0 例如：4 * 1024、8 *1024、16*1024、32*1024、64*1024
 * @param create_new : 0 表示维持原来的文件系统类型， 1 表示新建文件系统类型，当设备中没有文件系统的时候使用1
 *
 * @return  0成功，其他失败
 * 注：vfs指针 , 使用格式化接口之后vfs指针会被free，需要重新mount后获取
 */
/* ----------------------------------------------------------------------------*/
int vfs_format(void **ppvfs, const char *dev_name, const char *type, u32 clust_size, u8 create_new);

int vfs_fget_path(void *pvfile, struct vfscan *fscan, u8 *name, int len, u8 is_relative_path);

#endif

