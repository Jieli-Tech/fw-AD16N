/* #pragma bss_seg(".ext_flash.data.bss") */
/* #pragma data_seg(".ext_flash.data") */
/* #pragma const_seg(".ext_flash.text.const") */
/* #pragma code_seg(".ext_flash.text") */
/* #pragma str_literal_override(".ext_flash.text.const") */

#include "norflash.h"
#include "clock.h"
#include "cpu.h"
#include "device.h"
#include "typedef.h"
#include "gpio.h"
#include "errno-base.h"
#include "app_config.h"
/* #include "mem_heap.h" */
/* #include "pmalloc.h" */
/* #include "common/code/mem/mem_heap_lwip.h" */

#if defined(TCFG_NORFLASH_DEV_ENABLE)// && TCFG_NORFLASH_DEV_ENABLE
#define LOG_TAG_CONST       OFF
/* #define LOG_TAG_CONST       FLASH */
#define LOG_TAG            "[flash]"
#define LOG_ERROR_ENABLE
#define LOG_INFO_ENABLE
#include "log.h"

#if(SPI_SD_IO_REUSE)		//SD SPI IO复用
static u8 spi_busy = 0;
#include "sdmmc/sd_host_api.h"
#endif

#define MALLOC_EN 0

#define MAX_NORFLASH_PART_NUM       4

struct norflash_partition {
    const char *name;
    u32 start_addr;
    u32 size;
    struct device device;
};
static struct norflash_partition nor_part[MAX_NORFLASH_PART_NUM];

struct norflash_info {//last
    u32 flash_id;
    u32 flash_capacity;
    s8 spi_num;
    /* int spi_err; */
    u8 spi_cs_io;
    u8 spi_r_width;
    u8 part_num;
    u8 open_cnt;
    flash_mutex mutex;
    struct norflash_partition *const part_list;
    u32 max_end_addr;
};

static struct norflash_info _norflash = {
    .spi_num = (s8) - 1,
    .part_list = nor_part,
};

int _norflash_read(u32 addr, u8 *buf, u32 len, u8 cache);
int _norflash_eraser(u8 eraser, u32 addr);

#define spi_cs_init() \
    do { \
        gpio_set_die(_norflash.spi_cs_io, 1); \
        gpio_set_direction(_norflash.spi_cs_io, 0); \
        gpio_write(_norflash.spi_cs_io, 1); \
    } while (0)

#define spi_cs_uninit() \
    do { \
        gpio_set_die(_norflash.spi_cs_io, 0); \
        gpio_set_direction(_norflash.spi_cs_io, 1); \
        gpio_set_pull_up(_norflash.spi_cs_io, 0); \
        gpio_set_pull_down(_norflash.spi_cs_io, 0); \
    } while (0)
#define spi_cs_h()                  gpio_write(_norflash.spi_cs_io, 1)
#define spi_cs_l()                  gpio_write(_norflash.spi_cs_io, 0)


/*******************************flash 软硬件spi选择*****************************/
#if 1
#define spi_read_byte()             spi_recv_byte(_norflash.spi_num, NULL /* &_norflash.spi_err */)
#define spi_write_byte(x)           spi_send_byte(_norflash.spi_num, x)
#define spi_dma_read(x, y)          spi_dma_recv(_norflash.spi_num, x, y)
#define spi_dma_write(x, y)         spi_dma_send(_norflash.spi_num, x, y)
#define spi_set_width(x)            spi_set_bit_mode(_norflash.spi_num, x)
#define spi_init()                  spi_open(_norflash.spi_num)
#define spi_closed()                spi_close(_norflash.spi_num)
#define spi_suspend()               spi_close(_norflash.spi_num)
#define spi_resume()                hw_spi_resume(_norflash.spi_num)
/* #else */
/* #define spi_read_byte()     soft_spi_recv_byte(_norflash.spi_num, NULL [> &_norflash.spi_err <]) */
/* #define spi_write_byte(x)   soft_spi_send_byte(_norflash.spi_num, x) */
/* #define spi_dma_read(x, y)  soft_spi_dma_recv(_norflash.spi_num, x, y) */
/* #define spi_dma_write(x, y) soft_spi_dma_send(_norflash.spi_num, x, y) */
/* #define spi_set_width(x)    soft_spi_set_bit_mode(_norflash.spi_num, x) */
/* #define spi_init()          soft_spi_open(_norflash.spi_num) */
/* #define spi_closed()        soft_spi_close(_norflash.spi_num) */
/* #define spi_suspend()       soft_spi_suspend(_norflash.spi_num) */
/* #define spi_resume()        soft_spi_resume(_norflash.spi_num) */
#endif

static struct norflash_partition *norflash_find_part(const char *name)
{
    struct norflash_partition *part = NULL;
    u32 idx;
    for (idx = 0; idx < MAX_NORFLASH_PART_NUM; idx++) {
        part = &_norflash.part_list[idx];
        if (part->name == NULL) {
            continue;
        }
        if (!strcmp(part->name, name)) {
            return part;
        }
    }
    return NULL;
}

static struct norflash_partition *norflash_new_part(const char *name, u32 addr, u32 size)
{
    struct norflash_partition *part;
    u32 idx;
    for (idx = 0; idx < MAX_NORFLASH_PART_NUM; idx++) {
        part = &_norflash.part_list[idx];
        if (part->name == NULL) {
            break;
        }
    }
    if (part->name != NULL) {
        log_error("create norflash part fail\n");
        return NULL;
    }
    memset(part, 0, sizeof(*part));
    part->name = name;
    part->start_addr = addr;
    part->size = size;
    if (part->start_addr + part->size > _norflash.max_end_addr) {
        _norflash.max_end_addr = part->start_addr + part->size;
    }
    _norflash.part_num++;
    return part;
}

static void norflash_delete_part(const char *name)//未使用
{
    struct norflash_partition *part;
    u32 idx;
    for (idx = 0; idx < MAX_NORFLASH_PART_NUM; idx++) {
        part = &_norflash.part_list[idx];
        if (part->name == NULL) {
            continue;
        }
        if (!strcmp(part->name, name)) {
            part->name = NULL;
            _norflash.part_num--;
        }
    }
}

static int norflash_verify_part(struct norflash_partition *p)
{
    struct norflash_partition *part = NULL;
    u32 idx;
    for (idx = 0; idx < MAX_NORFLASH_PART_NUM; idx++) {
        part = &_norflash.part_list[idx];
        if (part->name == NULL) {
            continue;
        }
        if ((p->start_addr >= part->start_addr) && (p->start_addr < part->start_addr + part->size)) {
            if (strcmp(p->name, part->name) != 0) {
                return -1;
            }
        }
    }
    return 0;
}

