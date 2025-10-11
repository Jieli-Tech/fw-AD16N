#include "iic_hw.h"
#include "clock.h"
#include "app_config.h"
#include "gpio.h"

#define LOG_TAG_CONST       IIC
#define LOG_TAG             "[iic_hw]"
#include "log.h"

#ifndef     TCFG_IIC_SCL_IO
#define     TCFG_IIC_SCL_IO      IO_PORTA_04
#endif
#ifndef     TCFG_IIC_SDA_IO
#define     TCFG_IIC_SDA_IO      IO_PORTA_07
#endif

//硬件IIC设备数据初始化
#define IIC_CLOCK  clk_get("lsb");
#define EINVAL 22

static u8 hw_iic_state[MAX_HW_IIC_NUM] = {0};//bit7:init ok, bit6:suspend, bit5:role:1:slave
static struct iic_master_config hw_iic_cfg_cache[MAX_HW_IIC_NUM];

static JL_IIC_TypeDef *const iic_regs[IIC_HW_NUM] = {
    JL_IIC0,
};

/* spinlock_t iic_lock[MAX_HW_IIC_NUM]; */
#define IIC_HW_ENTER_CRITICAL() //spin_lock(&iic_lock[iic])
#define IIC_HW_EXIT_CRITICAL() //spin_unlock(&iic_lock[iic])

static int iic_port_init(hw_iic_dev iic)
{
    int ret = 0;
    enum gpio_port scl_port = hw_iic_cfg_cache[iic].scl_io / 16;
    u32 scl_pin = BIT(hw_iic_cfg_cache[iic].scl_io % 16);
    enum gpio_port sda_port = hw_iic_cfg_cache[iic].sda_io / 16;
    u32 sda_pin = BIT(hw_iic_cfg_cache[iic].sda_io % 16);
    if (iic == 0) {
        gpio_set_function(scl_port, scl_pin, PORT_FUNC_IIC0_SCL);
        gpio_set_function(sda_port, sda_pin, PORT_FUNC_IIC0_SDA);

        struct gpio_config gpio_info;
        gpio_info.pin = scl_pin;
        gpio_info.mode = hw_iic_cfg_cache[iic].io_mode;
        gpio_info.hd = hw_iic_cfg_cache[iic].hdrive;
        gpio_init(scl_port, &gpio_info);
        gpio_info.pin = sda_pin;
        gpio_init(sda_port, &gpio_info);
    } else {
        ret = IIC_ERROR_INDEX_ERROR;
    }
    return ret;
}

int hw_iic_set_baud(hw_iic_dev iic, u32 baud)
{
    //f_iic = f_sys / ((IIC_BAUD + 1) * 2)+1
    //=> IIC_BAUD = f_sys / (2 * (f_iic-1)) - 1
    u32 sysclk;
    /* JL_CLOCK->CLK_CON1 |= BIT(13); */
    /* delay(100); */
    /* JL_CLOCK->CLK_CON1 |= BIT(14); */

    sysclk = IIC_CLOCK;//clk_get("lsb");//16M
    if (sysclk < 2 * baud) {
        return IIC_ERROR_FREQUENCY_ERROR;
    }
    /* log_info("baud_reg:0x%x",sysclk / (2 * (baud-1)) - 1); */
    iic_baud_reg(iic_regs[iic]) = sysclk / (2 * (baud - 1)) - 1;
    return 0;
}


extern const struct iic_master_config hw_iic_cfg_const[MAX_HW_IIC_NUM];
struct iic_master_config *get_hw_iic_config(hw_iic_dev iic)
{
    return (struct iic_master_config *)&hw_iic_cfg_const[iic];
}
void hw_iic_isr();
enum iic_state_enum hw_iic_init(hw_iic_dev iic, struct iic_master_config *i2c_config)
{
    if (i2c_config == NULL) {
        log_error("hw iic%d param error!", iic);
        return IIC_ERROR_PARAM_ERROR;
    }

