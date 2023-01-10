#include "app_config.h"
#include "includes.h"
#include "jiffies.h"
#include "tick_timer_driver.h"
#include "device_drive.h"
#include "usb_config.h"
#include "usb/usb_phy.h"
#include "usb/usb.h"
#include "usb/host/usb_host.h"
#include "usb/host/usb_ctrl_transfer.h"
#include "usb/host/usb_storage.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[mount]"
#include "log.h"

static struct usb_host_device host_devices[USB_MAX_HW_NUM];// SEC(.usb_h_bss);

static u8 msd_h_dma_buffer[2][64 + 4]  __attribute__((aligned(4))) SEC(.usb_h_dma);
static struct host_var_t host_var SEC(.usb_h_dma);


#define     device_to_usbdev(device)	((struct usb_host_device *)((device)->private_data))

//////////////////////////////////////////usb stor var

static struct mass_storage mass_stor ;//SEC(.usb_h_udisk);
static struct usb_interface_info udisk_inf = {
    .ctrl = NULL,
    .dev.disk = &mass_stor,
};
//////////////////////////////////////////usb stor var end

int host_dev_status(const struct usb_host_device *host_dev)
{
    return ((host_dev)->private_data.status);
}

u32 host_device2id(const struct usb_host_device *host_dev)
{
#if USB_MAX_HW_NUM > 1
    return ((host_dev)->private_data.usb_id);
#else
    return 0;
#endif
}
u32 usb_get_jiffies()
{
    return maskrom_get_jiffies();
}
void usb_mdelay(unsigned int ms)
{
    unsigned int t;

    t = ms / 10 + ((ms % 10) > 5 ? 1 : 0);

    os_time_dly(t == 0 ? 1 : t);
}

const struct usb_host_device *host_id2device(const usb_dev id)
{
#if USB_MAX_HW_NUM > 1
    return &host_devices[id];
#else
    return &host_devices[0];
#endif
}
static volatile int usb_host_sem;
int usb_sem_init(struct usb_host_device  *host_dev)
{
    usb_dev usb_id = host_device2id(host_dev);

#if 0

    OS_SEM *sem = malloc(sizeof(OS_SEM));
    ASSERT(sem, "usb alloc sem error");
    host_dev->sem = sem;
    g_log_info("%s %x %x ", __func__, host_dev, sem);
    os_sem_create(host_dev->sem, 0);
#endif
    usb_host_sem = 0;
    return 0;
}
int usb_sem_pend(struct usb_host_device  *host_dev, u32 timeout)
{
#if 0
    if (host_dev->sem == NULL) {
        return 1;
    }
    int ret = os_sem_pend(host_dev->sem, timeout);
    if (ret) {
        r_log_info("%s %d ", __func__, ret);
    }
    return ret;
#else
    timeout =  usb_get_jiffies() + timeout;
    while (usb_host_sem == 0) {
        if (time_after(usb_get_jiffies(), timeout)) {
            return DEV_ERR_TIMEOUT;
        }
    }
    usb_host_sem = 0;
    return 0;
#endif
}
int usb_sem_post(struct usb_host_device  *host_dev)
{
#if 0
    if (host_dev->sem == NULL) {
        return 1;
    }
    int ret = os_sem_post(host_dev->sem);
    if (ret) {
        r_log_info("%s %d ", __func__, ret);
    }
    return 0;
#else
    usb_host_sem = 1;
    return 0;
#endif
}
int usb_sem_del(struct usb_host_device *host_dev)
{
#if 0
    usb_dev usb_id = host_device2id(host_dev);

    r_log_info("1");
    if (host_dev->sem == NULL) {
        return 0;
    }
    r_log_info("2");
    r_log_info("3");
    if (host_dev && host_dev->sem) {
        os_sem_del(host_dev->sem, 0);
    }
    r_log_info("4");
    g_log_info("%s %x %x ", __func__, host_dev, host_dev->sem);
    free(host_dev->sem);
    r_log_info("5");
    host_dev->sem = NULL;
    r_log_info("6");
    usb_host_free(usb_id);
    r_log_info("7");
    return 0;
#else
    return 0;
#endif
}

/**
 * @brief usb_descriptor_parser
 *
 * @param device
 * @param pBuf
 * @param total_len
 *
 * @return
 */
