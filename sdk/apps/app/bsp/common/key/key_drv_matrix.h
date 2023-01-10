#ifndef __KEY_DRV_MATRIX_H__
#define __KEY_DRV_MATRIX_H__

#include "gpio.h"
#include "key.h"


#define KEY_DETECT_LEVEL 0 //1:检测高电平; 0:检测低电平
static u32 matrix_key_row_value = 0xffffffff;
static u32 matrix_key_col_value = 0xffffffff;
static u8 matrix_key_row[] = { //normal io
    IO_PORTB_07,
    IO_PORTB_08,
    IO_PORTB_09,
    IO_PORTB_10
};
static u8 matrix_key_col[] = { //normal io or reuse lcd seg io
    IO_PORTA_00,
    IO_PORTA_01,
    IO_PORTA_02,
    IO_PORTA_03,
    IO_PORTA_04,
    IO_PORTA_05,
    IO_PORTA_06,
    IO_PORTA_07,
    IO_PORTA_08,
    IO_PORTA_09,
    IO_PORTA_10,
    IO_PORTA_11
};


extern const key_interface_t key_matrix_info;
void matrix_key_init(void);
void matrix_key_suspend();
void matrix_key_release();
u8 get_matrixkey_value(void);

#endif/*__KEY_DRV_MATRIX_H__*/