    if (iic >= MAX_HW_IIC_NUM) {
        log_error("hw iic index:%d error!", iic);
        return IIC_ERROR_INDEX_ERROR;
    }
    if ((hw_iic_state[iic]&BIT(7)) != 0) {
        log_error("hw iic%d has been occupied!", iic);
        return IIC_ERROR_INIT_FAIL;
    }
    memcpy(&hw_iic_cfg_cache[iic], i2c_config, sizeof(struct iic_master_config));

    int ret = iic_port_init(iic);
    if (ret) {
        log_error("invalid hardware iic port\n");
        return ret;
    }
    iic_reset0(iic_regs[iic]);
    if (i2c_config->role == IIC_MASTER) {
        iic_role_host(iic_regs[iic]);
        ret = hw_iic_set_baud(iic, i2c_config->master_frequency);
        if (ret) {
            log_error("iic baudrate is invalid\n");
            return ret ;
        }
        /* } else { */
        /*     iic_role_slave(iic_regs[iic]); */
        /* hw_iic_slave_set_addr(iic, iic_info_slave_addr(iic), 1);//0:不自动响应起始位,1:响应 */
        /* iic_recv_nack(iic_regs[iic]); */
        /* iic_buf_reg(iic_regs[iic]) = 0xff; */
        /*     iic_si_mode_en(iic_regs[iic]); */
    }
    /* iic_isel_filter_en(iic_regs[iic]);//en filter(only br27) */
    /* if (i2c_config->io_filter) { */
    /*     iic_isel_filter(iic_regs[iic]); */
    /* } else { */
    /*     iic_isel_direct(iic_regs[iic]); */
    /* } */

    if (i2c_config->ie_en) {
        /* request_irq(IRQ_IIC_IDX, HW_IIC0_IRQ_PRIORITY, hw_iic_isr, 0); */
        HWI_Install(IRQ_IIC_IDX, (u32)hw_iic_isr, HW_IIC0_IRQ_PRIORITY);//0: 中断优先级
        iic_set_ie(iic_regs[iic]);
        iic_set_end_ie(iic_regs[iic]);
        /* if (i2c_config->iic_slave_irq_callback) {//slave */
        /*     iic_slave_irq_cbfun = i2c_config->iic_slave_irq_callback; */
        /* } else { */
        /*     log_error("iic slave irq_callback is invalid\n"); */
        /* } */
    } else {
        iic_dis_ie(iic_regs[iic]);
        iic_dis_end_ie(iic_regs[iic]);
    }
    iic_pnd_clr(iic_regs[iic]);
    iic_end_pnd_clr(iic_regs[iic]);
    iic_start_pnd_clr(iic_regs[iic]);
    hw_iic_state[iic] = BIT(7);//bit7:init ok, bit5:role:0:master
    iic_enable(iic_regs[iic]);

    /* if (i2c_config->role == IIC_SLAVE) { */
    /*     iic_recv_nack(iic_regs[iic]); */
    /*     iic_dir_in(iic_regs[iic]); */
    /*     iic_buf_reg(iic_regs[iic]) = 0xff; */
    /*     iic_cfg_done(iic_regs[iic]); */
    /* } */
    iic_pnd_clr(iic_regs[iic]);
#if 0
    log_info("info->scl = %d", i2c_config->scl_io);
    log_info("info->sda = %d", i2c_config->sda_io);
    log_info("info->baudrate = %d", i2c_config->master_frequency);
    log_info("info->hdrive = %d", i2c_config->hdrive);
    log_info("info->io_filter = %d", i2c_config->io_filter);
    log_info("info->io_mode = %d", i2c_config->io_mode);
    log_info("info->role = %d", i2c_config->role);
    log_info("IIC_CON0 0x%04x", iic_regs[iic]->CON0);
    log_info("IIC_CON1 0x%04x", iic_regs[iic]->CON1);
    log_info("IIC_BAUD 0x%02x", iic_regs[iic]->BAUD);
    //log_info("IIC_BUF %02x", iic_regs[iic]->BUF);
#endif
    return IIC_OK;
}

