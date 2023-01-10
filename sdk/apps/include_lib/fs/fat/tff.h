#ifndef _FATFS_H_
#define _FATFS_H_

#include "typedef.h"

#define FF_FS_EXFAT 		1

#define D_MAX_DEEPTH 		9
#define D_LFN_MAX_SIZE 		512

#define MAX_DEEPTH 		D_MAX_DEEPTH		/* 0~9 deepth of system */

#define SZDIRE 			(32)
#define DIR_BUFF_LEN   	(SZDIRE*(2+4))

#define FS_WATCH_JLFAT_ENABLE 0 //外挂flash JLFAT 文件系统
#define FS_FILE_INSERT_DEAL 0  //文件插入、分割处理
#define FS_DIRINFO_SAVE_ENABLE 0 //文件搜索加速处理
#define FS_WRITE_VOL_ENABLE 0 //写卷标
#define FS_BPINFO_SET 0 //断点加速
#define FS_FOPEN_LONG 0 //长文件名打开方式
#define FS_USE_SIMPLE_MODE 1 //fselect 文件系统简单处理模式
#define FS_RENAME_ENABLE 0 //重命名使能
#define FS_W_WOL_ENABLE 0 //写卷标使能
#define FS_FORMAT_ENABLE 0 //格式化功能使能
#define FS_FGET_PATH_ENABLE 1 //获取路径使能

#define FS_SAVE_FAT_TABLE_EN 0 //seek 加速处理
#define FS_SEEK_OVER 0 //seek 加速处理,是否一开始就存完所有簇

#define     _xdata
// #define     _data
#define     _root
#define     _no_init
#define     _banked_func

#define my_memcpy		memcpy
#define my_memset		memset
#define my_mem_cmp		mem_cmp

/*******************************************************/
/********************驱动调试信息***********************/
/*******************************************************/
// #define FS_DEBUG

u16 ld_word_func(u8 *p);
u32 ld_dword_func(u8 *p);
void st_word_func(u8 *ptr, u16 val);
void st_dword_func(u8 *ptr, u32 val);

#define FS_LD_WORD(p)        ld_word_func(p)//(u16)(*(u16*)(u8*)(ptr))
#define FS_LD_DWORD(p)       ld_dword_func(p)//(u32)(*(u32*)(u8*)(ptr))

#define FS_ST_WORD(ptr,val)         st_word_func((u8 *)(ptr),(u16)(val)) //(u16)(*(u16*)(u8*)(ptr))
#define FS_ST_DWORD(ptr,val)        st_dword_func((u8 *)(ptr),(u32)(val)) //(u32)(*(u32*)(u8*)(ptr))


typedef u32	 CLUST;
typedef u32  UINT;

#define _FS_READONLY               0
#ifdef D_IS_FLASH_SYSTEM
#define FS_WRITE_EN                1//  [> 是否允许写文件 <]
#else
#define FS_WRITE_EN                0//  [> 是否允许写文件 <]
#endif


#ifdef  FS_WRITE_EN

#define FAT12_WRITE_EN

#if  FF_FS_EXFAT
#define EXFAT_WRITE_EN
#endif
#endif

#define WIN_DATA_DIRTY 0x08


#if FS_DIRINFO_SAVE_ENABLE
typedef struct {
    // u32 clust;
    u8 clust[4];
    u16 counter;
    u16 outdir_num;
    u16 file_in_dir;
    // u8 deepth;
    // u8 is_file;
} DirBaseInfo;

typedef struct {
    DirBaseInfo *d_info;
    u16 max_num;
    int c_num;
    u16 last_num;
    u8 flag;
    u8 scan_flag;
} DIR_INFO_SAVE;
#endif



typedef   u32(*DISK_READ)(void *hdev, u8 *buf, u32 lba);	/* device read function */
typedef   u32(*DISK_WRITE)(void *hdev, u8 *buf, u32 lba);	/* device read function */

typedef struct {
    u8      win[512];
    u32     sector;
    u8      w_flag;
} SWIN_BUF;

