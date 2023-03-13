#ifndef _NORFLASH_H
#define _NORFLASH_H

#include "device.h"
#include "ioctl_cmds.h"
// #include "asm/spi.h"
#include "printf.h"
#include "gpio.h"
#include "device_drive.h"
#include "malloc.h"
#include "app_config.h"

//#include "ioctl.h"
//#include "config.h"
//#include "clock.h"
//#include "cpu.h"
//#include "device.h"
#include "spi.h"
//#include "typedef.h"
//#include "gpio.h"
#if defined(TFG_EXT_FLASH_EN) && TFG_EXT_FLASH_EN
#define TCFG_NORFLASH_DEV_ENABLE //启动 nor flash
#endif

#define NORFLASH_NO_SYS                      1     //0:使用操作系统,1:不使用操作系统
#if NORFLASH_NO_SYS
// static inline void os_mutex_create(OS_MUTEX *mutex) {}
// static inline void os_mutex_pend(OS_MUTEX *mutex, int i) {}
// static inline void os_mutex_post(OS_MUTEX *mutex) {}
#include "jiffies.h"
#include "wdt.h"
typedef volatile u8 flash_mutex;
static inline void flash_mutex_create(flash_mutex *sem, u8 count)//初始化
{
    *sem = count;
}
static inline void flash_mutex_post(flash_mutex *sem)//
{
    (*sem) = 1;
}
extern u32 maskrom_get_jiffies(void);
static inline s8 flash_mutex_pend(flash_mutex *sem, u32 timeout)// 当为timeout=0时，死等
{
    u32 _jiffies = maskrom_get_jiffies();
    u32 _timeout = timeout + _jiffies;
    extern void wdt_clear();
    while (1) {
        if (*sem) {
            (*sem) = 0;
            break;
        }
        if ((timeout != 0) && (_timeout < _jiffies)) {
            return -1;
        }
        wdt_clear();
    }
    return 0;
}
static inline s8 query_flash_mutex_pend(flash_mutex *sem, u32 timeout)// 当为timeout=0时，死等
{
    while (1) {
        if (*sem) {
            (*sem) = 0;
            break;
        }
        return -1;
    }
    return 0;
}
static inline void flash_mutex_set(flash_mutex *sem, u8 count)
{
    *sem = count;
}
#else
#endif
/*************************************************/
/*
		COMMAND LIST - WinBond FLASH WX25X
*/
/***************************************************************/
#define WINBOND_WRITE_ENABLE		        0x06
#define WINBOND_READ_SR1			  		0x05
#define WINBOND_READ_SR2			  		0x35
#define WINBOND_WRITE_SR1			  		0x01
#define WINBOND_WRITE_SR2			  		0x31
#define WINBOND_READ_DATA		        	0x03
#define WINBOND_FAST_READ_DATA		    	0x0b
#define WINBOND_FAST_READ_DUAL_OUTPUT   	0x3b
#define WINBOND_PAGE_PROGRAM	            0x02
#define WINBOND_PAGE_ERASE                  0x81
#define WINBOND_SECTOR_ERASE		        0x20
#define WINBOND_BLOCK_ERASE		          	0xD8
#define WINBOND_CHIP_ERASE		          	0xC7
#define WINBOND_JEDEC_ID                    0x9F
#define WINBOND_POWER_DOWN                  0xB9
#define WINBOND_RELEASE_POWER_DOWN          0xAB

enum {
    FLASH_PAGE_ERASER,
    FLASH_SECTOR_ERASER,
    FLASH_BLOCK_ERASER,
    FLASH_CHIP_ERASER,
};


struct norflash_dev_platform_data {
    s8 spi_hw_num;         //只支持SPI1或SPI2
    u8 spi_cs_port;        //cs的引脚
    u8 spi_read_width;     //flash读数据的线宽
    const struct spi_platform_data *spi_pdata;
    u32 start_addr;         //分区起始地址
    u32 size;               //分区大小，若只有1个分区，则这个参数可以忽略
};

#define NORFLASH_DEV_PLATFORM_DATA_BEGIN(data) \
	const struct norflash_dev_platform_data data = {

#define NORFLASH_DEV_PLATFORM_DATA_END()  \
};


extern const struct device_operations norflash_dev_ops;
extern const struct device_operations norfs_dev_ops;

void _norflash_power_down();
void _norflash_release_power_down();
#endif



