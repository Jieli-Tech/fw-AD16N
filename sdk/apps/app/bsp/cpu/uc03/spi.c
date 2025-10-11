#pragma bss_seg(".spi.data.bss")
#pragma data_seg(".spi.data")
#pragma const_seg(".spi.text.const")
#pragma code_seg(".spi.text")
#pragma str_literal_override(".spi.text.const")



#include "gpio.h"
#include "clock.h"
#include "spi.h"
#include "app_config.h"
#include "errno-base.h"

#define LOG_TAG_CONST   SPI
#define LOG_TAG         "[SPI]"
#include "log.h"

#define spi_enable(reg)                     ((reg)->CON |= BIT(0))
#define spi_disable(reg)                    ((reg)->CON &= ~BIT(0))
#define is_spi_open(reg)                    ((reg)->CON & BIT(0))
#define spi_role_slave(reg)                 ((reg)->CON |= BIT(1))
#define spi_role_master(reg)                ((reg)->CON &= ~BIT(1))
#define spi_cs_en(reg)                      ((reg)->CON |= BIT(2))
#define spi_cs_dis(reg)                     ((reg)->CON &= ~BIT(2))
#define spi_bidir(reg)                      ((reg)->CON |= BIT(3))
#define spi_unidir(reg)                     ((reg)->CON &= ~BIT(3))
#define spi_smp_edge_rise(reg)              ((reg)->CON &= ~BIT(4))
#define spi_smp_edge_fall(reg)              ((reg)->CON |= BIT(4))
#define spi_ud_edge_rise(reg)               ((reg)->CON &= ~BIT(5))
#define spi_ud_edge_fall(reg)               ((reg)->CON |= BIT(5))
#define spi_clk_idle_h(reg)                 ((reg)->CON |= BIT(6))
#define spi_clk_idle_l(reg)                 ((reg)->CON &= ~BIT(6))
#define spi_clk_idle_sel(reg, x)            ((reg)->CON = (reg->CON&~(1<<6))|((x)<<6))
#define spi_cs_idle_h(reg)                  ((reg)->CON |= BIT(7))
#define spi_cs_idle_l(reg)                  ((reg)->CON &= ~BIT(7))
#define spi_data_width(reg, x)              ((reg)->CON = (reg->CON&~(3<<10))|((x)<<10))
#define spi_dir(reg)                        ((reg)->CON & BIT(12))
#define spi_dir_in(reg)                     ((reg)->CON |= BIT(12))
#define spi_dir_out(reg)                    ((reg)->CON &= ~BIT(12))
#define spi_ie_en(reg)                      ((reg)->CON |= BIT(13))
#define spi_ie_dis(reg)                     ((reg)->CON &= ~BIT(13))
#define spi_clr_pnd(reg)                    ((reg)->CON |= BIT(14))
#define spi_pnd(reg)                        ((reg)->CON & BIT(15))

#define spi_w_reg_con(reg, val)             ((reg)->CON = (val))
#define spi_w_reg_con1(reg, val)            ((reg)->CON1 = (val))
#define spi_r_reg_con(reg)                  ((reg)->CON)
#define spi_r_reg_con1(reg)                 ((reg)->CON1)
#define spi_w_reg_buf(reg, val)             ((reg)->BUF = (val))
#define spi_r_reg_buf(reg)                  ((reg)->BUF)
#define spi_w_reg_baud(reg, baud)           ((reg)->BAUD = (baud))
#define spi_r_reg_baud(reg)                 ((reg)->BAUD)
#define spi_w_reg_dma_addr(reg, addr)       ((reg)->ADR  = (addr))
#define spi_w_reg_dma_cnt(reg, cnt)         ((reg)->CNT = (cnt))


#define spi_r_reg_dma_cnt(reg)              ((reg)->CNT)
/* #define spi_mix_mode_en(reg)                ((reg)->CON1 |= BIT(2)) */
/* #define spi_mix_mode_dis(reg)               ((reg)->CON1 &=~ BIT(2)) */
//con1:bit1,bit0
//     00:[7,6,5,4,3,2,1,0]
//     01:[0,1,2,3,4,5,6,7]
//     10:[3,2,1,0,7,6,5,4]
//     11:[4,5,6,7,0,1,2,3]
#define spi_bit_mode(reg, val)              SFR((reg)->CON1, 0, 2, val)


