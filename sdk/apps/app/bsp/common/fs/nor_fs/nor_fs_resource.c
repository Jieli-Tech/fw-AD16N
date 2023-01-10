#include "nor_fs/nor_fs.h"
#include "common.h"
#include "uart.h"
#include "errno-base.h"
#include "vfs.h"
#include "boot.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[norfs_res]"
#include "log.h"

static RECFILESYSTEM recfs;
static REC_FILE recfile;

RECFILESYSTEM *norfs_fshdl_malloc(void)
{
    memset((u8 *)&recfs, 0x00, sizeof(RECFILESYSTEM));
    return &recfs;
}

RECFILESYSTEM *norfs_fshdl_free(void)
{
    return NULL;
}

REC_FILE *norfs_filehdl_malloc(void)
{
    memset((u8 *)&recfile, 0x00, sizeof(REC_FILE));
    return &recfile;
}

REC_FILE *norfs_filehdl_free(void)
{
    return NULL;
}

void norfs_init_api(void)
{
    log_info("norfs_init_api !!!\n");
    u32 sector_start;
    u32 sector_end;
    u32 sector_size;
    u32 sector_bit = 12;

    sector_size = (1 << sector_bit);
    sector_start = (boot_info.vm.vm_saddr + sector_size - 1) / sector_size;
    sector_end = (boot_info.vm.vm_size + boot_info.vm.vm_saddr) / sector_size;

    sector_end--;

    log_info("boot_info.vm.vm_saddr = 0x%x", boot_info.vm.vm_saddr);
    log_info("boot_info.vm.vm_size  = 0x%x", boot_info.vm.vm_size);

    log_info("sector_start = 0x%x", sector_start);
    log_info("sector_end   = 0x%x", sector_end);
    log_info("sector_bit   = %d", sector_bit);
    norfs_init(sector_start, sector_end, sector_bit); //1<<12 = 4K
}
//-----------------------------NOR FS API

u32 norfs_monut_api(void **ppfs, void *p_device)
{
    return norfs_mount((RECFILESYSTEM **)ppfs, (void *)p_device);
}

u32 norfs_openbyindex_api(void *pfs, void **ppfile, u32 index, void *parm)
{
    return norfs_openbyindex((RECFILESYSTEM *)pfs, (REC_FILE **)ppfile, index);
}

int norfs_filelen_api(void *pfile, u32 *plen)
{
    return norfs_filelen(pfile, plen);
}
u32 norfs_createfile_api(void *pfs, void **ppfile, u32 *pindex)
{
    return norfs_createfile((RECFILESYSTEM *)pfs, (REC_FILE **)ppfile, pindex);
}

u32 norfs_write_api(void *pfile, void *buff, u32 len)
{
    return (u32)norfs_write((REC_FILE *)pfile, (u8 *)buff, len);
}

u32 norfs_read_api(void *pfile, void *buff, u32 len)
{
    return (u32)norfs_read((REC_FILE *)pfile, (u8 *)buff, len);
}

u32 norfs_seek_api(void *pfile, u32 offset, u32 fromwhere)
{
    return (u32)norfs_seek((REC_FILE *)pfile, (u8)fromwhere, offset);
}


u32 norfs_file_close_api(void **ppfile)
{
    return (u32)norfs_closefile((REC_FILE **)ppfile);
}

int norfs_ioctl_api(void *pfile, int cmd, int arg)
{
    return norfs_ioctl((REC_FILE *)pfile, cmd, arg);

}

u32 norfs_name_api(void *pfile, void *name, u32 len)
{
    return norfs_name((REC_FILE *)pfile, (char *)name, len);
}

//REGISTER_VFS_OPERATIONS(sydfvfs_ops) = {
const struct vfs_operations norfs_vfs_ops sec_used(.vfs_operations) = {
    .fs_type = "norfs",
    /* .init        = norfs_init_api, */
    .init        = NULL,
    .mount       = norfs_monut_api,
    .openbypath  = NULL,
    .openbyindex = norfs_openbyindex_api,
    .createfile  = norfs_createfile_api,
    .read        = norfs_read_api,
    .write       = norfs_write_api,
    .seek        = norfs_seek_api,
    .close_fs 	 = NULL,
    .close_file  = norfs_file_close_api,
    .fget_attr  = NULL,
    .name        = norfs_name_api,
    .flen        = norfs_filelen_api,
    .ioctl       = norfs_ioctl_api,
};