static enum iic_state_enum hw_iic_io_uninit(hw_iic_dev iic)
{
    enum iic_state_enum ret = IIC_OK;
    enum gpio_port scl_port = hw_iic_cfg_cache[iic].scl_io / 16;
    u32 scl_pin = BIT(hw_iic_cfg_cache[iic].scl_io % 16);
    enum gpio_port sda_port = hw_iic_cfg_cache[iic].sda_io / 16;
    u32 sda_pin = BIT(hw_iic_cfg_cache[iic].sda_io % 16);
    if (iic == 0) {
        gpio_disable_function(scl_port, scl_pin, PORT_FUNC_IIC0_SCL);
        gpio_deinit(scl_port, scl_pin);
        gpio_disable_function(sda_port, sda_pin, PORT_FUNC_IIC0_SDA);
        gpio_deinit(sda_port, sda_pin);

        struct gpio_config gpio_info;
        gpio_info.pin = scl_pin;
        gpio_info.mode = PORT_HIGHZ;
        gpio_info.hd = PORT_DRIVE_STRENGT_2p4mA;
        gpio_init(scl_port, &gpio_info);
        gpio_info.pin = sda_pin;
        gpio_init(sda_port, &gpio_info);
    } else {
        ret = IIC_ERROR_INDEX_ERROR;
    }

    return ret;
}
enum iic_state_enum hw_iic_uninit(hw_iic_dev iic)
{
    if (hw_iic_state[iic] == 0) {
        log_error("hw iic%d has been no init!", iic);
        return IIC_ERROR_NO_INIT;
    }
    hw_iic_state[iic] = 0;//no init

    enum iic_state_enum iic_state = hw_iic_io_uninit(iic);
    if (iic_state != IIC_OK) {
        return iic_state;
    }
    iic_disable(iic_regs[iic]);
    return IIC_OK;
}


enum iic_state_enum hw_iic_suspend(hw_iic_dev iic)
{
    if ((hw_iic_state[iic] & 0xc0) != 0x80) {
        log_error("hw iic%d is no init or suspend!", iic);
        return IIC_ERROR_SUSPEND_FAIL;
    }
    if ((hw_iic_state[iic] & 0x0f) != 0) {
        log_error("hw iic%d is busy!", iic);
        return IIC_ERROR_BUSY;
    }

    enum iic_state_enum iic_state = hw_iic_io_uninit(iic);
    if (iic_state != IIC_OK) {
        return iic_state;
    }
    IIC_HW_ENTER_CRITICAL();
    hw_iic_state[iic] |= BIT(6);//bit6:suspend ok
    iic_disable(iic_regs[iic]);
    IIC_HW_EXIT_CRITICAL();
    return IIC_OK;
}

//return:0:ok, <0:error
enum iic_state_enum hw_iic_resume(hw_iic_dev iic)
{
    if ((hw_iic_state[iic] & 0xc0) != 0xc0) {
        log_error("hw iic%d is no init or no suspend!", iic);
        return IIC_ERROR_RESUME_FAIL;
    }

    int ret = iic_port_init(iic);
    if (ret) {
        log_error("invalid hardware iic port\n");
        return ret;
    }
    IIC_HW_ENTER_CRITICAL();
    hw_iic_state[iic] &= ~ BIT(6); //resume ok
    iic_pnd_clr(iic_regs[iic]);
    iic_end_pnd_clr(iic_regs[iic]);
    iic_start_pnd_clr(iic_regs[iic]);
    iic_enable(iic_regs[iic]);
    if (hw_iic_cfg_cache[iic].role == IIC_SLAVE) {
        iic_recv_nack(iic_regs[iic]);
        /* iic_dir_in(iic_regs[iic]); */
        iic_buf_reg(iic_regs[iic]) = 0xff;
        /* iic_cfg_done(iic_regs[iic]); */
    }
    IIC_HW_EXIT_CRITICAL();
    return IIC_OK;
}