struct spi_platform_data spix_p_data_cache[HW_SPI_MAX_NUM];
static void (*hw_spi_irq_cbfun[HW_SPI_MAX_NUM])(hw_spi_dev spi, enum hw_spi_isr_status sta) = {0};
#if 0
struct spi_platform_data spix_p_data[HW_SPI_MAX_NUM] = {
    //spi0
    {0},
    //spi1
    {
        .port = {
            TCFG_HW_SPI1_PORT_CLK,//clk
            TCFG_HW_SPI1_PORT_DO, //do
            TCFG_HW_SPI1_PORT_DI, //di
            TCFG_HW_SPI1_PORT_D2, //d2
            TCFG_HW_SPI1_PORT_D3, //d3
            0xff,//cs
        },
        .role = TCFG_HW_SPI1_ROLE,
        .mode = TCFG_HW_SPI1_MODE,
        .bit_mode = SPI_FIRST_BIT_MSB,
        .cpol = 0,//clk level in idle state:0:low,  1:high
        .cpha = 0,//sampling edge:0:first,  1:second
        .clk  = TCFG_HW_SPI1_BAUD,
    },
#if SUPPORT_SPI2
    //no spi2
    {
        .port = {
            IO_PORTC_00, //clk any io
            IO_PORTC_01, //do  any io
            IO_PORTC_02, //di  any io
            0xff,//d2
            0xff,//d3
            0xff,//cs
        },
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE_BIDIR_1BIT,
        .bit_mode = SPI_FIRST_BIT_MSB,
        .cpol = 0,//clk level in idle state:0:low,  1:high
        .cpha = 0,//sampling edge:0:first,  1:second
        .clk  = 1000000L,
    }
#endif
};
#endif

static JL_SPI_TypeDef *const spi_regs[HW_SPI_MAX_NUM] = {
    JL_SPI0,
    JL_SPI1,
};

static void spi1_iomc_config(hw_spi_dev spi, enum spi_mode mode)
{
    u8 *port = spix_p_data_cache[spi].port;
    gpio_set_function(IO_PORT_SPILT(port[0]), PORT_FUNC_SPI1_CLK);
    gpio_set_function(IO_PORT_SPILT(port[1]), PORT_FUNC_SPI1_DA0);
    if (mode != SPI_MODE_UNIDIR_1BIT) {
        gpio_set_function(IO_PORT_SPILT(port[2]), PORT_FUNC_SPI1_DA1);
    }
    if (mode == SPI_MODE_UNIDIR_4BIT) {
        gpio_set_function(IO_PORT_SPILT(port[3]), PORT_FUNC_SPI1_DA2);
        gpio_set_function(IO_PORT_SPILT(port[4]), PORT_FUNC_SPI1_DA3);
    }
}

static void spi2_iomc_config(hw_spi_dev spi, enum spi_mode mode)
{
#if SUPPORT_SPI2
    u8 *port = spix_p_data_cache[spi].port;
    gpio_set_function(IO_PORT_SPILT(port[0]), PORT_FUNC_SPI2_CLK);
    gpio_set_function(IO_PORT_SPILT(port[1]), PORT_FUNC_SPI2_DA0);
    if (mode != SPI_MODE_UNIDIR_1BIT) {
        gpio_set_function(IO_PORT_SPILT(port[2]), PORT_FUNC_SPI2_DA1);
    }
    if (mode == SPI_MODE_UNIDIR_4BIT) {
        //spi2 no 4bit
        /* gpio_set_function(IO_PORT_SPILT(port[3]), PORT_FUNC_SPI2_DA2); */
        /* gpio_set_function(IO_PORT_SPILT(port[4]), PORT_FUNC_SPI2_DA3); */
    }
#endif
}

static void (*pSPI_IOMC_CONFIG[])(hw_spi_dev spi, enum spi_mode mode) = {
    NULL,
    spi1_iomc_config,
    spi2_iomc_config,
};


static void spi_io_port_init(u8 port, u8 dir)
{
    if (port != (u8) - 1) {
        gpio_set_mode(IO_PORT_SPILT(port), PORT_INPUT_FLOATING);
        if (dir == 0) {
            gpio_set_mode(IO_PORT_SPILT(port), PORT_OUTPUT_LOW);
        }
    }
}

