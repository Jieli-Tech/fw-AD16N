
#pragma bss_seg(".finit.data.bss")
#pragma data_seg(".finit.data")
#pragma const_seg(".finit.text.const")
#pragma code_seg(".finit.text")
#pragma str_literal_override(".finit.text.const")
#include "flash_wp.h"
#include "flash_init.h"
#include "tick_timer_driver.h"
#include "device.h"
#include "vfs.h"
#include "msg.h"
#include "vm.h"
#include "my_malloc.h"
#include "init_app.h"
#include "audio.h"
#include "boot.h"
#if HAS_NORFS_EN
#include "nor_fs.h"
#endif

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[finit]"
#include "log.h"



/*
extern void exception_irq_handler(void);
void all_init_isr(void)
{
    u32 i;
    unsigned int *israddr = (unsigned int *)IRQ_MEM_ADDR;
    for (i = 0; i < 32; i++) {
        israddr[i] = (u32)exception_irq_handler;
    }
}
*/
static struct vfs_attr vm_attr;
int flash_info_init(void)
{
    u32 err;
    void *pvfs = 0;
    void *pvfile = 0;
    void *device = 0;
    u32 capacity = 0;

    err = vfs_mount(&pvfs, (void *)NULL, (void *) NULL);
    ASSERT(!err, "fii vfs mount : 0x%x\n", err)
    err = vfs_openbypath(pvfs, &pvfile, "/app_area_head/VM");
    ASSERT(!err, "fii vfs openbypath : 0x%x\n", err)
    err = vfs_ioctl(pvfile, FS_IOCTL_FILE_ATTR, (int)&vm_attr);
    ASSERT(!err, "fii vfs ioctl : 0x%x\n", err)
    log_info("file size : 0x%x\nfile sclust : 0x%x\n", vm_attr.fsize, vm_attr.sclust);
    log_info("boot info 0x%x\n", boot_info.flash_size);
    vfs_file_close(&pvfile);
    vfs_fs_close(&pvfs);

    boot_info.vm.vm_saddr = vm_attr.sclust;
    boot_info.vm.vm_size = vm_attr.fsize;

    log_info("boot_info.vm.vm_saddr = 0x%x", boot_info.vm.vm_saddr);
    log_info("boot_info.vm.vm_size  = 0x%x", boot_info.vm.vm_size);

    log_info("open device sfc");
    device = dev_open(__SFC_NANE, 0);
    ASSERT(((u32)device), "open sfc: 0x%x\n", (u32)device);
    dev_ioctl(device, IOCTL_GET_CAPACITY, (u32)&capacity);
    dev_ioctl(device, IOCTL_SET_VM_INFO, (u32)&boot_info);
    dev_ioctl(device, IOCTL_SET_PROTECT_INFO, (u32)flash_code_protect_callback);
    dev_close(device);

    norflash_set_write_protect(1);
    return 0;
}
struct vfs_attr *get_vm_attr_p(void)
{
    return &vm_attr;
}

AT_SPI_CODE/*该函数放置段不可更改*/
u32 flash_code_protect_callback(u32 offset, u32 len)
{
    u32 limit_addr = vm_attr.sclust;
    /* log_info("0x%x 0x%x", limit_addr, offset); */
    if ((offset < limit_addr) || ((offset + len) > boot_info.flash_size)) {
        /* 超过正常擦写区域，不进行擦写操作 */
        return 1;
    } else {
        /* 进行擦写操作 */
        return 0;
    }
}

static struct vfs_attr eeprom_attr;
void vm_init_api(void)
{
    u32 err;
    void *pvfs = 0;
    void *pvfile = 0;
    void *device = 0;
    u32 capacity = 0;


    err = vfs_mount(&pvfs, (void *)NULL, (void *)NULL);
    ASSERT(!err, "fii vfs mount : 0x%x\n", err)
    err = vfs_openbypath(pvfs, &pvfile, "/app_area_head/EEPROM");
    ASSERT(!err, "fii vfs openbypath : 0x%x\n", err)
    err = vfs_ioctl(pvfile, FS_IOCTL_FILE_ATTR, (int)&eeprom_attr);
    ASSERT(!err, "fii vfs ioctl : 0x%x\n", err)
    log_info("EEPROM size : 0x%x\nEEPROM sclust : 0x%x\n", eeprom_attr.fsize, eeprom_attr.sclust);
    vfs_file_close(&pvfile);
    vfs_fs_close(&pvfs);

    syscfg_vm_init(eeprom_attr.sclust, eeprom_attr.fsize);
    // extern void nvm_demo(u32 start, u32 size);
    // nvm_demo(eeprom_attr.sclust, eeprom_attr.fsize);
    /*
     *demo
    u8 data_buf[10];
    log_info("memset\n");
    memset(data_buf,0xaa,10);

    log_info("vm write\n");
    vm_write(VM_INDEX_DEMO, data_buf, 10);
    memset(data_buf,0x00,10);
    log_info("vm read\n");
    vm_read(VM_INDEX_DEMO, data_buf, 10);
    log_info_hexdump(data_buf,10);
    */
}
struct vfs_attr *get_eeprom_attr_p(void)
{
    return &eeprom_attr;
}
void flash_system_init(void)
{
    fs_resource_init();
    fs_init();
    flash_info_init();
    vm_init_api();
#if HAS_NORFS_EN
    norfs_init_api();
#endif
}



