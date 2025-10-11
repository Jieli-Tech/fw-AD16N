#ifndef _IIC_HW_V2_H_
#define _IIC_HW_V2_H_

#include "includes.h"
#include "gpio.h"

#define MASTER_IIC_WRITE_MODE_FAST_RESP 0
#define MASTER_IIC_READ_MODE_FAST_RESP 0//接收移位,客户根据协议修改
#define SLAVE_NO_STRETCH_AUTO_TASK 0 // 1+PULL_IIC_BUS=0:图3(no stretch)

#define IIC_WHILE_TIMEOUT_CNT_ 4240000 //48Mhz:400ms

#define HW_IIC0_IRQ_PRIORITY  7//中断优先级，范围:0~7(低~高)
#define HW_IIC1_IRQ_PRIORITY  7//中断优先级，范围:0~7(低~高)

#define iic_init_prepare(reg)       (reg->CON = 1)
#define iic_enable(reg)             (reg->CON |= BIT(0))
#define iic_disable(reg)            (reg->CON &= ~BIT(0))
#define iic_rst_rst(reg)            (reg->CON &= ~BIT(1))
#define iic_rst_release(reg)        (reg->CON |= BIT(1))
#define iic_role_host(reg)          (reg->CON &= ~BIT(2))
#define iic_role_slave(reg)         (reg->CON |= BIT(2))
#define iic_flt_sel(reg,x)          SFR(reg->CON, 3, 2, x)//0:close; 1:<1*Tiic_baud_clk, 2:<2*Tiic_baud_clk, 3:<3*Tiic_baud_clk
#define iic_stretch_en(reg)          (reg->CON &= ~BIT(5))
#define iic_stretch_dis(reg)         (reg->CON |= BIT(5))
#define iic_ignore_nack_dis(reg)     (reg->CON &= ~BIT(6))
#define iic_ignore_nack_en(reg)      (reg->CON |= BIT(6))
#define iic_addr_resp_manu(reg)     (reg->CON &= ~BIT(7))
#define iic_addr_resp_auto(reg)     (reg->CON |= BIT(7))
#define iic_slv_rx_manu(reg)         (reg->CON &= ~BIT(8))
#define iic_slv_rx_auto(reg)         (reg->CON |= BIT(8))
#define iic_slv_tx_manu(reg)         (reg->CON &= ~BIT(9))
#define iic_slv_tx_auto(reg)         (reg->CON |= BIT(9))

#define iic_baddr_resp_dis(reg)     (reg->CON &= ~BIT(10))
#define iic_baddr_resp_en(reg)      (reg->CON |= BIT(10))

typedef enum {
    I2C_TASK_SEND_RESET     =   0x0,
    I2C_TASK_SEND_ADDR      =   0x1,
    I2C_TASK_SEND_DATA      =   0x2,
    I2C_TASK_SEND_ACK       =   0x3,
    I2C_TASK_SEND_NACK      =   0x4,
    I2C_TASK_SEND_STOP      =   0x5,
    I2C_TASK_SEND_NACK_STOP =   0x6,
    I2C_TASK_RECV_DATA      =   0x7,
    I2C_TASK_RECV_DATA_ACK =   0x8,
    I2C_TASK_RECV_DATA_NACK  =   0x9,
} i2c_task_typedef;

#define iic_task_sel(reg,x)          SFR(reg->TASK, 0, 4, x)//i2c_task_typedef

typedef enum {
    I2C_PND_TASK_DONE           =  BIT(20),
    I2C_PND_START               =  BIT(21),
    I2C_PND_STOP                =  BIT(22),
    I2C_PND_RXACK               =  BIT(23),
    I2C_PND_RXNACK              =  BIT(24),
    I2C_PND_ADR_MATCH           =  BIT(25),
    I2C_PND_RXDATA_DONE         =  BIT(26),
    I2C_PND_TXTASK_LOAD         =  BIT(27),
    I2C_PND_RXTASK_LOAD         =  BIT(28),
} i2c_pnd_typedef;