//return:0:error, 1:ok
enum iic_state_enum hw_iic_check_busy(hw_iic_dev iic)
{
    if (hw_iic_state[iic] & 0x0f) {
        return IIC_ERROR_BUSY;//error
    }
    IIC_HW_ENTER_CRITICAL();
    hw_iic_state[iic]++;//busy
    IIC_HW_EXIT_CRITICAL();
    return IIC_OK;//ok
}
void hw_iic_idle(hw_iic_dev iic)
{
    hw_iic_state[iic] &= 0xf0;//idle
}

static u8 tx_stop = 0;
void hw_iic_start(hw_iic_dev iic)//必须字节发送前设置好。硬件无法清除，需手动清除
{
    IIC_HW_ENTER_CRITICAL();
    iic_preset_restart(iic_regs[iic]);
    iic_no_end(iic_regs[iic]);
    tx_stop = 0;
    IIC_HW_EXIT_CRITICAL();
}

void hw_iic_stop(hw_iic_dev iic) //必须最后一字节发送前设置好
{
    if (tx_stop) {
        tx_stop = 0;
        log_info("stop:no \n");
        return;
    }
    IIC_HW_ENTER_CRITICAL();
    iic_preset_end(iic_regs[iic]);
    /* while (!iic_end_pnd(iic_regs[iic])); */
    /* iic_end_pnd_clr(iic_regs[iic]); */
    hw_iic_idle(iic);
    IIC_HW_EXIT_CRITICAL();
    tx_stop = 1;
    log_info("stop:ok \n");
}

void hw_iic_reset(hw_iic_dev iic)//无效
{
    /* hw_iic_start(iic); */
    /* hw_iic_stop(iic); */
}

u8 hw_iic_tx_byte(hw_iic_dev iic, u8 byte)//最后1byte前设置stop
{
    u8 ack = 0;
    IIC_HW_ENTER_CRITICAL();
    iic_buf_reg(iic_regs[iic]) = byte;
    iic_recv_nack(iic_regs[iic]);
    while (!iic_pnd(iic_regs[iic]));
    iic_pnd_clr(iic_regs[iic]);
    iic_no_restart(iic_regs[iic]);
    iic_no_end(iic_regs[iic]);
    ack = iic_send_is_ack(iic_regs[iic]);
    IIC_HW_EXIT_CRITICAL();
    return ack;
}
//最后1byte前设置stop
u8 hw_iic_rx_byte(hw_iic_dev iic, u8 ack)//如需接收后结束，先设置结束位
{
    IIC_HW_ENTER_CRITICAL();
    if (ack) {
        iic_recv_ack(iic_regs[iic]);
    } else {
        iic_recv_nack(iic_regs[iic]);
    }
    iic_buf_reg(iic_regs[iic]) = 0xff;
    while (!iic_pnd(iic_regs[iic]));
    iic_pnd_clr(iic_regs[iic]);
    iic_no_end(iic_regs[iic]);
    u8 rx_byte = iic_buf_reg(iic_regs[iic]);
    IIC_HW_EXIT_CRITICAL();
    return rx_byte;
}
//return: =len:ok
int hw_iic_read_buf(hw_iic_dev iic, void *buf, int len)////////////////////////////////////////////
{
    int i;

    if (!buf || !len) {
        return IIC_ERROR_PARAM_ERROR;
    }
    IIC_HW_ENTER_CRITICAL();
    iic_recv_ack(iic_regs[iic]);
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            hw_iic_stop(iic);
            iic_recv_nack(iic_regs[iic]);
        }
        iic_buf_reg(iic_regs[iic]) = 0xff;
        while (!iic_pnd(iic_regs[iic]));
        iic_pnd_clr(iic_regs[iic]);
        ((u8 *)buf)[i] = iic_buf_reg(iic_regs[iic]);
    }
    IIC_HW_EXIT_CRITICAL();
    return len;
}
//return: =len:ok
int hw_iic_write_buf(hw_iic_dev iic, const void *buf, int len)///////////////////////////////////////
{
    int i = 0;

    if (!buf || !len) {
        return IIC_ERROR_PARAM_ERROR;
    }
    IIC_HW_ENTER_CRITICAL();
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            hw_iic_stop(iic);
        }
        iic_buf_reg(iic_regs[iic]) = ((u8 *)buf)[i];
        while (!iic_pnd(iic_regs[iic]));
        iic_pnd_clr(iic_regs[iic]);
        if (!iic_send_is_ack(iic_regs[iic])) {
            break;
        }
    }
    IIC_HW_EXIT_CRITICAL();
    return i;
}

