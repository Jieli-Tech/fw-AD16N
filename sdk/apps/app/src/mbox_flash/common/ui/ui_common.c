
#pragma bss_seg(".led_ui.data.bss")
#pragma data_seg(".led_ui.data")
#pragma const_seg(".led_ui.text.const")
#pragma code_seg(".led_ui.text")
#pragma str_literal_override(".led_ui.text.const")
/*--------------------------------------------------------------------------*/
/**@file    UI_common.c
   @brief   UI 显示公共函数
   @details
   @author  bingquan Cai
   @date    2012-8-30
   @note    AC109N
*/
/*----------------------------------------------------------------------------*/
#include "config.h"
#include "ui_api.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[ui]"
#include "log.h"

/* const u8 asc_number[] = "0123456789";    ///<0~9的ASCII码表 */
static const u8 asc_number[10] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9'
};    ///<0~9的ASCII码表

u8 bcd_number[5];
/*----------------------------------------------------------------------------*/
/**@brief  获取一个一位十进制的数的个位
   @param  i:输入的一个一位十进制的数
   @return 无
   @note   void itoa1(u8 i)
*/
/*----------------------------------------------------------------------------*/
void itoa1(u8 i)
{
    bcd_number[0] = asc_number[i % 10];
    bcd_number[1] = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief  获取一个2位十进制的数的各个位
   @param  i:输入的一个2位十进制的数
   @return 无
   @note   void itoa2(u8 i)
*/
/*----------------------------------------------------------------------------*/
void itoa2(u8 i)
{
    bcd_number[0] = asc_number[(i % 100) / 10];
    bcd_number[1] = asc_number[(i % 10)];
    bcd_number[2] = 0;
}
/*----------------------------------------------------------------------------*/
/**@brief  获取一个3位十进制数的各个位
   @param  i:输入的一个3位十进制数
   @return 无
   @note   void itoa3(u8 i)
*/
/*----------------------------------------------------------------------------*/
void itoa3(u16 i)
{
    bcd_number[0] = asc_number[(i % 1000) / 100];
    bcd_number[1] = asc_number[(i % 100) / 10];
    bcd_number[2] = asc_number[i % 10];
    bcd_number[3] = 0;
}
/*----------------------------------------------------------------------------*/
/**@brief  获取一个4位十进制的数的各个位
   @param  i:输入的一个4位十进制的数
   @return 无
   @note   void itoa4(u8 i)
*/
/*----------------------------------------------------------------------------*/
void itoa4(u16 i)
{
    bcd_number[0] = asc_number[(i % 10000) / 1000];
    bcd_number[1] = asc_number[(i % 1000) / 100];
    bcd_number[2] = asc_number[(i % 100) / 10];
    bcd_number[3] = asc_number[i % 10];
    bcd_number[4] = 0;
}

