#ifndef _SD_H_
#define _SD_H_

#include "typedef.h"
#include "device.h"

#define     SD_CMD_DECT     0
#define     SD_CLK_DECT     1
#define     SD_IO_DECT      2

enum cmd_dect_rule {
    CMD_DECT_WITH_TRANS = 1,        //卡在读写过程中，会穿插CMD检测命令
    CMD_DECT_WITHOUT_TRANS = 2,     //卡在读写过程中，不会穿插CMD检测命令
    CMD_DECT_FORCE_OFFLINE = 0x80,  //CMD检测强制离线(用于自定义检测)
    CMD_DECT_FORCE_ONLINE = 0x81,   //CMD检测强制在线(用于自定义检测)
};

struct sdmmc_platform_data {
    char port[3];
    u8 irq;
    u8 data_width;
    u8 priority;
    u8 detect_mode;
    u8 detect_io;
    u8 detect_io_level;
    u32 detect_time_interval;
    u32 detect_timeout;
    u32 speed;
    JL_SD_TypeDef *sfr;
    int (*detect_func)(const struct sdmmc_platform_data *);
    void (*port_init)(const struct sdmmc_platform_data *, int mode);
    int (*power)(int on);
};

extern const struct device_operations sd_dev_ops;
extern void sd0_dev_detect(void *p);
extern void sdx_force_set_online(char *sdx);

extern int sd_set_power(u8 enable);
extern u8 sd_io_suspend(u8 sdx, u8 sd_io);
extern u8 sd_io_resume(u8 sdx, u8 sd_io);
extern u32 sdx_dev_send_suspend_event(u8 event_idx);

extern void sdmmc_0_port_init(const struct sdmmc_platform_data *, int mode);

extern int sdmmc_0_clk_detect(const struct sdmmc_platform_data *);
extern int sdmmc_0_io_detect(const struct sdmmc_platform_data *);
extern int sdmmc_0_cmd_detect(const struct sdmmc_platform_data *);

#define SD0_PLATFORM_DATA_BEGIN(data) \
	/*static*/ const struct sdmmc_platform_data data = {

#define SD0_PLATFORM_DATA_END() \
	.irq 					= IRQ_SD0_IDX, \
    .sfr                    = JL_SD0, \
	.port_init 				= sdmmc_0_port_init, \
	.detect_time_interval 	= 250, \
	.detect_timeout     	= 500, \
};

extern int sdx_dev_init(const struct dev_node *node, void *arg);
extern bool sdx_dev_online(const struct dev_node *node);
extern int sdx_dev_open(const char *name, struct device **device, void *arg);
extern int sdx_dev_byte_read(struct device *device, void *buf, u32 len, u32 offset);
extern int sdx_dev_byte_write(struct device *device, void *buf, u32 len, u32 offset);
extern int sdx_dev_read(struct device *device, void *buf, u32 len, u32 offset);
extern int sdx_dev_write(struct device *device, void *buf, u32 len, u32 offset);
extern int sdx_dev_ioctl(struct device *device, u32 cmd, u32 arg);
extern int sdx_dev_close(struct device *device);
int set_sd_power(int en);

//*****************sd_cdrom区
int sd_cdrom_dev_init(const struct dev_node *node, void *arg);
bool sd_cdrom_dev_online(const struct dev_node *node);
int sd_cdrom_dev_open(const char *name, struct device **device, void *arg);
int sd_cdrom_dev_read(struct device *device, void *buf, u32 len, u32 offset);
int sd_cdrom_dev_ioctl(struct device *device, u32 cmd, u32 arg);
int sd_cdrom_dev_close(struct device *device);
//*****************sd_cdrom区

//*****************sd_enc区
int sd_enc_dev_init(const struct dev_node *node, void *arg);
bool sd_enc_dev_online(const struct dev_node *node);
int sd_enc_dev_open(const char *name, struct device **device, void *arg);
int sd_enc_dev_read(struct device *device, void *buf, u32 len, u32 offset);
int sd_enc_dev_write(struct device *device, void *buf, u32 len, u32 offset);
int sd_enc_dev_ioctl(struct device *device, u32 cmd, u32 arg);
int sd_enc_dev_close(struct device *device);
//*****************sd_enc区

#define SDMMC_CMD_PORT     PORTC
#define SDMMC_CMD_PORT_IM  1
#define SDMMC_CMD_IO       IO_PORTC_01


#define SDMMC_CLK_PORT     PORTC
#define SDMMC_CLK_PORT_IM  2
#define SDMMC_CLK_IO       IO_PORTC_02

#define SDMMC_DAT_PORT     PORTC
#define SDMMC_DAT_PORT_IM  0
#define SDMMC_DAT_IO       IO_PORTC_00



#endif


