#ifndef	_IIC_SOFT_H_
#define _IIC_SOFT_H_

#include "typedef.h"
#include "gpio.h"

typedef const u8 soft_iic_dev;
#include "iic_api.h"


#define MAX_SOFT_IIC_NUM     3


struct iic_master_config *get_soft_iic_config(soft_iic_dev iic);
enum iic_state_enum soft_iic_init(soft_iic_dev iic, struct iic_master_config *i2c_config);
enum iic_state_enum soft_iic_uninit(soft_iic_dev iic);
enum iic_state_enum soft_iic_suspend(soft_iic_dev iic);
enum iic_state_enum soft_iic_resume(soft_iic_dev iic);
enum iic_state_enum soft_iic_check_busy(soft_iic_dev iic);
void soft_iic_start(soft_iic_dev iic);
void soft_iic_stop(soft_iic_dev iic);
void soft_iic_reset(soft_iic_dev iic);//同iic_v2
u8 soft_iic_tx_byte(soft_iic_dev iic, u8 byte);
u8 soft_iic_rx_byte(soft_iic_dev iic, u8 ack);
//return: =len:ok
int soft_iic_read_buf(soft_iic_dev iic, void *buf, int len);
//return: =len:ok
int soft_iic_write_buf(soft_iic_dev iic, const void *buf, int len);


#endif

