#ifndef  __USBD_MSD_H__
#define  __USBD_MSD_H__

#include "usb/usb.h"
#include "usb_stack.h"
#include "usb/scsi.h"


#define MSD_BLOCK_SIZE       1
#define MAX_MSD_DEV                 2
#define MSD_DEV_NAME_LEN            12

struct msd_info {
    u8 bError;
    u8 bSenseKey;
    u8 bAdditionalSenseCode;
    u8 bAddiSenseCodeQualifier;
    u8 bDisk_popup[MAX_MSD_DEV];
    void *dev_handle[MAX_MSD_DEV];
    char dev_name[MAX_MSD_DEV][MSD_DEV_NAME_LEN];
    void (*msd_wakeup_handle)(struct usb_device_t *usb_device);
    void (*msd_reset_wakeup_handle)(struct usb_device_t *usb_device, u32 itf_num);
};

struct msd_vat_t {
    u32(*private_scsi_cmd)(const struct usb_device_t *usb_device, struct usb_scsi_cbw *cbw);
    void *ep_out_buffer;
    void *ep_in_buffer;
    const u8 *inquiry[2];
    u8 *msd_buf;
    u8 usb_wakeup;
    u8 cdrom_enable;
    u8 max_lun;
    u16 msd_buffer_size;
    struct usb_scsi_cbw cbw;
    struct usb_scsi_csw csw;
    struct msd_info info;
};

extern struct msd_vat_t msd_var;
void msd_init(void);

u32 msd_desc_config(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num);
void USB_MassStorage(const struct usb_device_t *usb_device);
u32 msd_set_wakeup_handle(void (*handle)(struct usb_device_t *usb_device));
void msd_register_disk_api(void);
u32 msd_register_disk(const char *name, void *arg);
u32 msd_unregister_disk(const char *name);
u32 msd_unregister_all();
u32 msd_register(struct msd_vat_t *p);
u32 msd_release();
void msd_set_reset_wakeup_handle(void (*handle)(struct usb_device_t *usb_device, u32 itf_num));
void msd_reset(struct usb_device_t *usb_device, u32 itf_num);
#endif  /*USBD_MSD_H*/
