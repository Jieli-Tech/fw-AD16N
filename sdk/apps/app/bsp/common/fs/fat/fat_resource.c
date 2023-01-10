#include "common.h"
/* #include "uart.h" */
#include "errno-base.h"
#include "fs/vfs.h"
/* #include "boot.h" */
/* #include "fat/ff_api.h" */
#include "fat/tff.h"
#include "fat/ff_opr.h"
#include "fat/fat_io_new.h"
#include "ioctl_cmds.h"
#include "device.h"
/* #include "mbr.h" */
/* #include "fat/fs_file_name.h" */
#include "fat/mbr.h"

extern u32 __dev_read(void *p, u8 *buf, u32 addr);
extern u32 __dev_write(void *p, u8 *buf, u32 addr);
#define LOG_TAG_CONST       FAT
//#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[fat_res]"
#include "log.h"

/****************resource manage*****************/
/* static FATFS g_fat_fs; */
/* static FIL g_fat_f; */
/* static char tmp_buf[644]; */
// FIL g_fat_f_lrc ;
/* static SWIN_BUF g_sector_buffer[MAX_DEEPTH]; */

/**************************************************
//       以下宏定义需要对应库修改
**************************************************/
#define FOPEN_LONG 0 //长文件名打开方式
#define RENAME_ENABLE 0 //重命名使能
#define W_WOL_ENABLE 0 //写卷标使能
//////////////////////////////////////////////////

#if FOPEN_LONG
static char lfn_buffer[LFN_MAX_SIZE] AT(.fat_buf);
#endif
static FAT_SCANDEV scan_buffer AT(.fat_buf);
/* static char ff_apis_buffer[sizeof(FF_APIS)]; */
#if FOPEN_LONG || RENAME_ENABLE || W_WOL_ENABLE
static char tmp_buf[512 + 260 + 256 + 6] AT(.fat_tmp_buf);
#else
static char tmp_buf[512] AT(.fat_tmp_buf);
#endif

#define FS_USE_MALLOC 1 //是否有malloc
#if FS_USE_MALLOC
#include "my_malloc.h"
#else
FATFS g_fat_fs;
FIL g_fat_f;
SWIN_BUF tmp_wbuf;
#endif

FATFS *fat_fshdl_alloc(void)
{
#if FS_USE_MALLOC
    return  my_malloc(sizeof(FATFS), MM_FATFS);
#else
    memset((u8 *)&g_fat_fs, 0x00, sizeof(FATFS));
    return &g_fat_fs;
#endif
}

FATFS *fat_fshdl_free(FATFS *fshdl)
{
#if FS_USE_MALLOC
    return my_free(fshdl);
#else
    return NULL;
#endif
}

FIL *fat_fhdl_alloc(void)
{
#if FS_USE_MALLOC
    return  my_malloc(sizeof(FIL), MM_FATFF);
#else
    memset((u8 *)&g_fat_f, 0x00, sizeof(FIL));
    return &g_fat_f;
#endif
}

FIL *fat_fhdl_free(FIL *pfile)
{
#if FS_USE_MALLOC
    return my_free(pfile);
#else
    return NULL;
#endif
}

void *fat_lfn_alloc(void)
{
#if FOPEN_LONG
    memset((u8 *)&lfn_buffer, 0x00, LFN_MAX_SIZE);
    return &lfn_buffer;
#else
    return NULL;
#endif
}

void fat_lfn_free(void *lfn_buffer)
{

}

void *fat_tmp_alloc(void)
{
    /* return  my_malloc(sizeof(FIL) + 1024, MM_FAT_TMP); */
    memset((u8 *)&tmp_buf, 0x00, sizeof(tmp_buf));
    return &tmp_buf;
}

void *fat_tmp_free(void *tmp_buf)
{
    /* return my_free(tmp_buf); */
    return 0;
}

#if 0
FF_APIS *fat_ff_apis_alloc(void)
{
    memset((u8 *)&ff_apis_buffer, 0x00, sizeof(ff_apis_buffer));
    return (FF_APIS *)&ff_apis_buffer;
    /* return  my_malloc(sizeof(FF_APIS), MM_FF_APIS_BUF); */
}

FF_APIS *fat_ff_apis_free(FF_APIS *ff_apis_buf)
{
    /* return my_free(ff_apis_buf); */
    return 0;
}
#endif

