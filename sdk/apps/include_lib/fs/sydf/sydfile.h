#ifndef __SYDFILE_H__
#define __SYDFILE_H__

#include "typedef.h"


#define SYDFILE_NAME_LEN 			16

typedef struct _SYDWIN_BUF {
    u8  start[512];
    u32  sector;
    struct _SYDFS  *fs;
    u8   flag;
} SYDWIN_BUF;

struct _SYDFS {
    u32 sfc_base_addr;
    u32 app_addr;
    u32 file_total;
    void    *p_device;
    SYDWIN_BUF *p_win;                /* Disk access window for Directory/FAT/File */
};
typedef struct _SYDFS SYDFS;

typedef struct _SYDFILE {
    u32 addr;
    u32 length;
    u32 fptr;
    u32 index;
    u32 file_total;
    u32 dir_offset;
    SYDFS *pfs;
    u8 name[SYDFILE_NAME_LEN];
} SYDFILE, *PSYDFILE, sydfile_t, *psydfile_t;


u32 sydf_phy_read(SYDFS *pfs, void *buff, u32 addr, u32 len);

void sydf_init(void);
// u32 sydf_monut(void **ppfs,void *p_device);
// u32 sydf_open(void *pfs, void **ppfile, const char * path);
// u32 sydf_openbyindex(void *pfs, void **ppfile, u32 index);
// u32 sydf_read(void *pfile,void * buff, u32 len);
// u32 sydf_seek(void *pfile, u32 offset, u32 fromwhere);
// int sydf_attrs(void *pfile, void * pvfs_attr);
// u32 sydf_file_close(void **ppfile);
// u32 sydf_fs_close(void **ppfs);


u32 sydf_monut(SYDFS **ppfs, void *p_device);
u32 sydf_open(SYDFS *pfs, SYDFILE **ppfile, const char *path);
u32 sydf_openbyindex(SYDFS *pfs, SYDFILE **ppfile, u32 index);
u32 sydf_openbysclust(SYDFS *pfs, SYDFILE **ppfile, u32 sclust);
u32 sydf_filelen(SYDFILE *pfile, u32 *plen);
u32 sydf_openbyfile(SYDFILE *p_curr_file, SYDFILE **ppfile, const char *ext);
u32 sydf_read(SYDFILE *pfile, void *buff, u32 len);
u32 sydf_seek(SYDFILE *pfile, u32 offset, u32 fromwhere);
int sydf_attrs(SYDFILE *pfile, void *pvfs);
u32 sydf_fs_close(SYDFS **ppfs);
u32 sydf_file_close(SYDFILE **ppfile);
u32 sydf_file_name(SYDFILE *pfile, char *name, u32 len);
int sydf_ioctl(SYDFILE *pfile, int cmd, int arg);

//----exfunction
int sydf_file_crc(SYDFILE *pfile);


#endif