#define FS_WIN_START  fs->wbuf.win
#define FS_WIN_SECTOR fs->wbuf.sector
#define FS_WIN_FLAG   fs->wbuf.w_flag



/* File system object structure */
struct _FATFS {
    u32		fatbase;		/* FAT start sector */
    u32		dirbase;		/* Root directory start sector */
    u32		database;		/* Data start sector */
    u32 	n_fatent;		/* Maximum cluster# + 1 */
    u32     total_file;		/* 当前设备的匹配文件总数*/
    u16		n_rootdir;		/* Number of root directory entries */
#ifdef FS_WRITE_EN
    u8      write_en;       //文件系统是否允许写操作：只允许FAT16，FAT32并且sector size只能是512bytes
    u8	    fsi_flag;		/* fsinfo dirty flag (1:must be written back) */
    u32	    last_clust;		/* Last allocated cluster */
    u32	    fsi_sector;		/* fsinfo sector (FAT32) */
    char    *nops;          //解析路径时的相对位置
#endif
    // u32     csect;          //当前访问的目录
    u32     boot_sect;
    u8		s_size;			/* sector size, 2 power n*/
    u8		csize;			/* Number of sectors per cluster,2 power n */
    u8		fs_type;		/* FAT sub type */
    u8		sector_512size;	/* size of per sector */
    // u8      *lfn_buff;      //用于保存目录项时的长文件名，长度至少为520 Bytes
    u32     file_total;         //文件总数
    u32     dir_total;			// 文件夹序总数
    void    *hdev;
    DISK_READ disk_read;
    DISK_WRITE disk_write;
    SWIN_BUF wbuf;
    u32		total_folder;
#if  FF_FS_EXFAT
    u8		dir_buff[DIR_BUFF_LEN];
#endif
    //u8      win[512];
    //u32     sector;
    //u8      w_flag;
    u8      pdrv;
    char 	*ext_type;
    char    *lfn;
    u8      lfn_len;
};
typedef struct _FATFS FATFS;

struct _FS_NAME {
    char    tpath[128];       //路径名称buffer
    char    *lfn;             //长文件名buffer，长度LFN_MAX_SIZE×2，（前LFN_MAX_SIZE）为长文件名，（后LFN_MAX_SIZE）为长文件夹名
    char    *ldn;             //长文件名buffer，长度LFN_MAX_SIZE×2，（前LFN_MAX_SIZE）为长文件名，（后LFN_MAX_SIZE）为长文件夹名
    u16     lfn_cnt;
    u16     ldn_cnt;
};
typedef struct _FS_NAME FS_NAME;

/* File object structure */
typedef u32 FSIZE_t;


typedef struct {
    u8      attrib;
    u32     sclust;
    FSIZE_t fsize;
} FFOBJID;

typedef struct {
    u32     n_frag;//新申请到的簇连续时的计数。
    u32     n_cont;
} W_DIRINFO;

typedef struct {
    FFOBJID obj;
    W_DIRINFO wdir;
#if FF_FS_EXFAT
    u32     cd2_sect;
    u32     cd2_clust;
    u16     cd2_index;
#endif
    u16     cindex;
    u32 	clust;		        /* Current cluster */
    u32		csect;		        /* Current sector */
} DIR;

typedef struct {
    FATFS   *fs;
    FFOBJID obj;
    W_DIRINFO wdir;
#if FF_FS_EXFAT
    u16     cd2_index;
    u32     cd2_sect;
    u32     cd2_clust;
#endif
    DIR     dj;
    u32     CrtTime;
    u32     WrtTime;
    u16     AccTime;
    char 	fname[16];
} DIRINFO;

/* Directory object structure */
#if 0
typedef struct {
    u32		csect;		        /* Current sector */
    u32		start_clust;		        /* Start cluster */
    u32     end_clust;
    u16     cindex;
    u8      attrib;
} DIRE;