int _usb_msd_parser(struct usb_host_device *host_dev, u8 interface_num, const u8 *pBuf)
{
    log_info("find udisk @ interface %d", interface_num);
#if TCFG_UDISK_ENABLE
    return   usb_msd_parser(host_dev, interface_num, pBuf, &udisk_inf);
#else
    return USB_DT_INTERFACE_SIZE;
#endif
}
int usb_descriptor_parser(struct usb_host_device *host_dev, const u8 *pBuf, u32 total_len, struct usb_device_descriptor *device_desc)
{
    int len = 0;
    u8 interface_num = 0;
    struct usb_private_data *private_data = &host_dev->private_data;

    struct usb_config_descriptor *cfg_desc = (struct usb_config_descriptor *)pBuf;

    if (cfg_desc->bDescriptorType != USB_DT_CONFIG ||
        cfg_desc->bLength < USB_DT_CONFIG_SIZE) {
        log_error("invalid descriptor for config bDescriptorType = %d bLength= %d",
                  cfg_desc->bDescriptorType, cfg_desc->bLength);
        return -USB_DT_CONFIG;
    }

    log_info("idVendor %x idProduct %x", device_desc->idVendor, device_desc->idProduct);

    len += USB_DT_CONFIG_SIZE;
    pBuf += USB_DT_CONFIG_SIZE;
    int i = 0;
    u32 have_find_valid_class = 0;
    while (len < total_len) {
        if (interface_num > 4) {
            log_error("interface_num too much");
            break;
        }

        struct usb_interface_descriptor *interface = (struct usb_interface_descriptor *)pBuf;
        if (interface->bDescriptorType == USB_DT_INTERFACE) {

            log_info("inf class %x subclass %x ep %d",
                     interface->bInterfaceClass, interface->bInterfaceSubClass, interface->bNumEndpoints);

            if (interface->bInterfaceClass == USB_CLASS_MASS_STORAGE) {
                i = _usb_msd_parser(host_dev, interface_num, pBuf);
                if (i < 0) {
                    log_error("---%s %d---", __func__, __LINE__);
                    len = total_len;
                } else {
                    interface_num++;
                    len += i;
                    pBuf += i;
                    have_find_valid_class = true;
                }
            } else {
                log_info("find unsupport [class %x subClass %x] @ interface %d",
                         interface->bInterfaceClass,
                         interface->bInterfaceSubClass,
                         interface_num);

                len += USB_DT_INTERFACE_SIZE;
                pBuf += USB_DT_INTERFACE_SIZE;
            }
        } else {
            /* log_error("unknown section %d %d", len, pBuf[0]); */
            if (pBuf[0]) {
                len += pBuf[0];
                pBuf += pBuf[0];
            } else {
                len = total_len;
            }
        }
    }


    log_info("len %d total_len %d", len, total_len);
    return !have_find_valid_class;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief usb_host_suspend
 *
 * @param usb
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
void usb_host_suspend(const usb_dev usb_id)
{
    usb_h_entry_suspend(usb_id);
}

void usb_host_resume(const usb_dev usb_id)
{
    usb_h_resume(usb_id);
}


u32 _usb_host_mount(const usb_dev usb_id, u32 retry, u32 reset_delay, u32 mount_timeout, u8 *desc_buf, u32 desc_len)
{
    u32 ret = DEV_ERR_NONE;
    struct usb_host_device *host_dev = &host_devices[usb_id];
    struct usb_private_data *private_data = &host_dev->private_data;

    for (int i = 0; i < retry; i++) {
        wdt_clear();
        usb_h_sie_init(usb_id);
        ret = usb_host_init(usb_id, reset_delay, mount_timeout);
        if (ret) {
            reset_delay += 10;
            continue;
        }


        void *const ep0_dma = usb_h_get_ep_buffer(usb_id, 0);
        usb_set_dma_taddr(usb_id, 0, ep0_dma);

        /* usb_sie_enable(usb_id);//enable sie intr */
        usb_mdelay(reset_delay);

        /**********get device descriptor*********/
        struct usb_device_descriptor device_desc;
        private_data->usb_id = usb_id;
        private_data->status = 0;
        private_data->devnum = 0;
        private_data->ep0_max_packet_size = 8;
        ret = usb_get_device_descriptor(host_dev, &device_desc);
        if (ret == -DEV_ERR_TIMEOUT) {
            wdt_clear();
            break;
        }

        /**********set address*********/
        usb_mdelay(20);
        u8 devnum = 8;//rand32() % 16 + 1;
        ret = set_address(host_dev, devnum);
        check_usb_mount(ret);
        private_data->devnum = devnum ;

        /**********get device descriptor*********/
        usb_mdelay(20);
        ret = usb_get_device_descriptor(host_dev, &device_desc);
        check_usb_mount(ret);
        private_data->ep0_max_packet_size = device_desc.bMaxPacketSize0;

        /**********get config descriptor*********/
        struct usb_config_descriptor cfg_desc;
        ret = get_config_descriptor(host_dev, &cfg_desc, USB_DT_CONFIG_SIZE);
        check_usb_mount(ret);

        cfg_desc.wTotalLength = min(desc_len, cfg_desc.wTotalLength);

        memset(desc_buf, 0, cfg_desc.wTotalLength);

        ret = get_config_descriptor(host_dev, desc_buf, cfg_desc.wTotalLength);
        check_usb_mount(ret);

        /**********set configuration*********/
        ret = set_configuration(host_dev);
        /* log_info_buf(desc_buf, cfg_desc.wTotalLength); */
        ret |= usb_descriptor_parser(host_dev, desc_buf, cfg_desc.wTotalLength, &device_desc);

        check_usb_mount(ret);

        break;//succ
    }

    if (ret) {
        goto __exit_fail;
    }
    private_data->status = 1;
    return DEV_ERR_NONE;

__exit_fail:
    log_info("usb_probe fail");
    private_data->status = 0;
    usb_sie_close(usb_id);
    return ret;
}
static int usb_event_notify(const struct usb_host_device *host_dev, u32 ev)
{
#if 0
    const usb_dev id = host_device2id(host_dev);
    struct sys_event event;
    if (ev == 0) {
        event.u.dev.event = DEVICE_EVENT_IN;
    } else {
        event.u.dev.event = DEVICE_EVENT_CHANGE;
    }
    u8 have_post_event = 0;
    u8 no_send_event;
    for (u8 i = 0; i < MAX_HOST_INTERFACE; i++) {
        no_send_event = 0;
        event.u.dev.value = 0;
        if (host_dev->interface_info[i]) {
            switch (host_dev->interface_info[i]->ctrl->interface_class) {
#if TCFG_UDISK_ENABLE
            case USB_CLASS_MASS_STORAGE:
                if (have_post_event & BIT(0)) {
                    no_send_event = 1;
                } else {
                    have_post_event |= BIT(0);
                }
                if (id == 0) {
                    event.u.dev.value = (int)"udisk0";
                } else {
                    event.u.dev.value = (int)"udisk1";
                }
                break;
#endif

#if TCFG_ADB_ENABLE
            case USB_CLASS_ADB:
                if (have_post_event & BIT(1)) {
                    no_send_event = 1;
                } else {
                    have_post_event |= BIT(1);
                }
                if (id == 0) {
                    event.u.dev.value = (int)"adb0";
                } else {
                    event.u.dev.value = (int)"adb1";
                }
                break;
#endif
#if TCFG_AOA_ENABLE
            case USB_CLASS_AOA:
                if (have_post_event & BIT(2)) {
                    no_send_event = 1;
                } else {
                    have_post_event |= BIT(2);
                }
                if (id == 0) {
                    event.u.dev.value = (int)"aoa0";
                } else {
                    event.u.dev.value = (int)"aoa1";
                }
                break;
#endif
#if TCFG_HID_HOST_ENABLE
            case USB_CLASS_HID:
                if (have_post_event & BIT(3)) {
                    no_send_event = 1;
                } else {
                    have_post_event |= BIT(3);
                }
                if (id == 0) {
                    event.u.dev.value = (int)"hid0";
                } else {
                    event.u.dev.value = (int)"hid1";
                }
                break;
#endif
            }

            if (!no_send_event && event.u.dev.value) {
                log_info("event %x interface %x class %x %s",
                         event.u.dev.event, i,
                         host_dev->interface_info[i]->ctrl->interface_class,
                         (const char *)event.u.dev.value);

                /* log_info("usb_host_mount notify >>>>>>>>>>>\n"); */
                event.arg = (void *)DEVICE_EVENT_FROM_USB_HOST;
                event.type = SYS_DEVICE_EVENT;
                sys_event_notify(&event);
            }
        }
    }
    if (have_post_event) {
        return DEV_ERR_NONE;
    } else {
        return DEV_ERR_UNKNOW_CLASS;
    }
#else
    return DEV_ERR_NONE;
#endif
}

const char *usb_host_valid_class_to_dev(const usb_dev id, u32 usbclass)
{
#if USB_MAX_HW_NUM > 1
    const usb_dev usb_id = id;
#else
    const usb_dev usb_id = 0;
#endif
    struct usb_host_device *host_dev = &host_devices[usb_id];
    u32 itf_class;

    for (int i = 0; i < MAX_HOST_INTERFACE; i++) {
        if (host_dev->interface_info[i] &&
            host_dev->interface_info[i]->ctrl) {
            itf_class = host_dev->interface_info[i]->ctrl->interface_class;
            if (itf_class == usbclass) {
                switch (itf_class) {
                case USB_CLASS_MASS_STORAGE:
                    if (usb_id == 0) {
                        return "udisk0";
                    } else if (usb_id == 1) {
                        return "udisk1";
                    }
                    break;
                case USB_CLASS_ADB:
                    if (usb_id == 0) {
                        return "adb0";
                    } else if (usb_id == 1) {
                        return "adb1";
                    }
                    break;
                case USB_CLASS_AOA:
                    if (usb_id == 0) {
                        return "aoa0";
                    } else if (usb_id == 1) {
                        return "aoa1";
                    }
                    break;
                case USB_CLASS_HID:
                    if (usb_id == 0) {
                        return "hid0";
                    } else if (usb_id == 1) {
                        return "hid1";
                    }
                    break;
                }
            }
        }
    }
    return NULL;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief usb_host_mount
 *
 * @param usb
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
u32 usb_host_mount(const usb_dev id, u32 retry, u32 reset_delay, u32 mount_timeout)
{
#if USB_MAX_HW_NUM > 1
    const usb_dev usb_id = id;
#else
    const usb_dev usb_id = 0;
#endif

    memset(&host_var, 0, sizeof(host_var));
    usb_host_config(0, &host_var);
    host_var.msd_h_dma_buffer = msd_h_dma_buffer;

    struct usb_host_device *host_dev = &host_devices[usb_id];
    memset(host_dev, 0, sizeof(*host_dev));

    log_info("%s() %d \n", __func__, __LINE__);
    host_dev->private_data.usb_id = id;

    usb_sem_init(host_dev);
    //usb_h_isr_reg(usb_id, IRQ_USB_IP, 0);
    /* usb_h_isr_reg(usb_id, 3, 0); */
    u32 ret = _usb_host_mount(usb_id, retry, reset_delay, mount_timeout, msd_h_dma_buffer[1], sizeof(msd_h_dma_buffer) / 2);

    usb_otg_resume(usb_id);  //打开usb host之后恢复otg检测
    if (ret) {
        goto __exit_fail;
    }
    return usb_event_notify(host_dev, 0);

__exit_fail:
    usb_sie_disable(usb_id);
    usb_sem_del(host_dev);

    return ret;
}

u32 _usb_host_unmount(const usb_dev usb_id)
{
    struct usb_host_device *host_dev = &host_devices[usb_id];

    struct usb_private_data *private_data = &host_dev->private_data;
    private_data->status = 0;

    usb_sem_post(host_dev);//拔掉设备时，让读写线程快速释放

    for (u8 i = 0; i < MAX_HOST_INTERFACE; i++) {
        if (host_dev->interface_info[i]) {
            host_dev->interface_info[i]->ctrl->set_power(host_dev, -1);
            host_dev->interface_info[i] = NULL;
        }
    }

    usb_sie_close(usb_id);
    return DEV_ERR_NONE;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief usb_host_unmount
 *
 * @param usb
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
/* u32 usb_host_unmount(const usb_dev usb_id, char *device_name) */
u32 usb_host_unmount(const usb_dev id)
{
#if 1
#if USB_MAX_HW_NUM > 1
    const usb_dev usb_id = id;
#else
    const usb_dev usb_id = 0;
#endif
    u32 ret;
    struct usb_host_device *host_dev = &host_devices[usb_id];
    log_info("%s() %d \n", __func__, __LINE__);

    ret = _usb_host_unmount(usb_id);
    if (ret) {
        goto __exit_fail;
    }
    usb_sem_del(host_dev);

    /* log_info("usb_host_unmount notify >>>>>>>>>>>\n"); */
    /* event.arg = (void *)DEVICE_EVENT_FROM_USB_HOST; */
    /* event.type = SYS_DEVICE_EVENT; */
    /* event.u.dev.event = DEVICE_EVENT_OUT; */
    /* sys_event_notify(&event); */
    return DEV_ERR_NONE;

__exit_fail:
    return ret;
#else
    return 0;
#endif
}

u32 usb_host_remount(const usb_dev id, u32 retry, u32 delay, u32 ot, u8 notify)
{
#if USB_MAX_HW_NUM > 1
    const usb_dev usb_id = id;
#else
    const usb_dev usb_id = 0;
#endif
    u32 ret;

    memset(&host_var, 0, sizeof(host_var));
    usb_host_config(0, &host_var);
    host_var.msd_h_dma_buffer = msd_h_dma_buffer;

    ret = _usb_host_unmount(usb_id);
    if (ret) {
        goto __exit_fail;
    }

    ret = _usb_host_mount(usb_id, retry, delay, ot, msd_h_dma_buffer[1], sizeof(msd_h_dma_buffer) / 2);
    if (ret) {
        goto __exit_fail;
    }

    if (notify) {
        struct usb_host_device *host_dev = &host_devices[usb_id];
        usb_event_notify(host_dev, 1);
    }
    return DEV_ERR_NONE;

__exit_fail:
    return ret;
}