static void spi_io_port_uninit(u8 port)
{
    if (port != (u8) - 1) {
        gpio_set_mode(IO_PORT_SPILT(port), PORT_HIGHZ);
    }
}
static void spi_io_crossbar_uninit(hw_spi_dev spi)
{
    u8 *port = spix_p_data_cache[spi].port;
    if (spi == HW_SPI1) { //spi1
        gpio_disable_function(IO_PORT_SPILT(port[0]), PORT_FUNC_SPI1_CLK);
        gpio_deinit(IO_PORT_SPILT(port[0]));
        gpio_disable_function(IO_PORT_SPILT(port[1]), PORT_FUNC_SPI1_DA0);
        gpio_deinit(IO_PORT_SPILT(port[1]));
        if (port[2] != (u8) - 1) {
            gpio_disable_function(IO_PORT_SPILT(port[2]), PORT_FUNC_SPI1_DA1);
            gpio_deinit(IO_PORT_SPILT(port[2]));
        }
        if (port[3] != (u8) - 1) {
            gpio_disable_function(IO_PORT_SPILT(port[3]), PORT_FUNC_SPI1_DA2);
            gpio_deinit(IO_PORT_SPILT(port[3]));
        }
        if (port[4] != (u8) - 1) {
            gpio_disable_function(IO_PORT_SPILT(port[4]), PORT_FUNC_SPI1_DA3);
            gpio_deinit(IO_PORT_SPILT(port[4]));
        }
    } else { //spi2

#if SUPPORT_SPI2
        gpio_disable_function(IO_PORT_SPILT(port[0]), PORT_FUNC_SPI2_CLK);
        gpio_deinit(IO_PORT_SPILT(port[0]));
        gpio_disable_function(IO_PORT_SPILT(port[1]), PORT_FUNC_SPI2_DA0);
        gpio_deinit(IO_PORT_SPILT(port[1]));
        if (port[2] != (u8) - 1) {
            gpio_disable_function(IO_PORT_SPILT(port[2]), PORT_FUNC_SPI2_DA1);
            gpio_deinit(IO_PORT_SPILT(port[2]));
        }
#endif
    }
}

/*
 * @brief 设置波特率
 * @parm spi  spi句柄
 * @parm baud  波特率
 * @return 0 成功，< 0 失败
 */
int spi_set_baud(hw_spi_dev spi, u32 baud)
{

    //SPICK = sysclk / (SPIx_BAUD + 1)
    //=> SPIx_BAUD = sysclk / SPICK - 1
    u32 sysclk;

    sysclk = clk_get("spi");
    log_debug("spi clock source freq %d", sysclk);
    if (sysclk < baud) {
        spi_w_reg_baud(spi_regs[spi], 0);
        return -EINVAL;
    }
    spi_w_reg_baud(spi_regs[spi], sysclk / baud - 1);
    spix_p_data_cache[spi].clk = baud;
    return 0;
}

/*
 * @brief 获取波特率
 * @parm spi  spi句柄
 * @return  波特率
 */
u32 spi_get_baud(hw_spi_dev spi)
{
    /* return clk_get("spi") / (spi_r_reg_baud(spi_regs[spi]) + 1);//spix_p_data_cache[spi].clk; */
    return spix_p_data_cache[spi].clk;
}


/*
 * @brief 设置spi[单向/双向，位数]模式
 * @parm spi  spi句柄
 * @parm mode  模式
 * @return null
 */
void spi_set_bit_mode(hw_spi_dev spi, enum spi_mode mode)
{
    u8 *port = spix_p_data_cache[spi].port;
    enum spi_role role = spix_p_data_cache[spi].role;

    switch (mode) {
    case SPI_MODE_BIDIR_1BIT:
        spi_bidir(spi_regs[spi]);
        spi_data_width(spi_regs[spi], 0);
        break;
    case SPI_MODE_UNIDIR_1BIT:
        spi_unidir(spi_regs[spi]);
        spi_data_width(spi_regs[spi], 0);
        break;
    case SPI_MODE_UNIDIR_2BIT:
        spi_unidir(spi_regs[spi]);
        spi_data_width(spi_regs[spi], 1);
        break;
    case SPI_MODE_UNIDIR_4BIT:
#if SUPPORT_SPI2
        if (spi == HW_SPI2) {
            log_error("spi2 have no unidir_4bit mode!");
        } else
#endif
        {
            spi_unidir(spi_regs[spi]);
            spi_data_width(spi_regs[spi], 2);
        }
        break;
    }

    pSPI_IOMC_CONFIG[spi](spi, mode);
    if (port[5] != (u8) - 1) {
        gpio_set_mode(IO_PORT_SPILT(port[5]), role == SPI_ROLE_MASTER ? PORT_OUTPUT_HIGH : PORT_INPUT_PULLUP_10K); //cs io
    }
    spi_io_port_init(port[0], role == SPI_ROLE_MASTER ? 0 : 1);
    spi_io_port_init(port[1], role == SPI_ROLE_MASTER ? 0 : 1);
    if (mode == SPI_MODE_BIDIR_1BIT) {
        spi_io_port_init(port[2], 1);
    } else if (mode == SPI_MODE_UNIDIR_2BIT) {
        spi_io_port_init(port[2], role == SPI_ROLE_MASTER ? 0 : 1);
    } else if (mode == SPI_MODE_UNIDIR_4BIT) {
        spi_io_port_init(port[2], role == SPI_ROLE_MASTER ? 0 : 1);
        spi_io_port_init(port[3], role == SPI_ROLE_MASTER ? 0 : 1);
        spi_io_port_init(port[4], role == SPI_ROLE_MASTER ? 0 : 1);
    }
    spix_p_data_cache[spi].mode = mode;
}