typedef struct {
    u32 	clust;		        /* Current cluster */
    u32		csect;		        /* Current sector */
    u32		sclust;		        /* Start cluster */
    u16     cindex;
    u8      attrib;
    DIRE    parent_dir;
} DIR;
#endif

typedef struct {
    u32 	clust;		        /* Current cluster */
    u32		csect;		        /* Current sector */
    u16     cindex;
} DIR_INDEX;


#define CLUST_RANG_NUM  10
typedef struct {
    u32     start_clust[CLUST_RANG_NUM];
    u32     end_clust[CLUST_RANG_NUM];
    u32     clust_num[CLUST_RANG_NUM];
} CLUST_RANG;

/* File system API info */
typedef struct _FSMSG {
    DIRINFO dinfo;
    DIR     dj[MAX_DEEPTH];
    u32	    musicdir_counter;    // 包含指定文件的文件夹序号
    u32		dir_counter;		 // 文件夹序号
    u32     file_total_indir;     // 当前目录的根下有效文件的个数
    u32     file_total_outdir;	// 当前目录前的文件总数，目录循环模式下，需要用它来计算文件序号
    u32     file_number;
    u32     file_counter;
    u8      deepth;
} FSMSG;


#if FS_SAVE_FAT_TABLE_EN
typedef struct _FAT_TABLE {
    u16 base;       //保存fat表项的间隔基数
    u16 total;      //保存的表项总数
    u16 buf_len;
    u32 *table;
} FAT_TABLE;
#endif


typedef struct {
    FATFS   *fs;
    FSMSG   fs_msg;
    FSIZE_t fptr;			// File R/W pointer
    u32		csect;			// Current sector
    u32 	clust;			// Current cluster
    u32     start_clust;
    u32     end_clust;
#ifdef FS_WRITE_EN
    u32     dir_sector;
    //u8      win[512];           // File private data read/write window
    //u32     sector;             // Sector number appearing in win[]
    //u8      w_flag;
    u8      dir_duty;
    SWIN_BUF *wbuf;
#endif
    u8      flag;               // File status flags
    FS_NAME	fs_n;
    char    *lfn;
    u8      lfn_len;
    // char    upcase_name[12];

#if 0
    u32     offset; //sdfile小文件 起始偏移
    u32     file_len; //sdfile小文件的大小
    u32     in_fseek; //是否跑了seek
    u32     elen;     //长度达到后擦除
    u32     first_readfat;//起始先读一次fat,保存多簇块。
    CLUST_RANG clust_rang;
#endif
#if FS_SAVE_FAT_TABLE_EN
    FAT_TABLE fat;
#endif
} FIL;

typedef struct _LDIR_INFO {
    u8 ldir_ord;
    u8 ldir_name1[10];             //文件的目录项信息
    u8 ldir_attr0;
    u8 ldir_type0;
    u8 ldir_chksum0;
    u8 ldir_name2[12];
    u16 ldir_fst_clus_lo;
    u8 ldir_name3[4];
} LDIR_INFO;

/* File function return code (FRESULT) */
typedef enum {
    FR_OK = 0,
    FR_FIND_DIR = 0x80,
    FR_FIND_FILE,
    FR_DIR_END,         //前面几个位置不能变
    FR_NO_FILE,
    FR_NO_PATH,
    FR_EXIST,
    FR_INVALID_NAME,
    FR_INVALID_DRIVE,
    FR_DENIED,
    FR_RW_ERROR,
    FR_WRITE_PROTECTED,
    FR_NO_FILESYSTEM,
    FR_DEEP_LIMIT,
    FR_END_PATH,
    FR_FILE_LIMIT,
    FR_END_FILE,
    FR_LFN_ERR,
    FR_MKFS_ABORTED,
    FR_MKFP_ABORTED,
    FR_DIR_DELETE,
    FR_DISK_ERROR,
    FR_FILE_END,
    FR_FILE_ERR,
    FR_NO_WINBUF,
    FR_INT_ERR,				/* (2) Assertion failed */
    FR_NO_SEL_DRIVE,
    FR_PART_PATH_OK,
    FR_ALL_PATH_OK,
} FRESULT;


