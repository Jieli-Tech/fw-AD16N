/**
 * @file descriptor.c
 * @brief overwrite usb device descriptor
 * @version 1.00
 * @date 2019-05-06
 */

#include "usb/device/usb_stack.h"
#include "usb/device/descriptor.h"
#include "usb/device/uac_audio.h"

#include "app_config.h"

#if 1//TCFG_PC_ENABLE

static const u8 sDeviceDescriptor[] = { //<Device Descriptor
    USB_DT_DEVICE_SIZE,      // bLength: Size of descriptor
    USB_DT_DEVICE,       // bDescriptorType: Device
#if defined(FUSB_MODE) && FUSB_MODE
    0x10, 0x01,     // bcdUSB: USB 1.1
#elif defined(FUSB_MODE) && (FUSB_MODE ==0 )
    0x00, 0x02,     // bcdUSB: USB 2.0
#else
#error "USB_SPEED_MODE not defined"
#endif
    0x00,       // bDeviceClass: none
    0x00,       // bDeviceSubClass: none
    0x00,       // bDeviceProtocol: none
    EP0_SETUP_LEN,//EP0_LEN,      // bMaxPacketSize0: 8/64 bytes
    'J', 'L',     // idVendor: 0x4a4c - JL
    '5', '4',     // idProduct: chip id
    0x00, 0x01,     // bcdDevice: version 1.0
    0x01,       // iManufacturer: Index to string descriptor that contains the string <Your Name> in Unicode
    0x02,       // iProduct: Index to string descriptor that contains the string <Your Product Name> in Unicode
#if TCFG_USB_APPLE_DOCK_EN
    0x00,       // iSerialNumber: none
#else
    0x03,
#endif
    0x01        // bNumConfigurations: 1
};

static const u8 LANGUAGE_STR[] = {
    0x04, 0x03, 0x09, 0x04
};


static const u8 product_string[] = {
    24,
    0x03,
    'U', 0x00,
    'A', 0x00,
    'C', 0x00,
    'D', 0x00,
    'e', 0x00,
    'm', 0x00,
    'o', 0x00,
    'V', 0x00,
    '1', 0x00,
    '.', 0x00,
    '0', 0x00,
};
static const u8 MANUFACTURE_STR[] = {
    34,         //该描述符的长度为34字节
    0x03,       //字符串描述符的类型编码为0x03
    0x4a, 0x00, //J
    0x69, 0x00, //i
    0x65, 0x00, //e
    0x6c, 0x00, //l
    0x69, 0x00, //i
    0x20, 0x00, //
    0x54, 0x00, //T
    0x65, 0x00, //e
    0x63, 0x00, //c
    0x68, 0x00, //h
    0x6e, 0x00, //n
    0x6f, 0x00, //o
    0x6c, 0x00, //l
    0x6f, 0x00, //o
    0x67, 0x00, //g
    0x79, 0x00, //y
};
static const u8 sConfigDescriptor[] = {	//<Config Descriptor
//ConfiguraTIon
    USB_DT_CONFIG_SIZE,    //bLength
    USB_DT_CONFIG,    //DescriptorType : ConfigDescriptor
    0, 0, //TotalLength
    0,//bNumInterfaces: 在set_descriptor函数里面计算
    0x01,    //bConfigurationValue - ID of this configuration
    0x00,    //Unused
#if USB_ROOT2
    0xA0,    //Attributes:Bus Power remotewakeup
#else
    0x80,    //Attributes:Bus Power
#endif
#if TCFG_USB_APPLE_DOCK_EN
    50,     //MaxPower = 50*2ma
#else
    200,    //MaxPower = 200*2ma
#endif
};
static const u8 serial_string[] = {
    0x22, 0x03, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x36, 0x00, 0x46, 0x00, 0x36, 0x00,
    0x34, 0x00, 0x30, 0x00, 0x39, 0x00, 0x36, 0x00, 0x42, 0x00, 0x32, 0x00, 0x32, 0x00, 0x45, 0x00,
    0x37, 0x00
};

const u8 *usb_get_string_desc(u32 id)
{
    const u8 *pstr = uac_get_string(id);
    if (pstr != NULL) {
        return pstr;
    }
    return NULL;
}

static struct usb_device_descriptor_t desc_t = {
    .device_descriptor = sDeviceDescriptor,
    .language_str = LANGUAGE_STR,
    .manufacture_str = MANUFACTURE_STR,
    .product_str = product_string,
    .iserialnumber_str = NULL,
    /* .iserialnumber_str = serial_string, */
    .config_desc = sConfigDescriptor,
};
const struct usb_device_descriptor_t *usb_get_desc_config()
{
    return &desc_t;
}
#endif
