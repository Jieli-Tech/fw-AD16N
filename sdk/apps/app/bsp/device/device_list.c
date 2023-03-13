
#pragma bss_seg(".dev_lst.data.bss")
#pragma data_seg(".dev_lst.data")
#pragma const_seg(".dev_lst.text.const")
#pragma code_seg(".dev_lst.text")
#pragma str_literal_override(".dev_lst.text.const")

#include "config.h"
#include "common.h"
#include "app_config.h"
#include "device.h"
#include "norflash.h"
#include "sd.h"
#include "gpio.h"
#include "msg.h"
#include "usb/host/usb_storage.h"
#include "usb/otg.h"
#include "usb/usb_config.h"

#define LOG_TAG_CONST       NORM
/* #define LOG_TAG_CONST       OFF */
#define LOG_TAG             "[device_list]"
#include "log.h"

// *INDENT-OFF*
#if TFG_EXT_FLASH_EN
#include "norflash.h"
const struct spi_platform_data spi1_p_data = {
    .port = {
        TFG_SPI_CLK_PORT_SEL,
        TFG_SPI_DO_PORT_SEL,
        TFG_SPI_DI_PORT_SEL,
    },
    .mode = TFG_SPI_WORK_MODE,
    .role = SPI_ROLE_MASTER,
    .clk = 10000000,
};
NORFLASH_DEV_PLATFORM_DATA_BEGIN(norflash_data)
    .spi_hw_num = TFG_SPI_HW_NUM,
    .spi_cs_port = TFG_SPI_CS_PORT_SEL,
    .spi_read_width = TFG_SPI_READ_DATA_WIDTH,
    .spi_pdata = &spi1_p_data,
NORFLASH_DEV_PLATFORM_DATA_END()
extern const struct device_operations norflash_dev_ops;
#endif

#if TFG_SD_EN
#include "sd.h"
SD0_PLATFORM_DATA_BEGIN(sd0_data)
    .port = {
        SDMMC_CMD_IO,//CMD
        SDMMC_CLK_IO,//CLK
        SDMMC_DAT_IO,//DAT0
    },
    .speed                  = 12000000,
#if 0 //CMD检测
    .detect_mode            = SD_CMD_DECT,
    .detect_func            = sdmmc_0_cmd_detect,
#endif
#if 1 //CLK检测
    .detect_mode            = SD_CLK_DECT,
    .detect_func            = sdmmc_0_clk_detect,
    .detect_io_level        = 0,//0:低电平检测到卡  1:高电平检测到卡
#endif
#if 0 //IO检测
    .detect_mode            = SD_IO_DECT,
    .detect_func            = sdmmc_0_io_detect,
    .detect_io              = IO_PORTx_xx,//用于检测的引脚
    .detect_io_level        = x,//0:低电平检测到卡  1:高电平检测到卡
#endif
#if TFG_SDPG_ENABLE
    .power                  = set_sd_power,
#else
    .power                  = NULL,
#endif
    .data_width             = 1,
    .priority               = 3,
SD0_PLATFORM_DATA_END()

const struct device_operations sd_dev_ops = {
    .init   = sdx_dev_init,
    .online = sdx_dev_online,
    .open   = sdx_dev_open,
    .read   = sdx_dev_byte_read,
    .write  = sdx_dev_byte_write,
    .bulk_read   = sdx_dev_read,
    .bulk_write  = sdx_dev_write,
    .ioctl  = sdx_dev_ioctl,
    .close  = sdx_dev_close,
};
#endif

#if SD_CDROM_EN
const struct device_operations sd_cdrom_dev_ops = {
    .init   = sd_cdrom_dev_init,
    .online = sd_cdrom_dev_online,
    .open   = sd_cdrom_dev_open,
    .read   = NULL,//sdx_dev_byte_read,
    .write  = NULL,//sdx_dev_byte_write,
    .bulk_read   = sd_cdrom_dev_read,
    .bulk_write  = NULL,//sdx_dev_write,
    .ioctl  = sd_cdrom_dev_ioctl,
    .close  = sd_cdrom_dev_close,
};