/* File access control and file status flags (FIL.flag) */
#define FA_OPEN_EXISTING	0x00
#ifdef  FS_WRITE_EN
#define FA_CREATE_HIDDEN    0x02
#define FA_WRITE			0x04            //是否允许写文件
#define FA_CREATE_NEW		0x08            //文件不存在时创建
#define FA_CREATE_ALWAYS	0x10            //无论文件中否存在，均创建
#endif

#define FA__ERROR           0x80            //文件错误
#define FDISK__ERROR		0x40            //设备错误


#define DDE					0xE5	/* Deleted directory entry mark in DIR_Name[0] */
#define NDDE				0x05	/* Replacement of the character collides with DDE */

#define SZ_DIR      32


//#ifdef __SMART_CPU__

//#endif


/* File function return code (FRESULT) */


#define _DF1S      1
/* Character code support macros */

#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))

#if _DF1S		/* DBCS configuration */

#ifdef _DF2S	/* Two 1st byte areas */
#define IsDBCS1(c)	(((u8)(c) >= _DF1S && (u8)(c) <= _DF1E) || ((u8)(c) >= _DF2S && (u8)(c) <= _DF2E))
#else			/* One 1st byte area */
//#define IsDBCS1(c)	((u8)(c) >= _DF1S && (u8)(c) <= _DF1E)
#define IsDBCS1(c)	((u8)(c) < ' ' && (u8)(c) > '~')
#endif

#ifdef _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	(((u8)(c) >= _DS1S && (u8)(c) <= _DS1E) || ((u8)(c) >= _DS2S && (u8)(c) <= _DS2E) || ((u8)(c) >= _DS3S && (u8)(c) <= _DS3E))
#else			/* Two 2nd byte areas */
#define IsDBCS2(c)	(((u8)(c) >= _DS1S && (u8)(c) <= _DS1E) || ((u8)(c) >= _DS2S && (u8)(c) <= _DS2E))
#endif

#else			/* SBCS configuration */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* _DF1S */

/* FAT sub-type boundaries */
/* Note that the FAT spec by Microsoft says 4085 but Windows works with 4087! */
#define MIN_FAT16	4086	/* Minimum number of clusters for FAT16 */
#define	MIN_FAT32	65526	/* Minimum number of clusters for FAT32 */


#define ROOT_DIR    0

/* FAT sub type */
#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3
#define FS_EXFAT	4


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_FCH	0x80    /* exFAT下,文件簇连续标志 */
#define AM_FRG	0x40    /* exFAT下,文件簇BITMAP 有碎片 */
#define AM_LFN0_START	0x1
#define AM_LFN0_LEN	    10
#define AM_LFN1_START	0xE
#define AM_LFN1_LEN	    12
#define AM_LFN2_START	0x1C
#define AM_LFN2_LEN	    4
#define AM_LFN_LEN	    26

#define AM_EX_DIR_STRETCHED 0x08

/* Offset of FAT structure members */

#define BS_jmpBoot			0
#define BS_OEMName			3
#define BPB_BytsPerSec_l	11
#define BPB_BytsPerSec_h	12
#define BPB_SecPerClus		13
#define BPB_RsvdSecCnt		14
#define BPB_NumFATs			16
#define BPB_RootEntCnt		17
#define BPB_TotSec16		19
#define BPB_Media			21
#define BPB_FATSz16			22
#define BPB_SecPerTrk		24
#define BPB_NumHeads		26
#define BPB_HiddSec			28
#define BPB_TotSec32		32
#define BS_55AA				510

#define BS_DrvNum			36
#define BS_BootSig			38
#define BS_VolID			39
#define BS_VolLab			43
#define BS_FilSysType		54

