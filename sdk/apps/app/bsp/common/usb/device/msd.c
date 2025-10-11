/**
 * @file descriptor.c
 * @brief overwrite usb device descriptor
 * @version 1.00
 * @date 2019-05-06
 */
#include "typedef.h"
#include "app_config.h"

#if USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS

#include "usb/device/usb_stack.h"
#include "usb/otg.h"
#include "usb/device/msd.h"
#include "usb/usb_config.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[msd]"
#include "log.h"

#define USB_MSD_BULK_DEV_USE_ASYNC  1

#if defined(CONFIG_CPU_UC03)
#define     CPU_NAME    'U','C','0','3'
#elif defined(CONFIG_CPU_BD49)
#define     CPU_NAME    'B','D','4','9'
#else
#define     CPU_NAME    'J','L','K','J'
#endif

extern void wdt_clear(void);
//               Mass Storage Class
/*********************************************************/
static const u8 sMassDescriptor[] = {  //<Interface & Endpoint Descriptor
///Interface
    USB_DT_INTERFACE_SIZE,   //Length
    USB_DT_INTERFACE,   //DescriptorType:Inerface
    0x00,   //InterfaceNum:0
    0x00,   //AlternateSetting:0
    0x02,   //NumEndpoint:0
    USB_CLASS_MASS_STORAGE,   //InterfaceClass:MSD
    0x06,   //InterfaceSubClass SCSI transparent command set Allocated by USB-IF for SCSI. SCSI standards are defined outside of USB.
    0x50,   //InterfaceProtocol BBB USB Mass Storage Class Bulk-Only (BBB) Transport
    0x00,   //Interface String
///Endpoint IN
    USB_DT_ENDPOINT_SIZE,
    USB_DT_ENDPOINT,
    USB_DIR_IN | MSD_BULK_EP_IN,
    USB_ENDPOINT_XFER_BULK,
    LOBYTE(MAXP_SIZE_BULKIN), HIBYTE(MAXP_SIZE_BULKIN),
    0x01,
///Endpoint OUT
    USB_DT_ENDPOINT_SIZE,
    USB_DT_ENDPOINT,
    MSD_BULK_EP_OUT,
    USB_ENDPOINT_XFER_BULK,
    LOBYTE(MAXP_SIZE_BULKOUT), HIBYTE(MAXP_SIZE_BULKOUT),
    0x01,
};
static const u8 cdrom_top_info[] = {
    0x00, 0x12, // (number of bytes below = 18 decimal)
    0x01,// (first track)
    0x01,// (last track)
    0x00,// (reserved)
    0x17,// (adr, control)
    0x01,// (track being described)
    0x00,// (reserved)
    0x00, 0x00, 0x02, 0x00,// (start logical block address 0X00)
    0x00,// (reserved)
    0x17,// (adr, control)
    0xAA,// (track being described (leadout))
    0x00,// (reserved)
    0x00, 0x00, 0x00, 0x00 // (lead oout start logical block address 0xC6C)
};

static const u8 EX_SCSIInquiryData[] = {
    0x00,//  // Peripheral Device Type: direct access devices  0x05,//
    0x80,   // Removable: UFD is removable
    0x02,   // iso ecma ANSI version
    0x02,   // Response Data Format: compliance with UFI
    0x20,   // Additional Length (Number of UINT8s following this one): 31, totally 36 UINT8s
    0x00, 0x00, 0x00,   // reserved
    CPU_NAME,    //-- Vender information start
    ' ',
    ' ',
    ' ',
    ' ',   //" " -- Vend Information end

    'E',    //-- Production Identification start
    'X',
    'T',
    ' ',
    'F',
    'L',
    'A',
    'S',
    'H',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',    //" " -- Production Identification end

    0x32,   //"1" -- Production Revision Level start
    0x2e,   //"."
    0x30,   //"0"
    0x30    //"0" -- Production Revision Level end
};

static const u8 SCSIInquiryData[] = {
    0x00,//  // Peripheral Device Type: direct access devices  0x05,//
    0x80,   // Removable: UFD is removable
    0x02,   // iso ecma ANSI version
    0x02,   // Response Data Format: compliance with UFI
    0x20,   // Additional Length (Number of UINT8s following this one): 31, totally 36 UINT8s
    0x00, 0x00, 0x00,   // reserved
    CPU_NAME,    //-- Vender information start
    ' ',
    ' ',
    ' ',
    ' ',   //" " -- Vend Information end

    'U',    //-- Production Identification start
    'D',
    'I',
    'S',
    'K',
    ' ',
    'O',
    'T',
    'P',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',    //" " -- Production Identification end

    0x31,   //"1" -- Production Revision Level start
    0x2e,   //"."
    0x30,   //"0"
    0x30    //"0" -- Production Revision Level end
};
static const u8 CD_SCSIInquiryData[] = {
    0x05,//  // Peripheral Device Type: direct access devices  0x05,//
    0x00,   // Removable: UFD is removable
    0x02,   // iso ecma ANSI version
    0x02,   // Response Data Format: compliance with UFI
    0x20,// Additional Length (Number of UINT8s following this one): 31, totally 36 UINT8s
    0x00, 0x00, 0x00,   // reserved
    'U',    //-- Vender information start
    'C',
    '0',
    '3',
    ' ',
    ' ',
    ' ',
    ' ',   //" " -- Vend Information end

    'C',    //-- Production Identification start
    'D',
    'O',
    'M',
    ' ',
    'O',
    'T',
    'P',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',    //" " -- Production Identification end

    0x32,   //"1" -- Production Revision Level start
    0x2e,   //"."
    0x30,   //"0"
    0x30    //"0" -- Production Revision Level end
};