extern const struct spi_platform_data spix_p_data[HW_SPI_MAX_NUM];
struct spi_platform_data *get_hw_spi_config(hw_spi_dev spi)
{
    return (struct spi_platform_data *)&spix_p_data[spi];
}

/*
 * @brief 打开spi
 * @parm spi  spi句柄
 * @return 0 成功，< 0 失败
 */
void hw_spi1_isr();
void hw_spi2_isr();
int spi_open(hw_spi_dev spi, spi_hardware_info *spi_info)
{
    if (spi_info == NULL) {
        return -2;
    }
    if (spi >= HW_SPI_MAX_NUM) {
        log_error("hw spi index:%d error!", spi);
        return -3;
    }
    int err;
    memcpy(&spix_p_data_cache[spi], spi_info, sizeof(spi_hardware_info));

    spi_w_reg_con(spi_regs[spi], BIT(14));
    spi_set_bit_mode(spi, spi_info->mode);
    spi_cs_dis(spi_regs[spi]);
    spi_cs_idle_h(spi_regs[spi]);

    spi_clk_idle_sel(spi_regs[spi], spi_info->cpol);
    if (spi_info->cpol == spi_info->cpha) {
        spi_smp_edge_rise(spi_regs[spi]);
        spi_ud_edge_fall(spi_regs[spi]);
    } else {
        spi_smp_edge_fall(spi_regs[spi]);
        spi_ud_edge_rise(spi_regs[spi]);
    }
    if ((err = spi_set_baud(spi, spi_info->clk))) {
        log_error("invalid spi baudrate");
        /* return 0; */
    }
    spi_bit_mode(spi_regs[spi], spi_info->bit_mode);
    if (spi_info->role == SPI_ROLE_MASTER) {
        spi_role_master(spi_regs[spi]);
        /* spi_w_reg_buf(spi_regs[spi], 0);//设定spi初始化后DO口默认电平为低 */
    } else if (spi_info->role == SPI_ROLE_SLAVE) {
        spi_dir_in(spi_regs[spi]);
        spi_role_slave(spi_regs[spi]);
        /* spi_w_reg_buf(spi_regs[spi], 0xff); */
    }

    if (spi_info->ie_en == 1) {
        spi_set_ie(spi, 1);
        if (spi_info->spi_isr_callback) {
            hw_spi_irq_cbfun[spi] = spi_info->spi_isr_callback;
        }
        if (spi == HW_SPI1) {
            request_irq(IRQ_SPI1_IDX, HW_SPI_IRQ_PRIORITY, hw_spi1_isr, 0);//7:配置中断优先级，中断函数
#if SUPPORT_SPI2
        } else if (spi == HW_SPI2) {
            request_irq(IRQ_SPI2_IDX, HW_SPI_IRQ_PRIORITY, hw_spi2_isr, 0);//7:配置中断优先级，中断函数
#endif
        }
    }
    spi_clr_pnd(spi_regs[spi]);
    spi_enable(spi_regs[spi]);
    spi_clr_pnd(spi_regs[spi]);
#if 0
    u8 *port = spi_info->port;
    log_debug("spi%d clk     = %d", spi, spi_info->clk);
    log_debug("spi%d mode    = %d", spi, spi_info->mode);
    log_debug("spi%d role    = %d", spi, spi_info->role);
    log_debug("spi%d clk_pin = %d", spi, port[0]);
    log_debug("spi%d do_pin  = %d", spi, port[1]);
    log_debug("spi%d di_pin  = %d", spi, port[2]);
    log_debug("spi%d di_pin  = %d", spi, port[3]);
    log_debug("spi%d di_pin  = %d", spi, port[4]);
    log_debug("spi%d cs_pin  = %d", spi, port[5]);
    log_debug("spi%d CON     = %04x", spi, spi_r_reg_con(spi_regs[spi]));
    log_debug("spi%d CON1    = %04x", spi, spi_r_reg_con1(spi_regs[spi]));
    log_debug("spi%d BAUD    = 0x%08x", spi, spi_r_reg_baud(spi_regs[spi]));
    log_debug("spi%d dma-adr = 0x%08x", spi, (spi_regs[spi])->ADR);
    log_debug("spi%d dma-cnt = 0x%08x", spi, spi_r_reg_dma_cnt(spi_regs[spi]));
#endif
    return 0;
}

/*
 * @brief 关闭spi
 * @parm spi  spi句柄
 * @return null
 */