#define BPB_FATSz32			36
#define BPB_ExtFlags		40
#define BPB_FSVer			42
#define BPB_RootClus		44
#define BPB_FSInfo			48
#define BPB_BkBootSec		50
#define BS_DrvNum32			64
#define BS_BootSig32		66
#define BS_VolID32			67
#define BS_VolLab32			71
#define BS_FilSysType32		82
#define BS_FileSysTypeexFAT				5
#define BPB_FatOffset					80
#define BPB_FatLength					84
#define BPB_ClusterHeapOffset			88
#define BPB_ClusterCount				92
#define BPB_FirstClusterOfRootDirectory	96
#define BPB_VolumeFlags					106
#define BPB_BytesPerSectorShift			108
#define BPB_SectorsPerClusterShift		109
#define BPB_NumberOfFats				110
#define MBR_Table			            446
#define	FSI_LeadSig			0	/* FSI: Leading signature (4) */
#define	FSI_StrucSig		484	/* FSI: Structure signature (4) */
#define	FSI_Free_Count		488	/* FSI: Number of free clusters (4) */
#define	FSI_Nxt_Free		492	/* FSI: Last allocated cluster (4) */

///for FAT12/FAT16/FAT32
#define	DIR_Name			0	/* Short file name (11) */
#define	DIR_Attr			11	/* Attribute (1) */
#define	DIR_NTres			12	/* NT flag (1) */
#define DIR_CrtTimeTenth	13	/* Created time sub-second (1) */
#define	DIR_CrtTime			14	/* Created time (2) */
#define	DIR_CrtDate			16	/* Created date (2) */
#define DIR_LstAccDate		18	/* Last accessed date (2) */
#define	DIR_FstClusHI		20	/* Higher 16-bit of first cluster (2) */
#define	DIR_WrtTime			22	/* Modified time (2) */
#define	DIR_WrtDate			24	/* Modified date (2) */
#define	DIR_FstClusLO		26
#define	DIR_FileSize		28
#define	LDIR_Attr			11	/* LFN attribute (1) */
#define	LDIR_Type			12	/* LFN type (1) */
#define	LDIR_Chksum			13	/* Sum of corresponding SFN entry */
////for exFAT
#define DIR_FileChainFlags	1
#define DIR_NameLen		    3
#define DIR_AttrexFAT		4
#define DIR_FileSizeexFAT   8
#define DIR_FstClustexFAT	20
#define DIR_FileSize2exFAT  24

//0x85
#define XDIR_Type2exFAT      0
#define XDIR_Num2exFAT       1
#define XDIR_Sum2exFAT       2
#define XDIR_Attr2exFAT      4
#define XDIR_CreTime2exFAT   8
#define XDIR_ModTime2exFAT   12
#define XDIR_AccTime2exFAT   16

//0xc0
#define XDIR_GenFlags2exFAT  1
#define XDIR_NumName2exFAT   3
#define XDIR_NameHash2exFAT  4
#define XDIR_FstClus2exFAT   20
#define XDIR_fsize12exFAT    8
#define XDIR_fsize22exFAT    0x18


#define DDEM   0xE5

#define EXFAT_YEAR    ((2000L-1980L) << 25)
#define EXFAT_MONTH   (1L << 21)
#define EXFAT_DATE    (1L << 16)
#define EXFAT_HOUR    (0L << 11)
#define EXFAT_MINUTE  (0L << 5)
#define EXFAT_SEC     (0L / 2)
#define EXFAT_TIME    ( EXFAT_YEAR | EXFAT_MONTH | EXFAT_DATE | EXFAT_HOUR | EXFAT_MINUTE | EXFAT_SEC   )