#define MSD_BUFFER_SIZE     (MSD_BLOCK_SIZE * 512)
static u8 msd_buf[MSD_BUFFER_SIZE * 2] SEC(.mass_storage) __attribute__((aligned(64)));

struct msd_vat_t _msd_var AT(.mass_storage);
static struct msd_vat_t *msd_var ;

void msd_init(void)
{
    memset((void *)&_msd_var, 0, sizeof(_msd_var));
    _msd_var.msd_buf = msd_buf;
    _msd_var.msd_buffer_size = MSD_BUFFER_SIZE;
    _msd_var.ep_in_buffer = usb_get_ep_buffer(0, MSD_BULK_EP_IN | USB_DIR_IN);
    _msd_var.ep_out_buffer  = usb_get_ep_buffer(0, MSD_BULK_EP_OUT);

#if SD_CDROM_EN

    _msd_var.max_lun = 2;
    _msd_var.cdrom_enable = 1;
    _msd_var.inquiry[0]  = CD_SCSIInquiryData;
    _msd_var.inquiry[1]  = SCSIInquiryData;

#else

    _msd_var.max_lun = 1;
    _msd_var.inquiry[0]  = SCSIInquiryData;
#if TCFG_USB_EXFLASH_UDISK_ENABLE
    _msd_var.max_lun = 2;
    _msd_var.inquiry[1]  = EX_SCSIInquiryData;
#endif

#endif
}

void msd_register_disk_api(void)
{
#if SD_CDROM_EN

    msd_register_disk("sd_cdrom", NULL);
    msd_register_disk("sd_enc", NULL);

#else

    msd_register_disk("sd0", NULL);
#if TCFG_USB_EXFLASH_UDISK_ENABLE
    msd_register_disk("ext_flsh", NULL);
#endif

#endif
}



inline static u8 __get_max_msd_dev()
{
    if (msd_var) {
        return msd_var->max_lun;
    } else {
        return 0;
    }
}
static u32 msd_cdrom_enable()
{
    if (msd_var) {
        return msd_var->cdrom_enable && (msd_var->cbw.bCBWLUN == 0);
    } else {
        return 0;
    }
}
u32 msd_set_wakeup_handle(void (*handle)(struct usb_device_t *usb_device))
{
    if (msd_var) {
        msd_var->info.msd_wakeup_handle = handle;
        return 1;
    }
    return 0;
}
static void msd_wakeup(struct usb_device_t *usb_device, u32 ep)
{
    msd_var->usb_wakeup = 1;
    /* if ((msd_var) && (msd_var->info.msd_wakeup_handle)) { */
    /*     msd_var->info.msd_wakeup_handle(usb_device);      */
    /* }                                                     */
}
static u8 get_cardreader_popup(u32 lun)
{
    return msd_var->info.bDisk_popup[lun];//弹出
}
static void set_cardreader_popup(u32 lun, u8 status)
{
    if (msd_var->info.dev_handle[lun] != NULL) {
        msd_var->info.bDisk_popup[lun] = status;//弹出
    }
}
static void recover_set_cardreader_popup(u32 lun)
{
    if (msd_var->info.dev_handle[lun] != NULL) {
        msd_var->info.bDisk_popup[lun] = 0;//未弹出
    }
}
static void msd_endpoint_init(struct usb_device_t *usb_device, u32 itf)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    u8 *ep_buffer = msd_var->ep_in_buffer;
    ASSERT(ep_buffer, "usb alloc ep buffer failed");
    usb_g_ep_config(usb_id, MSD_BULK_EP_IN | USB_DIR_IN, USB_ENDPOINT_XFER_BULK, 0, ep_buffer, MAXP_SIZE_BULKIN);
    ep_buffer = msd_var->ep_out_buffer;
    ASSERT(ep_buffer, "usb alloc ep buffer failed");
    usb_g_ep_config(usb_id, MSD_BULK_EP_OUT, USB_ENDPOINT_XFER_BULK, 1, ep_buffer, MAXP_SIZE_BULKOUT);
    usb_g_set_intr_hander(usb_id, MSD_BULK_EP_OUT, msd_wakeup);
    usb_enable_ep(usb_id, MSD_BULK_EP_OUT);
}
static void msd_reset_wakeup(struct usb_device_t *usb_device, u32 itf_num)
{
    msd_var->usb_wakeup = 0xfe;
    const usb_dev usb_id = usb_device2id(usb_device);
    if (msd_var && msd_var->info.msd_reset_wakeup_handle) {
        msd_var->info.msd_reset_wakeup_handle(usb_device, itf_num);
    }
}
void msd_set_reset_wakeup_handle(void (*handle)(struct usb_device_t *usb_device, u32 itf_num))
{
    if (msd_var) {
        msd_var->info.msd_reset_wakeup_handle = handle;
    }
}
void msd_reset(struct usb_device_t *usb_device, u32 itf_num)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    log_info("%s", __func__);