SWIN_BUF *fat_wbuf_alloc(void)
{
#if FS_USE_MALLOC
    return  my_malloc(sizeof(SWIN_BUF), MM_SWIN_BUF);
#else
    memset((u8 *)&tmp_wbuf, 0x00, sizeof(SWIN_BUF));
    return &tmp_wbuf;
#endif
}

SWIN_BUF *fat_wbuf_free(SWIN_BUF *tmp_wbuf)
{
#if FS_USE_MALLOC
    return my_free(tmp_wbuf);
#else
    return NULL;
#endif

}

VFSCAN fsn_wbuf AT(.fat_buf);//FAT扫盘buf使用全局变量
VFSCAN *fat_vfscan_alloc(void)
{
#if 0//FS_USE_MALLOC
    return  my_malloc(sizeof(VFSCAN), MM_VFSCAN_BUF);
#else
    memset((u8 *)&fsn_wbuf, 0x00, sizeof(VFSCAN));
    return &fsn_wbuf;
#endif
}

VFSCAN *fat_vfscan_free(VFSCAN *fsn_wbuf)
{
#if 0//FS_USE_MALLOC
    return my_free(fsn_wbuf);
#else
    return NULL;
#endif

}

FAT_SCANDEV *fat_scan_alloc(void)
{
    memset((u8 *)&scan_buffer, 0x00, sizeof(FAT_SCANDEV));
    return (FAT_SCANDEV *)&scan_buffer;
    /* return  my_malloc(sizeof(FAT_SCANDEV), MM_SCAN_BUF); */
}

FAT_SCANDEV *fat_scan_free(FAT_SCANDEV *scan_buf)
{
    /* return my_free(scan_buf); */
    return 0;
}

/****************api*****************/
u32 fat_monut_api(void **ppfs, void *p_device)
{
    _FS_DEV_INFO dev_info;

    if (p_device == NULL) {
        return -1;
    }
    {
        //get mbr
        MBR_DRIVE_INFO mbr_info;
        int res = mbr_scan_parition(&mbr_info, p_device);
        if (res != MBR_OK && mbr_info.drive_cnt) {
            log_info("mbr error !!!! \n");
            return -1;
        }

        dev_info.drive_base = mbr_info.drive_boot_sect[0];
    }

    dev_ioctl(p_device, IOCTL_GET_BLOCK_SIZE, (u32)&dev_info.block_size);
    dev_info.block_size /= 512;
    dev_info.hdl = p_device;
    /* dev_info.read_p = dev_lba_read; */
    /* dev_info.write_p = dev_lba_write; */
    dev_info.read_p = __dev_read;
#if FS_WRITE_EN
    dev_info.write_p = __dev_write;
#else
    dev_info.write_p = NULL;//__dev_write;
#endif

    s32 res = fat_drive_open(ppfs, &dev_info);
    if (res != 0) {
        log_info("fat_mount error:%d !!! \n", res);
        return -1;
    }

    return 0;
}


u32 fat_openR_api(void *pfs, void **ppfile, const char *path)
{
    return fat_open(pfs, ppfile, (char *)path, "r");
}
#if FS_WRITE_EN
u32 fat_openW_api(void *pfs, void **ppfile, u32 *pindex)
{
    return fat_open(pfs, ppfile, (char *)pindex, "w+");
}

u32 fat_write_api(void *pfile, void *buff, u32 len)
{
    u32 wlen = fat_write(pfile, buff, len);
    if (wlen != len) {
        return 0;
    }
    return wlen;
}

u32 fat_delete_file_api(void *pfile)
{
    s32 res = fat_delete(pfile);
    return res;
}

#endif

u32 fat_read_api(void *pfile, void *buff, u32 len)
{
    int rlen = fat_read(pfile, buff, len);
    if (rlen < 0) {
        return 0;
    }
    return rlen;
}

u32 fat_seek_api(void *pfile, u32 offset, u32 fromwhere)
{
    return fat_seek(pfile, fromwhere, offset);
}

u32 fat_file_close_api(void **ppfile)
{
    s32 res = fat_close(ppfile);
    return (u32) * ppfile;
//    if (res != 0) {
//        return -1;
//    }
//
//    return 0;
}