/*-----------------------------------------------------*/
/* Tiny-FatFs module application interface             */
#ifdef __cplusplus
extern "C" {
#endif
u8 get_powerof2(u32 n);
FRESULT _f_mount(FATFS *fs, u32 bootsect, char pdrv_sel);
//FRESULT f_open(FATFS *fs, FIL *fp, const u8 *path, u8 mode);

int f_read(FIL *fp, u8 *buff, u32 btr);

FRESULT  f_seek(FIL *fp, u8 type, u32 offsize);

FRESULT  f_seek_watch(FIL *fp, u8 type, u32 offsize);//手表seek
u32 get_fat_extern(FATFS *fs, u32 clust);

FRESULT f_readnextdir(FATFS *fs, DIR *dj, DIRINFO *dinfo, FS_NAME *fs_n);
void  f_opendir(FATFS *fs, DIR *dj);
void f_open_by_dirinfo(FATFS *fs, FIL *fp, DIRINFO *dinfo, u8 mode);
bool file_name_cmp(char *src, char *dst, u8 len);
u8 f_tell_status(FIL *fp);

u32 clust2sect(FATFS *fs, u32 clust);

FRESULT f_mkdir(FATFS *fs, const char *path, u8 mode, FIL *fp);
FRESULT f_open(FATFS *fs, FIL *fp, char *path, char *lfn_buf, u8 mode);

FRESULT dir_clear(FATFS *fs, u32 clst);

FRESULT  move_window(FATFS *fs, SWIN_BUF *wbuf, u32 sector);
FRESULT  sync_window(FATFS *fs, SWIN_BUF *wbuf);
FRESULT f_open_dir(FATFS *fs, DIR *dj, char *path);

FRESULT check_fs(FATFS *fs, u32 bootsect);

FRESULT fat_format(FATFS _xdata *fs, u32 capacity, u32 clust_size);

FRESULT f_write_vol(FATFS *fs,  char *vol_name);

FRESULT f_ReName(FATFS *fs, FIL *fp, DIR *dj, char *sfn);
u16 lfn_decode(char  *p, u16 max_copy);
FRESULT f_GetName(FATFS *fs,  DIR *dj, u8 *pbuf, int *buflen, u8 *is_dir);
FRESULT f_get_path_dj(FATFS *fs, DIR *dj, DIR *dir_dj, u32 clust);

FRESULT f_save_table(FIL *fp, u8 *buff, u16 btr);

int f_insert(FIL *fp, FIL *i_fp, u32 fptr);

int f_division_file(FIL *fp, u32 fptr, char *file_name);

u32 get_cluster_rang(FIL *fp, u32 clust);

void create_name(
    u8 *sfn,			/* Pointer to the directory object */
    const char **path	/* Pointer to pointer to the segment in the path string */
);

FRESULT fill_last_frag(
    //DIRINFO* dinfo,   /* Pointer to the corresponding object */
    FATFS *fs,
    W_DIRINFO *wdir,
    u32 lcl,      /* Last cluster of the fragment */
    u32 term      /* Value to set the last FAT entry */
);
FRESULT fill_first_frag(
    //DIRINFO *dinfo,        /* Pointer to the corresponding object */
    FATFS *fs,
    FFOBJID *obj,
    W_DIRINFO *wdir
);
u32 create_chain(  /* 0:No free cluster, 1:Internal error, 0xFFFFFFFF:Disk error, >=2:New cluster# */
    FATFS *fs,          /* File system object */
    FFOBJID *obj,
    W_DIRINFO *wdir,
    u32 clst            /* Cluster# to stretch. 0 means create a new chain. */
);
FRESULT follow_path(  /* FR_OK(0): successful, !=0: error code */
    FATFS *fs,
    DIR *dj,			/* Directory object to return last directory and found object */
    char *sfn,
    DIRINFO *dinfo,
    const char *path,	/* Full-path string to find a file or directory */
    FS_NAME *lfn
);
int f_write(
    FIL *fp,            /* Pointer to the file object */
    u8 *buff,   /* Pointer to the data to be written */
    u32 btw         /* Number of bytes to write */
);
FRESULT f_sync_fs(
    FIL *fp     /* Pointer to the file object to be closed */
);
FRESULT f_unlink(
    FIL *fp
);
void load_dirinfo(DIR *dj, DIRINFO  *dir_info);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern u32 g_cnt;
#ifdef __cplusplus
}
#endif

#endif /* _FATFS */