#if USB_ROOT2
    usb_disable_ep(usb_id, MSD_BULK_EP_OUT);
#else
    msd_endpoint_init(usb_device, itf_num);
#endif
}
u32 dev_manager_list_check_by_logo(const char *dev_name)
{
    return dev_online(dev_name);
}
static void *check_disk_status(u8 cur_lun)
{
    u32 ret;
    u32 online_status;
    void *dev_fd = msd_var->info.dev_handle[cur_lun]  ;
    const char *dev_name = msd_var->info.dev_name[cur_lun];
    void *pvfs = 0;

    if (dev_manager_list_check_by_logo((char *)dev_name)) {
        if (dev_fd == NULL) {
            msd_var->info.dev_handle[cur_lun] = dev_open(dev_name, NULL);
            if (msd_var->info.dev_handle[cur_lun]) {
                log_info("%s open sucess %d\n", dev_name, cur_lun);
            }
        } else {
            //FIXME:need add device state check??
        }
        if (get_cardreader_popup(cur_lun)) {
            return NULL;
        }
    } else {
        if (dev_fd) {
            dev_close(dev_fd);
            recover_set_cardreader_popup(cur_lun);
            msd_var->info.dev_handle[cur_lun] = NULL;
        }
    }

    return msd_var->info.dev_handle[cur_lun];
}
static u32 msd_itf_hander(struct usb_device_t *usb_device, struct usb_ctrlrequest *setup)
{
    u32 tx_len;
    u8 *tx_payload = usb_get_setup_buffer(usb_device);
    u32 bRequestType = setup->bRequestType & USB_TYPE_MASK;
    switch (bRequestType) {
    case USB_TYPE_STANDARD:
        switch (setup->bRequest) {
        case USB_REQ_GET_STATUS:
            usb_set_setup_phase(usb_device, USB_EP0_STAGE_SETUP);
            break;
        case USB_REQ_GET_INTERFACE:
            tx_len = 1;
            u8 i = 0;
            usb_set_data_payload(usb_device, setup, &i, tx_len);
            break;
        case USB_REQ_SET_INTERFACE:
            usb_set_setup_phase(usb_device, USB_EP0_STAGE_SETUP);
            break;
        default:
            break;
        }
        break;
    case USB_TYPE_CLASS:
        switch (setup->bRequest) {
        case USB_MSD_MAX_LUN:
            msd_endpoint_init(usb_device, -1);
            tx_len = 1;
            tx_payload[0] = __get_max_msd_dev() - 1;
            usb_set_data_payload(usb_device, setup, tx_payload, tx_len);
            break;
        case USB_MSD_RESET :
            usb_set_setup_phase(usb_device, USB_EP0_STAGE_SETUP);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return 0;
}
u32 msd_desc_config(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num)
{
    log_info("%s() %d", __func__, *cur_itf_num);
    memcpy(ptr, sMassDescriptor, sizeof(sMassDescriptor));
    ptr[2] = *cur_itf_num;
    if (usb_set_interface_hander(usb_id, *cur_itf_num, msd_itf_hander) != *cur_itf_num) {

    }
    if (usb_set_reset_hander(usb_id, *cur_itf_num, msd_reset_wakeup) != *cur_itf_num) {

    }
    *cur_itf_num = *cur_itf_num + 1;
    return sizeof(sMassDescriptor);
}
u32 msd_usb2mcu(const struct usb_device_t *usb_device, u8 *buffer, u32 len)
{
    usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_bulk_read(usb_id, MSD_BULK_EP_OUT, buffer, len, 1);
}
u32 msd_mcu2usb(const struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_bulk_write(usb_id, MSD_BULK_EP_IN, buffer, len);
}
static u32 msd_usb2mcu_64byte_fast(const struct usb_device_t *usb_device, u8 *buffer, u32 len)
{
    usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_bulk_read64byte_fast(usb_id, MSD_BULK_EP_OUT, buffer, len);
}
static u32 get_cbw(const struct usb_device_t *usb_device)
{
    msd_var->cbw.dCBWSignature = 0;
    memset((u8 *)(&(msd_var->cbw)), 0, sizeof(struct usb_scsi_cbw));
    u32 rx_len = msd_usb2mcu(usb_device, (u8 *)(&(msd_var->cbw)), sizeof(struct usb_scsi_cbw));
    if (rx_len == 0) {
        return 1;
    }
    if (msd_var->cbw.dCBWSignature ^ CBW_SIGNATURE) {
        if ((msd_var->cbw.dCBWSignature ^ CBW_SIGNATURE) != CBW_SIGNATURE) {
        }
        return 1;
    }
    msd_var->csw.dCSWTag = msd_var->cbw.dCBWTag;
    msd_var->csw.uCSWDataResidue = msd_var->cbw.dCBWDataTransferLength;

    return 0;
}
static void inquiry(const struct usb_device_t *usb_device)
{
    u32 err;
    u32 len = min(msd_var->csw.uCSWDataResidue, sizeof(SCSIInquiryData));
    u8 scsiinquiryData[sizeof(SCSIInquiryData)];

    if (msd_cdrom_enable()) {
        if (msd_var->inquiry[0]) {
            memcpy(scsiinquiryData, msd_var->inquiry[0], sizeof(SCSIInquiryData));
        } else {
            memcpy(scsiinquiryData, CD_SCSIInquiryData, sizeof(CD_SCSIInquiryData));
        }
    } else if (msd_var->inquiry[msd_var->cbw.bCBWLUN]) {
        memcpy(scsiinquiryData, msd_var->inquiry[msd_var->cbw.bCBWLUN], sizeof(SCSIInquiryData));
    } else {
        memcpy(scsiinquiryData, SCSIInquiryData, sizeof(SCSIInquiryData));
    }
    err = msd_mcu2usb(usb_device, scsiinquiryData, len);
    if (err == 0) {
        log_error("write usb err %d", __LINE__);
    }
    msd_var->csw.uCSWDataResidue -= len;
    msd_var->csw.bCSWStatus = 0;
}
static void send_csw(const struct usb_device_t *usb_device)
{
    u32 err;
    msd_var->csw.dCSWSignature = CSW_SIGNATURE;
    err = msd_mcu2usb(usb_device, (u8 *) & (msd_var->csw), sizeof(struct usb_scsi_csw));
    if (err == 0) {
        log_error("write usb err %d", __LINE__);
    }
}
static void stall_inep(const struct usb_device_t *usb_device)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    usb_write_txcsr(usb_id, MSD_BULK_EP_IN, TXCSRP_SendStall);
    u32 ot = 2000;
    while (1) {
        if (ot-- == 0) {
            break;
        }
        if (usb_otg_online(usb_id) == DISCONN_MODE) {
            break;
        }
        if ((usb_read_txcsr(usb_id, MSD_BULK_EP_IN) & TXCSRP_SendStall) == 0) {
            break;
        }
    }
}

static void stall_outep(const struct usb_device_t *usb_device)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    usb_write_rxcsr(usb_id, MSD_BULK_EP_OUT, RXCSRP_SendStall);
    u32 ot = 2000;
    while (1) {
        if (ot-- == 0) {
            break;
        }
        if (usb_otg_online(usb_id) == DISCONN_MODE) {
            break;
        }
        if ((usb_read_rxcsr(usb_id, MSD_BULK_EP_OUT) & RXCSRP_SendStall) == 0) {
            break;
        }
    }
}
static void unknow(const struct usb_device_t *usb_device, u8 rw)
{
    if (rw) {
        stall_outep(usb_device);
    } else {
        stall_inep(usb_device);
    }

    msd_var->csw.uCSWDataResidue = 0;
    msd_var->csw.bCSWStatus = 0x01;
    msd_var->info.bError = 0x1;
    /* msd_var->info.bError = 0x2; */
}
static void stall_error(const struct usb_device_t *usb_device, u8 rw, u8 error)
{
    msd_var->csw.bCSWStatus = 0x01;
    unknow(usb_device, rw);
    msd_var->info.bError = error;
}
static void test_unit_ready(const struct usb_device_t *usb_device)
{
    int err;
    u8 cur_lun = msd_var->cbw.bCBWLUN;

    if (NULL == check_disk_status(cur_lun)) {
        msd_var->csw.bCSWStatus = 0x1;
        msd_var->info.bError = 0x0f;
    } else {
        msd_var->csw.uCSWDataResidue = 0;
        msd_var->csw.bCSWStatus = 0x0;
    }
}
static void allow_medium_removal(const struct usb_device_t *usb_device)
{
    msd_var->csw.bCSWStatus = 0x0;
    if (msd_var->cbw.lba[2]) {
        msd_var->info.bError = 0x02;
        msd_var->csw.bCSWStatus = 0x01;
    }
    msd_var->csw.uCSWDataResidue = 0;
}
static void request_sense(const struct usb_device_t *usb_device)
{
    u32 err;
    u32 len;
    u8 sense_data[0x12];
    len = min(msd_var->csw.uCSWDataResidue, 0x12);
    if (msd_var->csw.uCSWDataResidue == 0) {
        stall_inep(usb_device);
        msd_var->info.bError = NOT_READY;
        msd_var->csw.uCSWDataResidue = 0;
        msd_var->csw.bCSWStatus = 0x01;
        return;
    } else {
        memset(sense_data, 0x0, sizeof(sense_data));
        sense_data[2] = msd_var->info.bSenseKey;
        sense_data[12] = msd_var->info.bAdditionalSenseCode;
        sense_data[13] = msd_var->info.bAddiSenseCodeQualifier;
        sense_data[0] = 0x70;
        sense_data[7] = 0x0a;
        err = msd_mcu2usb(usb_device, sense_data, len);
        if (err == 0) {
            log_error("write usb err %d", __LINE__);
        }
    }
    msd_var->csw.uCSWDataResidue -= len;
    msd_var->csw.bCSWStatus = 0;
}
static u8 const scsi_mode_sense[4] = {
    0x03, 0x00, 0x00, 0x00
};
static void mode_sense(const struct usb_device_t *usb_device)
{
    u32 err;
    u32 sense;
    err = msd_mcu2usb(usb_device, (u8 *)scsi_mode_sense, sizeof(scsi_mode_sense));
    if (err == 0) {
        log_error("write usb err %d", __LINE__);
    }
    msd_var->csw.uCSWDataResidue = 0;
    msd_var->csw.bCSWStatus = 0;
}
static void verify(const struct usb_device_t *usb_device)
{
    msd_var->csw.uCSWDataResidue = 0;
    msd_var->csw.bCSWStatus = 0;
}
static void msd_read_cdrom_top(const struct usb_device_t *usb_device)
{
    /* printf("%s() %d\n", __func__, len); */
    u32 len = min(msd_var->csw.uCSWDataResidue, sizeof(cdrom_top_info));
    u8 cdrom_data[sizeof(cdrom_top_info)];
    memcpy(cdrom_data, cdrom_top_info, sizeof(cdrom_top_info));
    msd_mcu2usb(usb_device, (u8 *)cdrom_data, len);
    msd_var->csw.uCSWDataResidue -= len;
    msd_var->csw.bCSWStatus = 0;
}
#if USB_MSD_BULK_DEV_USE_ASYNC
static void msd_write_10_async(const struct usb_device_t *usb_device, u8 cur_lun, u32 lba, u16 lba_num)
{
    u32 err = 0;
    u16 num = 0;
    u8  buf_idx = 0;
    u32 have_send_stall = FALSE;
    void *dev_fd = NULL;
    while (lba_num) {
        wdt_clear();
        num = lba_num > MSD_BLOCK_SIZE ? MSD_BLOCK_SIZE : lba_num;
        if (msd_var->csw.uCSWDataResidue == 0) {
            msd_var->csw.bCSWStatus = 1;
            break;
        }
        if (msd_var->csw.uCSWDataResidue >= num * 0x200) {
            msd_var->csw.uCSWDataResidue -= num * 0x200;
        }
        err = msd_usb2mcu_64byte_fast(usb_device, msd_var->msd_buf + buf_idx * msd_var->msd_buffer_size, num * 0x200);
        if (err != num * 0x200) {
            log_error("read usb_err %d, dev = %s",
                      __LINE__, msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 1, MEDIUM_ERROR);
            have_send_stall = TRUE;
            break;
        }
        dev_fd = check_disk_status(cur_lun);
        if (dev_fd) {
            dev_ioctl(dev_fd, IOCTL_SET_ASYNC_MODE, 0);
            err = dev_bulk_write(dev_fd, msd_var->msd_buf + buf_idx * msd_var->msd_buffer_size, lba, num);
            if (err != num) {
                log_error("write_10 write fail, %d, dev = %s",
                          err, msd_var->info.dev_name[cur_lun]);
                stall_error(usb_device, 1, MEDIUM_ERROR);
                have_send_stall = TRUE;
                break;
            }
        } else {
            log_error("write_10 disk offline, dev = %s",
                      msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 1, MEDIUM_ERROR);
            have_send_stall = TRUE;
            break;
        }
        buf_idx = !buf_idx;
        lba_num -= num;
        lba += num;
    }
    dev_fd = check_disk_status(cur_lun);
    if (dev_fd) {
        //async mode last block flush
        dev_ioctl(dev_fd, IOCTL_FLUSH, 0);
    } else {
        if (have_send_stall == FALSE) {
            log_error("write_10 disk offline while sync, dev = %s",
                      msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 1, MEDIUM_ERROR);
        }
    }
}