#define iic_set_task_ie(reg)             (reg->PND |= BIT(0))
#define iic_clr_task_ie(reg)             (reg->PND &= ~BIT(0))
#define iic_task_pnd(reg)                (reg->PND & BIT(20))
#define iic_task_pnd_clr(reg)            (reg->PND |= BIT(10))

#define iic_set_start_ie(reg)             (reg->PND |= BIT(1))
#define iic_clr_start_ie(reg)             (reg->PND &= ~BIT(1))
#define iic_start_pnd(reg)                (reg->PND & BIT(21))
#define iic_start_pnd_clr(reg)            (reg->PND |= BIT(11))

#define iic_set_stop_ie(reg)             (reg->PND |= BIT(2))
#define iic_clr_stop_ie(reg)             (reg->PND &= ~BIT(2))
#define iic_stop_pnd(reg)                (reg->PND & BIT(22))
#define iic_stop_pnd_clr(reg)            (reg->PND |= BIT(12))

#define iic_set_rxack_ie(reg)             (reg->PND |= BIT(3))
#define iic_clr_rxack_ie(reg)             (reg->PND &= ~BIT(3))
#define iic_rxack_pnd(reg)                (reg->PND & BIT(23))
#define iic_rxack_pnd_clr(reg)            (reg->PND |= BIT(13))

#define iic_set_rxnack_ie(reg)             (reg->PND |= BIT(4))
#define iic_clr_rxnack_ie(reg)             (reg->PND &= ~BIT(4))
#define iic_rxnack_pnd(reg)                (reg->PND & BIT(24))
#define iic_rxnack_pnd_clr(reg)            (reg->PND |= BIT(14))

#define iic_set_adr_match_ie(reg)             (reg->PND |= BIT(5))
#define iic_clr_adr_match_ie(reg)             (reg->PND &= ~BIT(5))
#define iic_adr_match_pnd(reg)                (reg->PND & BIT(25))
#define iic_adr_match_pnd_clr(reg)            (reg->PND |= BIT(15))

#define iic_set_rxdata_done_ie(reg)             (reg->PND |= BIT(6))
#define iic_clr_rxdata_done_ie(reg)             (reg->PND &= ~BIT(6))
#define iic_rxdata_done_pnd(reg)                (reg->PND & BIT(26))
#define iic_rxdata_done_pnd_clr(reg)            (reg->PND |= BIT(16))

#define iic_set_txtask_load_ie(reg)             (reg->PND |= BIT(7))
#define iic_clr_txtask_load_ie(reg)             (reg->PND &= ~BIT(7))
#define iic_txtask_load_pnd(reg)                (reg->PND & BIT(27))
#define iic_txtask_load_pnd_clr(reg)            (reg->PND |= BIT(17))

#define iic_set_rxtask_load_ie(reg)             (reg->PND |= BIT(8))
#define iic_clr_rxtask_load_ie(reg)             (reg->PND &= ~BIT(8))
#define iic_rxtask_load_pnd(reg)                (reg->PND & BIT(28))
#define iic_rxtask_load_pnd_clr(reg)            (reg->PND |= BIT(18))
#define iic_reset_pnd(reg)                     (reg->PND = 0x1ff << 10)


#define iic_png_reg(reg)            (reg->PND)
#define iic_tx_buf_reg(reg)         (reg->TX_BUF)
#define iic_rx_buf_reg(reg)         (reg->RX_BUF)//8bit only read
#define iic_addr_reg(reg)           (reg->ADDR)//bit0:r/w
#define iic_baud_reg(reg)           (reg->BAUD)//12bit BAUD_CNT > (SETUP_CNT+HOLD_CNT)>0
#define iic_tsu_reg(reg)         (reg->TSU)//7bit(0~6) sda信号保持时间
#define iic_thd_reg(reg)         (reg->THD)//7bit(0~6) sda信号建立时间
#define iic_dbg_reg(reg)         (reg->DBG)//32bit only read

