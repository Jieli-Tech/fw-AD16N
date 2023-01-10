#ifndef _FF_OPR
#define _FF_OPR

#include "typedef.h"
// #if SIMPLE_FATFS_ENABLE
// #include "simple_fat/simple_fat.h"
// #else
#include "tff.h"
// #endif


#ifndef FSELECT_MODE
#define FSELECT_MODE
#define    FSEL_FIRST_FILE     			 0
#define    FSEL_LAST_FILE      			 1
#define    FSEL_NEXT_FILE      			 2
#define    FSEL_PREV_FILE      			 3
#define    FSEL_CURR_FILE      			 4
#define    FSEL_BY_NUMBER      			 5
#define    FSEL_BY_SCLUST      			 6
#define    FSEL_AUTO_FILE      			 7
#define    FSEL_NEXT_FOLDER_FILE       	 8
#define    FSEL_PREV_FOLDER_FILE         9
#define    FSEL_BY_PATH                 10
#endif

#ifndef FCYCLE_MODE
#define FCYCLE_MODE
#define    FCYCLE_LIST			0
#define    FCYCLE_ALL			1
#define    FCYCLE_ONE			2
#define    FCYCLE_FOLDER		3
#define    FCYCLE_RANDOM		4
#define    FCYCLE_MAX			5
#endif


typedef struct _FF_APIS {
    // FSAPIMSG fs_msg;
    const char *ftypes;
    DIRINFO  dir_info;
    DIR     path_dj[MAX_DEEPTH];				/* 用于搜索文件时，作为搜索路径的缓存 */
    FATFS 	*pFatHdler;
    u32     sclust_id;
    u32     fsize_id;
    u16     fileCounter;
    u16	    fileNumber;			// 当前文件序号

    u16     dir_totalnumber;          // 文件夹总数
    u16     musicdir_counter;          // 播放文件所在文件夹序号
    char    *tpath;                 //指定存放指定路径名

    int     brower_flag;           //用于浏览接口
    u8      continue_flag;          //接着搜标志

    u16	    totalFileNumber;
    u16     fileTotalInDir;     // 当前目录的根下有效文件的个数
    u16     fileTotalOutDir;	// 当前目录前的文件总数，目录循环模式下，需要用它来计算文件序号
    u8      max_deepth;
    u8      deepth;
    u8      attr;
    FS_NAME	fs_n;
    const char *filt_dir;
    int (*interrupt_fscan)(void);
} FF_APIS;

enum {
    SEARCH_FILE_BY_NUMBER,  //按文件序号搜索文件
    SEARCH_SPEC_FILE,       //搜索指定的文件
    SEARCH_DISK,            //搜索总有效文件数
    SEARCH_FOLDER           //搜索当前目录根下的有效文件数。
};


typedef struct __DIR_FIND {
    u32 cur_file_number;
    u32 file_counter;
    const char *ext_table;
} DIR_FIND;

typedef struct ffscan {
    u32 magic;
    u8 order;
    // u8 sort_by;
    u8 subpath;
    u8 store_mode;
    u8 cycle_mode;
    u16 cur_index;
    u16 prev_fnum;
    u16 cur_file_number;
    u16 first_file_number;
    u16 last_file_number;
    u32 first_CrtTime;
    u32 last_CrtTime;
    u32 cur_time;
    FF_APIS ff_api;
} SCAN;

typedef struct vfscan {
    u8 scan_file;
    u8 subpath; //子目录，设置是否只扫描一层
    u8 scan_dir;
    u8 attr;
    u8 cycle_mode;
    char sort;
    char ftype[20 * 3 + 1];
    u16 file_number;
    u16 file_counter;

    u16  dir_totalnumber;          // 文件夹总数
    u16  musicdir_counter;          // 播放文件所在文件夹序号
    u16  fileTotalInDir;           //文件夹下的文件数目

    void *priv;
    // struct vfs_devinfo *dev;
    // struct vfs_partition *part;
    char  filt_dir[12];
} VFSCAN;

typedef struct fat_scan_dev_handl {
    SCAN scn; //must first
    u8  use;
} FAT_SCANDEV;

int ff_scan(FATFS *pFatHdler, struct ffscan *scan, const char *path, bool subpath,
            const char *ftypes, const char *filt_dir, u8 attr, u8 max_deepth, int sort_by);

int ff_scan_subpath_or_pre_dir(FATFS *pFatHdler, struct ffscan *scan, const char *path, bool subpath,
                               const char *ftypes, const char *filt_dir, u8 attr, u8 max_deepth, int flag);

int ff_select_file(struct ffscan *scan, int sel_mode, FIL *file, int arg);

u32 fs_open_dir_info(FATFS _xdata *f_s, FIL *f_p, void *dir_dj);
u32 fs_exit_dir_info(FATFS _xdata *f_s, FIL *f_p);
u32 fs_get_dir_info(FATFS _xdata *f_s, FIL *f_p, u32 start_num, u32 total_num, void *buf_info);

u32 fs_getfile_byname_indir(FATFS _xdata *f_s, FIL *tag_file, FIL *get_file, char *ext_name);

u32 fs_getfolder_fileinfo(FATFS _xdata *fs, char *folder, char *ext, u32 *first_fn);

void fs_lfn_deal(FATFS _xdata *f_s, FIL *f_p);

int f_Open(FATFS *fs, FIL *fp, char *path, char *lfn_buf, u8 mode);

int f_Rename(FIL *fp, char *path);

int ff_fast_scan_files(FF_APIS *ff_api, DIR *dj, int max_deepth, bool subpath,
                       const char *ftypes, int attr, int *fileTotal);

int open_file_bynumber(FIL *file, FF_APIS *ff_api, int fileNumber);
int open_file_by_clust(FIL *file, FF_APIS *ff_api, int clust);
int f_Getname(FIL *fp, u8 *pbuf, int len);

int f_Getpath(FIL *fp, FF_APIS *ff_api, u8 *pbuf, int len, u8 is_relative_path);

int long_name_fix(u8 *str, u16 len);

int fat_get_free_space(FATFS *fat_fs, u32 *space);

int f_opendir_by_name(FATFS *pFatHdler, const char *path, DIR *dj);

#endif