#if FLASH_CACHE_ENABLE
#define    FLASH_READ_NO_USE_CACHE  0
#define    FLASH_READ_OCCUPY_CACHE  1
#define    FLASH_READ_PREPARE_USE_CACHE  2
static volatile u8 flash_cache_sync_enter_critical_dis = 0;
static u8 flash_read_use_cache = FLASH_READ_NO_USE_CACHE;
static u32 flash_cache_addr;
static u8 *flash_cache_buf = NULL; //缓存4K的数据，与flash里的数据一样。
#if (MALLOC_EN==0)
static u8 norfs_cache_buf[4096]  __attribute__((aligned(4)))SEC(.norflash_cache_buf);
#endif
static u8 flash_cache_is_dirty;
static volatile u8 flash_cache_timer;

/* static int _check_0xff(u8 *buf, u32 len) */
/* { */
/*     for (u32 i = 0; i < len; i ++) { */
/*         if ((*(buf + i)) != 0xff) { */
/*             return 1; */
/*         } */
/*     } */
/*     return 0; */
/* } */
#endif


static u32 _pow(u32 num, int n)//num^n
{
    u32 powint = 1;
    int i;
    for (i = 1; i <= n; i++) {
        powint *= num;
    }
    return powint;
}

static int _norflash_wait_ok()
{
    u32 timeout = 8 * 1000 * 1000 / 100;
    u8 reg_1 = 0;
    while (timeout--) {
        spi_cs_l();
        spi_write_byte(WINBOND_READ_SR1);
        reg_1 = spi_read_byte();
        spi_cs_h();
        if (!(reg_1 & BIT(0))) {
            break;
        }
        delay(100);
    }
    if (timeout == 0) {
        log_error("norflash_wait_ok timeout!\r\n");
        return 1;
    }
    return 0;
}

static void _norflash_send_write_enable()
{
    spi_cs_l();
    spi_write_byte(WINBOND_WRITE_ENABLE);
    spi_cs_h();
}

void _norflash_power_down()
{
    spi_cs_l();
    spi_write_byte(WINBOND_POWER_DOWN);
    spi_cs_h();
    udelay(5);
}

void _norflash_release_power_down()
{
    spi_cs_l();
    spi_write_byte(WINBOND_RELEASE_POWER_DOWN);
    spi_cs_h();
    udelay(5);//3us
}

static u8 is4byte_address = 0;
static void _norflash_send_addr(u32 addr)
{
    if (is4byte_address == 1) {
        spi_write_byte(addr >> 24);
    }
    spi_write_byte(addr >> 16);
    spi_write_byte(addr >> 8);
    spi_write_byte(addr);
}
static u8 _S25FLxxx_read_id();
static u32 _norflash_read_id()
{
    u8 id[3];
    int reg = 0;
    spi_cs_l();
    spi_write_byte(WINBOND_JEDEC_ID);
    for (u8 i = 0; i < sizeof(id); i++) {
        id[i] = spi_read_byte();
    }
    spi_cs_h();
    reg = _norflash_wait_ok();
    if (reg) {
        log_error("wait id fail\n");
        return 0;
    }
    if (id[0] == 0x01) {
        if (_S25FLxxx_read_id()) {
            return 0;
        }
    }
    return id[0] << 16 | id[1] << 8 | id[2];
}

int _norflash_init(const char *name, struct norflash_dev_platform_data *pdata)
{
    log_info("norflash_init !\n");
    if (_norflash.spi_num == (s8) - 1) {
        _norflash.spi_num 	  = pdata->spi_hw_num;
        _norflash.spi_cs_io   = pdata->spi_cs_port;
        _norflash.spi_r_width = pdata->spi_read_width;
        _norflash.flash_id = 0;
        _norflash.flash_capacity = 0;
        flash_mutex_create(&_norflash.mutex, 1);
        _norflash.max_end_addr = 0;
        _norflash.part_num = 0;
    }
    ASSERT(_norflash.spi_num == pdata->spi_hw_num);
    ASSERT(_norflash.spi_cs_io == pdata->spi_cs_port);
    ASSERT(_norflash.spi_r_width == pdata->spi_read_width);
    struct norflash_partition *part;
    part = norflash_find_part(name);
    if (!part) {
        part = norflash_new_part(name, pdata->start_addr, pdata->size);
        ASSERT(part, "not enough norflash partition memory in array\n");
        ASSERT(norflash_verify_part(part) == 0, "norflash partition %s overlaps\n", name);
        log_info("norflash new partition %s\n", part->name);
    } else {
        ASSERT(0, "norflash partition name already exists\n");
    }
    return 0;
}

static void clock_critical_enter()
{

}
static void clock_critical_exit()
{
    if (!(_norflash.flash_id == 0 || _norflash.flash_id == 0xffff)) {
        spi_set_baud(_norflash.spi_num, spi_get_baud(_norflash.spi_num));
    }
}
/* CLOCK_CRITICAL_HANDLE_REG(spi_norflash, clock_critical_enter, clock_critical_exit); */
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
static u8 S25FL512_enter_4byte_addr(u32 id);
void norflash_enter_4byte_addr()
{
    spi_cs_l();
    spi_write_byte(0xb7);
    spi_cs_h();
}
void norflash_exit_4byte_addr()
{
    spi_cs_l();
    spi_write_byte(0xe9);
    spi_cs_h();
}
static int _norflash_write_pages(u32 addr, u8 *buf, u32 len);
int _norflash_open(void *arg)
{
    int reg = 0;
    u32 temporary_flash_addr = 0;
    log_info("norflash open\n");
    if (!_norflash.open_cnt) {
        spi_cs_init();
        /* spi_open(_norflash.spi_num); */

        log_info("----flash use hw spi!----");
        /* #if(TCFG_FLASH_SPI_TYPE_SELECT) */
        /* log_info("----flash use hw spi!----"); */
        /* #else */
        /* log_info("----flash use soft spi!----"); */
        /* #endif */
        spi_init();
        _norflash_release_power_down();
        reg = _norflash_wait_ok();
        if (reg) {
            log_error("wait id fail\n");
            return 1;
        }
        _norflash.flash_id = _norflash_read_id();//ID=0xEF4016
        log_info("norflash_read_id: 0x%x\n", _norflash.flash_id);
        if ((_norflash.flash_id == 0) || (_norflash.flash_id == 0xffffff)) {
            log_error("read norflash id error !\n");
            reg = -ENODEV;
            goto _open_exit;
        }
        temporary_flash_addr = _norflash.flash_id;
        switch (_norflash.flash_id) {
        case 0x10220: //compatibility S25FL512
            _norflash.flash_id <<= 8;
            _norflash.flash_id |= 0x1a;
            break;
        }
        _norflash.flash_capacity = 64 * _pow(2, (_norflash.flash_id & 0xff) - 0x10) * 1024;
        log_info("norflash_capacity: 0x%x\n", _norflash.flash_capacity);
        is4byte_address = 0;
        if (_norflash.flash_capacity > 0x1000000) { //容量大于16M
            if ((((temporary_flash_addr >> 16) & 0xff) == 0x01)) {//S25FL512芯片
                is4byte_address = 1;
                if (S25FL512_enter_4byte_addr(temporary_flash_addr)) {
                    is4byte_address = 0;
                    return 1;
                }
            } else {
                norflash_enter_4byte_addr();
                is4byte_address = 1;
            }
        }

#if FLASH_CACHE_ENABLE
#if MALLOC_EN
        flash_cache_buf = (u8 *)malloc(4096);
#else
        flash_cache_buf = norfs_cache_buf;
#endif
        ASSERT(flash_cache_buf, "flash_cache_buf is not ok\n");
        flash_cache_addr = 2;
        if (flash_read_use_cache == FLASH_READ_OCCUPY_CACHE) {
            flash_cache_addr = 4096;//先给一个大于4096的数
            _norflash_read(0, flash_cache_buf, 4096, 0);
            flash_cache_addr = 0;
        }
#endif
        log_info("norflash open success !\n");
    }
    if (_norflash.flash_id == 0 || _norflash.flash_id == 0xffffff)  {
        log_error("re-open norflash id error !\n");
        reg = -EFAULT;
        goto _open_exit;
    }
    ASSERT(_norflash.max_end_addr <= _norflash.flash_capacity, "max partition end address is greater than flash capacity\n");
    _norflash.open_cnt++;

_open_exit:
    return reg;
}