static void msd_read_10_async(const struct usb_device_t *usb_device, u8 cur_lun, u32 lba, u16 lba_num)
{
    u32 err = 0;
    u16 num = 0;
    u8 buf_idx = 0;
    u32 last_lba = 0, last_num = 0;
    void *dev_fd = NULL;

    if (lba_num == 0) {
        log_error("lba_num == 0\n");
        stall_error(usb_device, 0, 0x02);
        return;
    }
    num = lba_num > MSD_BLOCK_SIZE ? MSD_BLOCK_SIZE : lba_num;
    dev_fd = check_disk_status(cur_lun);
    if (dev_fd) {
        dev_ioctl(dev_fd, IOCTL_SET_ASYNC_MODE, 0);
        err = dev_bulk_read(dev_fd, msd_var->msd_buf + buf_idx * msd_var->msd_buffer_size, lba, num);
        if (err != num) {
            log_error("read disk error0 = %d, dev = %s\n", err, msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 0, MEDIUM_ERROR);
            return;
        }
    } else {
        log_error("read_10 disk offline, dev = %s\n", msd_var->info.dev_name[cur_lun]);
        stall_error(usb_device, 0, MEDIUM_ERROR);
        return;
    }

    while (lba_num) {
        wdt_clear();
        last_num = num;
        last_lba = lba;
        buf_idx = !buf_idx;
        lba += num;
        lba_num -= num;
        num = lba_num > MSD_BLOCK_SIZE ? MSD_BLOCK_SIZE : lba_num;
        if (msd_var->csw.uCSWDataResidue == 0) {
            msd_var->csw.bCSWStatus = 1;
            break;
        }
        if (msd_var->csw.uCSWDataResidue >= last_num * 0x200) {
            msd_var->csw.uCSWDataResidue -= last_num * 0x200;
        }
        dev_fd = check_disk_status(cur_lun);
        if (dev_fd) {
            if (num) {
                dev_ioctl(dev_fd, IOCTL_SET_ASYNC_MODE, 0);
                err = dev_bulk_read(dev_fd, msd_var->msd_buf + buf_idx * msd_var->msd_buffer_size, lba, num);
                if (err != num) {
                    log_error("read disk error1 = %d, dev = %s",
                              err, msd_var->info.dev_name[cur_lun]);
                    stall_error(usb_device, 0, MEDIUM_ERROR);
                    break;
                }
            } else {
                //async mode last block flush
                dev_ioctl(dev_fd, IOCTL_FLUSH, 0);
            }
        } else {
            log_error("read_10 disk offline, dev = %s",
                      msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 0, MEDIUM_ERROR);
            break;
        }
        err = msd_mcu2usb(usb_device, msd_var->msd_buf + !buf_idx * msd_var->msd_buffer_size, last_num * 0x200);
        if (err != last_num * 0x200) {
            log_error("read_10 data transfer err %d, dev = %s",
                      __LINE__, msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 0, 0x05);
            if (num) {
                dev_ioctl(dev_fd, IOCTL_FLUSH, 0);
            }
            break;
        }
    }
}
#endif
static u32 read_32(u8 *p)
{
    return ((u32)(p[0]) << 24) | ((u32)(p[1]) << 16) | ((u32)(p[2]) << 8) | (p[3]);
}
static void write_10(const struct usb_device_t *usb_device)
{
    u32 err = 0 ;
    u32 lba;
    u16 lba_num;
    u16 num = 0;
    lba = read_32(msd_var->cbw.lba);
    lba_num = ((u16)(msd_var->cbw.LengthH) << 8) | (msd_var->cbw.LengthL);
    u8 cur_lun = msd_var->cbw.bCBWLUN;
    void *dev_fd = msd_var->info.dev_handle[cur_lun];

#if USB_MSD_BULK_DEV_USE_ASYNC
    msd_write_10_async(usb_device, cur_lun, lba, lba_num);
#else

    while (lba_num) {
        wdt_clear();
        num = lba_num > MSD_BLOCK_SIZE ? MSD_BLOCK_SIZE : lba_num;
        if (msd_var->csw.uCSWDataResidue == 0) {
            msd_var->csw.bCSWStatus = 0x1;
            break;
        }
        if (msd_var->csw.uCSWDataResidue >= (num * 0x200)) {
            msd_var->csw.uCSWDataResidue -= (num * 0x200);
        }
        err = msd_usb2mcu_64byte_fast(usb_device, msd_var->msd_buf, num * 0x200);
        if (err == 0) {
            stall_error(usb_device, 1, MEDIUM_ERROR);
            log_error("read usb, dev = %s", err, msd_var->info.dev_name[cur_lun]);
            break;
        }
        dev_fd = check_disk_status(cur_lun);
        if (dev_fd) {
            dev_ioctl(dev_fd, IOCTL_CMD_RESUME, 0);
            err = dev_bulk_write(dev_fd, msd_var->msd_buf, lba, num);
            dev_ioctl(dev_fd, IOCTL_CMD_SUSPEND, 0);
            if (err != num) {
                stall_error(usb_device, 1, MEDIUM_ERROR);
                log_error("write disk error =%d, dev = %s", err, msd_var->info.dev_name[cur_lun]);
                break;
            } else {
                break;
            }
            lba += num;
            lba_num -= num;
        } else {
            log_error("write disk offline, dev = %s", msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 1, MEDIUM_ERROR);
            break;
        }
    }
#endif
}
static void read_10(const struct usb_device_t *usb_device)
{
    u32 err = 0;
    u32 lba;
    u16 lba_num;
    u16 num = 0;
    lba = read_32(msd_var->cbw.lba);
    lba_num = ((u16)(msd_var->cbw.LengthH) << 8) | (msd_var->cbw.LengthL);
    u8 cur_lun = msd_var->cbw.bCBWLUN;
    void *dev_fd = msd_var->info.dev_handle[cur_lun];

#if USB_MSD_BULK_DEV_USE_ASYNC
    msd_read_10_async(usb_device, cur_lun, lba, lba_num);
#else

    while (lba_num) {
        wdt_clear();
        num = lba_num > MSD_BLOCK_SIZE ? MSD_BLOCK_SIZE : lba_num;
        if (msd_var->csw.uCSWDataResidue == 0) {
            msd_var->csw.bCSWStatus = 0x1;
            break;
        }
        if (msd_var->csw.uCSWDataResidue >= (num * 0x200)) {
            msd_var->csw.uCSWDataResidue -= (num * 0x200);
        }
        dev_fd = check_disk_status(cur_lun);
        if (dev_fd) {
            dev_ioctl(dev_fd, IOCTL_CMD_RESUME, 0);
            err = dev_bulk_read(dev_fd, msd_var->msd_buf, lba, num);
            dev_ioctl(dev_fd, IOCTL_CMD_SUSPEND, 0);
            if (err != num) {
                log_error("read disk error =%d, dev = %s", err, msd_var->info.dev_name[cur_lun]);
                stall_error(usb_device, 0, MEDIUM_ERROR);
                break;
            }
        } else {
            log_error("read disk offline, dev = %s", err, msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 0, MEDIUM_ERROR);
            break;
        }
        err = msd_mcu2usb(usb_device, msd_var->msd_buf, num * 0x200);
        if (err == 0) {
            log_error("write usb err %d, dev = %s", __LINE__, msd_var->info.dev_name[cur_lun]);
            stall_error(usb_device, 0, MEDIUM_ERROR);
            break;
        }
        lba += num;
        lba_num -= num;
    }
#endif
}
static void read_capacity(const struct usb_device_t *usb_device)
{
    u32 err;
    u32 capacity_temp;
    u8 capacity[8] = {0};
    u8 cur_lun = msd_var->cbw.bCBWLUN;
    void *dev_fd = check_disk_status(cur_lun);
    if (!dev_fd) {
        stall_error(usb_device, 0, 0x0f);
        return;
    }
    dev_ioctl(dev_fd, IOCTL_GET_CAPACITY, (u32)&capacity_temp);
    /* r_printf(">>>[test]:capacity_temp1 = %d\n", capacity_temp); */
    capacity_temp = cpu_to_be32(capacity_temp - 1);
    memcpy(capacity, &capacity_temp, 4);
    dev_ioctl(dev_fd, IOCTL_GET_BLOCK_SIZE, (u32)&capacity_temp);
    /* r_printf(">>>[test]:capacity_temp2 = %d\n", capacity_temp); */
    capacity_temp = cpu_to_be32(capacity_temp);
    memcpy(&capacity[4], &capacity_temp, 4);
    err = msd_mcu2usb(usb_device, capacity, 8);
    if (err == 0) {
        log_error("write usb err %d", __LINE__);
    }
    msd_var->csw.uCSWDataResidue = 0;
    msd_var->csw.bCSWStatus = 0;

}

