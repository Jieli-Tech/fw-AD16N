#ifndef __USB_STORAGE_H__
#define __USB_STORAGE_H__

/* #include "system/task.h" */
#include "dev_mg/device.h"
#include "usb/scsi.h"
#include "usb/host/usb_host.h"

/* u盘预读功能配置, 二选一
 * 当两种方式都不使能，则表示不开启预读 */
#define  UDISK_READ_BIGBLOCK_ASYNC_ENABLE    0   //使能大扇区预读方式(不需要额外buf,速度比512预读慢10%)
#define  UDISK_READ_512_ASYNC_ENABLE         0   //使能512Byte预读方式(需要额外的512byte buffer,速度比大扇区预读快10%)
/****************************/

#define UDISK_READ_ASYNC_BLOCK_NUM  (16) //预读扇区数

#define  MULTI_DISK    0
#define  ENABLE_DISK_HOTPLUG   0
//设备状态：
typedef enum usb_sta {
    DEV_IDLE = 0,
    DEV_INIT,
    DEV_OPEN,
    DEV_READ,
    DEV_WRITE,
    DEV_CLOSE,
    DEV_SUSPEND,
    DEV_READY,
} USB_STA ;

struct mass_storage {
    OS_MUTEX mutex;

    struct usb_scsi_cbw cbw;
    struct usb_scsi_csw csw;
    struct request_sense_data sense;

    struct read_capacity_data capacity[2];

    u8 lun;
    u8 curlun;

    u8 host_epout;
    u8 target_epout;

    u8 host_epin;
    u8 target_epin;

    u8 dev_status;
    u8 suspend_cnt;

    u32 remain_len;
    u32 prev_lba;

    u8 read_only;

};

enum usb_async_mode {
    BULK_ASYNC_MODE_EXIT = 0, //取消异步模式
    BULK_ASYNC_MODE_ENTER, //异步512预读
    BULK_ASYNC_MODE_SEM_PEND, //异步预读等待信号量
};

#define MASS_LBA_INIT    (-2)

int usb_msd_parser(struct usb_host_device *host_dev, u8 interface_num, const u8 *pBuf, struct usb_interface_info *inf);


extern bool usb_stor_online(const struct dev_node *node);
extern int usb_stor_open(const char *name, struct device **device, void *arg);
extern int usb_stor_read(struct device *device, void *pBuf, u32 num_lba, u32 lba);
extern int usb_stor_write(struct device *device, void *pBuf,  u32 num_lba, u32 lba);
extern int usb_stor_ioctrl(struct device *device, u32 cmd, u32 arg);
extern int usb_stor_close(struct device *device);
#endif