int _norflash_close(void)
{
    int reg = 0;

    log_info("norflash close start\n");
    if (_norflash.open_cnt) {
        _norflash.open_cnt--;
    }
    if (!_norflash.open_cnt) {

#if FLASH_CACHE_ENABLE
        if (flash_cache_is_dirty) {
            reg = _norflash_eraser(FLASH_SECTOR_ERASER, flash_cache_addr);
            if (reg) {
                log_error("flash close sync fail!");
                /* return 1; *///错了不能return，只提示出错
            }
            reg = _norflash_write_pages(flash_cache_addr, flash_cache_buf, 4096);
            if (reg) {
                log_error("flash close sync fail!");
                /* return 1; */
            }
            flash_cache_is_dirty = 0;
            flash_cache_timer = 0;
        }
        flash_cache_addr = 2;
        flash_read_use_cache = FLASH_READ_NO_USE_CACHE;//不论是否调用ioctl用命令关闭，默认都关闭读buf
        flash_cache_sync_enter_critical_dis = 0;
#if MALLOC_EN
        free(flash_cache_buf);
        flash_cache_buf = NULL;
#else
        flash_cache_buf = NULL;
#endif
#endif
        spi_closed();
        spi_cs_uninit();
        log_info("norflash close done\n");
    }
    return 0;
}

int _norflash_read(u32 addr, u8 *buf, u32 len, u8 cache)
{
    int reg = 0;
    u32 align_addr;
    /* flash_mutex_pend(&_norflash.mutex, 2); */
    /* y_printf("flash read  addr = %d, len = %d\n", addr, len); */
    /* putchar('a'); */
#if FLASH_CACHE_ENABLE
    if (!cache) {
        goto __no_cache1;
    }
    u32 r_len = 4096 - (addr % 4096);
    if (!(flash_cache_addr % 4096)) {
        if ((addr >= flash_cache_addr) && (addr < (flash_cache_addr + 4096))) {
            if (len <= r_len) {
                memcpy(buf, flash_cache_buf + (addr - flash_cache_addr), len);
                goto _read_exit;
            } else {
                memcpy(buf, flash_cache_buf + (addr - flash_cache_addr), r_len);
                addr += r_len;
                buf += r_len;
                len -= r_len;
            }
        }
    }
__no_cache1:
#endif
    spi_cs_l();
#if 1//soft spi only have 1BIT
    if (_norflash.spi_r_width == SPI_MODE_UNIDIR_2BIT) {
        spi_write_byte(WINBOND_FAST_READ_DUAL_OUTPUT);
        _norflash_send_addr(addr);
        spi_write_byte(0);
        spi_set_width(SPI_MODE_UNIDIR_2BIT);
        spi_dma_read(buf, len);
        spi_set_width(SPI_MODE_BIDIR_1BIT);
    } else if (_norflash.spi_r_width == SPI_MODE_UNIDIR_4BIT) {
        log_error("flash write error!SPI1 don't support 4BIT!");
        /* spi_write_byte(0x6b); */
        /* _norflash_send_addr(addr); */
        /* spi_write_byte(0); */
        /* spi_set_width(SPI_MODE_UNIDIR_4BIT); */
        /* spi_dma_read(buf, len); */
        /* spi_set_width(SPI_MODE_BIDIR_1BIT);	 */
    } else
#endif
    {
        spi_write_byte(WINBOND_FAST_READ_DATA);
        _norflash_send_addr(addr);
        spi_write_byte(0);
        spi_dma_read(buf, len);
    }
    spi_cs_h();
_read_exit:
    /* flash_mutex_post(&_norflash.mutex); */
    return reg;
}

static int _norflash_write_pages(u32 addr, u8 *buf, u32 len)
{
    int reg;
    u32 first_page_len = 256 - (addr % 256);
    first_page_len = len > first_page_len ? first_page_len : len;
    _norflash_send_write_enable();
    spi_cs_l();
    spi_write_byte(WINBOND_PAGE_PROGRAM);
    _norflash_send_addr(addr) ;
    spi_dma_write(buf, first_page_len);
    spi_cs_h();
    reg = _norflash_wait_ok();
    if (reg) {
        return 1;
    }
    addr += first_page_len;
    buf += first_page_len;
    len -= first_page_len;
    /* putchar('+'); */
    while (len) {
        u32 cnt = len > 256 ? 256 : len;
        _norflash_send_write_enable();
        spi_cs_l();
        spi_write_byte(WINBOND_PAGE_PROGRAM);
        _norflash_send_addr(addr) ;
        spi_dma_write(buf, cnt);
        spi_cs_h();
        reg = _norflash_wait_ok();
        if (reg) {
            return 1;
        }
        addr += cnt;
        buf += cnt;
        len -= cnt;
    }
    return 0;
}
u8 get_flash_cache_timer(void)
{
#if FLASH_CACHE_ENABLE
    return flash_cache_timer;
#else
    return 0;
#endif
}
#if NORFLASH_NO_SYS
static u32 idle_cnt = 0;
#endif