static void read_format_capacity(const struct usb_device_t *usb_device)
{
    u32 err;
    u8 capacity[12] = {0};
    u32 capacity_temp;
    u8 cur_lun = msd_var->cbw.bCBWLUN;
    void *dev_fd = check_disk_status(cur_lun);
    if (!dev_fd) {
        stall_error(usb_device, 0, 0x0f);
        return;
    }

    dev_ioctl(dev_fd, IOCTL_GET_CAPACITY, (u32)&capacity_temp);

    capacity_temp = cpu_to_be32(capacity_temp);
    memcpy(&capacity[4], &capacity_temp, 4);
    dev_ioctl(dev_fd, IOCTL_GET_BLOCK_SIZE, (u32)&capacity_temp);
    capacity_temp = cpu_to_be32(capacity_temp);
    memcpy(&capacity[8], &capacity_temp, 4);
    capacity[3] = 0x08;
    capacity[8] = 0x02;
    err = msd_mcu2usb(usb_device, capacity, 12);
    if (err == 0) {
        log_error("write usb err %d", __LINE__);
    }
    msd_var->csw.uCSWDataResidue -= 12;
    msd_var->csw.bCSWStatus = 0;
}

static void stop_start()
{
    int i;
    u8 loej, start;
    /* set_cardreader_popup(0, !(msd_var->cbw.lba[2])); */
    loej = msd_var->cbw.lba[2] & BIT(1);
    start = msd_var->cbw.lba[2] & BIT(0);
    for (i = 0; i < __get_max_msd_dev(); i++) {
        if (loej && !start) {
            set_cardreader_popup(i, 1);
        } else if (loej && start) {
            set_cardreader_popup(i, 0);
        }
    }

    msd_var->csw.uCSWDataResidue = 0;
    msd_var->csw.bCSWStatus = 0x0;
}


