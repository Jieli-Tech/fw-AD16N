#if 0
#include "app_config.h"
#include "usb/host/usb_host.h"
#include "usb/device/usb_stack.h"
#include "usb/otg.h"
#include "usb/device/msd.h"
#include "sound_mge.h"
#include "circular_buf.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[mount]"
#include "log.h"

u8 msd_test_buf[1 * 1024];
u32 user_scsi_intercept(const struct usb_device_t *usb_device, struct usb_scsi_cbw *cbw);
void dump_usb_buf();
void usb_host_test()
{
___usb_test:
    usb_host_mount(0, 3, 20, 200); //usb测试使用
    struct device *device;
    usb_stor_open("udisk0", &device, NULL);
    log_info("%s---%d\n", __func__, __LINE__);
    u32 lba = 0;
    while (1) {

        u32 *p = (u32 *)msd_test_buf;
        u32 numOflba = sizeof(msd_test_buf) / 512;

        asm("trigger");
        if ((usb_stor_read(device, p, numOflba, lba)) != numOflba) {
            printf("read error %d", lba);
            while (1);
        }
#if 1
        for (int i = 0; i < numOflba * 512 / 4; i++) {
            if (p[i] != (i + lba * 0x80)) {
                printf("RRR@lba:%x offset %x   %x != %x ", lba, i, p[i], i + lba * 0x80);
                printf_buf(p, numOflba * 512);
                while (1);
            }
        }
        if (usb_stor_write(device, p, numOflba, lba) != numOflba) {

            printf("write error %d", lba);
            while (1);
        }

        if ((usb_stor_read(device, p, numOflba, lba)) != numOflba) {
            printf("read error %d", lba);
            while (1);
        }
#endif
        for (int i = 0; i < numOflba * 512 / 4; i++) {
            if (p[i] != (i + lba * 0x80)) {
                printf("WWW@lba:%x offset %x   %x != %x ", lba, i, p[i], i + lba * 0x80);
                printf_buf(p, numOflba * 512);
                while (1);
            }
        }
        wdt_clear();

        lba += numOflba;
    }
}
#if defined(CONFIG_CPU_BR23)
#define     CPU_NAME    'B','R','2','3'
#elif defined(CONFIG_CPU_BR25)
#define     CPU_NAME    'B','R','2','5'
#elif defined(CONFIG_CPU_BD29)
#define     CPU_NAME    'B','D','2','9'
#else
#define     CPU_NAME    'U','C','0','3'
/* #error "not define cpu name" */
#endif
// *INDENT-ON*
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
    'R',
    'O',
    'M ',
    '_',
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

    0x32,   //"1" -- Production Revision Level start
    0x2e,   //"."
    0x30,   //"0"
    0x30    //"0" -- Production Revision Level end
};

#define MSD_BUFFER_SIZE     (MSD_BLOCK_SIZE * 512)