void spi_flash_io_suspend();
void spi_flash_io_resume();
int sd_io_reuse_suspend();
void sd_io_reuse_resume();
void _norflash_cache_sync_timer(u32 sync_step)//sync_step:调节同步写入时间间隔(5即5*100ms写入)
{
#if FLASH_CACHE_ENABLE
    int reg = 0;
    if (flash_cache_is_dirty) {
#if NORFLASH_NO_SYS
        idle_cnt++;
        if (idle_cnt >= sync_step)
#endif
        {
            if (!flash_cache_sync_enter_critical_dis) {
                OS_ENTER_CRITICAL();
            }
            if (query_flash_mutex_pend(&_norflash.mutex, 5) < 0) {
                log_error("flash sync wait mutex fail!");
                if (!flash_cache_sync_enter_critical_dis) {
                    OS_EXIT_CRITICAL();
                }
                return;
            }
            /* putchar('x'); */
#if(SPI_SD_IO_REUSE)
            if (sd_io_reuse_suspend() == 0) {
                spi_busy = 1;
                spi_flash_io_resume();
#endif
                reg = _norflash_eraser(FLASH_SECTOR_ERASER, flash_cache_addr);
                if (reg) {
                    log_error("flash sync eraser fail!");
                    goto _sync_exit;
                }
                reg = _norflash_write_pages(flash_cache_addr, flash_cache_buf, 4096);
                if (reg) {
                    log_error("flash sync write fail!");
                    goto _sync_exit;
                }
                flash_cache_is_dirty = 0;
                if (flash_cache_timer) {
                    flash_cache_timer = 0;
                }
_sync_exit:
#if(SPI_SD_IO_REUSE)
                spi_flash_io_suspend();
                spi_busy = 0;
                sd_io_reuse_resume();
            } else {
                log_error("sd io suspend fail(flash sync)!");
            }
#endif

            flash_mutex_post(&_norflash.mutex);
            if (!flash_cache_sync_enter_critical_dis) {
                OS_EXIT_CRITICAL();
            }
        }
    }
#endif
}

int _norflash_write(u32 addr, void *buf, u32 len, u8 cache)
{
    int reg = 0;

    u8 *w_buf = (u8 *)buf;
    u32 w_len = len;

    /* y_printf("flash write addr = %d, num = %d\n", addr, len); */
#if FLASH_CACHE_ENABLE
    if (!cache) {
        reg = _norflash_write_pages(addr, w_buf, w_len);
        goto _write_exit;
    }
    u32 align_addr = addr / 4096 * 4096;
    u32 align_len = 4096 - (addr - align_addr);
    align_len = w_len > align_len ? align_len : w_len;
    //空间共享：必须close flash写入异步数据使地址不等，否则可能写入共享空间数据
    if (align_addr != flash_cache_addr) {
        if (flash_cache_is_dirty) {
            /* putchar('c'); */
            reg = _norflash_eraser(FLASH_SECTOR_ERASER, flash_cache_addr);
            if (reg) {
                goto _write_exit;
            }
            reg = _norflash_write_pages(flash_cache_addr, flash_cache_buf, 4096);
            if (reg) {
                goto _write_exit;
            }
            flash_cache_is_dirty = 0;
            flash_cache_timer = 0;
        }
        _norflash_read(align_addr, flash_cache_buf, 4096, 0);
        flash_cache_addr = align_addr;
        if (flash_read_use_cache == FLASH_READ_PREPARE_USE_CACHE) {
            flash_read_use_cache = FLASH_READ_OCCUPY_CACHE;
        }
    }
    memcpy(flash_cache_buf + (addr - align_addr), w_buf, align_len);
    if ((addr + align_len) % 4096) {
        /* putchar('d'); */
        flash_cache_is_dirty = 1;
        if (flash_cache_timer) { //上次未写入
        } else {
            flash_cache_timer = 1;
        }
    } else {
        /* putchar('e'); */
        flash_cache_is_dirty = 0;
        reg = _norflash_eraser(FLASH_SECTOR_ERASER, align_addr);
        if (reg) {
            goto _write_exit;
        }
        reg = _norflash_write_pages(align_addr, flash_cache_buf, 4096);
        if (reg) {
            goto _write_exit;
        }
    }
    addr += align_len;
    w_buf += align_len;
    w_len -= align_len;
    while (w_len) {
        u32 cnt = w_len > 4096 ? 4096 : w_len;
        _norflash_read(addr, flash_cache_buf, 4096, 0);
        flash_cache_addr = addr;
        memcpy(flash_cache_buf, w_buf, cnt);
        if ((addr + cnt) % 4096) {
            /* putchar('f'); */
            flash_cache_is_dirty = 1;
            if (flash_cache_timer) { //上次数据未写入
            } else {
                flash_cache_timer = 1;
            }
        } else {
            /* putchar('g'); */
            flash_cache_is_dirty = 0;
            reg = _norflash_eraser(FLASH_SECTOR_ERASER, addr);
            if (reg) {
                goto _write_exit;
            }
            reg = _norflash_write_pages(addr, flash_cache_buf, 4096);
            if (reg) {
                goto _write_exit;
            }
        }
        addr += cnt;
        w_buf += cnt;
        w_len -= cnt;
    }
#else
    reg = _norflash_write_pages(addr, w_buf, w_len);
#endif
_write_exit:
    return reg;
}

int _norflash_eraser(u8 eraser, u32 addr)
{
    u8 eraser_cmd;
    /* putchar('h'); */
    switch (eraser) {
    case FLASH_PAGE_ERASER:
        eraser_cmd = WINBOND_PAGE_ERASE;
        addr = addr / 256 * 256;
        break;
    case FLASH_SECTOR_ERASER:
        eraser_cmd = WINBOND_SECTOR_ERASE;
        //r_printf(">>>[test]:addr = %d\n", addr);
        addr = addr / 4096 * 4096;
        break;
    case FLASH_BLOCK_ERASER:
        eraser_cmd = WINBOND_BLOCK_ERASE;
        addr = addr / 65536 * 65536;
        break;
    case FLASH_CHIP_ERASER:
        eraser_cmd = WINBOND_CHIP_ERASE;
        break;
    }
    _norflash_send_write_enable();
    spi_cs_l();
    spi_write_byte(eraser_cmd);
    if (eraser_cmd != WINBOND_CHIP_ERASE) {
        _norflash_send_addr(addr);
    }
    spi_cs_h();
    return _norflash_wait_ok();
}

int norflash_eraser(u8 eraser, u32 addr)
{
    int reg = 0;
    if (flash_mutex_pend(&_norflash.mutex, 2) < 0) { //若flash忙，等待5*10ms；形参为0：死等
        log_error("flash eraser wait mutex fail!");
        return 1;
    }
    reg = _norflash_eraser(eraser, addr);
    flash_mutex_post(&_norflash.mutex);
    return reg;
}