void spi_close(hw_spi_dev spi)
{
    if (!is_spi_open(spi_regs[spi])) {
        return;
    }

    u8 *port = spix_p_data_cache[spi].port;
    spi_io_port_uninit(port[0]);
    spi_io_port_uninit(port[1]);
    spi_io_port_uninit(port[2]);
    spi_io_port_uninit(port[3]);
    spi_io_port_uninit(port[4]);
    spi_io_port_uninit(port[5]);
    spi_io_crossbar_uninit(spi);
    if (spix_p_data_cache[spi].ie_en) {
        hw_spi_irq_cbfun[spi] = NULL;
        if (spi == HW_SPI1) {
            bit_clr_ie(IRQ_SPI1_IDX);
#if SUPPORT_SPI2
        } else if (spi == HW_SPI2) {
            bit_clr_ie(IRQ_SPI2_IDX);
#endif
        }
    }
    spi_disable(spi_regs[spi]);
    memset((u8 *)&spix_p_data_cache[spi], 0, sizeof(spi_hardware_info));
}

void spi_suspend(hw_spi_dev spi)
{
    if (!is_spi_open(spi_regs[spi])) {
        return;
    }

    u8 *port = spix_p_data_cache[spi].port;
    spi_io_port_uninit(port[0]);
    spi_io_port_uninit(port[1]);
    spi_io_port_uninit(port[2]);
    spi_io_port_uninit(port[3]);
    spi_io_port_uninit(port[4]);
    spi_io_port_uninit(port[5]);
    spi_io_crossbar_uninit(spi);
    spi_disable(spi_regs[spi]);
}

void spi_resume(hw_spi_dev spi)
{
    spi_set_bit_mode(spi, spix_p_data_cache[spi].mode);
    spi_enable(spi_regs[spi]);
}

static int __spi_wait_ok(hw_spi_dev spi, u32 n)
{
    /* u32 baud = spi_get_info_clock(spi); */
    /* baud = clk_get("spi") / baud - 1; */
    u32 baud = spi_r_reg_baud(spi_regs[spi]) + 1;
    u32 retry = baud * (clk_get("sys") / clk_get("spi")) * 8 * n * 5;  //500% spi baudate

    while (!spi_pnd(spi_regs[spi])) {
        __asm__ volatile("nop");
        if (--retry == 0) {
            log_error("spi wait pnd timeout");
#if HW_SPI_MASTER_CS_EN
            gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_HIGH);//cs io
#endif
            return -EFAULT;
        }
    }
    spi_clr_pnd(spi_regs[spi]);
#if HW_SPI_MASTER_CS_EN
    gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_HIGH);//cs io
#endif
    return 0;
}

/*
 * @brief 发送1个字节
 * @parm spi  spi句柄
 * @parm byte 发送的字节
 * @return 0 成功，< 0 失败
 */
int spi_send_byte(hw_spi_dev spi, u8 byte)
{
    if (!is_spi_open(spi_regs[spi])) {
        return -1;
    }

#if HW_SPI_MASTER_CS_EN
    gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_LOW);//cs io
#endif
    spi_dir_out(spi_regs[spi]);
    spi_w_reg_buf(spi_regs[spi], byte);
    return __spi_wait_ok(spi, 1);
}

/*
 * @brief 接收1个字节
 * @parm spi  spi句柄
 * @parm err  返回错误信息，若err为非空指针，0 成功，< 0 失败，若为空指针，忽略
 * @return 接收的字节
 */
u8 spi_recv_byte(hw_spi_dev spi, int *err)
{
    int ret;
    if (!is_spi_open(spi_regs[spi])) {
        return 0;
    }

#if HW_SPI_MASTER_CS_EN
    gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_LOW);//cs io
#endif
    spi_dir_in(spi_regs[spi]);
    spi_w_reg_buf(spi_regs[spi], 0xff);
    ret = __spi_wait_ok(spi, 1);
    if (ret) {
        err != NULL ? *err = ret : 0;
        return 0;
    }
    err != NULL ? *err = 0 : 0;
    return spi_r_reg_buf(spi_regs[spi]);
}


/*
 * @brief 发送并接收1个字节，在8个时钟内完成，仅使用于SPI_MODE_BIDIR_1BIT
 * @parm spi  spi句柄
 * @parm byte  发送的字节
 * @parm err  返回错误信息，若err为非空指针，0 成功，< 0 失败，若为空指针，忽略
 * @return 接收的字节
 */
u8 spi_send_recv_byte(hw_spi_dev spi, u8 byte, int *err)
{
    int ret;
    if (!is_spi_open(spi_regs[spi])) {
        return 0;
    }

#if HW_SPI_MASTER_CS_EN
    gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_LOW);//cs io