u32 private_scsi_cmd(const struct usb_device_t *usb_device, struct usb_scsi_cbw *cbw);

void USB_MassStorage(const struct usb_device_t *usb_device)
{
    if (!msd_var) {
        return;
    }
    if (!msd_var->usb_wakeup) {
        return;
    }
    if (msd_var->usb_wakeup == 0xfe) {
        msd_reset((struct usb_device_t *)usb_device, -1);
    }
    msd_var->usb_wakeup = 0;
    const usb_dev usb_id = usb_device2id(usb_device);
    usb_clr_intr_rxe(usb_id, MSD_BULK_EP_OUT);
    if (get_cbw(usb_device)) {
        usb_set_intr_rxe(usb_id, MSD_BULK_EP_OUT);
        /* putchar('.'); */
        return;
    }
    /* log_debug("opcode %x", msd_var->cbw.operationCode); */
    if (msd_var->private_scsi_cmd && msd_var->private_scsi_cmd(usb_device, &(msd_var->cbw))) {
        //msd_var->info.bError = 0;
    } else {
        wdt_clear();
        switch (msd_var->cbw.operationCode) {
        case INQUIRY://0x12
            inquiry(usb_device);
            break;
        case TEST_UNIT_READY://0x00
            test_unit_ready(usb_device);
            break;
        case VERIFY://0x2f
            verify(usb_device);
            break;
        case ALLOW_MEDIUM_REMOVAL://0x1e
            allow_medium_removal(usb_device);
            break;
        case REQUEST_SENSE://0x03
            request_sense(usb_device);
            break;
        case MODE_SENSE://0x1a
            mode_sense(usb_device);
            break;
        case READ_10://0x28
            read_10(usb_device);
            break;
        case WRITE_10://0x2a
            write_10(usb_device);
            break;
        case READ_CAPACITY://0x25
            read_capacity(usb_device);
            break;
        case READ_FORMAT_CAPACITIES://0x23
            read_format_capacity(usb_device);
            break;
        case READ_TOC:
            msd_read_cdrom_top(usb_device);
            break;
        case START_STOP://0x1b
            stop_start();
            break;

        default:
            log_error("opcode %x", msd_var->cbw.operationCode);
            stall_error(usb_device, 0, !(msd_var->cbw.bmCBWFlags & 0x80));
            break;
        }
    }
    switch (msd_var->info.bError) {
    case 0x00:
        msd_var->info.bSenseKey = NO_SENSE;
        msd_var->info.bAdditionalSenseCode = ASC_NO_ADDITIONAL_SENSE_INFORMATION;
        msd_var->info.bAddiSenseCodeQualifier = ASCQ_NO_ADDITIONAL_SENSE_INFORMATION;
        break;

    case 0x02:
        msd_var->info.bSenseKey = ILLEGAL_REQUEST;
        msd_var->info.bAdditionalSenseCode = ASC_INVALID_FIELD_IN_COMMAND_PACKET;
        msd_var->info.bAddiSenseCodeQualifier = ASCQ_INVALID_FIELD_IN_COMMAND_PACKET;
        break;
    case MEDIUM_ERROR:
        msd_var->info.bSenseKey = MEDIUM_ERROR;
        msd_var->info.bAdditionalSenseCode = ASC_WRITE_FAULT;
        msd_var->info.bAddiSenseCodeQualifier = ASCQ_WRITE_FAULT;
        break;


    case 0x0f:
    case 0x05:
        msd_var->info.bSenseKey = NOT_READY;
        msd_var->info.bAdditionalSenseCode = ASC_MEDIUM_PRESENT;
        msd_var->info.bAddiSenseCodeQualifier = ASCQ_MEDIUM_PRESENT;
        break;
    case 0x06:
        msd_var->info.bSenseKey = ABORTED_COMMAND;
        msd_var->info.bAdditionalSenseCode = ASC_INVALID_COMMAND_OPERATION_CODE;
        msd_var->info.bAddiSenseCodeQualifier = ASCQ_INVALID_COMMAND_OPERATION_CODE;
        break;
    }
    usb_set_intr_rxe(usb_id, MSD_BULK_EP_OUT);
    send_csw(usb_device);
}