int _norflash_ioctl(u32 cmd, u32 arg, u32 unit, void *_part)
{
    int reg = 0;
    struct norflash_partition *part = _part;
    /* flash_mutex_pend(&_norflash.mutex, 2); */
    switch (cmd) {
    case IOCTL_GET_STATUS:
        *(u32 *)arg = 1;
        break;
    case IOCTL_GET_ID:
        *((u32 *)arg) = _norflash.flash_id;
        break;
    case IOCTL_GET_CAPACITY:
        if (_norflash.flash_capacity == 0)  {
            *(u32 *)arg = 0;
        } else if (_norflash.part_num == 1 && part->start_addr == 0) {
            *(u32 *)arg = _norflash.flash_capacity / unit;
        } else {
            *(u32 *)arg = part->size / unit;
        }
        break;
    case IOCTL_GET_BLOCK_SIZE:
        *(u32 *)arg = 512;
        break;
    case IOCTL_ERASE_PAGE:
        reg = norflash_eraser(FLASH_PAGE_ERASER, arg * unit + part->start_addr);
        break;
    case IOCTL_ERASE_SECTOR:
        reg = norflash_eraser(FLASH_SECTOR_ERASER, arg * unit + part->start_addr);
        break;
    case IOCTL_ERASE_BLOCK:
        reg = norflash_eraser(FLASH_BLOCK_ERASER, arg * unit + part->start_addr);
        break;
    case IOCTL_ERASE_CHIP:
        reg = norflash_eraser(FLASH_CHIP_ERASER, 0);
        break;
    case IOCTL_FLUSH:
        break;
    case IOCTL_CMD_RESUME:
        break;
    case IOCTL_CMD_SUSPEND:
        break;
#if FLASH_CACHE_ENABLE
    case IOCTL_SET_READ_USE_CACHE:
        OS_ENTER_CRITICAL();
        if (arg == 1) {
            flash_read_use_cache = FLASH_READ_PREPARE_USE_CACHE;
        } else {
            flash_read_use_cache = FLASH_READ_NO_USE_CACHE;
        }
        OS_EXIT_CRITICAL();
        _norflash_cache_sync_timer(0);
        break;
    case IOCTL_SET_CACHE_SYNC_ISR_EN:
        flash_cache_sync_enter_critical_dis = 1;
        break;
#endif
    case IOCTL_SET_POWER_DOWN:
        _norflash_power_down();
        break;
    case IOCTL_RELEASE_POWER_DOWN:
        _norflash_release_power_down();
        break;
    default:
        reg = -EINVAL;
        break;
    }
_ioctl_exit:
    /* flash_mutex_post(&_norflash.mutex); */
    return reg;
}

/**	@brief	SPI_FLASH_IO挂起
 *
 *
 *
 */
void spi_flash_io_suspend()
{
    spi_suspend();
    spi_cs_init();				//挂起时cs脚仍需处于高电平态
}

/**	@brief	SPI_FLASH_IO释放
 *
 *
 *
 */
void spi_flash_io_resume()
{
    spi_cs_init();
    /* spi_open(_norflash.spi_num); */
    spi_resume();
}

/**	@brief	SD复用IO挂起
 *
 *
 *
 */
u8 sd_io_suspend(u8 sdx, u8 sdx_io);
u8 sd_io_resume(u8 sdx, u8 sdx_io);
int sd_io_reuse_suspend()
{
    u8 sd_io_suspend_status = 0;
    u16 retry_cnt = 0;
_retry:
    OS_ENTER_CRITICAL();
    if (0 != sd_io_suspend(0, 0)) {
        /* log_error("sd io suspend 0 fail!\n");	//cmd_sem suspend失败 */
        goto _exit;
    }
    sd_io_suspend_status |= BIT(0);
    if (0 != sd_io_suspend(0, 1)) {
        /* log_error("sd io suspend 1 fail!\n");	//clk_sem suspend失败 */
        goto _exit;
    }
    sd_io_suspend_status |= BIT(1);
    if (0 != sd_io_suspend(0, 2)) {
        /* log_error("sd io suspend 2 fail!\n");	//dat_sem suspend失败 */
        goto _exit;
    }
    OS_EXIT_CRITICAL();
    return 0;

_exit:
    if (sd_io_suspend_status & BIT(0)) {
        sd_io_resume(0, 0);						//若suspend中途失败，需将已经suspend的脚释放
    }
    if (sd_io_suspend_status & BIT(1)) {
        sd_io_resume(0, 1);
    }
    OS_EXIT_CRITICAL();

    if (retry_cnt++ < 10000) {
        udelay(10);
        goto _retry;
    }

    log_error("sd io suspend fail!\n");
    return -1;
}
/**	@brief	SD复用IO释放
 *
 *
 *
 */
void sd_io_reuse_resume()
{
    OS_ENTER_CRITICAL();
    sd_io_resume(0, 2);
    sd_io_resume(0, 1);
    sd_io_resume(0, 0);
    OS_EXIT_CRITICAL();
}

/*************************************************************************************
 *                                  挂钩 device_api
 ************************************************************************************/

static int norflash_dev_init(const struct dev_node *node, void *arg)
{
    int err = 1;
    struct norflash_dev_platform_data *pdata = arg;
    err = _norflash_init(node->name, pdata);
    return err;//0:success, other:fail
}