typedef enum {
    HW_IIC_0,
    // HW_IIC_1,
    MAX_HW_IIC_NUM,
} hw_iic_dev;
#include "iic_api.h"

// enum {IIC_MASTER, IIC_SLAVE};

struct hw_iic_slave_config {
    struct iic_master_config config;
    void (*iic_slave_irq_callback)(void);
    u8 slave_addr;//bit7~bit1
};




struct iic_master_config *get_hw_iic_config(hw_iic_dev iic);
enum iic_state_enum hw_iic_init(hw_iic_dev iic, struct iic_master_config *i2c_config);
enum iic_state_enum hw_iic_uninit(hw_iic_dev iic);
enum iic_state_enum hw_iic_resume(hw_iic_dev iic);
enum iic_state_enum hw_iic_suspend(hw_iic_dev iic);
enum iic_state_enum hw_iic_check_busy(hw_iic_dev iic);

void hw_iic_start(hw_iic_dev iic);
// u8 hw_iic_start_addr(hw_iic_dev iic, u8 addr);
void hw_iic_stop(hw_iic_dev iic);
void hw_iic_reset(hw_iic_dev iic);
u8 hw_iic_tx_byte(hw_iic_dev iic, u8 byte);
u8 hw_iic_rx_byte(hw_iic_dev iic, u8 ack);
int hw_iic_read_buf(hw_iic_dev iic, void *buf, int len);
int hw_iic_write_buf(hw_iic_dev iic, const void *buf, int len);
int hw_iic_set_baud(hw_iic_dev iic, u32 baud);

void hw_iic_set_ack(hw_iic_dev iic, u8 ack_en);
void hw_iic_set_ie(hw_iic_dev iic, i2c_pnd_typedef png, u8 en);
u8 hw_iic_get_pnd(hw_iic_dev iic, i2c_pnd_typedef png);
void hw_iic_clr_pnd(hw_iic_dev iic, i2c_pnd_typedef png);
void hw_iic_clr_all_pnd(hw_iic_dev iic);




//从机接口:
enum iic_slave_rx_state {
    IIC_SLAVE_RX_PREPARE_TIMEOUT = -1,
    IIC_SLAVE_RX_PREPARE_OK = 0,
    IIC_SLAVE_RX_PREPARE_END_OK = 1,
    IIC_SLAVE_RX_ADDR_NO_MATCH = -2,
    IIC_SLAVE_RX_ADDR_TX = 2,
    IIC_SLAVE_RX_ADDR_RX = 3,
    IIC_SLAVE_RX_DATA = 4,
};

enum iic_state_enum hw_iic_slave_init(hw_iic_dev iic, struct hw_iic_slave_config *i2c_config);
void hw_iic_slave_set_addr(hw_iic_dev iic, u8 addr, u8 addr_ack);
void hw_iic_slave_set_callback(hw_iic_dev iic, void (*iic_slave_irq_callback)(void));
u8 hw_iic_slave_get_addr(hw_iic_dev iic);

enum iic_slave_rx_state hw_iic_slave_rx_prepare(hw_iic_dev iic, u8 ack, u32 wait_time);//轮询, 准备收
//判断地址，返回数据类型, 不检查结束位
enum iic_slave_rx_state hw_iic_slave_rx_byte(hw_iic_dev iic, u8 *rx_byte);
int hw_iic_slave_rx_nbyte(hw_iic_dev iic, u8 *rx_buf);//轮询,含结束位
u8 hw_iic_slave_tx_check_ack(hw_iic_dev iic);//return:1:ack; 0:no ack
void hw_iic_slave_tx_byte(hw_iic_dev iic, u8 byte);//准备发
int hw_iic_slave_tx_nbyte(hw_iic_dev iic, u8 *tx_buf);//轮询,含结束位
#endif