u32 fat_fs_close_api(void **ppfs)
{
    s32 res = fat_drive_close(ppfs);
    return (u32) * ppfs;
//    if (res != 0) {
//        return -1;
//    }
//
//    return 0;
}

int fat_attrs_api(void *pfile, void *attr)
{
    return fat_get_attrs(pfile, (struct vfs_attr *)attr);
}

u32 fat_file_name_api(void *pfile, void *name, u32 len)
{
    return fat_get_name(pfile, (char *)name, len);
}

int fat_get_fsize_api(void *pfile, u32 *parm)
{
    return fat_get_fsize(pfile, parm);
}

int fat_get_pos_api(void *pfile, u32 *parm)
{
    return fat_get_ftell(pfile, parm);
}


u32 fat_openbyfile_api(void *pcvfile, void **ppfile, void *ext_name)
{
    return fat_get_file_byfile(pcvfile, ppfile, ext_name);
}

int fat_fscan_interrupt_api(void *fsn, void *pfs, const char *path, u8 max_deepth, int (*callback)(void))
{
    return fat_fscan((struct vfscan *)fsn, pfs, path, max_deepth, callback);
    /* return fat_get_file_total(pfs, file_type, max_deepth, clust, ret_p); */
}

void fat_fscan_release_api(void *fsn)
{
    fat_fscan_release((struct vfscan *)fsn);
}

int fat_sel_api(void *fsn, void *pfs, int sel_mode, void **ppfile, int arg)
{
    return fat_fsel((struct vfscan *)fsn, pfs, sel_mode, ppfile, arg);
}

u32 fat_openbyindex_api(void *pfs, void **ppfile, u32 index, void *parm)
{
    log_info("fat_openbyindex_api\n");
    return fat_fsel((struct vfscan *)parm, pfs, FSEL_BY_NUMBER, ppfile, index);
}

u32 fat_openbyclust_api(void *pfs, void **ppfile, u32 clust, void *parm)
{
    return fat_fsel((struct vfscan *)parm, pfs, FSEL_BY_SCLUST, ppfile, clust);
}

int fat_ioctl_api(void *pfile, int cmd, int arg)
{
    switch (cmd) {
    case FS_IOCTL_FILE_INDEX:
        /* int *inparam = (int *)arg; */
        /* *inparam = scan_buffer.scn.cur_file_number; */
        return 0 ;
        break;
    default:
        return fat_io_ctrl(pfile, cmd, arg);
        break;
    }
    return -1;
}

#if HAS_VFS_EN

const struct vfs_operations fat_vfs_ops sec_used(.vfs_operations) = {
    .fs_type = "fat",
    .mount       = fat_monut_api,
    .openbypath  = fat_openR_api,
#if FS_WRITE_EN
    .createfile  = fat_openW_api,
    .write       = fat_write_api,
    .fdelete     = fat_delete_file_api,
#endif
    .read        = fat_read_api,
    .seek        = fat_seek_api,
    .close_fs 	 = fat_fs_close_api,
    .close_file  = fat_file_close_api,
    .fget_attr   = fat_attrs_api,
    .name        = fat_file_name_api,
    .flen        = fat_get_fsize_api,
    .ftell       = fat_get_pos_api,
    .openbyindex = fat_openbyindex_api,
    .openbyclust = fat_openbyclust_api,
    .openbyfile  = fat_openbyfile_api,
    .fscan_interrupt  = fat_fscan_interrupt_api,
    .fscan_release  = fat_fscan_release_api,
    .fsel        = fat_sel_api,
    .ioctl       = fat_ioctl_api,
};

#endif

/*
 * -t  文件类型
 * -r  包含子目录
 * -d  扫描文件夹
 * -a  文件属性 r: 读， /: 非
 * -s  排序方式， t:按时间排序， n:按文件号排序
 */
