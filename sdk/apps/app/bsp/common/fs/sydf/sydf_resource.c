#include "sydf_resource.h"
#include "vfs.h"
#include "my_malloc.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

#if 0
#define  SYDF_MAX_FILE_HDL 2 //0~8
typedef struct _sydf_file_hdl {
    SYDFILE file[SYDF_MAX_FILE_HDL];
    u8 flag;
} _SYDF_FILE_HDL;

#define  SYDF_MAX_FS_HDL 2 //0~8
typedef struct _sydf_fs_hdl {
    SYDFS fs[SYDF_MAX_FS_HDL];
    u8 flag;
} _SYDF_FS_HDL;

_SYDF_FILE_HDL sydf_file_hdl;
_SYDF_FS_HDL sydf_fs_hdl;

void sydf_hdl_init(void)
{
    memset(&sydf_file_hdl, 0, sizeof(_SYDF_FILE_HDL));
}

SYDFILE *sydf_fhdl_malloc(void)
{

    /* log_info("sydf hdl : 0x%x\n", sydf_file_hdl.flag); */
    for (u8 i = 0; i < SYDF_MAX_FILE_HDL; i++) {
        if (0 == (BIT(i) & sydf_file_hdl.flag)) {
            sydf_file_hdl.flag |= BIT(i);
            return &sydf_file_hdl.file[i];
        }
    }
    log_info(" err no null file\n");
    return (void *) NULL;
}

SYDFILE *sydf_fhdl_free(SYDFILE *pfile)
{

    for (u8 i = 0; i < SYDF_MAX_FILE_HDL; i++) {
        /* log_info("sydf free file : 0x%x 0x%x\n", (u32)pfile, (u32)&sydf_file_hdl.file[i]); */
        if (pfile == &sydf_file_hdl.file[i]) {
            /* log_info("free file: 0x%x\n", i); */
            sydf_file_hdl.flag &= ~BIT(i);
            return (void *) NULL;
        }
    }
    return pfile;
}

SYDFS *sydf_fshdl_malloc(void)
{
    for (u8 i = 0; i < SYDF_MAX_FS_HDL; i++) {
        if (0 == (BIT(i) & sydf_fs_hdl.flag)) {
            sydf_fs_hdl.flag |= BIT(i);
            return &sydf_fs_hdl.fs[i];
        }
    }
    log_info(" err no null file\n");
    return (void *) NULL;
}

SYDFS *sydf_fshdl_free(SYDFS *pfs)
{
    for (u8 i = 0; i < SYDF_MAX_FS_HDL; i++) {
        /* log_info("sydf free fs : 0x%x 0x%x\n", (u32)pfs, (u32)&sydf_fs_hdl.fs[i]); */
        if (pfs == &sydf_fs_hdl.fs[i]) {
            /* log_info("free fs: 0x%x\n", i); */
            sydf_fs_hdl.flag &= ~BIT(i);
            return (void *) NULL;
        }
    }
    return pfs;
}

#else

void sydf_hdl_init(void)
{
}

SYDFILE *sydf_fhdl_malloc(void)
{
    return  my_malloc(sizeof(SYDFILE), MM_SYDFF);
}

SYDFILE *sydf_fhdl_free(SYDFILE *pfile)
{
    return my_free(pfile);
}

SYDFS *sydf_fshdl_malloc(void)
{
    return my_malloc(sizeof(SYDFS), MM_SYDFS);
}

SYDFS *sydf_fshdl_free(SYDFS *pfs)
{
    return my_free(pfs);
}
#endif




//-----------------------------SYDF API
void sydf_init_api(void)
{
    log_info("sydf_init_api !!!\n");
    sydf_init();
}

u32 sydf_monut_api(void **ppfs, void *p_device)
{
    return sydf_monut((SYDFS **)ppfs, p_device);
}

u32 sydf_open_api(void *pfs, void **ppfile, const char *path)
{
    return sydf_open(pfs, (SYDFILE **)ppfile, path);
}

u32 sydf_openbyindex_api(void *pfs, void **ppfile, u32 index, void *parm)
{
    return sydf_openbyindex(pfs, (SYDFILE **)ppfile, index);
}
u32 sydf_openbysclust_api(void *pfs, void **ppfile, u32 sclust, void *parm)
{
    return sydf_openbysclust(pfs, (SYDFILE **)ppfile, sclust);
}

int sydf_filelen_api(void *pfile, u32 *plen)
{
    return sydf_filelen(pfile, plen);

}

u32 sydf_openbyfile_api(void *pfs, void **ppfile, void *ext_name)
{
    return sydf_openbyfile(pfs, (SYDFILE **)ppfile, ext_name);
}

u32 sydf_read_api(void *pfile, void *buff, u32 len)
{
    return sydf_read(pfile,  buff, len);
}

u32 sydf_seek_api(void *pfile, u32 offset, u32 fromwhere)
{
    return sydf_seek(pfile, offset, fromwhere);
}

int sydf_attrs_api(void *pfile, void *pvfs_attr)
{
    return sydf_attrs(pfile,   pvfs_attr);
}

u32 sydf_file_close_api(void **ppfile)
{
    return sydf_file_close((SYDFILE **)ppfile);
}

u32 sydf_fs_close_api(void **ppfs)
{
    return sydf_fs_close((SYDFS **)ppfs);
}

u32 sydf_file_name_api(void *pfile, void *name, u32 len)
{
    return sydf_file_name((SYDFILE *)pfile, (char *)name, len);
}

int sydf_ioctl_api(void *pfile, int cmd, int arg)
{
    /* log_info("sydf_ioctl_api\n"); */
    return sydf_ioctl((SYDFILE *)pfile, cmd, arg);

}
int sydf_file_crc_api(void *pfile)
{
    return sydf_file_crc((SYDFILE *)pfile);

}
//REGISTER_VFS_OPERATIONS(sydfvfs_ops) = {
#if 1
const struct vfs_operations sydf_vfs_ops sec_used(.vfs_operations) = {
    .fs_type = "sydfile",
    .init        = sydf_init_api,
    /* .init        = NULL, */
    .mount       = sydf_monut_api,
    .openbypath  = sydf_open_api,
    .openbyindex = sydf_openbyindex_api,
    .openbyclust = sydf_openbysclust_api,
    .openbyfile  = sydf_openbyfile_api,
    .createfile  = NULL,
    .read        = sydf_read_api,
    .write       = NULL,
    .seek        = sydf_seek_api,
    .close_fs 	 = sydf_fs_close_api,
    .close_file  = sydf_file_close_api,
    .fget_attr   = sydf_attrs_api,
    .name        = sydf_file_name_api,
    .flen        = sydf_filelen_api,
    .ioctl       = sydf_ioctl_api,
    .file_crc    = sydf_file_crc_api,
};
#endif