void hw_iic_set_ie(hw_iic_dev iic, i2c_pnd_typedef png, u8 en)
{
    if (en) {
        if (png == I2C_PND_STOP) {
            iic_set_end_ie(iic_regs[iic]);
        } else if (png == I2C_PND_TASK_DONE) { //tx rx ie
            iic_set_ie(iic_regs[iic]);
        }
    } else {
        if (png == I2C_PND_STOP) {
            iic_dis_end_ie(iic_regs[iic]);
        } else if (png == I2C_PND_TASK_DONE) { //tx rx ie
            iic_dis_ie(iic_regs[iic]);
        }
    }
}
u8 hw_iic_get_pnd(hw_iic_dev iic, i2c_pnd_typedef png)
{
    if (png == I2C_PND_START) {
        return !!iic_start_pnd(iic_regs[iic]);
    } else if (png == I2C_PND_TASK_DONE) { //tx rx ie
        return !!iic_pnd(iic_regs[iic]);
    } else if (png == I2C_PND_STOP) {
        return !!iic_end_pnd(iic_regs[iic]);
    }
    return 0;
}
void hw_iic_clr_pnd(hw_iic_dev iic, i2c_pnd_typedef png)
{
    if (png == I2C_PND_START) {
        iic_start_pnd_clr(iic_regs[iic]);
    } else if (png == I2C_PND_TASK_DONE) { //tx rx ie
        iic_pnd_clr(iic_regs[iic]);
    } else if (png == I2C_PND_STOP) {
        iic_end_pnd_clr(iic_regs[iic]);
    }
}

void hw_iic_clr_all_pnd(hw_iic_dev iic)
{
    IIC_HW_ENTER_CRITICAL();
    iic_start_pnd_clr(iic_regs[iic]);
    iic_pnd_clr(iic_regs[iic]);
    iic_end_pnd_clr(iic_regs[iic]);
    IIC_HW_EXIT_CRITICAL();
}

/***************************iic slave interrupt test***************************/
//中断法: hw_iic_cfg结构体配置从机模式,地址,使能iic中断即可
//从机中断接收不可被其它打断(如高优先级中断)，否则可能丢失结束信号导致出错
u8 iic_slave_rxdata[9];//接收字节数<=9
SET(interrupt(""))
void iic_isr()
{
    static u8 cnt = 0;
    if (iic_pnd(iic_regs[0])) {
        if (iic_start_pnd(iic_regs[0])) {
            /*log_info(" start!\n");*/
            iic_start_pnd_clr(iic_regs[0]);
        } else {
            /*log_info("no start!\n");*/
        }
        iic_slave_rxdata[cnt++] = iic_buf_reg(iic_regs[0]);
        iic_recv_ack(iic_regs[0]);
        iic_buf_reg(iic_regs[0]) = 0xff;
        iic_pnd_clr(iic_regs[0]);
    }
    if (iic_end_pnd(iic_regs[0])) {
        iic_start_pnd_clr(iic_regs[0]);
        iic_pnd_clr(iic_regs[0]);
        iic_end_pnd_clr(iic_regs[0]);
        iic_recv_nack(iic_regs[0]);

        log_info_hexdump(iic_slave_rxdata, 9);
        log_info("iic end!\n");
        for (cnt = 0; cnt < 9; cnt++) {
            iic_slave_rxdata[cnt] = 0;
        }
        cnt = 0;
    }
}

/***************************iic no slave***************************/

