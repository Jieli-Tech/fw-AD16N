#pragma bss_seg(".usb_slave.hid.data.bss")
#pragma data_seg(".usb_slave.hid.data")
#pragma const_seg(".usb_slave.hid.text.const")
#pragma code_seg(".usb_slave.hid.text")
#pragma str_literal_override(".usb_slave.hid.text.const")

#include "usb/device/usb_stack.h"
#include "usb/device/hid.h"
#include "usb_config.h"
#include "usb.h"
#include "app_config.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[HID]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE

#include "log.h"

#if USB_DEVICE_CLASS_CONFIG & HID_CLASS

static const u8 sHIDDescriptor[] = {
//HID
    //InterfaceDeszcriptor:
    USB_DT_INTERFACE_SIZE,     // Length
    USB_DT_INTERFACE,          // DescriptorType
    /* 0x04,                      // bInterface number */
    0x00,                       // bInterface number
    0x00,                      // AlternateSetting
    0x01,                      // NumEndpoint
    USB_CLASS_HID,             // Class = Human Interface Device
    0x00,                      // Subclass, 0 No subclass, 1 Boot Interface subclass
    0x00,                      // Procotol, 0 None, 1 Keyboard, 2 Mouse
    0x00,                      // Interface Name


    //HIDDescriptor:
    0x09,                      // bLength
    USB_HID_DT_HID,            // bDescriptorType, HID Descriptor
    0x01, 0x02,                // bcdHID, HID Class Specification release NO.
    0x00,                      // bCuntryCode, Country localization (=none)
    0x01,                       // bNumDescriptors, Number of descriptors to follow
    0x22,                       // bDescriptorType, Report Desc. 0x22, Physical Desc. 0x23
    0,//LOW(ReportLength)
    0, //HIGH(ReportLength)

    //EndpointDescriptor:
    USB_DT_ENDPOINT_SIZE,       // bLength
    USB_DT_ENDPOINT,            // bDescriptorType, Type
    USB_DIR_IN | HID_EP_IN,     // bEndpointAddress
    USB_ENDPOINT_XFER_INT,      // Interrupt
    LOBYTE(MAXP_SIZE_HIDIN), HIBYTE(MAXP_SIZE_HIDIN),// Maximum packet size
    0x01,     // Poll every 10msec seconds
};

#define HIDKEY_REPORT_ID               0x1
static const u8 sHIDReportDesc[] = {
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, HIDKEY_REPORT_ID,  //   Report ID (1)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x09, 0xE9,        //   Usage (Volume Increment)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xEA,        //   Usage (Volume Decrement)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xCD,        //   Usage (Play/Pause)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB5,        //   Usage (Scan Next Track)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB6,        //   Usage (Scan Previous Track)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB7,        //   Usage (Stop)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB3,        //   Usage (Fast Forward)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB4,        //   Usage (Rewind)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xE2,        //   Usage (Mute)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x0B,        //   Usage Page (Telephony)
    0x09, 0x24,        //   Usage (Redial)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x20,        //   Usage (Hook Switch)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x2F,        //   Usage (Phone Mute)
    0x81, 0x06,        //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x04,        //   Report Count (4)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection

    // 73 bytes

    // best guess: USB HID Report Descriptor
};

__attribute__((always_inline))
u8 usb_get_hidkey_report_id(void)
{
    return HIDKEY_REPORT_ID;
}

struct hid_device_var_t _hid_var AT(.hid_config_var);
struct hid_device_var_t *hid_var ;

void hid_init(void)
{
    memset((void *)&_hid_var, 0, sizeof(_hid_var));
    _hid_var.ep_in_buffer = usb_get_ep_buffer(0, HID_EP_IN | USB_DIR_IN);
#if HID_EP_OUT_EN
    _hid_var.ep_out_buffer = usb_get_ep_buffer(0, HID_EP_OUT | USB_DIR_OUT);
#endif
}

//add dongle set
static u8 *hid_report_map = (u8 *)sHIDReportDesc;
static int hid_report_map_size = sizeof(sHIDReportDesc);
void usb_hid_set_repport_map(const u8 *map, int size)
{
    hid_report_map_size = size;
    hid_report_map = (u8 *)map;
}

static u32 get_hid_report_desc_len(u32 index)
{
    u32 len = 0;
    len = hid_report_map_size;
    return len;
}
static const void *get_hid_report_desc(u32 index)
{
    u8 *ptr  = NULL;
    ptr = hid_report_map;
    return ptr;
}

static u32 hid_tx_data(struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_intr_write(usb_id, HID_EP_IN, buffer, len);
}
#if HID_EP_OUT_EN
static void hid_rx_data(struct usb_device_t *usb_device, u32 ep)
{
    if (hid_var->rx_data_wakeup) {
        hid_var->rx_data_wakeup(usb_device, ep);
    }
}
#endif

static void hid_endpoint_init(struct usb_device_t *usb_device, u32 itf)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    u8 *ep_buffer = hid_var->ep_in_buffer;
    usb_g_ep_config(usb_id, HID_EP_IN | USB_DIR_IN, USB_ENDPOINT_XFER_INT, 0, ep_buffer, MAXP_SIZE_HIDIN);

#if HID_EP_OUT_EN
    ep_buffer = hid_var->ep_out_buffer;
    if (ep_buffer) {
        usb_g_set_intr_hander(usb_id, HID_EP_OUT, hid_rx_data);
        usb_g_ep_config(usb_id, HID_EP_OUT, USB_ENDPOINT_XFER_INT, 1, ep_buffer, MAXP_SIZE_HIDOUT);
    }
#endif
    usb_enable_ep(usb_id, HID_EP_OUT);
}