static int norflash_dev_open(const char *name, struct device **device, void *arg)
{
    int err = 1;
#if(SPI_SD_IO_REUSE)
    if (sd_io_reuse_suspend() == 0) {
        spi_busy = 1;
        spi_flash_io_resume();
#endif

        struct norflash_partition *part;
        part = norflash_find_part(name);
        if (!part) {
            err = -ENODEV;
            log_error("no norflash partition is found\n");
            goto __dev_open_exit1;
        }
        *device = &part->device;
        (*device)->private_data = part;
        if (atomic_read(&part->device.ref)) {
            err = 0;
            goto __dev_open_exit1;
        }
        if (flash_mutex_pend(&_norflash.mutex, 2) < 0) { //若flash忙，等待5*10ms；形参为0：死等
            log_error("flash open wait mutex fail!");
            goto __dev_open_exit1;
        }
        err = _norflash_open(arg);
        flash_mutex_post(&_norflash.mutex);
__dev_open_exit1:
#if(SPI_SD_IO_REUSE)
        spi_flash_io_suspend();
        spi_busy = 0;
        sd_io_reuse_resume();
    } else {
        log_error("sd io suspend fail(flash open)!");
    }
#endif
    return err;//0:success, other:fail
}
static int norflash_dev_close(struct device *device)
{
    int err = 1;
#if(SPI_SD_IO_REUSE)
    if (sd_io_reuse_suspend() == 0) {
        spi_busy = 1;
        spi_flash_io_resume();
#endif
        if (flash_mutex_pend(&_norflash.mutex, 2) < 0) { //若flash忙，等待5*10ms；形参为0：死等
            log_error("flash close wait mutex fail!");
            goto __close_exit;
        }

        err =  _norflash_close();
        flash_mutex_post(&_norflash.mutex);
__close_exit:
#if(SPI_SD_IO_REUSE)
        spi_flash_io_suspend();
        spi_busy = 0;
        sd_io_reuse_resume();
    } else {
        log_error("sd io suspend fail(flash close)!");
    }
#endif
    return err;//0:success, other:fail
}
static int norflash_bulk_read(struct device *device, void *buf, u32 len, u32 offset)
{
    if (!len) {
        return 0;
    }

    int reg = 0;
#if NORFLASH_NO_SYS
    idle_cnt = 0;
#endif
#if(SPI_SD_IO_REUSE)
    if (sd_io_reuse_suspend() == 0) {
        spi_busy = 1;
        spi_flash_io_resume();
#endif
        if (flash_mutex_pend(&_norflash.mutex, 2) < 0) { //若flash忙，等待5*10ms；形参为0：死等
            log_error("flash read wait mutex fail!");
            reg = 0;
            goto __dev_bulk_read_exit2;
        }
        /* log_info("flash read sector = %d, num = %d\n", offset, len); */
        struct norflash_partition *part;
        part = (struct norflash_partition *)device->private_data;
        if (!part) {
            log_error("norflash partition invalid\n");
            reg = -EFAULT;
            goto __dev_bulk_read_exit1;
        }
        offset = offset * 512;
        len = len * 512;
        offset += part->start_addr;

#if FLASH_CACHE_ENABLE
        if (flash_read_use_cache == FLASH_READ_OCCUPY_CACHE) {
            reg = _norflash_read(offset, buf, len, 1);
        } else
#endif
        {
            reg = _norflash_read(offset, buf, len, 0);
        }
        if (reg) {
            r_printf(">>>[r error]:\n");
            reg = 0;
        } else {
            reg = len / 512;
        }
__dev_bulk_read_exit1:
        flash_mutex_post(&_norflash.mutex);
__dev_bulk_read_exit2:
#if(SPI_SD_IO_REUSE)
        spi_flash_io_suspend();
        spi_busy = 0;
        sd_io_reuse_resume();
    } else {
        log_error("sd io suspend fail(bulk read)!");
        reg = 0;
    }
#endif
    return reg;//len:success, other:fail
}
//写入前进行检查，非空会执行擦除（支持4k）
static int norflash_bulk_write(struct device *device, void *buf, u32 len, u32 offset)
{
    if (!len) {
        return 0;
    }

    int reg = 0;
#if NORFLASH_NO_SYS
    idle_cnt = 0;
#endif
#if(SPI_SD_IO_REUSE)
    if (sd_io_reuse_suspend() == 0) {
        spi_busy = 1;
        spi_flash_io_resume();
#endif
        if (flash_mutex_pend(&_norflash.mutex, 2) < 0) { //若flash忙，等待5*10ms；形参为0：死等
            log_error("flash write wait mutex fail!");
            reg = 0;
            goto __dev_bulk_read_exit2;
        }
        /* log_info("flash write sector = %d, num = %d\n", offset, len); */
        struct norflash_partition *part = device->private_data;
        if (!part) {
            log_error("norflash partition invalid\n");
            reg = -EFAULT;
            goto __dev_bulk_read_exit1;
        }
        offset = offset * 512;
        len = len * 512;
        offset += part->start_addr;

        reg = _norflash_write(offset, buf, len, 1);
        if (reg) {
            r_printf(">>>[w error]:\n");
            reg = 0;
        } else {
            reg = len / 512;
        }
__dev_bulk_read_exit1:
        flash_mutex_post(&_norflash.mutex);
__dev_bulk_read_exit2:
#if(SPI_SD_IO_REUSE)
        spi_flash_io_suspend();
        spi_busy = 0;
        sd_io_reuse_resume();
    } else {
        log_error("sd io suspend fail(bulk write)!");
        reg = 0;
    }
#endif
    return reg;//len:success, other:fail
}
static bool norflash_dev_online(const struct dev_node *node)
{
    return 1;
}

static int norflash_bulk_ioctl(struct device *device, u32 cmd, u32 arg)
{
    int err = 1;
#if(SPI_SD_IO_REUSE)
    if (sd_io_reuse_suspend() == 0) {
        spi_busy = 1;
        spi_flash_io_resume();
#endif
        struct norflash_partition *part = device->private_data;
        if (!part) {
            log_error("norflash partition invalid\n");
#if(SPI_SD_IO_REUSE)
            spi_flash_io_suspend();
            sd_io_reuse_resume();
            spi_busy = 0;
#endif
            return -EFAULT;
        }
        err = _norflash_ioctl(cmd, arg, 512, part);
#if(SPI_SD_IO_REUSE)
        spi_flash_io_suspend();
        spi_busy = 0;
        sd_io_reuse_resume();
    } else {
        log_error("sd io suspend fail(bulk ioctl)!");
    }
#endif
    return err;//0:success, other:fail
}


/*
 * 1. 外部调用时以512字节为单位的地址和长度，且需要驱动write自己处理擦除，
 * 请使用norflash_dev_ops接口，否则使用本文件内的其他ops
 *
 * 2. 如果不需要驱动自己处理擦除，可以把宏FLASH_CACHE_ENABLE清零，或者把
 * norflash_dev_read()里面调用的_norflash_read()的实参cache填0，
 * norflash_dev_write()同理
 *
 * 3. norflash_dev_ops可以被多个设备名注册，每个设备名被认为是不同分区，所以
 * 需要填不同的分区起始地址和大小，若分区地址有重叠或者最大分区结束地址大于
 * flash容量，会触发ASSERT()
 *
 * 4. 关于IOCTL_GET_CAPACITY，有多个分区注册时返回分区的大小，如果只注册了1
 * 个分区，分区起始地址 == 0时返回flash容量，起始地址 != 0时返回分区大小，
 * norflash_dev_ops返回的长度以512字节为单位
 *
 * 5. 本文件内的各个ops可以同时使用
 */
const struct device_operations norflash_dev_ops = {
    .init   = norflash_dev_init,
    .online = norflash_dev_online,
    .open   = norflash_dev_open,
    .read   = NULL,
    .write  = NULL,
    .bulk_read   = norflash_bulk_read, //按块(bulk)读取
    .bulk_write  = norflash_bulk_write,//按块写入，写前会检查非空则进行擦除（支持擦除单位为4k的flash）
    .ioctl  = norflash_bulk_ioctl,
    .close  = norflash_dev_close,
};



