#ifndef	_SPI_H_
#define _SPI_H_

#include "typedef.h"
#include "ioctl.h"

#define SUPPORT_SPI0  0   //是否使能SPI0
#define SUPPORT_SPI1  1   //是否使能SPI1
#define SUPPORT_SPI2  0   //无spi2

typedef enum spi_index {
    HW_SPI0, //SPI0系统已使用
    HW_SPI1,
    HW_SPI_MAX_NUM,
} hw_spi_dev;

enum spi_mode {
    SPI_MODE_BIDIR_1BIT,    //支持SPIx(x=0,1,2)，全双工，di接收，do发送
    SPI_MODE_UNIDIR_1BIT,   //支持SPIx(x=0,1,2)，半双工，do分时发送/接收
    SPI_MODE_UNIDIR_2BIT,   //支持SPIx(x=0,1,2)，半双工，di & do共2bit分时发送/接收
    SPI_MODE_UNIDIR_4BIT,   //支持SPIx(x=1)，半双工，di & do & wp & hold 共4bit分时发送/接收
};
enum spi_role {
    SPI_ROLE_MASTER,
    SPI_ROLE_SLAVE,
};
enum spi_bit_mode {
    SPI_FIRST_BIT_MSB,  //7,6,5,4,3,2,1,0
    SPI_FIRST_BIT_LSB,  //0,1,2,3,4,5,6,7
    SPI_FIRST_BIT_BIT3, //3,2,1,0,7,6,5,4
    SPI_FIRST_BIT_BIT4, //4,5,6,7,0,1,2,3
};


enum hw_spi_isr_status {
    SPI_WAITING_PND,
    SPI_TX_FINISH,
    SPI_RX_FINISH,
    SPI_PND_ERROR,
};

typedef struct spi_platform_data {
    u8 port[6];                //CLK, DO, DI, D2(wp), D3(hold), cs(只用于slave),未使用的io配0xff
    enum spi_role role;        //master or slave
    enum spi_mode mode;        //模式，选项为enum spi_mode中的枚举常量
    enum spi_bit_mode bit_mode;
    u8 cpol: 1; //clk level in idle state:0:low,  1:high
    u8 cpha: 1; //sampling edge:0:first,  1:second
    u8 ie_en: 1; //ie enbale:0:disable,  1:enable
    void (*spi_isr_callback)(hw_spi_dev spi, enum hw_spi_isr_status sta);  //spi isr callback
    u32 clk;  //波特率
} spi_hardware_info;


#define HW_SPI_MASTER_CS_EN 0//0:close,1:open
#define HW_SPI_IRQ_PRIORITY 7//低到高:0~7

struct spi_platform_data *get_hw_spi_config(hw_spi_dev spi);
int spi_open(hw_spi_dev spi, spi_hardware_info *spi_info);
void spi_close(hw_spi_dev spi);
void spi_suspend(hw_spi_dev spi);
void spi_resume(hw_spi_dev spi);

void spi_set_bit_mode(hw_spi_dev spi, enum spi_mode mode);
int spi_set_baud(hw_spi_dev spi, u32 baud);
u32 spi_get_baud(hw_spi_dev spi);

/*******************主机 阻塞接口*********************/
u8 spi_recv_byte(hw_spi_dev spi, int *err);
int spi_send_byte(hw_spi_dev spi, u8 byte);
u8 spi_send_recv_byte(hw_spi_dev spi, u8 byte, int *err);//全双工

int spi_dma_recv(hw_spi_dev spi, void *buf, u32 len);
int spi_dma_send(hw_spi_dev spi, const void *buf, u32 len);

void spi_set_ie(hw_spi_dev spi, u8 en);
u8 spi_get_pending(hw_spi_dev spi);
void spi_clear_pending(hw_spi_dev spi);

/*******************主机，从机中断接口*********************/
//切换中断回调函数
void hw_spix_irq_change_callback(hw_spi_dev spi, void (*spi_isr_callback_f)(hw_spi_dev spi, enum hw_spi_isr_status sta));
/*
 * @brief 发送1个字节，不等待pnd，用于中断,无cs, 适用主从
 * @parm spi  spi句柄
 * @parm tx_byte 发送的数据，rw=rx时无效
 * @parm rx_byte 接收的数据的地址，没有时给NULL
 * @parm rw :1-rx; 0-tx
 * @return null
 */
void spi_byte_transmit_for_isr(hw_spi_dev spi, u8 tx_byte, u8 *rx_byte, u8 rw);//rw:1-rx; 0-tx
/*
 * @brief 发送n个字节，不等待pnd，用于中断,无cs, 适用主从
 * @parm spi  spi句柄
 * @parm tx_buf 发送的buf的地址，没有时给NULL
 * @parm rx_buf 接收的buf的地址，没有时给NULL
 * @parm len:接收或发送的长
 * @parm rw :1-rx; 0-tx
 * @return null
 */
void spi_buf_transmit_for_isr(hw_spi_dev spi, u8 *tx_buf, u8 *rx_buf, int len, u8 rw);//rw:1-rx; 0-tx

void hw_spix_clear_buf_isr_addr_inc(hw_spi_dev spi);
/*
 * @brief spi 配置dma，不等待pnd，用于中断,无cs, 适用主从
 * @parm spi  spi句柄
 * @parm buf  缓冲区基地址
 * @parm len  期望长度
 * @parm rw  1 接收 / 0 发送
 * @return null
 */
void spi_dma_transmit_for_isr(hw_spi_dev spi, void *buf, int len, u8 rw);//rw:1-rx; 0-tx


//返回spi中断状态isr_status:见枚举
enum hw_spi_isr_status hw_spix_get_isr_status(hw_spi_dev spi);
//每包数据前清空,在通信中间调用,会导致数据长不准
void hw_spix_clear_isr_len(hw_spi_dev spi);
/*返回中断通信数据长度 带符号
 *    符号表示dma(-)还是byte(+)
 *    数据表示长度
 *    结合isr_status可知是接收还是发送 */
int hw_spix_get_isr_len(hw_spi_dev spi);

/* spi从机接口
 * spi从机不开中断dma模式
 *返回dma通信数据长度(当前已传输个数)
 *    符号表示dma(-)
 *    数据表示长度
 *    不能区分收发*/
int hw_spix_slave_get_dma_len(hw_spi_dev spi);
#endif

