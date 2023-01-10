#include "vfs_fat.h"
#include "vfs.h"
#include "errno-base.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

int vfs_get_fsize(void *pvfile, void *parm)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->flen)) {
        u32 res;
        return ops->flen(p_vfile->pfile, (u32 *)parm);
    }
    return 0;
}

int vfs_ftell(void *pvfile, void *parm)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ftell)) {
        u32 res;
        return ops->ftell(p_vfile->pfile, (u32 *)parm);
    }
    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief 文件删除
 *
 * @param pvfile  文件句柄
 * @note SH系列文件关闭在外面应用，删除接口里面不处理
 *
 * @return 0成功
 */
/* ----------------------------------------------------------------------------*/
u32 vfs_file_delete(void *pvfile)
{
    struct imount *p_vfile = pvfile;
    if ((void *)NULL == p_vfile) {
        return E_FS_PFILE;
    }
    struct vfs_operations *ops;
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->fdelete)) {
        u32 res = E_VFS_OPS;
        if (NULL != (p_vfile->pfile)) {
            res = ops->fdelete(p_vfile->pfile);
            if (0 != res) {
                return E_FS_PFILE;
            }
        }
        return res;
    }
    return  E_VFS_OPS;
}

static int vfs_reset_vfscan(void *pvfs, struct vfscan *fs, u16 file_total, u16 dir_total, const char *path, const char *pram, u8 max_deepth, int (*callback)(void))
{
    int err = -1;
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfs) {
        return -1;
    }
    ops = p_vfs->ops;

    __fscan_arg_handler(fs, pram);
    int arg[6] = {0};
    arg[0] = (int)p_vfs->pfs;
    arg[1] = (int)path;
    arg[2] = (int)file_total;
    arg[3] = (int)dir_total;
    arg[4] = (int)max_deepth;
    arg[5] = (int)callback;
    if (ops->ioctl) {
        err = ops->ioctl(fs, FS_IOCTL_RESET_VFSCAN, (int)arg);
    }
    if (err) {
        fat_vfscan_free(fs);
        fs = NULL;
    }
    return err;
}

struct vfscan *vfs_fscan_new(\
                             void *pvfs,                     \
                             const char *path,               \
                             const char *arg,                \
                             u8 max_deepth,                  \
                             int (*callback)(void),          \
                             struct vfscan *fsn,             \
                             struct vfscan_reset_info *info  \
                            )
{
    int err = -1;
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfs) {
        return NULL;
    }
    ops = p_vfs->ops;
    if (info->scan_over && info->active && fsn) {
        /* y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__); */
        err = vfs_reset_vfscan(pvfs, fsn, info->file_total, info->dir_total, path, arg, max_deepth, callback);
        if (err) {
            return NULL;
        }
        return fsn;
    }
    if (fsn) {
        fat_vfscan_free(fsn);
        fsn = NULL;
    }
    struct vfscan *fs = (struct vfscan *)fat_vfscan_alloc();
    ASSERT(fs);

    __fscan_arg_handler(fs, arg);
    if (ops->fscan_interrupt) {
        err = ops->fscan_interrupt(fs, p_vfs->pfs, path, max_deepth, callback);
    }
    if (err) {
        fat_vfscan_free(fs);
        fs = NULL;
        return fs;
    }
    info->file_total = fs->file_number;
    info->dir_total = fs->dir_totalnumber;
    info->scan_over = 1;

    return fs;
}

struct vfscan *vfs_fscan(void *pvfs, const char *path, const char *arg, u8 max_deepth, int (*callback)(void))
{
    int err = -1;
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfs) {
        return NULL;
    }
    ops = p_vfs->ops;
    struct vfscan *fs = (struct vfscan *)fat_vfscan_alloc();
    ASSERT(fs);

    __fscan_arg_handler(fs, arg);
    if (ops->fscan_interrupt) {
        err = ops->fscan_interrupt(fs, p_vfs->pfs, path, max_deepth, callback);
    }
    /*  else { */
    /*     err = mt->ops->fscan(fs, dir, max_deepth); */
    /* } */
    if (err) {
        fat_vfscan_free(fs);
        fs = NULL;
    }

    return fs;
}

void vfs_fscan_release(void *pvfs, struct vfscan *fs)
{
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    ASSERT(p_vfs);
    ops = p_vfs->ops;

    ops->fscan_release(fs);
    fat_vfscan_free(fs);
}

int vfs_select(void *pvfs, void **ppvfile, struct vfscan *fs, int sel_mode, int arg)
{
    struct imount *p_vfs = pvfs;
    if ((void *)NULL == p_vfs) {
        return E_VFS_HDL;
    }

    if ((void *)NULL == *ppvfile) {
        *ppvfile = vfs_hdl_malloc();
        if ((void *)NULL == *ppvfile) {
            return E_NO_VFS;
        }
    }

    int err;
    struct vfs_operations *ops;
    struct imount *p_vfile = *ppvfile;
    p_vfile->ops = p_vfs->ops;
    ops = p_vfs->ops;

    if (ops->fsel) {
        err = ops->fsel(fs, p_vfs->pfs, sel_mode, &p_vfile->pfile, arg);
    } else {
        err = E_VFS_OPS;
    }

    if (0 != err) {
        if (((void *)NULL != ops)  && ((void *)NULL !=  ops->close_file)) {
            if (NULL != (p_vfile->pfile)) {
                ops->close_file(&p_vfile->pfile);
            }

        }
        *ppvfile = vfs_fhdl_free(*ppvfile);

        /* log_info("func : %s, line : %d, p_vfile->ops : 0x%x\n", __func__, __LINE__, (u32)p_vfile->ops); */
        /* log_info("func : %s, line : %d\n", __func__, __LINE__); */
        /* vfs_file_close(ppvfile); */
    }
    return err;
}

int vfs_mk_dir(void *pvfs, char *folder, u8 mode)
{
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfs) {
        return 0;
    }
    ops = p_vfs->ops;
    int arg[2] = {0};
    arg[0] = (int)folder;
    arg[1] = (int)mode;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ioctl)) {
        u32 res;
        return ops->ioctl(p_vfs->pfs, FS_IOCTL_MK_DIR, (int)arg);
    }
    return 0;
}

int vfs_get_encfolder_info(void *pvfs, char *folder, char *ext, u32 *last_num, u32 *total_num)
{
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfs) {
        return 0;
    }
    ops = p_vfs->ops;
    int arg[4] = {0};
    arg[0] = (int)folder;
    arg[1] = (int)ext;
    arg[2] = (int)last_num;
    arg[3] = (int)total_num;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ioctl)) {
        u32 res;
        return ops->ioctl(p_vfs->pfs, FS_IOCTL_GET_ENCFOLDER_INFO, (int)arg);
    }
    return 0;
}

/* int vfs_get_folderinfo(void *pvfile, struct vfscan *fs, int *start_num, int *end_num) */
/* { */
/*     struct imount *p_vfile = pvfile; */
/*     struct vfs_operations *ops; */
/*     if ((void *)NULL == p_vfile) { */
/*         return 0; */
/*     } */
/*     ops = p_vfile->ops; */
/*     int arg[2] = {0}; */
/*     arg[0] = (int)start_num; */
/*     arg[1] = (int)end_num; */
/*     if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ioctl)) { */
/*         u32 res; */
/*         return ops->ioctl(fs, FS_IOCTL_GET_FOLDER_INFO, (int)arg); */
/*     } */
/*     return 0; */
/* } */

