
#ifndef _NORFLASH_H
#define _NORFLASH_H

#include "device.h"
#include "ioctl_cmds.h"
#include "spi.h"
#include "gpio.h"

/*************************************************/
/*
		COMMAND LIST - WinBond FLASH WX25X
*/
/***************************************************************/
#define WINBOND_WRITE_ENABLE		                        0x06
#define WINBOND_READ_SR1			                        0x05
#define WINBOND_READ_SR2			                        0x35
#define WINBOND_WRITE_SR1			                        0x01
#define WINBOND_WRITE_SR2			                        0x31
#define WINBOND_READ_DATA		                            0x03
#define WINBOND_FAST_READ_DATA		                        0x0b
#define WINBOND_FAST_READ_DUAL_OUTPUT                       0x3b
#define WINBOND_PAGE_PROGRAM	                            0x02
#define WINBOND_PAGE_ERASE                                  0x81
#define WINBOND_SECTOR_ERASE		                        0x20
#define WINBOND_BLOCK_ERASE		                            0xD8
#define WINBOND_CHIP_ERASE		                            0xC7
#define WINBOND_JEDEC_ID                                    0x9F

enum {
    FLASH_PAGE_ERASER,
    FLASH_SECTOR_ERASER,
    FLASH_BLOCK_ERASER,
    FLASH_CHIP_ERASER,
};

struct norflash_dev_platform_data {
    int spi_hw_num;         //只支持SPI1或SPI2
    u32 spi_cs_port;        //cs的引脚
    u32 spi_read_width;     //flash读数据的线宽
    const struct spi_platform_data *spi_pdata;
};

#define NORFLASH_DEV_PLATFORM_DATA_BEGIN(data) \
	const struct norflash_dev_platform_data data = {

#define NORFLASH_DEV_PLATFORM_DATA_END()  \
};


extern const struct device_operations norflash_dev_ops;

void set_norflash_base_addr(u32 addr);
void set_norflash_user_capacity(u32 capacity);
int norflash_init(struct norflash_dev_platform_data *pdata);
int norflash_open(void);
int norflash_close(void);
int norflash_read(u32 addr, u8 *buf, u32 len);
int norflash_write(u32 offset, void *buf, u32 len);
int norflash_eraser(u8 eraser, u32 addr);
int norflash_ioctl(u32 cmd, u32 arg);

#endif