#endif
    spi_w_reg_buf(spi_regs[spi], byte);
    ret = __spi_wait_ok(spi, 1);
    if (ret) {
        err != NULL ? *err = ret : 0;
        return 0;
    }
    err != NULL ? *err = 0 : 0;
    return spi_r_reg_buf(spi_regs[spi]);
}

/*
 * @brief spi dma接收
 * @parm spi  spi句柄
 * @parm buf  接收缓冲区基地址
 * @parm len  期望接收长度
 * @return 实际接收长度，< 0表示失败
 */
int spi_dma_recv(hw_spi_dev spi, void *buf, u32 len)
{
    if (!is_spi_open(spi_regs[spi])) {
        return -1;
    }

    /* ASSERT((u32)buf % 4 == 0, "spi dma addr need 4-aligned"); */
#if HW_SPI_MASTER_CS_EN
    gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_LOW);//cs io
#endif
    spi_dir_in(spi_regs[spi]);
    spi_w_reg_dma_addr(spi_regs[spi], (u32)buf);
    spi_w_reg_dma_cnt(spi_regs[spi], len);
    asm("csync");
    if (__spi_wait_ok(spi, len)) {
        return -EFAULT;
    }
    return len;
}

/*
 * @brief spi dma发送
 * @parm spi  spi句柄
 * @parm buf  发送缓冲区基地址
 * @parm len  期望发送长度
 * @return 实际发送长度，< 0表示失败
 */
int spi_dma_send(hw_spi_dev spi, const void *buf, u32 len)
{
    if (!is_spi_open(spi_regs[spi])) {
        return -1;
    }

    /* ASSERT((u32)buf % 4 == 0, "spi dma addr need 4-aligned"); */
#if HW_SPI_MASTER_CS_EN
    gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_LOW);//cs io
#endif
    spi_dir_out(spi_regs[spi]);
    spi_w_reg_dma_addr(spi_regs[spi], (u32)buf);
    spi_w_reg_dma_cnt(spi_regs[spi], len);
    asm("csync");
    if (__spi_wait_ok(spi, len)) {
        return -EFAULT;
    }
    return len;
}

/*
 * @brief 中断使能
 * @parm spi  spi句柄
 * @parm en  1 使能，0 失能
 * @return null
 */
void spi_set_ie(hw_spi_dev spi, u8 en)
{
    en ? spi_ie_en(spi_regs[spi]) : spi_ie_dis(spi_regs[spi]);
    spix_p_data_cache[spi].ie_en = en;
}

/*
 * @brief 判断中断标志
 * @parm spi  spi句柄
 * @return 0 / 1
 */
u8 spi_get_pending(hw_spi_dev spi)
{
    if (!is_spi_open(spi_regs[spi])) {
        return 0;
    }

    return spi_pnd(spi_regs[spi]) ? 1 : 0;
}

/*
 * @brief 清除中断标志
 * @parm spi  spi句柄
 * @return null
 */
void spi_clear_pending(hw_spi_dev spi)
{
    if (!is_spi_open(spi_regs[spi])) {
        return;
    }

    spi_clr_pnd(spi_regs[spi]);
}
void spi_disable_for_ota()
{
    for (int i = 0; i < 2; i++) {
        spi_disable(spi_regs[i]);
    }
}



/*************************中断接口：无cs，适用主,从************************/

/*
 * @brief 发送1个字节，不等待pnd，用于中断,无cs, 适用主从
 * @parm spi  spi句柄
 * @parm tx_byte 发送的数据，rw=rx时无效
 * @parm rx_byte 接收的数据的地址，没有时给NULL
 * @parm rw :1-rx; 0-tx
 * @return null
 */
static volatile u8 *rec_data_byte[HW_SPI_MAX_NUM] = {0};
static volatile int spi_isr_data_len[HW_SPI_MAX_NUM] = {0};
AT(.spi.text.cache.L1)
void spi_byte_transmit_for_isr(hw_spi_dev spi, u8 tx_byte, u8 *rx_byte, u8 rw)//rw:1-rx; 0-tx
{
    if (!is_spi_open(spi_regs[spi])) {
        return;
    }
    if ((rw == 1) && (rx_byte == NULL)) {
        return;
    }

#if HW_SPI_MASTER_CS_EN
    if (spix_p_data_cache[spi].role == SPI_ROLE_MASTER) {
        gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_LOW);//cs io
    }
#endif
    rec_data_byte[spi] = rx_byte;
    /* spi_isr_data_len[spi] = 0; */
    if (rw) {
        spi_dir_in(spi_regs[spi]);
        spi_w_reg_buf(spi_regs[spi], 0xff);
    } else {
        spi_dir_out(spi_regs[spi]);
        spi_w_reg_buf(spi_regs[spi], tx_byte);
    }
}

