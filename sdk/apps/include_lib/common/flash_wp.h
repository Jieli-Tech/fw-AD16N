#ifndef _FLASHE_WP_H_
#define _FLASHE_WP_H_

#include "dev_mg/device.h"
#include "boot.h"
#include "typedef.h"

#define FLASH_WP_INFO_PATH  "/dir_sys_info/wp_flash.bin"
#define FLASH_WP_HEAD_TAG   (0x20220823)

#define FLASH_WP_UART_DEBUG
#ifdef FLASH_WP_UART_DEBUG
#define wp_printf         log_info
#define wp_buf            log_info_hexdump
#else
#define wp_printf(...)
#define wp_buf(...)
#endif

struct flash_wp_arg {
    u8 split_mode: 1;       //0 连续写，1，分开写
    u8 write_en_use_50h: 1; //0:06H     1:50H
    u8 res: 2;
    u8 numOfwp_array: 4;            //写保护参数的个数
    struct {
        u8 sr1;
        u8 sr2;
        u16 wp_addr;    //写保护结束地址,单位K
    } wp_array[9];
} __attribute__((packed));

struct flash_wp_head {
    u32 tag;
    u16 crc;
    u16 version;
    u32 res;
    u32 flash_cnt;
};

struct flash_wp_item_head {
    u16 crc;
    u16 data_crc;
    u32 flash_id;
    u32 offset;
    u32 length;
};

struct flash_wp_item {
    u16 crc;
    u16 length;
    struct flash_wp_arg arg;
} __attribute__((packed));

extern int norflash_write_protect_config(struct device *device, u32 addr, struct flash_wp_arg *p);
extern u16 norflash_read_sr1_sr2(void);
struct flash_wp_arg *find_inrflash_wp_arg(void);
struct flash_wp_arg *find_flash_wp_file_arg(void);
int norflash_set_write_protect(u8 enable_write_protect);
#endif
