#ifndef _UPDATE_H_
#define _UPDATE_H_

#include "typedef.h"
#include "csfr.h"

struct data_info {
    u32 addr;
    u32 len;
    u32 run_addr;
};

struct ufw_syd_head_v1 {
    u16 Crc;                // crc16 for this struct
    u16 CrcOfSydFileHead;
    u32 FileLength;         // file length
    u16 FileCount;          // 此FW文件中包含的子文件个数
    u16 Version;
    u16 HeadAlignmentSize;
    u16 Res;
    char szChipName[16];    // 此FW文件对应的芯片类型，如AC690X，AC691X
    u32 Res2[4];
    u32 Res3[4];
} _GNU_PACKED_;

struct ufw_file_head_v1 {
    u8 FileType;            // 文件类型
    u8 Res;                 // 保留
    u16 Index;              // 文件索引号
    u16 Crc;                // 明文数据的校验码
    u16 Version;            // 本结构体版本
    u32 Addr;               // 地址
    u32 Length;             // 数据长度
    u32 AllLength;          // 数据长度+尾部对齐的数据长度
    u32 EncryptedAddr;      // 加密数据的地址偏移（相对u32Addr的地址）
    u32 EncryptedLength;    // 加密的数据长度
    union {
        // 当文件类型是FILE_TYPE_FW_RESERVE_ZONE_FILE有效
        struct {
            u32 ReserveZoneAddress;
            u32 ReserveZoneLength;
            char szReserveZoneName[12];
        } _GNU_PACKED_;
        struct {
            u32 Res0;                 // 保留
            u32 Res1[4];             // 保留
            u32 Res2[4];             // 保留
        } _GNU_PACKED_;
    };
    char name[16];
} _GNU_PACKED_;

struct UPDATA_PARM {
    u16 parm_crc;
    u16 parm_type;              //UPDATA_TYPE:sdk pass parm to uboot
    u16 parm_result;            //UPDATA_TYPE:uboot return result to sdk
    u16 magic;                  //0x5441
    u8  file_patch[32];         //updata file patch
    u8  ota_loader_patch[32];          //sd updata
    u32 ota_addr;
    u16 ext_arg_len;
    u16 ext_arg_crc;
};

struct UPDATA_EXT_PARM {
    char sd_port;
    u32 sd_speed;
    u32 porta_die_dir;
    u32 porta_pu_pd;
    u32 porta_out_dieh;
    u32 portb_die_dir;
    u32 portb_pu_pd;
    u32 portb_out_dieh;
};

struct jlfs_file_head {
    u16 head_crc;
    u16 data_crc;
    u32 addr;
    u32 len;
    u8 attr;
    u8 res;
    u16 index;
    char name[16];
};

enum {
    NO_ERROR,
    SFC_OPEN_ERROR,
    DEVIVE_OPEN_ERROR,
    FAT_MOUNT_ERROR,
    FILE_OPEN_ERROR,
    FIND_FLASH_BIN_ERROR,
    FIND_FLASH2_BIN_ERROR,
    NO_NEED_UPGRADE,
    FIND_OTA_ERROR,
    FIND_LOADER0_ERROR,
    FLASH_SIZE_ERROR,
};

u32 try_to_upgrade(char *dev_name, char *up_file_path);

// u32 get_up_suc_flag(void);
#endif
