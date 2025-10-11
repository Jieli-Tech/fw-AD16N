#ifndef	_IIC_API_H_
#define _IIC_API_H_

#include "typedef.h"
#include "gpio.h"


enum iic_state_enum {
    IIC_OK = 0,
    IIC_ERROR_INIT_FAIL = -1,
    IIC_ERROR_NO_INIT = -2,
    IIC_ERROR_SUSPEND_FAIL = -3,
    IIC_ERROR_RESUME_FAIL = -4,
    IIC_ERROR_BUSY = -5,
    IIC_ERROR_PARAM_ERROR = -6,
    IIC_ERROR_DEV_ADDR_ACK_ERROR = -7,
    IIC_ERROR_REG_ADDR_ACK_ERROR = -8,
    IIC_ERROR_INDEX_ERROR = -9,
    IIC_ERROR_FREQUENCY_ERROR = -10,
};

enum iic_role {IIC_MASTER, IIC_SLAVE};

struct iic_master_config {
    enum iic_role role;     //软件只有IIC_MASTER
    int scl_io;
    int sda_io;
    enum gpio_mode io_mode;//上拉或浮空
    enum gpio_drive_strength hdrive;   //enum GPIO_HDRIVE 0:2.4MA, 1:8MA, 2:26.4MA, 3:40MA
    u32 master_frequency; //软件iic频率(hz 不准)
    u8 ie_en;     //中断使能
    //br27,28,36:0:close filter; 1:enable filter
    //br50:0:close filter; 1:<1*Tiic_baud_clk, 2:<2*Tiic_baud_clk, 3:<3*Tiic_baud_clk
    u8 io_filter;
};
#include "iic_soft.h"
#if defined CONFIG_CPU_BD49 || defined CONFIG_CPU_BR29
#include "iic_hw_v2.h"
#elif defined CONFIG_CPU_UC03
#include "iic_hw.h"
#endif

/******************************soft iic*****************************/
//如果无reg_addr:reg_addr=NULL,reg_len=0
//return: <0:error,  =read_len:ok
int soft_i2c_master_read_nbytes_from_device_reg(soft_iic_dev iic,  //iic索引
        unsigned char dev_addr, //设备地址
        unsigned char *reg_addr, unsigned char reg_len,//设备寄存器地址，长度
        unsigned char *read_buf, int read_len);//缓存buf，读取长度

//如果无reg_addr:reg_addr=NULL,reg_len=0
//return: =write_len:ok, other:error
int soft_i2c_master_write_nbytes_to_device_reg(soft_iic_dev iic,
        unsigned char dev_addr, //设备地址
        unsigned char *reg_addr, unsigned char reg_len,//设备寄存器地址，长度
        unsigned char *write_buf, int write_len);//数据buf, 写入长度




/******************************hw iic master*****************************/
//如果无reg_addr:reg_addr=NULL,reg_len=0
//return: <0:error,  =read_len:ok
int hw_i2c_master_read_nbytes_from_device_reg(hw_iic_dev iic,
        unsigned char dev_addr, //设备地址
        unsigned char *reg_addr, unsigned char reg_len,//设备寄存器地址，长度
        unsigned char *read_buf, int read_len);//缓存buf，读取长度

//如果无reg_addr:reg_addr=NULL,reg_len=0
//return: =write_len:ok, other:error
int hw_i2c_master_write_nbytes_to_device_reg(hw_iic_dev iic,
        unsigned char dev_addr, //设备地址
        unsigned char *reg_addr, unsigned char reg_len,//设备寄存器地址，长度
        unsigned char *write_buf, int write_len);//数据buf, 写入长度


#ifdef _IIC_USE_HW
#define get_iic_config(iic)                 get_hw_iic_config(iic)
#define iic_init(iic, config)               hw_iic_init(iic, config)
#define iic_uninit(iic)                     hw_iic_uninit(iic)
#define iic_start(iic)                      hw_iic_start(iic)
#define iic_stop(iic)                       hw_iic_stop(iic)
#define iic_reset(iic)                      hw_iic_reset(iic)
#define iic_tx_byte(iic, byte)              hw_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               hw_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         hw_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    hw_iic_suspend(iic)
#define iic_resume(iic)                     hw_iic_resume(iic)

#define i2c_master_read_nbytes_from_device_reg(iic, dev_addr, reg_addr, reg_len, read_buf, read_len) \
        hw_i2c_master_read_nbytes_from_device_reg(iic, dev_addr, reg_addr, reg_len, read_buf, read_len)
#define i2c_master_write_nbytes_to_device_reg(iic, dev_addr, reg_addr, reg_len, write_buf, write_len) \
        hw_i2c_master_write_nbytes_to_device_reg(iic, dev_addr, reg_addr, reg_len, write_buf, write_len)
#else
#define get_iic_config(iic)                 get_soft_iic_config(iic)
#define iic_init(iic, config)               soft_iic_init(iic, config)
#define iic_uninit(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_reset(iic)                      soft_iic_reset(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    soft_iic_suspend(iic)
#define iic_resume(iic)                     soft_iic_resume(iic)

#define i2c_master_read_nbytes_from_device_reg(iic, dev_addr, reg_addr, reg_len, read_buf, read_len) \
        soft_i2c_master_read_nbytes_from_device_reg(iic, dev_addr, reg_addr, reg_len, read_buf, read_len)
#define i2c_master_write_nbytes_to_device_reg(iic, dev_addr, reg_addr, reg_len, write_buf, write_len) \
        soft_i2c_master_write_nbytes_to_device_reg(iic, dev_addr, reg_addr, reg_len, write_buf, write_len)
#endif

/******************************hw iic slave*****************************/
int hw_iic_slave_polling_rx(hw_iic_dev iic, u8 *rx_buf);
int hw_iic_slave_polling_tx(hw_iic_dev iic, u8 *tx_buf);
#endif