static void hid_reset(struct usb_device_t *usb_device, u32 itf)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    log_info("%s\n", __func__);
    hid_endpoint_init(usb_device, itf);
}
static u32 hid_recv_output_report(struct usb_device_t *usb_device, struct usb_ctrlrequest *setup)
{
    if (hid_var->output_report) {
        return hid_var->output_report(usb_device, setup);
    }
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 ret = 0;
    u8 read_ep[8];
    u8 mute;
    u16 volume = 0;
    usb_read_ep0(usb_id, read_ep, MIN(sizeof(read_ep), setup->wLength));
    ret = USB_EP0_STAGE_SETUP;
    log_info_hexdump(read_ep, setup->wLength);

    return ret;
}

static u32 hid_itf_hander(struct usb_device_t *usb_device, struct usb_ctrlrequest *req)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 tx_len;
    u8 *tx_payload = usb_get_setup_buffer(usb_device);
    u32 bRequestType = req->bRequestType & USB_TYPE_MASK;
    switch (bRequestType) {
    case USB_TYPE_STANDARD:
        switch (req->bRequest) {
        case USB_REQ_GET_DESCRIPTOR:
            switch (HIBYTE(req->wValue)) {
            case USB_HID_DT_HID:
                tx_payload = (u8 *)sHIDDescriptor + USB_DT_INTERFACE_SIZE;
                tx_len = 9;
                tx_payload = usb_set_data_payload(usb_device, req, tx_payload, tx_len);
                tx_payload[7] = LOBYTE(get_hid_report_desc_len(req->wIndex));
                tx_payload[8] = HIBYTE(get_hid_report_desc_len(req->wIndex));
                break;
            case USB_HID_DT_REPORT:
                hid_endpoint_init(usb_device, req->wIndex);
                tx_len = get_hid_report_desc_len(req->wIndex);
                tx_payload = (void *)get_hid_report_desc(req->wIndex);
                usb_set_data_payload(usb_device, req, tx_payload, tx_len);
                break;
            }// USB_REQ_GET_DESCRIPTOR
            break;
        case USB_REQ_SET_DESCRIPTOR:
            usb_set_setup_phase(usb_device, USB_EP0_STAGE_SETUP);
            break;
        case USB_REQ_SET_INTERFACE:
            if (usb_device->bDeviceStates == USB_DEFAULT) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_ADDRESS) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_CONFIGURED) {
                //只有一个interface 没有Alternate
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            }
            break;
        case USB_REQ_GET_INTERFACE:
            if (req->wLength) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_DEFAULT) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_ADDRESS) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_CONFIGURED) {
                tx_len = 1;
                tx_payload[0] = 0x00;
                usb_set_data_payload(usb_device, req, tx_payload, tx_len);
            }
            break;
        case USB_REQ_GET_STATUS:
            if (usb_device->bDeviceStates == USB_DEFAULT) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else if (usb_device->bDeviceStates == USB_ADDRESS) {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            } else {
                usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
            }
            break;
        }//bRequest @ USB_TYPE_STANDARD
        break;

    case USB_TYPE_CLASS: {
        switch (req->bRequest) {
        case USB_REQ_SET_IDLE:
            usb_set_setup_phase(usb_device, USB_EP0_STAGE_SETUP);
            break;
        case USB_REQ_GET_IDLE:
            tx_len = 1;
            tx_payload[0] = 0;
            usb_set_data_payload(usb_device, req, tx_payload, tx_len);
            break;
        case USB_REQ_SET_REPORT:
            usb_set_setup_recv(usb_device, hid_recv_output_report);
            break;
        }//bRequest @ USB_TYPE_CLASS
    }
    break;
    }
    return 0;
}
void hid_register(const usb_dev usb_id, void *p)
{
    hid_var = p;
}
u32 hid_desc_config(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num)
{
    log_info("hid interface num:%d\n", *cur_itf_num);
    u8 *_ptr = ptr;

    memcpy(ptr, sHIDDescriptor, sizeof(sHIDDescriptor));
    ptr[2] = *cur_itf_num;
    if (usb_set_interface_hander(usb_id, *cur_itf_num, hid_itf_hander) != *cur_itf_num) {
        ASSERT(0, "hid set interface_hander fail");
    }
    if (usb_set_reset_hander(usb_id, *cur_itf_num, hid_reset) != *cur_itf_num) {
        ASSERT(0, "hid set interface_reset_hander fail");
    }

    ptr[USB_DT_INTERFACE_SIZE + 7] = LOBYTE(get_hid_report_desc_len(0));
    ptr[USB_DT_INTERFACE_SIZE + 8] = HIBYTE(get_hid_report_desc_len(0));
    *cur_itf_num = *cur_itf_num + 1;
    return sizeof(sHIDDescriptor) ;
}

void hid_key_handler(u32 hid_key, u32 event_type)
{
    u16 key_buf = hid_key;

    if (event_type == 0) { //click
        hid_tx_data(NULL, (const u8 *)&key_buf, 2);
        os_time_dly(3);
        key_buf = 0;
        hid_tx_data(NULL, (const u8 *)&key_buf, 2);

    } else if (event_type == 1) { //long press
        hid_tx_data(NULL, (const u8 *)&key_buf, 2);
    } else if (event_type == 2) { //press hold

    } else if (event_type == 3) { //long press up
        hid_key = 0;
        hid_tx_data(NULL, (const u8 *)&key_buf, 2);
    }
}

void usb_write_hid_key(u8 *data, u16 len)
{
    hid_tx_data(NULL, (const u8 *)data, len);
}

/* u32 usb_hid_control(u32 value) */
/* { */
/*     u16 key_buf = value; */
/*     hid_tx_data(NULL, (const u8 *)&key_buf, sizeof(key_buf)); */
/*  */
/*     return 0; */
/* } */



#endif