static u8 msd_buf[MSD_BUFFER_SIZE * 2] SEC(.mass_storage) __attribute__((aligned(64)));
struct msd_vat_t msd_var = {
    .msd_buf = msd_buf,
    .msd_buffer_size = MSD_BUFFER_SIZE,
    .max_lun = 2,
    .cdrom_enable = 1,
    .inquiry[0] = CD_SCSIInquiryData,
    .inquiry[1] = SCSIInquiryData,
};
struct hid_device_var_t hid_var;/*{
    void *ep_in_dma;
    u32(*output_report)(struct usb_device_t *, struct usb_ctrlrequest *);
#if HID_EP_OUT_EN
    void *ep_out_dma;
    u32(*rx_data_wakeup)(struct usb_device_t *, u32 ep);
#endif
    const void *report_desc;
    const void *interface_desc;
    u8 report_desc_len;
    u8 interface_desc_len;
};*/
#include "usb/device/hid.h"
struct _cdrom_cmd cdrom_cmd;
struct device *cdrom_fd;
void usb_device_test()
{
    /* memset(&msd_var,0,sizeof(msd_var)); */
    hid_var.ep_in_buffer = usb_get_ep_buffer(0, HID_EP_IN | USB_DIR_IN);



    msd_var.ep_in_buffer = usb_get_ep_buffer(0, MSD_BULK_EP_IN | USB_DIR_IN);
    msd_var.ep_out_buffer  = usb_get_ep_buffer(0, MSD_BULK_EP_OUT);
    //msd_var.inquiry[0]  = CD_SCSIInquiryData;
    //msd_var.inquiry[1]  = SCSIInquiryData;
    msd_var.private_scsi_cmd = user_scsi_intercept;
    printf("%x %x\n", msd_var.inquiry[0], msd_var.inquiry[1]);
    printf("%x %x\n", CD_SCSIInquiryData, SCSIInquiryData);
    /* msd_register_disk("SFC", NULL); */
    /* msd_register_disk("norflash", NULL); */
    /* usb_device_mode(0,   HID_CLASS | AUDIO_CLASS); */
    //usb_device_mode(0, SPEAKER_CLASS | HID_CLASS);
    usb_device_mode(0, USB_DEVICE_CLASS_CONFIG);
    //msd_register_disk("sd0", NULL);
    msd_register_disk("sd_cdrom", NULL);
    msd_register_disk("sd_enc", NULL);
    cdrom_fd = 0;
    u8 ret = 0;
    while (1) {
        ret = getchar();
        if (ret == 1) {
            usb_hid_control(USB_AUDIO_PP);
        } else if (ret == 2) {
            usb_hid_control(USB_AUDIO_PREFILE);
        } else if (ret == 3) {
            usb_hid_control(USB_AUDIO_NEXTFILE);
        } else if (ret == 4) {
            usb_hid_control(USB_AUDIO_VOLDOWN);
        } else if (ret == 5) {
            usb_hid_control(USB_AUDIO_VOLUP);
        }
        if (ret) {
            delay_10ms(3);
            usb_hid_control(0);
        }
        /* delay_10ms(10); */
        USB_MassStorage(NULL);
    }
}
extern volatile u8 test_flag;
sound_out_obj uac_sound;
static cbuffer_t cbuf_aux_o;
static u8 obuf_aux_o [192 * 5] ALIGNED(4) ;

#include "sound_mge.h"
#include "circular_buf.h"
static sound_out_obj t_sound;
static u8 t_obuf[32 * 30];
static cbuffer_t t_cbuf;


void usb_test()
{
#if 1


    audio_init();
    dac_phy_vol(16384, 16384);
    dac_mode_init(31);
    dac_init_api(48000);

    memset(&uac_sound, 0, sizeof(uac_sound));
    cbuf_init(&cbuf_aux_o, &obuf_aux_o[0], sizeof(obuf_aux_o));
    uac_sound.p_obuf = &cbuf_aux_o;
    regist_dac_channel(&uac_sound, NULL); //注册到DAC;
    uac_sound.enable |= B_DEC_RUN_EN;

    /* test_audio_usb(); */
    usb_device_test();
#elif 0
    usb_host_test();
#else
    while (1) {
        if (test_flag == 1) {
            test_flag = 0;
            printf("udisk mount\n");
            usb_host_mount(0, 3, 20, 200); //usb测试使用
            vfs_demo();
        } else if (test_flag == 2) {
            test_flag = 0;
            printf("udisk unmount\n");
            usb_host_unmount(0);
        }
    }
#endif
}


#include "crc16.h"
#define SDMMC0_ENC_EN   1
#define SDMMC1_ENC_EN   1
extern s32 sd0_dev_read();
extern s32 sd1_dev_read();
struct _cdrom_cmd {
    u16 head_crc;
    u8  reg[2];
    u32 password_addr;
    u32 cdrom_addr;
    u32 sd_offset;
};


static u32 my_password_addr;
static u32 sd_enc_offset = 0;
static u32 cdrom_offset = 0;
static u8 login_sd_enc_success = 0;
static u8 check_cdrom_success = 0;
static u8 transfer_key[4];
static u8 my_password[512];
static u8 tmp_ep1_rx_buf[512];

