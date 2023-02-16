#ifndef __BOOT_H__
#define __BOOT_H__

#include "asm_type.h"

#define USE_SYDFILE_NEW 1

struct vm_info {
#if (USE_SYDFILE_NEW == 1)
    u8 align;		  			//from uboot, 按 n * 256 对齐
#endif
    u32 vm_saddr;				//from sdfile, flash addr
    u32 vm_res;					//reverse_bytes
    u32 vm_size;		  		//from sdfile
};

struct sfc_info {
    u32 sfc_base_addr;  		//flash memory addr, from uboot
    u32 app_addr;  				//cpu logic addr, from uboot
};


#if (USE_SYDFILE_NEW == 1)
struct bt_mac_addr {
    u8 value[6];
    u16 value_crc;
};

typedef struct _boot_info {
    struct vm_info vm;
    struct sfc_info sfc;
    u32 flash_size; 		//from uboot
    u16 chip_id; 			//from uboot
    u16 trim_value; 		//from uboot
    // struct bt_mac_addr mac;
    u32 up_suc_flag;
} BOOT_INFO;

//=====================================
struct flash_head {
    u16 crc;
    u16 size4burner;
    u8 vid[4]; 	//u32 vm_eaddr;
    u32 FlashSize;
    u8 FsVersion;  //flash文件结构
    u8 align; 	//对齐 n * 256
    u8 res;
    u8 SpecialOptFlag;
    u8 pid[16];
};

typedef struct boot_device_info {
    struct flash_head *fs_info;
    struct sfc_info sfc;
    u16 chip_id; 			//from uboot
    u16 trim_value; 		//from uboot
    u8 bt_mac_addr[8];
} BOOT_DEVICE_INFO;

struct flash_wp_arg {
    u8 split_mode: 1;       //0：连续写，1：分开写
    u8 write_en_use_50h: 1;//0：06H     1：50H
    u8 res: 2;
    u8 numOfwp_array: 4;    //写保护参数的个数
    struct {
        u8 sr1;
        u8 sr2;
        u16 wp_addr;        //写保护结束地址，单位K
    } wp_array[9];
} __attribute__((packed));
#else

#endif  /* #if (USE_SYDFILE_NEW == 1) */

extern BOOT_INFO boot_info;

#endif

