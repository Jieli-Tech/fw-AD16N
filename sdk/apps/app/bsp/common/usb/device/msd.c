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


#if defined(CONFIG_CPU_UC03)
#define     CPU_NAME    'U','C','0','3'
#endif


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

struct msd_vat_t msd_var AT(.mass_storage);
void msd_init(void)
{
    memset((void *)&msd_var, 0, sizeof(msd_var));
    msd_var.msd_buf = msd_buf;
    msd_var.msd_buffer_size = MSD_BUFFER_SIZE;
#if SD_CDROM_EN
    msd_var.max_lun = 2;
    msd_var.cdrom_enable = 1;
#else
    msd_var.max_lun = 1;
    msd_var.cdrom_enable = 0;
#endif
    msd_var.inquiry[0] = NULL;
    msd_var.inquiry[1] = NULL;

    msd_var.ep_in_buffer = usb_get_ep_buffer(0, MSD_BULK_EP_IN | USB_DIR_IN);
    msd_var.ep_out_buffer  = usb_get_ep_buffer(0, MSD_BULK_EP_OUT);
#if SD_CDROM_EN
    msd_var.inquiry[0]  = CD_SCSIInquiryData;
    msd_var.inquiry[1]  = SCSIInquiryData;
    msd_register_disk("sd_cdrom", NULL);
    msd_register_disk("sd_enc", NULL);
#else
    msd_var.inquiry[0]  = SCSIInquiryData;
    msd_register_disk("sd0", NULL);
#endif

}
#endif