static void PasswordDecode(void  *buf, u16 len, u8 *crckey)
{
    const u8 keylcode[] = {
        0xC3, 0xCF, 0xC0, 0xE8, 0xCE, 0xD2, 0xB0, 0xAE,
        0xC4, 0xE3, 0xA3, 0xAC, 0xD3, 0xF1, 0xC1, 0xD6
    };
    //const u8 crckey[4] = {0xff, 0xff, 0xff, 0xff} ;
    u8 i = 0;

    JL_CRC->REG = (crckey[1] << 8) | crckey[0] ;
    /* asm volatile ("csync"); */
    JL_CRC->FIFO = crckey[2] ;
    JL_CRC->FIFO = crckey[3] ;
    /* asm volatile ("csync"); */

    while (len--) {
        JL_CRC->FIFO = keylcode[i];

        if (i++ >= 15) {
            i  = 0 ;
        }

        asm volatile("csync");
        *((u8 *)buf) = *((u8 *)buf) ^ (u8)JL_CRC->REG ;
        /* asm volatile ("csync"); */
        buf = ((u8 *)buf) + 1 ;

    }
}

static u16 u16_big_or_little_end(u16 data)
{
    u8 data_l = (data >> 8) & 0xff;
    u8 data_h = data & 0xff;
    u16 new_data = ((data_h << 8) | data_l);
    return new_data;
}

static u32 u32_big_or_little_end(u32 data)
{
    u8 data_ll = (data >> 24) & 0xff;
    u8 data_l = (data >> 16) & 0xff;
    u8 data_h = (data >> 8) & 0xff;
    u8 data_hh = data & 0xff;
    u32 new_data = ((data_hh << 24) | (data_h << 16) | (data_l << 8) | data_ll);
    return new_data;
}

void get_cdrom_cmd(void)    //卡驱动里库要调用该函数
{
    u16 tmp_crc = 0;
    sd_enc_offset = 0;
    cdrom_offset = 0;
    my_password_addr = 0;
    check_cdrom_success = 0;
    memset((u8 *)&cdrom_cmd, 0, sizeof(cdrom_cmd));
    memset(tmp_ep1_rx_buf, 0, 512);
    dev_bulk_read(cdrom_fd, tmp_ep1_rx_buf, 0, 1);

    printf_buf(tmp_ep1_rx_buf, 512);
    memcpy((u8 *)&cdrom_cmd, tmp_ep1_rx_buf, sizeof(cdrom_cmd));
    tmp_crc = CRC16((u8 *)&cdrom_cmd.reg[0], sizeof(cdrom_cmd) - 2);
    cdrom_cmd.head_crc = u16_big_or_little_end(cdrom_cmd.head_crc);
    if ((tmp_crc != cdrom_cmd.head_crc) || (cdrom_cmd.head_crc == 0)) {
        printf("cdrom_cmd head crc error\n");
        printf("tmp_crc = 0x%x  head_crc = 0x%x\n", tmp_crc, cdrom_cmd.head_crc);
        memset((u8 *)&cdrom_cmd, 0, sizeof(cdrom_cmd));
        return;
    }
    cdrom_cmd.password_addr = u32_big_or_little_end(cdrom_cmd.password_addr);
    cdrom_cmd.cdrom_addr = u32_big_or_little_end(cdrom_cmd.cdrom_addr);
    cdrom_cmd.sd_offset = u32_big_or_little_end(cdrom_cmd.sd_offset);
    check_cdrom_success = 1;
    sd_enc_offset = cdrom_cmd.sd_offset;
    cdrom_offset = cdrom_cmd.cdrom_addr;
    my_password_addr = cdrom_cmd.password_addr;
    printf("check_cdrom_success = %d\n", check_cdrom_success);
    printf("my_password_addr = 0x%x\n", my_password_addr);
    printf("cdrom_offset = 0x%x\n", cdrom_offset);
    printf("sd_enc_offset = 0x%x\n", sd_enc_offset);
}


