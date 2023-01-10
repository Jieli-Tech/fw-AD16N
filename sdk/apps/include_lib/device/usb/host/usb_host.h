#ifndef  __USB_HOST_H__
#define  __USB_HOST_H__
/* #include "system/task.h" */
#include "dev_mg/device.h"
#include "usb/usb.h"
#include "usb/ch9.h"
#include "usb/usb_phy.h"

#define     USB_HOST_ASYNC          0

struct usb_private_data {
    usb_dev usb_id;
    u8 status;
    u8 devnum;
    u8 ep0_max_packet_size;
    /*
    u8 speed;
    u16 vendor_id;
    u16 product_id;
    u16 language;
    u8 manufacturer[64];
    u8 product[64];
    */
};
#define     OS_SEM  void
struct usb_host_device;
struct interface_ctrl {
    u8 interface_class;
    int (*set_power)(struct usb_host_device *host_dev, u32 value);
    int (*get_power)(struct usb_host_device *host_dev, u32 value);
    int (*ioctl)(struct usb_host_device *host_dev, u32 cmd, u32 arg);
};
struct usb_interface_info {
    const struct interface_ctrl *ctrl;
    union {
        struct mass_storage *disk;
        struct adb_device_t *adb;
        struct hid_device_t *hid;
        struct aoa_device_t *aoa;
        void *p;
    } dev;
};
#define   HUSB_MODE   0
#define     MAX_HOST_INTERFACE  1
struct usb_host_device {
    OS_SEM *sem;
    struct usb_private_data private_data;
    const struct usb_interface_info *interface_info[MAX_HOST_INTERFACE];
};


#define     device_to_usbdev(device)	((struct usb_host_device *)((device)->private_data))

u32 host_device2id(const struct usb_host_device *host_dev);

int host_dev_status(const struct usb_host_device *host_dev);

const struct usb_host_device *host_id2device(const usb_dev id);

#define     check_usb_mount(ret)    \
    if(ret == -DEV_ERR_OFFLINE){\
        log_info("%s() @ %d DEV_ERR_OFFLINE\n", __func__, __LINE__);\
        goto __exit_fail;\
    } else if(ret){\
        log_info("%s() @ %d %x\n", __func__, __LINE__, ret);\
        continue;\
    }


typedef void(*usb_h_interrupt)(struct usb_host_device *, u32 ep);

#define     MAX_HOST_EP_RX  2
#define     MAX_HOST_EP_TX  2

struct host_var_t {
    void *msd_h_dma_buffer;
    struct usb_ep_addr_t host_ep_addr ;
    usb_h_interrupt usb_h_interrupt_rx[MAX_HOST_EP_RX] ;
    usb_h_interrupt usb_h_interrupt_tx[MAX_HOST_EP_TX] ;
    struct usb_host_device *dev_at_ep[MAX_HOST_EP_RX];
};

int usb_sem_init(struct usb_host_device *host_dev);
int usb_sem_pend(struct usb_host_device *host_dev, u32 timeout);
int usb_sem_post(struct usb_host_device *host_dev);
int usb_sem_del(struct usb_host_device *host_dev);

void usb_h_set_ep_isr(struct usb_host_device *host_dev, u32 ep, usb_h_interrupt hander, void *p);
u32 usb_h_set_intr_hander(const usb_dev usb_id, u32 ep, usb_h_interrupt hander);
u32 usb_host_mount(const usb_dev usb_id, u32 retry, u32 reset_delay, u32 mount_timeout);
u32 usb_host_unmount(const usb_dev usb_id);
u32 usb_host_remount(const usb_dev usb_id, u32 retry, u32 delay, u32 ot, u8 notify);
void usb_host_suspend(const usb_dev usb_id);
void usb_host_resume(const usb_dev usb_id);
void usb_host_config(usb_dev usb_id, void *__host_var);
void usb_host_free(usb_dev usb_id);
void *usb_h_get_ep_buffer(const usb_dev usb_id, u32 ep);
void usb_h_isr_reg(const usb_dev usb_id, u8 priority, u8 cpu_id);

#endif  /*USB_HOST_H*/