/****************************************************************************************/
static int norflash_byte_read(struct device *device, void *buf, u32 len, u32 offset)
{
    if (!len) {
        return 0;
    }
    int reg = 0;

#if NORFLASH_NO_SYS
    idle_cnt = 0;
#endif
#if(SPI_SD_IO_REUSE)
    if (sd_io_reuse_suspend() == 0) {
        spi_busy = 1;
        spi_flash_io_resume();
#endif
        if (flash_mutex_pend(&_norflash.mutex, 2) < 0) { //若flash忙，等待5*10ms；形参为0：死等
            log_error("flash read wait mutex fail!");
            reg = 0;
            goto __dev_byte_read_exit2;
        }
        /* log_info("flash read sector = %d, num = %d\n", offset, len); */
        struct norflash_partition *part;
        part = (struct norflash_partition *)device->private_data;
        if (!part) {
            log_error("norflash partition invalid\n");
            reg = -EFAULT;
            goto __dev_byte_read_exit1;
        }
        offset += part->start_addr;

        reg = _norflash_read(offset, buf, len, 0);
        if (reg) {
            r_printf(">>>[r error]:\n");
            reg = 0;
        } else {
            reg = len;
        }
__dev_byte_read_exit1:
        flash_mutex_post(&_norflash.mutex);
__dev_byte_read_exit2:
#if(SPI_SD_IO_REUSE)
        spi_flash_io_suspend();
        spi_busy = 0;
        sd_io_reuse_resume();
    } else {
        log_error("sd io suspend fail(byte read)!");
        reg = 0;
    }
#endif

    return reg;//len:success, other:fail
}
//写入前不进行检查不擦除
static int norflash_byte_write(struct device *device, void *buf, u32 len, u32 offset)
{
    if (!len) {
        return 0;
    }

    int reg = 0;
#if NORFLASH_NO_SYS
    idle_cnt = 0;
#endif
#if(SPI_SD_IO_REUSE)
    if (sd_io_reuse_suspend() == 0) {
        spi_busy = 1;
        spi_flash_io_resume();
#endif
        if (flash_mutex_pend(&_norflash.mutex, 2) < 0) { //若flash忙，等待5*10ms；形参为0：死等
            log_error("flash write wait mutex fail!");
            reg = 0;
            goto __dev_byte_read_exit2;
        }
        /* log_info("flash write sector = %d, num = %d\n", offset, len); */
        struct norflash_partition *part = device->private_data;
        if (!part) {
            log_error("norflash partition invalid\n");
            reg = -EFAULT;
            goto __dev_byte_read_exit1;
        }
        offset += part->start_addr;

        reg = _norflash_write(offset, buf, len, 0);
        if (reg) {
            r_printf(">>>[w error]:\n");
            reg = 0;
        } else {
            reg = len;
        }
__dev_byte_read_exit1:
        flash_mutex_post(&_norflash.mutex);
__dev_byte_read_exit2:
#if(SPI_SD_IO_REUSE)
        spi_flash_io_suspend();
        spi_busy = 0;
        sd_io_reuse_resume();
    } else {
        log_error("sd io suspend fail(byte write)!");
        reg = 0;
    }
#endif
    return reg;//len:success, other:fail
}
static int norflash_byte_ioctl(struct device *device, u32 cmd, u32 arg)
{
    int err = 1;
#if(SPI_SD_IO_REUSE)
    if (sd_io_reuse_suspend() == 0) {
        spi_busy = 1;
        spi_flash_io_resume();
#endif
        struct norflash_partition *part = device->private_data;
        if (!part) {
#if(SPI_SD_IO_REUSE)
            spi_flash_io_suspend();
            sd_io_reuse_resume();
            spi_busy = 0;
#endif
            log_error("norflash partition invalid\n");
            return -EFAULT;
        }
        err = _norflash_ioctl(cmd, arg, 1, part);
#if(SPI_SD_IO_REUSE)
        spi_flash_io_suspend();
        spi_busy = 0;
        sd_io_reuse_resume();
    } else {
        log_error("sd io suspend fail(byte ioctl)!");
    }
#endif
    return err;//0:success, other:fail
}

/*
 * 1. 外部调用时以1字节为单位的地址和长度，且驱动write自己不处理擦除，
 * 请使用norfs_dev_ops接口，否则使用本文件内的其他ops。注意：有些文件系
 * 统需要满足这个条件的驱动，如果期望修改成驱动内部处理擦除，需要测试所
 * 有关联的文件系统是否支持，或者新建一个符合需求的ops
 *
 * 2. 如果需要驱动自己处理擦除，需要把宏FLASH_CACHE_ENABLE置1，且
 * norfs_dev_read()里面调用的_norflash_read()的实参cache填1，
 * norfs_dev_write()同理
 *
 * 3. norfs_dev_ops可以被多个设备名注册，每个设备名被认为是不同分区，所以
 * 需要填不同的分区起始地址和大小，若分区地址有重叠或者最大分区结束地址大于
 * flash容量，会触发ASSERT()
 *
 * 4. 关于IOCTL_GET_CAPACITY，有多个分区注册时返回分区的大小，如果只注册了1
 * 个分区，分区起始地址 == 0时返回flash容量，起始地址 != 0时返回分区大小
 * norfs_dev_ops返回的长度以1字节为单位
 *
 * 5. 本文件内的各个ops可以同时使用
 */
const struct device_operations norfs_dev_ops = {
    .init   = norflash_dev_init,
    .online = norflash_dev_online,
    .open   = norflash_dev_open,
    .read   = norflash_byte_read,//按字节(byte)读取
    .write  = norflash_byte_write,//按字节写入，写前不进行检查与擦除
    .bulk_read   = NULL,
    .bulk_write  = NULL,
    .ioctl  = norflash_byte_ioctl,
    .close  = norflash_dev_close,
};