//*****************sd_cdrom区
static struct device sd_cdrom_dev;
int sd_cdrom_dev_init(const struct dev_node *node, void *arg)
{
    //return dev_init();
    return 0;
}
bool sd_cdrom_dev_online(const struct dev_node *node)
{
    if (cdrom_fd) {
        return 1;
    } else {
        return 0;
    }
}
int sd_cdrom_dev_open(const char *name, struct device **device, void *arg)
{
    *device = &sd_cdrom_dev;
    return 0;//dev_open("sd0", NULL);
}
int sd_cdrom_dev_read(struct device *device, void *buf, u32 len, u32 offset)
{
    return dev_bulk_read(cdrom_fd, buf, cdrom_offset + offset, len);
}
int sd_cdrom_dev_ioctl(struct device *device, u32 cmd, u32 arg)
{
    if (cmd == IOCTL_GET_CAPACITY) {
        *(u32 *)arg = sd_enc_offset - cdrom_offset;
        return !sd_enc_offset;
    }
    return dev_ioctl(cdrom_fd, cmd, arg);
}
int sd_cdrom_dev_close(struct device *device)
{
    return 0;//dev_close(cdrom_fd);
}
//*****************sd_cdrom区

//*****************sd_enc区
static struct device sd_enc_dev;
int sd_enc_dev_init(const struct dev_node *node, void *arg)
{
    //return dev_init();
    return 0;
}
bool sd_enc_dev_online(const struct dev_node *node)
{
    if (cdrom_fd) {
        if (login_sd_enc_success == 1) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}
int sd_enc_dev_open(const char *name, struct device **device, void *arg)
{
    *device = &sd_enc_dev;
    return 0;//dev_open("sd0", NULL);
}
int sd_enc_dev_read(struct device *device, void *buf, u32 len, u32 offset)
{
    return dev_bulk_read(cdrom_fd, buf, sd_enc_offset + offset, len);
}
int sd_enc_dev_write(struct device *device, void *buf, u32 len, u32 offset)
{
    return dev_bulk_write(cdrom_fd, buf, sd_enc_offset + offset, len);
}
int sd_enc_dev_ioctl(struct device *device, u32 cmd, u32 arg)
{
    if (cmd == IOCTL_GET_CAPACITY) {
        int ret = dev_ioctl(cdrom_fd, cmd, arg);
        *(u32 *)arg -= sd_enc_offset;
        return ret;
    }
    return dev_ioctl(cdrom_fd, cmd, arg);
}
int sd_enc_dev_close(struct device *device)
{
    return 0;//dev_close(cdrom_fd);
}
//*****************sd_enc区


static u32 usb_read_sd(void *buf, u32 lba)
{
    return dev_bulk_read(cdrom_fd, buf, lba, 1);
}

static u32 usb_write_sd(void *buf, u32 lba)
{
    return dev_bulk_write(cdrom_fd, buf, lba, 1);
}

static void crack_transfer_key(struct msd_vat_t *msd_var)//接收秘钥
{
    printf("crack_transfer_key\n");

    u32 tmp_len = 64;
    usb_g_ep_read(0, MSD_BULK_EP_OUT, tmp_ep1_rx_buf, tmp_len, 1);
    u16 tmp_key = CRC16(tmp_ep1_rx_buf, tmp_len);
    transfer_key[0] = (tmp_key >> 8) & 0xff;
    transfer_key[1] = tmp_key & 0xff;
    printf("transfer_key[0] = 0x%x   transfer_key[1] = 0x%x\n", transfer_key[0], transfer_key[1]);
}

static void send_transfer_key_buf(struct msd_vat_t *msd_var)//发送秘钥
{
    printf("send_transfer_key_buf\n");

    u32 tmp_len = 64;
    u32 i;
    for (i = 0; i < tmp_len; i ++) {
        tmp_ep1_rx_buf[i] = JL_RAND->R64L;
    }
    printf_buf(tmp_ep1_rx_buf, tmp_len);
    u16 tmp_key = CRC16(tmp_ep1_rx_buf, tmp_len);
    transfer_key[2] = (tmp_key >> 8) & 0xff;
    transfer_key[3] = tmp_key & 0xff;
    usb_g_ep_write(0, MSD_BULK_EP_IN, tmp_ep1_rx_buf, tmp_len);
    printf("transfer_key[2] = 0x%x   transfer_key[3] = 0x%x\n", transfer_key[2], transfer_key[3]);
}

static u8 compare_login_password(struct msd_vat_t *msd_var)//登录
{
    printf("compare_login_password\n");

    u32 tmp_len = 512;
    login_sd_enc_success = 0;
    if (check_cdrom_success == 0) {
        return 1;
    }
    usb_read_sd(my_password, my_password_addr);
    printf_buf(my_password, 512);
    usb_g_ep_read(0, MSD_BULK_EP_OUT, tmp_ep1_rx_buf, tmp_len, 1);
    printf_buf(tmp_ep1_rx_buf, tmp_len);
    PasswordDecode(tmp_ep1_rx_buf, tmp_len, transfer_key);
    printf_buf(tmp_ep1_rx_buf, tmp_len);
    if (memcmp(my_password, tmp_ep1_rx_buf, tmp_len)) {
        printf("login_error\n");
        return 1;                                 //密码不匹配
    }
    printf("login_sd_enc_success\n");
    login_sd_enc_success = 1;
    return 0;
}

static u8 set_new_login_password(struct msd_vat_t *msd_var)//修改密码
{
    printf("set_new_login_password\n");

    u32 tmp_len = 512;
    if (!login_sd_enc_success) {
        return 1;
    }
    printf_buf(my_password, 512);
    usb_g_ep_read(0, MSD_BULK_EP_OUT, tmp_ep1_rx_buf, tmp_len, 1);
    printf_buf(tmp_ep1_rx_buf, tmp_len);
    PasswordDecode(tmp_ep1_rx_buf, tmp_len, transfer_key);
    printf_buf(tmp_ep1_rx_buf, tmp_len);
    if (!memcmp(my_password, tmp_ep1_rx_buf, tmp_len)) {
        printf("set_login_password_success\n");
        return 0;
    }
    memcpy(my_password, tmp_ep1_rx_buf, tmp_len);
    usb_write_sd(my_password, my_password_addr);
    usb_read_sd(my_password, my_password_addr);
    printf_buf(my_password, 512);
    if (memcmp(my_password, tmp_ep1_rx_buf, tmp_len)) {
        printf("set_login_password_error\n");
        return 1;
    }
    printf("set_login_password_success\n");
    return 0;
}

static u8 send_dev_ack(struct msd_vat_t *msd_var)
{
    printf("send ack\n");

    tmp_ep1_rx_buf[0] = 0xfa;
    tmp_ep1_rx_buf[1] = 0xc0;
    tmp_ep1_rx_buf[2] = check_cdrom_success;
    tmp_ep1_rx_buf[3] = 0x02;
    tmp_ep1_rx_buf[4] = 0xe9;
    tmp_ep1_rx_buf[5] = 0x6a;

    usb_g_ep_write(0, MSD_BULK_EP_IN, tmp_ep1_rx_buf, 6);
    return 0;
}

static u8 write_cdrom(struct msd_vat_t *msd_var)
{
    printf("write cdrom\n");
    u32 addr = 0;
    u16 len = 0;
    ((u8 *)&addr)[3] = msd_var->cbw.lba[0];
    ((u8 *)&addr)[2] = msd_var->cbw.lba[1];
    ((u8 *)&addr)[1] = msd_var->cbw.lba[2];
    ((u8 *)&addr)[0] = msd_var->cbw.lba[3];
    ((u8 *)&len)[1] = msd_var->cbw.Reserved;
    ((u8 *)&len)[0] = msd_var->cbw.LengthH;
    printf("addr = 0x%x   len = %d\n", addr, len);
    while (len) {
        usb_g_ep_read(0, MSD_BULK_EP_OUT, tmp_ep1_rx_buf, 512, 1);
        usb_write_sd(tmp_ep1_rx_buf, addr);
        addr += 1;
        len -= 1;
    }
    return 0;
}

static u8 send_cdrom_crc(struct msd_vat_t *msd_var)
{
    printf("send crc\n");
    u32 addr = 0;
    u16 len = 0;
    u16 tmp_crc = 0;
    ((u8 *)&addr)[3] = msd_var->cbw.lba[0];
    ((u8 *)&addr)[2] = msd_var->cbw.lba[1];
    ((u8 *)&addr)[1] = msd_var->cbw.lba[2];
    ((u8 *)&addr)[0] = msd_var->cbw.lba[3];
    ((u8 *)&len)[1] = msd_var->cbw.Reserved;
    ((u8 *)&len)[0] = msd_var->cbw.LengthH;
    printf("addr = 0x%x   len = %d\n", addr, len);
    while (len) {
        usb_read_sd(tmp_ep1_rx_buf, addr);
        tmp_crc = CRC16_with_initval(tmp_ep1_rx_buf, 512, tmp_crc);
        addr += 1;
        len -= 1;
    }
    printf("tmp_crc = 0x%x\n", tmp_crc);
    tmp_ep1_rx_buf[0] = 0xfa;
    tmp_ep1_rx_buf[1] = 0x13;
    tmp_ep1_rx_buf[2] = ((u8 *)&tmp_crc)[1];
    tmp_ep1_rx_buf[3] = ((u8 *)&tmp_crc)[0];
    usb_g_ep_write(0, MSD_BULK_EP_IN, tmp_ep1_rx_buf, 4);
    return 0;
}

static u8 operation_cdrom_finish(struct msd_vat_t *msd_var)
{
    printf("operation finish\n");
    get_cdrom_cmd();
    tmp_ep1_rx_buf[0] = 0xfa;
    tmp_ep1_rx_buf[1] = 0xc1;
    usb_g_ep_write(0, MSD_BULK_EP_IN, tmp_ep1_rx_buf, 2);
    return 0;
}

u32 user_scsi_intercept(const struct usb_device_t *usb_device, struct usb_scsi_cbw *cbw)
{
    //msd_var;
    if (cbw->operationCode == 0xfa) {
        switch (cbw->lun) {
        case 0x00://接收秘钥
            crack_transfer_key(&msd_var);
            break;
        case 0x01://发送秘钥
            send_transfer_key_buf(&msd_var);
            break;
        case 0x02://登录
            if (compare_login_password(&msd_var)) {
                msd_var.info.bError = 0x02;
                msd_var.csw.bCSWStatus = 0x01;
                return 1;//succes
            }
            break;
        case 0x03://修改密码
            if (set_new_login_password(&msd_var)) {
                msd_var.info.bError = 0x02;
                msd_var.csw.bCSWStatus = 0x01;
                return 1;//succes
            }
            break;
        case 0x04://写数据
            write_cdrom(&msd_var);
            break;
        case 0x13://读CRC
            send_cdrom_crc(&msd_var);
            break;
        case 0xc0://检测设备
            send_dev_ack(&msd_var);
            break;
        case 0xc1://操作完成
            operation_cdrom_finish(&msd_var);
            break;
        default:
            return 0;
        }
        msd_var.info.bError = 0;
        msd_var.csw.bCSWStatus = 0x00;
        return 1;//success
    } else if (cbw->operationCode == TEST_UNIT_READY) {
        if (dev_online("sd0")) {
            if (cdrom_fd == NULL) {
                cdrom_fd = dev_open("sd0", NULL);
                if (cdrom_fd == NULL) {
                    printf("cdrom open fail!!!\n");
                } else {
                    get_cdrom_cmd();
                }
            }
        } else if (cdrom_fd) {
            dev_close(cdrom_fd);
            cdrom_fd = NULL;
        }
    }
    return 0;//fail
}


#else
void usb_test()
{

}

#endif