#if 1//中断byte地址自增
/*
 * @brief 发送n个字节，不等待pnd，用于中断,无cs, 适用主从
 * @parm spi  spi句柄
 * @parm tx_buf 发送的buf的地址，没有时给NULL
 * @parm rx_buf 接收的buf的地址，没有时给NULL
 * @parm len:接收或发送的长
 * @parm rw :1-rx; 0-tx
 * @return null
 */
static volatile u8 *send_data_byte[HW_SPI_MAX_NUM] = {0};
static volatile int isr_addr_inc[HW_SPI_MAX_NUM] = {0};
AT(.spi.text.cache.L1)
void spi_buf_transmit_for_isr(hw_spi_dev spi, u8 *tx_buf, u8 *rx_buf, int len, u8 rw)//rw:1-rx; 0-tx
{
    if (!is_spi_open(spi_regs[spi])) {
        return;
    }
    if (len == 0) {
        return;
    }
    if ((rw == 1) && (rx_buf == NULL)) {
        return;
    }
    if ((rw == 0) && (tx_buf == NULL)) {
        return;
    }

#if HW_SPI_MASTER_CS_EN
    if (spix_p_data_cache[spi].role == SPI_ROLE_MASTER) {
        gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_LOW);//cs io
    }
#endif
    rec_data_byte[spi] = rx_buf;
    send_data_byte[spi] = tx_buf;
    isr_addr_inc[spi] = len - 1;
    /* spi_isr_data_len[spi] = 0; */
    if (rw) {
        spi_dir_in(spi_regs[spi]);
        spi_w_reg_buf(spi_regs[spi], 0xff);
    } else {
        spi_dir_out(spi_regs[spi]);
        spi_w_reg_buf(spi_regs[spi], *tx_buf);
    }
}
//通信未达到len提前结束,需调用该函数
void hw_spix_clear_buf_isr_addr_inc(hw_spi_dev spi)
{
    isr_addr_inc[spi] = 0;
}
#endif
/*
 * @brief spi 配置dma，不等待pnd，用于中断,无cs, 适用主从
 * @parm spi  spi句柄
 * @parm buf  缓冲区基地址
 * @parm len  期望长度
 * @parm rw  1 接收 / 0 发送
 * @return null
 */
AT(.spi.text.cache.L1)
void spi_dma_transmit_for_isr(hw_spi_dev spi, void *buf, int len, u8 rw)//rw:1-rx; 0-tx
{
    if (!is_spi_open(spi_regs[spi])) {
        return;
    }
    if ((len == 0) || (buf == NULL)) {
        return;
    }

#if HW_SPI_MASTER_CS_EN
    if (spix_p_data_cache[spi].role == SPI_ROLE_MASTER) {
        gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_LOW);//cs io
    }
#endif
    spi_isr_data_len[spi] = -len;
    /* ASSERT((u32)buf % 4 == 0, "spi dma addr need 4-aligned"); */
    rw ? spi_dir_in(spi_regs[spi]) : spi_dir_out(spi_regs[spi]);
    spi_w_reg_dma_addr(spi_regs[spi], (u32)buf);
    spi_w_reg_dma_cnt(spi_regs[spi], len);
}




volatile enum hw_spi_isr_status spi_isr_txrx_state[HW_SPI_MAX_NUM] = {0};
/*中断函数*/
AT(.spi.text.cache.L1)
static void hw_spix_isr_func(hw_spi_dev spi)
{
    /* putchar('i'); */
    spi_isr_txrx_state[spi] = SPI_PND_ERROR;
    if (spi_pnd(spi_regs[spi])) { //byte收发，DMA收发
        if (rec_data_byte[spi]) {
            *rec_data_byte[spi] = spi_r_reg_buf(spi_regs[spi]);
            if (isr_addr_inc[spi]) { //inc
                rec_data_byte[spi]++;//inc
                spi_w_reg_buf(spi_regs[spi], 0xff);
            }//inc
        }
        spi_isr_txrx_state[spi] = SPI_WAITING_PND;
        if (spi_dir(spi_regs[spi])) { //rx:byte/dma
            if (isr_addr_inc[spi] == 0) { //inc
                spi_isr_txrx_state[spi] = SPI_RX_FINISH;
            } else { //inc
                isr_addr_inc[spi]--;//inc
            }//inc
        } else {//tx:byte/dma
            if ((isr_addr_inc[spi]) && (send_data_byte[spi])) { //inc
                isr_addr_inc[spi]--;
                send_data_byte[spi]++;
                spi_w_reg_buf(spi_regs[spi], *send_data_byte[spi]);
            } else {
                spi_dir_in(spi_regs[spi]);
                spi_isr_txrx_state[spi] = SPI_TX_FINISH;
            }
        }
        spi_clr_pnd(spi_regs[spi]);
#if HW_SPI_MASTER_CS_EN
        if (spix_p_data_cache[spi].role == SPI_ROLE_MASTER) {
            gpio_set_mode(IO_PORT_SPILT(spix_p_data_cache[spi].port[5]), PORT_OUTPUT_HIGH);//cs io
        }
#endif
        if (spi_isr_data_len[spi] < 0) {
            spi_isr_data_len[spi] += spi_r_reg_dma_cnt(spi_regs[spi]);
        } else {
            spi_isr_data_len[spi]++;
        }
        /* log_noinfo("%d,%d,%d.\t", isr_addr_inc[spi], spi_isr_txrx_state[spi], spi_isr_data_len[spi]); */
        if (hw_spi_irq_cbfun[spi]) {
            hw_spi_irq_cbfun[spi](spi, spi_isr_txrx_state[spi]);
        }
    }
    if (JL_SPI1->CON & BIT(21)) { //slave rx dma overflow
        JL_SPI1->CON &= ~BIT(20); //clr
        log_error("spi slave rx dma overflow");
    }
    if (JL_SPI1->CON & BIT(18)) { //slave tx dma underflow
        JL_SPI1->CON &= ~BIT(17); //clr
        log_error("spi slave tx dma underflow");
    }
}