void __fscan_arg_handler(struct vfscan *fs, const char *arg)
{
    int step = 0;
    char *p;

    /*
     * fs->attr = F_ATTR_RO: 搜索只读文件
     * fs->attr = F_ATTR_ARC: 搜索非读文件
     * fs->attr = F_ATTR_ARC | F_ATTR_RO: 搜索所有文件
     */

    fs->attr = F_ATTR_ARC | F_ATTR_RO;

    while (*arg) {
        switch (step) {
        case 0:
            if (*arg == '-') {
                step = 1;
            }
            break;
        case 1:
            if (*arg == 't') {
                step = 2;
                p = fs->ftype;
                fs->scan_file = 1;
            } else if (*arg == 'r') {
                step = 0;
                fs->subpath = 1;
            } else if (*arg == 'd') {
                step = 0;
                fs->attr |= F_ATTR_DIR;
            } else if (*arg == 's') {
                step = 3;
            } else if (*arg == 'a') {
                step = 4;
            } else if (*arg == 'm') {
                step = 5;
                p = fs->filt_dir;
            }

            break;
        case 2:
            if (*arg == '-') {
                step = 1;
            } else if (*arg != ' ') {
                *p++ = *arg;
            }
            break;
        case 3:
            if (*arg == '-') {
                step = 1;
            } else if (*arg != ' ') {
                fs->sort = *arg;
            }
            break;
        case 4:
            if (*arg == '-') {
                step = 1;
            } else if (*arg == '/') {
                if (*++arg == 'r') {
                    fs->attr &= ~F_ATTR_RO;
                }
            } else if (*arg == 'r') {
                fs->attr &= ~F_ATTR_ARC;
            }
            break;
        case 5:
            if (*arg == '-') {
                step = 1;
            } else if (*arg != ' ') {
                *p++ = *arg;
            }
            break;
        default:
            break;
        }

        arg++;
    }
}

struct vfscan *fat_fscan_new(\
                             void *pfs,                     \
                             const char *path,              \
                             const char *arg,               \
                             u8 max_deepth,                 \
                             int (*callback)(void),         \
                             struct vfscan *fsn,            \
                             struct vfscan_reset_info *info \
                            )
{
    int err = -1;
    if ((void *)NULL == pfs) {
        return NULL;
    }
    if (info->scan_over && info->active && fsn) {
        __fscan_arg_handler(fsn, arg);
        int t_arg[6] = {0};
        t_arg[0] = (int)pfs;
        t_arg[1] = (int)path;
        t_arg[2] = (int)info->file_total;
        t_arg[3] = (int)info->dir_total;
        t_arg[4] = (int)max_deepth;
        t_arg[5] = (int)callback;
        err = fat_ioctl_api(fsn, FS_IOCTL_RESET_VFSCAN, (int)t_arg);
        if (err) {
            fat_vfscan_free(fsn);
            return NULL;
        }
        return fsn;
    }

    if (fsn) {
        fat_vfscan_free(fsn);
        fsn = NULL;
    }
    // struct vfscan *
    fsn = (struct vfscan *)fat_vfscan_alloc();
    ASSERT(fsn);

    __fscan_arg_handler(fsn, arg);

    err = fat_fscan_interrupt_api(fsn, pfs, path, max_deepth, callback);

    if (err) {
        fat_vfscan_free(fsn);
        fsn = NULL;
        return fsn;
    }

    info->file_total = fsn->file_number;
    info->dir_total = fsn->dir_totalnumber;
    info->scan_over = 1;
    return fsn;
}

struct vfscan *fat_fscan_api(\
                             void *pfs,                     \
                             const char *path,              \
                             const char *arg,               \
                             u8 max_deepth,                 \
                             int (*callback)(void)          \
                            )
{
    int err = -1;
    if ((void *)NULL == pfs) {
        return NULL;
    }
    struct vfscan *fsn = (struct vfscan *)fat_vfscan_alloc();
    ASSERT(fsn);

    __fscan_arg_handler(fsn, arg);

    err = fat_fscan_interrupt_api(fsn, pfs, path, max_deepth, callback);

    if (err) {
        fat_vfscan_free(fsn);
        fsn = NULL;
    }
    return fsn;
}
int fat_get_encfolder_info(void *pfs, char *folder, char *ext, u32 *last_num, u32 *total_num)
{
    int err = 0;
    *total_num = fs_getfolder_fileinfo(pfs, folder, ext, last_num);
    if (0 == *total_num) {
        err = E_DIR_NULL;
    }
    return err;
}