static u8 S25FL512_enter_4byte_addr(u32 id)
{
    u8 read_bank = 0;
    if ((((id >> 16) & 0xff) == 0x01)) { //S25FLxxx系列
        spi_cs_l();
        spi_write_byte(0x16);//读取Bank Register Read 寄存器，判断地址模式
        read_bank = spi_read_byte();
        spi_cs_h();
        _norflash_wait_ok();
        log_info("read_bank:%x\n", read_bank);
        if ((read_bank & 0x80) == 0) {
            spi_cs_l();
            spi_write_byte(0x17);//写入Bank Register Read 寄存器，
            spi_write_byte(read_bank | 0x80);//
            spi_cs_h();
            _norflash_wait_ok();
            spi_cs_l();
            spi_write_byte(0x16);//读取Bank Register Read 寄存器，判断地址模式
            read_bank = spi_read_byte();
            spi_cs_h();
            _norflash_wait_ok();
            log_info("read_bank:%x\n", read_bank);
            if ((read_bank & 0x80) == 0) {
                log_error("open 4byte addr fail!\n");
                return 1;
            }
        }
    }
    return 0;
}
static u8 _S25FLxxx_read_id()
{
    u8 id1[6], temp = 0;
    u8 reg = 0;
    spi_cs_l();
    spi_write_byte(WINBOND_JEDEC_ID);
    for (u8 i = 0; i < sizeof(id1); i++) {
        id1[i] = spi_read_byte();
    }
    spi_cs_h();
    log_info("flash: s25flxxx --------- id:%06x %06x\n", id1[0] << 16 | id1[1] << 8 | id1[2], id1[3] << 16 | id1[4] << 8 | id1[5]);
    if (id1[4] & 0xff) {
        log_info("s25flxxx Sector Architecture:4-KB parameter sectors with uniform 64-KB sectors, with a 256B programming buffer.\n");
    } else {
        log_info("s25flxxx Sector Architecture: Uniform 256-KB sectors, with a 512B programming buffer.\n");
    }
    reg = _norflash_wait_ok();
    if (reg) {
        log_error("wait flash id fail\n");
        return 1;
    }
    delay(1000);
    _norflash_send_write_enable();
    if (_norflash_wait_ok())while (1);
    spi_cs_l();
    spi_write_byte(0X01);//设置配置寄存器命令
    spi_write_byte(0X00);//设置状态寄存器1
    spi_write_byte(0X80);//设置配置寄存器  注意时钟频率需小于104Mhz
    spi_cs_h();
    reg = _norflash_wait_ok();
    if (reg) {
        log_error("wait flash fail\n");
        return 1;
    }
    delay(1000);
    spi_cs_l();
    spi_write_byte(0X35);//读取配置寄存器命令
    temp = spi_read_byte();
    spi_cs_h();
    if (temp == 0x80) {
        log_info("flash config_reg ok!\n");
        return 0;
    } else {
        log_error("flash config_reg fail!%x\n", temp);
        return 1;
    }
}







#if 0   //两种flash不能同时运行
extern const struct dev_node device_node_begin[];
extern const struct dev_node device_node_end[];
#define n_n 1
u8 w25q_read_buf[n_n * 512] = {0};
u8 w25q_write_buf[n_n * 512];
void norflash_test()
{
    u32 i = 0;
    struct device *device;
    for (i = 0; i < n_n * 512; i++) {
        w25q_write_buf[i] = i % 26 + 'a';
    }
    log_info("w25q_write_buf init_data:%s\n", w25q_write_buf);
    log_info("\ndevice_node_begin:  %x, device_node_end:%x\n", device_node_begin, device_node_end);
    devices_init();
    device = (struct device *)dev_open(__EXT_FLASH_NANE, 0);
    if (device == NULL) {
        log_error("w25q32 open fail!\n");
    } else {
        log_info("w25q32 open ok!\n");
#if 0  //块(512byte)操作测试---norflash_dev_ops

        log_info("flash dev bulk write/read test!");
        dev_bulk_write(device, w25q_write_buf, 0, n_n);
        delay(10000);
        memset(w25q_read_buf, 48, 512);
        log_info("w25q_read_buf_data:%s", w25q_read_buf);
        dev_bulk_read(device, w25q_read_buf, 0, n_n);
        log_info("w25q bulk read buf:");
        log_info_hexdump(w25q_read_buf, n_n * 512);/* log_info("w25q_read_buf:%s\n", w25q_read_buf);//字符打印 */

#else  //字节(1byte)操作测试---norfs_dev_ops
        log_char('\n');
        log_info("flash dev byte write/read test!");
        log_info("--------------flash sector erase(0)-------------");
        if (dev_ioctl(device, IOCTL_ERASE_SECTOR, 0)) { //返回0：擦出正常；1：擦出等待失败（等flash不忙失败）
            log_error("erase err\n");
            while (1);
        }
        log_info("after erase w25q_byte_read_buf:");
        dev_byte_read(device, w25q_read_buf, 0, n_n * 512);
        log_info_hexdump(w25q_read_buf, n_n * 512);
        dev_byte_write(device, w25q_write_buf, 0, n_n * 512);
        delay(10000);
        dev_byte_read(device, w25q_read_buf, 0, n_n * 512);
        log_info("after byte_write w25q_byte_read_buf:");/* log_info("w25q_read_buf:%s\n", w25q_read_buf); */
        /* log_info_hexdump(w25q_read_buf, n_n * 512); */
        log_info(":%s\n", w25q_read_buf);
#endif
    }
}
#endif
/*************************S25FL256************************/
#if 0  //两种flash不能同时运行
extern const struct dev_node device_node_begin[];
extern const struct dev_node device_node_end[];
#define srw_len 512
u8 s25_read_buf[srw_len] = {0};
u8 s25_write_buf[srw_len];
void s25_flash_test()
{
    u32 i = 0;
    u32 s25fl_start_addr = 0;
    u8 erase_sector = 0;
    struct device *device;

    for (i = 0; i < srw_len ; i++) {
        s25_write_buf[i] = i % 26 + 'a';
    }
    log_info("S25FL256_write_data:%s\n", s25_write_buf);
    log_info("\ndevice_node_begin:  %x, device_node_end:%x\n", device_node_begin, device_node_end);
    devices_init();
    device = (struct device *)dev_open("S25FL256", 0);
    if (device == NULL) {
        log_error("S25FL256 open fail!\n");
    } else {
        log_info("S25FL256 open ok!\n");
        s25fl_start_addr = 0x1020020;//32个4k 扇区：地址范围：0-0x20000;  510个64k扇区：地址范围：0x20000-0x1ffffff
        //手动擦出扇区
        if (s25fl_start_addr < 0x20000) {
            erase_sector = IOCTL_ERASE_SECTOR;//扇区为256k时没有擦除相关程序(如S25FL512)
        } else if (s25fl_start_addr < 0x1ffffff) {
            erase_sector = IOCTL_ERASE_BLOCK;
        } else while (1);
        //IOCTL_ERASE_SECTOR  IOCTL_ERASE_BLOCK  IOCTL_ERASE_CHIP //扇区为256k时没有擦除相关程序(如S25FL512)
        if (dev_ioctl(device, erase_sector, s25fl_start_addr)) { //返回0：擦出正常；1：擦出等待失败（等flash不忙）
            log_error("erase err\n");
            while (1);
        }
        dev_byte_read(device, s25_read_buf, s25fl_start_addr, srw_len);
        log_info("S25FL256_read_buf:%s\n", s25_read_buf);
        //写入
        dev_byte_write(device, s25_write_buf, s25fl_start_addr, srw_len);
        delay(10000);
        //读取
        /* log_info("S25FL256_read_buf:%s\n", s25_read_buf); */
        dev_byte_read(device, s25_read_buf, s25fl_start_addr, srw_len);
        log_info("S25FL256_read_buf:%s\n", s25_read_buf);
    }
}
#endif

#endif