//返回spi中断状态isr_status:见枚举
AT(.spi.text.cache.L1)
enum hw_spi_isr_status hw_spix_get_isr_status(hw_spi_dev spi)
{
    enum hw_spi_isr_status ret = spi_isr_txrx_state[spi];
    if (ret) {
        spi_isr_txrx_state[spi] = 0;
    }
    return ret;
}
//每包数据前清空,在通信中间调用,会导致数据长不准
void hw_spix_clear_isr_len(hw_spi_dev spi)
{
    /* if (spi_isr_data_len[spi] < 0) {//except dma */
    /*     return; */
    /* } */
    spi_isr_data_len[spi] = 0;
}

//返回中断通信数据长度 带符号
//    符号表示dma(-)还是byte(+)
//    数据表示长度
//    结合isr_status可知是接收还是发送
int hw_spix_get_isr_len(hw_spi_dev spi)
{
    return spi_isr_data_len[spi];
}

//spi从机不开中断dma模式
//返回dma通信数据长度(当前已传输个数)
//    符号表示dma(-)
//    数据表示长度
//    不能区分收发
int hw_spix_slave_get_dma_len(hw_spi_dev spi)
{
    int ret = spi_isr_data_len[spi] + spi_r_reg_dma_cnt(spi_regs[spi]);
    if (spi_dir(spi_regs[spi])) {
    } else {
        if (spi_r_reg_dma_cnt(spi_regs[spi])) {
            ret += 4;//从机dma发送,dma cnt没结束前读取会多出4byte
        }
    }
    return ret;
}

void hw_spix_irq_change_callback(hw_spi_dev spi, void (*spi_isr_callback_f)(hw_spi_dev spi, enum hw_spi_isr_status sta))
{
    hw_spi_irq_cbfun[spi] = spi_isr_callback_f;
    spix_p_data_cache[spi].spi_isr_callback = spi_isr_callback_f;
}
AT(.spi.text.cache.L1)
__attribute__((interrupt("")))
void hw_spi1_isr()
{
    /* if (spix_p_data_cache[1].role == SPI_ROLE_MASTER) {//master */
    /* } */
    hw_spix_isr_func(1);
}
AT(.spi.text.cache.L1)
__attribute__((interrupt("")))
void hw_spi2_isr()
{
    /* if (spix_p_data_cache[2].role == SPI_ROLE_MASTER) {//master */
    /* } */
    hw_spix_isr_func(2);
}





/*************************slave ************************/
/* AT(.spi.text.cache.L1) */
/* void spi_dma_set_addr_for_slave(hw_spi_dev spi, void *buf, u32 len, u8 rw)//rw:1-rx; 0-tx */
/* { */
/*     rw ? spi_dir_in(spi_regs[spi]) : spi_dir_out(spi_regs[spi]); */
/*     spi_w_reg_dma_addr(spi_regs[spi], (u32)buf); */
/*     spi_w_reg_dma_cnt(spi_regs[spi], len); */
/* } */
/* AT(.spi.text.cache.L1) */
/* void spi_send_recv_byte_for_slave(hw_spi_dev spi, u8 *byte, u8 rw)//rw:1-rx; 0-tx */
/* { */
/*     if (rw) { */
/*         spi_dir_in(spi_regs[spi]); */
/*         rec_data_byte[spi] = byte; */
/*     } else { */
/*         spi_dir_out(spi_regs[spi]); */
/*         spi_w_reg_buf(spi_regs[spi], *byte); */
/*     } */
/* } */