u32 msd_register_disk(const char *name, void *arg)
{
    int i;
    u32 lens, ret = -1;
    log_info("%s %s\n", __func__, name);
    if (msd_var) {
        for (i = 0; i < __get_max_msd_dev(); i++) {
            if (msd_var->info.dev_name[i][0] == 0) {
                ASSERT(strlen(name) <= MSD_DEV_NAME_LEN, "MSD_DEV_NAME_LEN too small");
                strcpy(msd_var->info.dev_name[i], name);
                msd_var->info.dev_handle[i] = NULL;
                break;
            }
        }
        if (i == __get_max_msd_dev()) {
            ret = -3;
        }
    }

    return ret;
}
u32 msd_unregister_disk(const char *name)
{
    u32 err;
    int i;
    if (msd_var) {
        for (i = 0; i < __get_max_msd_dev(); i++) {
            if (!strcmp(msd_var->info.dev_name[i], name)) {
                msd_var->info.dev_name[i][0] = 0;
                return 0;
            }
        }
    }
    return -1;
}

u32 msd_unregister_all()
{
    u32 err;
    int i;
    if (msd_var) {
        for (i = 0; i < __get_max_msd_dev(); i++) {
            if (msd_var->info.dev_handle[i]) {
                err = dev_close(msd_var->info.dev_handle[i]);
                msd_var->info.dev_handle[i] = NULL;
                /* memset(msd_var->info.dev_name[i], 0, MSD_DEV_NAME_LEN); */
            }
        }
    }
    return 0;
}
u32 msd_register(struct msd_vat_t *p)
{
    msd_var = p;
    log_info("msd_var = %x", msd_var);
    return 0;
}
u32 msd_release()
{
    if (msd_var) {
        for (int i = 0; i < __get_max_msd_dev(); i++) {
            void *dev_fd = msd_var->info.dev_handle[i] ;
            if (dev_fd) {
                dev_close(dev_fd);
                msd_var->info.dev_handle[i] = NULL;
            }
        }

        msd_var = NULL;
    }
    return 0;
}




#endif