const struct device_operations sd_enc_dev_ops = {
    .init   = sd_enc_dev_init,
    .online = sd_enc_dev_online,
    .open   = sd_enc_dev_open,
    .read   = NULL,//sdx_dev_byte_read,
    .write  = NULL,//sdx_dev_byte_write,
    .bulk_read   = sd_enc_dev_read,
    .bulk_write  = sd_enc_dev_write,
    .ioctl  = sd_enc_dev_ioctl,
    .close  = sd_enc_dev_close,
};
#endif

#if TCFG_UDISK_ENABLE
const struct device_operations mass_storage_ops = {
    .init = NULL,
    .online = usb_stor_online,
    .open = usb_stor_open,
    .read = usb_stor_read,
    .write = usb_stor_write,
    .bulk_read = usb_stor_read,
    .bulk_write = usb_stor_write,
    .ioctl = usb_stor_ioctrl,
    .close = usb_stor_close,
};
#endif

#if (TCFG_PC_ENABLE || TCFG_UDISK_ENABLE)
const struct otg_dev_data otg_data = {
    .usb_dev_en = 1,//TCFG_OTG_USB_DEV_EN,
    .slave_online_cnt = 2,//TCFG_OTG_SLAVE_ONLINE_CNT,
    .slave_offline_cnt = 2,//TCFG_OTG_SLAVE_OFFLINE_CNT,
    .host_online_cnt = 2,//TCFG_OTG_HOST_ONLINE_CNT,
    .host_offline_cnt = 3,//TCFG_OTG_HOST_OFFLINE_CNT,
    .detect_mode = TCFG_OTG_MODE,
    .detect_time_interval = 50,//TCFG_OTG_DET_INTERVAL,
    .usb_otg_sof_check_init = usb_otg_sof_check_init,
};
/* const  struct device_operations usb_dev_ops = { */
    /* .init = usb_otg_init, */
/* }; */
struct device_operations usb_dev_ops;
#endif

REGISTER_DEVICES(device_table) = {
#if TFG_EXT_FLASH_EN
#if TCFG_USB_EXFLASH_UDISK_ENABLE
    {.name = __EXT_FLASH_NANE, .ops = &norflash_dev_ops, .priv_data = (void *) &norflash_data},
#else
    {.name = __EXT_FLASH_NANE, .ops = &norfs_dev_ops, .priv_data = (void *) &norflash_data},
#endif
#endif
#if TFG_SD_EN
    {.name = __SD0_NANE, .ops = &sd_dev_ops, .priv_data = (void *) &sd0_data},
#endif
#if TCFG_UDISK_ENABLE
    {.name = __UDISK, .ops = &mass_storage_ops, .priv_data = (void *)NULL},
#endif
#if (TCFG_PC_ENABLE || TCFG_UDISK_ENABLE)
    {.name = __OTG, .ops = &usb_dev_ops, .priv_data = (void *)&otg_data},
#endif
#if SD_CDROM_EN
    {.name = "sd_cdrom", .ops = &sd_cdrom_dev_ops, .priv_data = (void *) NULL},
    {.name = "sd_enc", .ops = &sd_enc_dev_ops, .priv_data = (void *) NULL},
#endif
#ifdef VM_SFC_ENABLE
#if VM_SFC_ENABLE
    {.name = __SFC_NANE, .ops = &sfc_dev_ops, .priv_data = (void *)NULL},
#endif
#endif
};
// *INDENT-ON*

int	devices_init_api()
{
#if (TCFG_PC_ENABLE || TCFG_UDISK_ENABLE)
    usb_dev_ops.init = usb_otg_init;
#endif
    set_device_node((struct dev_node *)device_node_begin, (struct dev_node *)device_node_end);

    return devices_init();
}