#if 0
#define SET_BP_TEST  0
#define CREATE_DIR_TEST 0
#define CREATE_FILE_TEST 1
#define LONG_NAME_CREATE_FILE_TEST 0
#define VFS_OPEN_BY_PATH 0
#define VFS_OPEN_BY_NUMBER 0
#define VFS_DELETE_FILE 0
#define VFS_TEST_RANDOM_READ 0
static u8 buf_test[512];
/* static int test_buf[1024]; */
extern void wdt_clear(void);
static const char scan_parm_test[] = "-t"
                                     "MP1MP2MP3WAVTXT"
                                     " -sn -r"
                                     ;

struct vfscan			*fsn_test;//设备扫描句柄
void fat_demo(void)
{
    /* void *device = device_get_dev_hdl("sd0");; */
    void *device = dev_open("sd0", NULL);
    /* void *device = dev_open("udisk0", NULL); */
    if (device == NULL) {
        log_info("dev null !!!! \n");
        return;
    }
    static void *pfs = NULL, *pfile = NULL;
    /* u32 res = fat_monut_api(&pfs, device); */
    u32 res = fs_mount(&pfs, device, "fat");
    if (res != 0) {
        log_info("dev mount error !!! \n");
        return;
    }

#if VFS_TEST_RANDOM_READ
    extern u32 get_random();
    r_printf(">>>[test]:start read\n");
    char path[64] = "/JL01.TXT";
    int d = 1;
    fs_openbypath(pfs, &pfile, path);
    struct vfs_attr attr = {0};
    fs_get_attrs(pfile, (void *)&attr);
    r_printf(">>>[test]:sclust = %d, fsize = %d, attr =0x%x\n", attr.sclust, attr.fsize, attr.attr);
    int cnt = 0;
__again:
    cnt++;
    if (cnt > 100) {
        return;
    }
    int pos = get_random() % attr.fsize / 4 * 4 ;
    int rlen = get_random() % 4096 / 4;
    fs_seek(pfile, pos, SEEK_SET);
    wdt_clear();
    log_info("start random read");
    memset(test_buf, 0, 4096);
    int len = fs_read(pfile, (u8 *)test_buf, rlen * 4);
    if ((rlen * 4) != len) {
        r_printf(">>>[test]:rlen %d, len %d\n", rlen, len);
        return;
    }
    for (int i = 0, b = pos / 4; i < rlen; i++, b++) {
        int tmp_buf[1] = {0};
        tmp_buf[0] = b;
        u8 *num = (u8 *)&tmp_buf[0];
        num[3] = d & 0xff;
        u8 *tmp1 = (u8 *)&test_buf[i];
        if ((tmp1[3] != d & 0xff) || test_buf[i] != tmp_buf[0]) {
            y_printf(">>>[test]:i = %d, j = %d, d= %d, tmp_buf[0]= 0x%x, test_buf[i] = 0x%x,tmp1[3] = 0x%x\n", i, pos, d, tmp_buf[0], test_buf[i], tmp1[3]);
            log_info_hexdump(test_buf, 4096);
            return ;
        }
    }
    goto __again;
#endif

#if VFS_DELETE_FILE
    int err = -1;
    fs_openbypath(pfs, &pfile, "/123.txt");
    if (pfile) {
        err = fs_file_delete(pfile);
        if (err) {
            r_printf(">>>[test]:err delete!!!!!!!\n");
        }
    } else {
        r_printf(">>>[test]:err open!!!!!!!\n");
    }
#endif

#if LONG_NAME_CREATE_FILE_TEST
    /* char file_path[128] = {'/', '\\', 'U', 0x31, 0x0, 0x32, 0x0, 0x33, 0x0, 0x34, 0x0, 0x35, 0x0, 0x36, 0x0, 0X37, 0x0, 0x38, 0x0, 0x39, 0x0, 0x31, 0, 0x32, 0, 0x33, 0, 0x2E, 0x0, 0x6D, 0x0, 0x70, 0x0, 0x33, 0}; */
    char file_path[128] = "/abcd.txt";
    int err = fs_createfile(pfs, &pfile, (u32 *)file_path);
    if (err) {
        log_info("err open w+");
        return;
    }
    fs_file_close(&pfile);
    fs_openbypath(pfs, &pfile, file_path);
    fs_file_close(&pfile);
#endif

#if CREATE_FILE_TEST
    u8 write_err_lag = 0;
    for (int i = 0; i < 512; i++) {
        buf_test[i] = i & 0xff;
    }
    int fs_size = 1 * 1024 * 1024;
    /* char path[64] = {"storage/sd0/C#<{(|.txt"}; */
    char path[16] = {"/**.txt"};
    /* char *change = strchr(path, '*'); */
    char *change = &path[1];
    for (int d = 0; d < 100; d++) {
        change[0] = d % 100 / 10 + '0';
        change[1] = d % 10 + '0';
        y_printf(">>>[test]:path = %s\n", path);
        int err = fs_createfile(pfs, &pfile, (u32 *)path);
        if (err) {
            log_info("err open w+");
            return;
        }
        for (int i = 0; i < fs_size;) {
            wdt_clear();
            int wlen = fs_write(pfile, buf_test, sizeof(buf_test));
            if (wlen != sizeof(buf_test)) {
                log_info("write errr wlen = %d", wlen);
                /* write_err_lag = 1; */
                /* goto __delete; */
                return ;
            }
            i += wlen;
            if (i == fs_size) {
                break;
            }
            if (i + wlen > fs_size) {
                wlen = fs_size - i;
            }
        }
        int parm = 0;
        fs_ftell(pfile, &parm);
        log_info("write ok !!!!!!!!!! len = %d", parm);
        fs_file_close(&pfile);

        r_printf(">>>[test]:goto read !!!!!!!!\n");
        fs_openbypath(pfs, &pfile, path);
        for (int i = 0; i < fs_size;) {
            wdt_clear();
            memset(buf_test, 0, sizeof(buf_test));
            int rlen = fs_read(pfile, buf_test, sizeof(buf_test));
            if (rlen != sizeof(buf_test)) {
                log_info("read errr rlen = %d", rlen);
                break ;
            }
            for (int j = 0; j < sizeof(buf_test); j++) {
                if (buf_test[j] != (j & 0xff)) {
                    log_info("j = %d, buf_test[j] = %d", j, buf_test[j]);
                    log_info_hexdump(buf_test, sizeof(buf_test));
                    return;
                }
            }
            i += rlen;
            if (i == fs_size) {
                break;
            }
            if (i + rlen > fs_size) {
                rlen = fs_size - i;
            }
        }
        y_printf(">>>[test]:read OK !!!!!!!!\n");
        fs_file_close(&pfile);
    }
    /* __delete: */
    /*     if (write_err_lag) { */
    /*     } */

    return;
#endif
#if CREATE_DIR_TEST
    char folder[] = "/JL_REC";
    fs_mk_dir(pfs, folder, 0);
    u32 last_num = 0;
    u32 total = 0;
    fs_get_encfolder_info(pfs, folder, "MP3", &last_num, &total);
    r_printf(">>>[test]:last_num = %d, total = %d\n", last_num, total);
    return;
#endif

#if SET_BP_TEST
    /* u32 total_num = fat_get_file_total(pfs, "MP3WAV", 0, NULL); */
    u32 *find = 0;
    extern u32 *fat_set_bp_info(u32 clust, u32 fsize);
    find = fat_set_bp_info(28406, 46116908);
    r_printf(">>>[test]:*find = 0x%x\n", find);
    /* u32 total_num = fs_fscan(pfs, "MP3WAV", 9, 0, NULL); */
    fsn_test = fs_fscan(pfs, "/", scan_parm_test, 9, NULL);
    u32 total_num = fsn_test->file_number;
    r_printf(">>>[test]:*find = 0x%x, find = %d\n", find, *find);
    log_info("total_num:%d \n", total_num);
    /* fs_set_lfn_buf(); ///长文件名buf设置 */
    fs_fscan_release(pfs, fsn_test);
    return;
#endif
#if VFS_OPEN_BY_PATH
////////////////////////////write/////////////////////////////////////////
    /* fs_ext_setting("MP3WAVTXT"); */
    /* char path[64] = "#<{(|/4*.?"; */
    char path[64] = "/456.MP3";
    u8 buf[128];
    /* fsn_test = fs_fscan(pfs, "/", scan_parm_test, 9, NULL); */
    fs_openbypath(pfs, &pfile, path);
    /* fs_select(pfs, &pfile, fsn_test, FSEL_BY_PATH, (int)path); */
    int name_len = fs_file_name(pfile, buf, sizeof(buf));
    log_info(">>>[test]:name = %s\n", buf);
    log_info_hexdump(buf, name_len);
    memset(buf_test, 0, 512);
    int rlen = fs_read(pfile, buf_test, 512);
    r_printf(">>>[test]:rlen = %d\n", rlen);
    log_info_hexdump(buf_test, rlen);
    fs_fscan_release(pfs, fsn_test);
///////////////////////////////////////////////////////////////////
#endif

#if VFS_OPEN_BY_NUMBER
    fs_ext_setting("MP3WAV");
    fsn_test = fs_fscan(pfs, "/", scan_parm_test, 9, NULL);
    /* res = fs_openbyindex(pfs, &pfile, 3); */
    res = fs_select(pfs, &pfile, fsn_test, FSEL_BY_NUMBER, 3);
    if (res != 0) {
        log_info("file open error !!! \n");
        /* fat_file_close_api(&pfile); */
        fs_file_close(&pfile);
        return;
    }
    log_info("file open succ \n");
    u8 file_name[VFS_FILE_NAME_LEN];
    int name_len = fs_file_name(pfile, file_name, sizeof(file_name));
    /* fat_get_file_name(pfile, &file_name); */
    log_info(">>>>filename:%s , name_len = %d\n", file_name, name_len);
    /* put_buf(file_name, name_len); */
    ///////////////////////////////////////////////////////////////////////
    /* res = fs_openbyindex(pfs, &pfile, 5); */
    res = fs_select(pfs, &pfile, fsn_test, FSEL_BY_NUMBER, 5);
    if (res != 0) {
        log_info("file open error !!! \n");
        /* fat_file_close_api(&pfile); */
        fs_file_close(&pfile);
        return;
    }
    log_info("file open succ \n");

    //////////////////////////////////////////////////////////////////////
    struct vfs_attr attr = {0};
    fs_get_attrs(pfile, (void *)&attr);
    r_printf(">>>[test]:sclust = %d, fsize = %d, attr =0x%x\n", attr.sclust, attr.fsize, attr.attr);
    u32 fsize = 0;
    fs_get_fsize(pfile, (void *)&fsize);
    r_printf(">>>[test]:fsize = %d\n", fsize);
    int start = 0, end = 0;
    int arg[3];
    arg[0] = (int)fsn_test;
    arg[1] = (int)&start;
    arg[2] = (int)&end;
    //fs_get_folderinfo(pfile, fsn_test, &start, &end);
    fs_ioctl(pfile, FS_IOCTL_GET_FOLDER_INFO_3, (int)arg);
    r_printf(">>>[test]:s= %d, e= %d\n", start, end);

    return;

    int pos = 0;
    static u8 buff[512];
    /* fat_read_api(pfile, buff, 100); */
    fs_read(pfile, buff, 100);
    put_buf(buff, 100);
    /* fat_read_api(pfile, buff, 100); */
    fs_read(pfile, buff, 100);
    put_buf(buff, 100);
    r_printf(">>>[test]:seek 0 :\n");
    fs_seek(pfile, 0, SEEK_SET);
    fs_read(pfile, buff, 100);
    put_buf(buff, 100);
    fs_ftell(pfile, (void *)&pos);
    y_printf(">>>[test]:pos = %d\n", pos);

    /////////////////////////////////////////////////////////////////////
    void *file = NULL;
    /* res = fs_openbyclust(pfs, &file, 5); */
    res = fs_select(pfs, &file, fsn_test, FSEL_BY_SCLUST, 5);
    if (res != 0) {
        log_info("file open error !!! \n");
        fs_file_close(&file);
        return;
    }
    log_info("file open succ \n");
    /* char file_name[12]; */
    memset(file_name, 0, 12);
    name_len = fs_file_name(file, file_name, sizeof(file_name));
    log_info(">>>>filename:%s \n", file_name);
    /* struct vfs_attr attr = {0}; */
    /* memset(&attr, 0, sizeof(vfs_attr)); */
    fs_get_attrs(file, (void *)&attr);
    r_printf(">>>[test]:sclust = %d, fsize = %d, attr =0x%x\n", attr.sclust, attr.fsize, attr.attr);
    fs_file_close(&file);

    fs_fscan_release(pfs, fsn_test);

#endif

    /* fat_file_close_api(&pfile); */
    /* fat_fs_close_api(&pfs); */
    fs_file_close(&pfile);
    fs_fs_close(&pfs);
}
#endif