/* volatile u8 test_flag = 0;//固件三部测试使用 */
int device_status_emit(const char *device_name, const u8 status)
{
    int event = 0;
    log_info("device_name:%s status:%d \n", device_name, status);

    if (!strcmp(device_name, "sd0")) {
        log_info(">>>>>>>sd ");
        if (status) {
            post_event(EVENT_SD0_IN);
            log_noinfo("online \n");
        } else {
            post_event(EVENT_SD0_OUT);
            log_noinfo("offline \n");
        }
    } else if (!strncmp(device_name, "otg:h", 5)) {
        log_info(">>>>>>>udisk ");
        if (status == 1) {
            post_event(EVENT_OTG_IN);
            log_noinfo("online \n");
            /* test_flag = 1; */
        } else {
            post_event(EVENT_OTG_OUT);
            log_noinfo("offline \n");
            /* test_flag = 2; */
        }
    } else if (!strncmp(device_name, "otg:s", 5)) {
        log_info(">>>>>>>pc ");
        if (status) {
            post_event(EVENT_PC_IN);
            log_noinfo("online \n");
        } else {
            post_event(EVENT_PC_OUT);
            log_noinfo("offline \n");
        }
    }
    return 0;
}





#if 0
#define SD0_DEV_TEST	0
#if SD0_DEV_TEST
static u8 sd_buffer[512];
void *sdx_dev_get_cache_buf(void)
{
    return sd_buffer;
}
#endif
#define n_n	1
static u8 dev_read_buf[n_n * 512] = {0};
static u8 dev_write_buf[n_n * 512] = {0};
static u8 dev_backup_buf[n_n * 512] = {0};
extern void delay(volatile u32 t);

void device_test_demo(void)
{
    log_info("%s()  enter!\n", __func__);
    u32 i = 0;
    struct device *device;
    /* log_info("device_node_begin : 0x%x, device_node_end : 0x%x\n", (u32)device_node_begin, (u32)device_node_end); */

    devices_init_api();

#if SD0_DEV_TEST
    log_info("%s(), line:%d\n", __func__, __LINE__);
    sdx_force_set_online("sd0");
    device = dev_open("sd0", 0);
#endif
    log_info("%s(), line:%d\n", __func__, __LINE__);
    if (NULL == device) {
        log_info("device null!\n");
        return;
    } else {
        log_info("device open ok!!\n");
    }
    log_info("%s(), line:%d\n", __func__, __LINE__);

    log_info("original dev data:\n");
    dev_bulk_read(device, dev_read_buf, 0, n_n);
    memcpy(dev_backup_buf, dev_read_buf, n_n * 512);
    log_info_hexdump(dev_read_buf, n_n * 512);

#if 0	//erase test
    log_info("--------------dev sector erase(0)-------------\n");
    if (dev_ioctl(device, IOCTL_ERASE_SECTOR, 0)) {
        log_info("erase error!\n");
        while (1);
    }
    log_info("after erase read_buf:\n");
    dev_byte_read(device, dev_read_buf, 0, n_n * 512);
    log_info_hexdump(dev_read_buf, n_n * 512);
#endif
#if 1	//bulk test
    for (int i = 0; i < 512; i++) {
        dev_write_buf[i] = 'a' + i;
    }
    log_info("dev bulk write/read test!\n");
    dev_bulk_write(device, dev_write_buf, 0, n_n);
    delay(10000);
    dev_bulk_read(device, dev_read_buf, 0, n_n);
    log_info("bulk read buf:\n");
    log_info_hexdump(dev_read_buf, n_n * 512);
#endif
#if	1	//byte test
    for (int i = 0; i < 512; i++) {
        dev_write_buf[i] = 'A' + i;
    }
    log_info("dev byte write/read test!\n");
    dev_byte_write(device, dev_write_buf, 0, n_n * 512);
    delay(10000);
    dev_byte_read(device, dev_read_buf, 0, n_n * 512);
    log_info("byte read buf:\n");
    log_info_hexdump(dev_read_buf, n_n * 512);
#endif

    log_info("backup release!\n");
    dev_bulk_write(device, dev_backup_buf, 0, n_n * 512);
    dev_bulk_read(device, dev_read_buf, 0, n_n * 512);
    log_info_hexdump(dev_read_buf, n_n * 512);

    dev_close(device);
}
#endif
