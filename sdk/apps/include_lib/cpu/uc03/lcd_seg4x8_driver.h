#ifndef _LCD_SEG4X8_CFG_H_
#define _LCD_SEG4X8_CFG_H_

#include "gpio.h"

#if LCD_4X8_EN
//LCD_SEG4X8 真值表选择
#define UI_LCD_SEG4X8_TRUE_TABLE1

typedef struct _lcd_seg4x8_var {
    u8  bCoordinateX;
    u8  bFlashChar;
    u8  bShowBuff[4]; //[0] ~ [3]显示的数字/字母显示的所有段
    u32 bShowIcon;    //每1bit对应LED4的1段, maybe icon > 7段
    u32 bFlashIcon;   //闪烁标志
} LCD_SEG4X8_VAR;

enum LCD_SEG3X9_COM_NUMBER {
    LCD_SEG3X9_COM_NUMBER_3 = 0,
    LCD_SEG3X9_COM_NUMBER_4,
    LCD_SEG3X9_COM_NUMBER_5,
    LCD_SEG3X9_COM_NUMBER_6,
};

typedef u32 UI_LCD_SEG4X8_ICON;

enum HW_SEG_INDEX {
    LCD_SEG0 = 0,
    LCD_SEG1,
    LCD_SEG2,
    LCD_SEG3,
    LCD_SEG4,
    LCD_SEG5,
    LCD_SEG6,
    LCD_SEG7,
    LCD_SEG8,
    LCD_SEG9,
    LCD_SEG10,
    LCD_SEG11,
    LCD_SEG12,
    LCD_SEG13,
    LCD_SEG14,
    LCD_SEG15,
    LCD_SEG16,
    LCD_SEG17,
    LCD_SEG18,
    LCD_SEG19,
    LCD_SEG20,
    LCD_SEG21,
    LCD_SEG22,
    LCD_SEG23,
    LCD_SEG24,
    LCD_SEG25,
    LCD_SEG26,
    LCD_SEG27,
    LCD_SEG28,
    LCD_SEG29,
    LCD_SEG30,
    LCD_SEG31,
};

enum HW_COM_INDEX {
    LCD_COM0 = 0,
    LCD_COM1,
    LCD_COM2,
    LCD_COM3,
    LCD_COM4,
    LCD_COM5,
};

struct lcd_seg2pin {
    enum HW_COM_INDEX com_index;
    enum HW_SEG_INDEX seg_index;
};

struct lcd_seg_icon_seg2pin {
    UI_LCD_SEG4X8_ICON icon;
    struct lcd_seg2pin seg2pin;
};

struct lcd_seg4x8_pin {
    u8 pin_com[4];
    u8 pin_seg[8];
};

enum LCD_SEG4X8_BIAS {
    LCD_SEG4X8_BIAS_OFF = 0,
    LCD_SEG4X8_BIAS_1_2, 	// 1/2 bias
    LCD_SEG4X8_BIAS_1_3, 	// 1/3 bias
    LCD_SEG4X8_BIAS_1_4,  	// 1/4 bias
};
enum LCD_SEG4X8_HD_ISEL {
    LCD_SEG4X8_HD_ISEL_200NA = 0, // 200nA
    LCD_SEG4X8_HD_ISEL_300NA,
    LCD_SEG4X8_HD_ISEL_400NA,
    LCD_SEG4X8_HD_ISEL_500NA,
};

enum LCD_SEG4X8_COM_NUMBER {
    LCD_SEG4X8_COM_NUMBER_3 = 0,
    LCD_SEG4X8_COM_NUMBER_4,
    LCD_SEG4X8_COM_NUMBER_5,
    LCD_SEG4X8_COM_NUMBER_6,
};

enum LCD_SEG4X8_VOLTAGE {
    LCD_SEG4X8_VOLTAGE_2_6V = 0, 	//000b
    LCD_SEG4X8_VOLTAGE_2_7V, 		//001b
    LCD_SEG4X8_VOLTAGE_2_8V, 		//010b
    LCD_SEG4X8_VOLTAGE_2_9V, 		//011b
    LCD_SEG4X8_VOLTAGE_3_0V, 		//100b
    LCD_SEG4X8_VOLTAGE_3_1V, 		//101b
    LCD_SEG4X8_VOLTAGE_3_2V, 		//110b
    LCD_SEG4X8_VOLTAGE_3_3V, 		//111b
};

struct lcd_seg4x8_platform_data {
    enum LCD_SEG4X8_VOLTAGE vlcd;
    enum LCD_SEG4X8_BIAS bias;
    enum LCD_SEG4X8_HD_ISEL hd_isel;
    u8 ctu_en;//0:断续推屏 1:连续推屏
    struct lcd_seg4x8_pin pin_cfg;
};

//=================================================================================//
//                        		LCD段码屏配置 									   //
//=================================================================================//
#ifdef UI_LCD_SEG4X8_TRUE_TABLE1

/*********** 段码屏4x8真值表**************/
/* 		         0     1     2     3     4     5	 6 	   7
		COM0  	1A    1B    2A    2B    3A    3B    4A	  4B
		COM1  	1F    1G    2F    2G    3F    3G    4F	  4G
		COM2  	1E    1C    2E    2C    3E    3C    4E	  4C
		COM3  	1P    1D    2P    2D    3P    3D    4P    4D
 */
static const struct lcd_seg2pin lcd_seg4x8_seg2pin[] = {
    //com_index, seg_index
    {LCD_COM0, LCD_SEG0}, //1A
    {LCD_COM0, LCD_SEG1}, //1B
    {LCD_COM2, LCD_SEG1}, //1C
    {LCD_COM3, LCD_SEG1}, //1D
    {LCD_COM2, LCD_SEG0}, //1E
    {LCD_COM1, LCD_SEG0}, //1F
    {LCD_COM1, LCD_SEG1}, //1G

    {LCD_COM0, LCD_SEG2}, //2A
    {LCD_COM0, LCD_SEG3}, //2B
    {LCD_COM2, LCD_SEG3}, //2C
    {LCD_COM3, LCD_SEG3}, //2D
    {LCD_COM2, LCD_SEG2}, //2E
    {LCD_COM1, LCD_SEG2}, //2F
    {LCD_COM1, LCD_SEG3}, //2G

    {LCD_COM0, LCD_SEG4}, //3A
    {LCD_COM0, LCD_SEG5}, //3B
    {LCD_COM2, LCD_SEG5}, //3C
    {LCD_COM3, LCD_SEG5}, //3D
    {LCD_COM2, LCD_SEG4}, //3E
    {LCD_COM1, LCD_SEG4}, //3F
    {LCD_COM1, LCD_SEG5}, //3G

    {LCD_COM0, LCD_SEG6}, //4A
    {LCD_COM0, LCD_SEG7}, //4B
    {LCD_COM2, LCD_SEG7}, //4C
    {LCD_COM3, LCD_SEG7}, //4D
    {LCD_COM2, LCD_SEG6}, //4E
    {LCD_COM1, LCD_SEG6}, //4F
    {LCD_COM1, LCD_SEG7}, //4G
};


#define LCD_SEG4X8_1POINT   		BIT(0) 		//0: 冒号
#define LCD_SEG4X8_2POINT			BIT(1) 		//1: 点
#define LCD_SEG4X8_3POINT 			BIT(2) 		//2: 点
#define LCD_SEG4X8_4POINT			BIT(3) 		//3: 点

static const struct lcd_seg_icon_seg2pin lcd_seg4x8_icon_seg2pin[] = {
    //icon       		        com_index, seg_index
    {LCD_SEG4X8_1POINT, 		{LCD_COM3, LCD_SEG0}},
    {LCD_SEG4X8_2POINT,  		{LCD_COM3, LCD_SEG2}},
    {LCD_SEG4X8_3POINT, 		{LCD_COM3, LCD_SEG4}},
    {LCD_SEG4X8_4POINT, 		{LCD_COM3, LCD_SEG6}},
    // {LCD_SEG4X8_DIGIT1,		{LCD_COM0, LCD_SEG2}},
};

//=================================================================================//
//                        		关于LCD段码屏接线问题 							   //
// 1.COM口需要从COM0按顺序开始连接, 如4COM, 则IO必须占用COM0/1/2/3，而不能COM1/2/3/4;
// 2.COM0/1/2/3与LCD屏可以不按照对应需要接线, 如:
// 		LCD_COM0 --> CHIP_COM3;
// 		LCD_COM1 --> CHIP_COM2;
// 		LCD_COM2 --> CHIP_COM1;
// 		LCD_COM3 --> CHIP_COM0;
// 	但一般建议按顺序接线;
// 2.SEG可以根据实际情况选择SEG0~SEG21接线, 如;
// 		LCD_SEG0 --> CHIP_SEG7;
// 		LCD_SEG1 --> CHIP_SEG8;
// 		LCD_SEG2 --> CHIP_SEG9;
// 		LCD_SEG3 --> CHIP_SEG10;
//=================================================================================//
#define LCD_SEG4X8_PLATFORM_DATA_BEGIN(data) \
		const struct lcd_seg4x8_platform_data data = {

#define LCD_SEG4X8_PLATFORM_DATA_END() \
};

//UI LED7 API:
//=================================================================================//
//                        		模块初始化显示接口                    			   //
//=================================================================================//
void lcd_seg4x8_init(const struct lcd_seg4x8_platform_data *_data);
const struct ui_display_api *ui_lcd_seg4x8_init(void *para);
//=================================================================================//
//                        		设置显示坐标接口                    			   //
/*
  	   ___    ___    ___	___
	  |___|  |___|  |___|  |___|
	  |___|  |___|  |___|  |___|
	 ---0------1------2------3----> X
*/
//=================================================================================//
void lcd_seg4x8_setX(u8 X);

//=================================================================================//
//                        		字符类显示接口                    				   //
//=================================================================================//
void lcd_seg4x8_show_char(u8 chardata); 			//显示字符(追加方式)
void lcd_seg4x8_flash_char_start(u8 index);       	//闪烁单个字符
void lcd_seg4x8_flash_char_stop(u8 index);        	//取消闪烁单个字符
void lcd_seg4x8_show_string(u8 *str);   			//显示字符串(追加方式)
void lcd_seg4x8_show_string_reset_x(u8 *str);  		//显示字符串, x从0开始
void lcd_seg4x8_show_string_align_right(u8 *str); //led7显示字符串(清屏&右方式)
void lcd_seg4x8_show_string_align_left(u8 *str);  //led7显示字符串(清屏&左方式)

//=================================================================================//
//                        		数字类显示接口                    				   //
//=================================================================================//
void lcd_seg4x8_show_number(u16 val); //显示数字(清屏&高位显示0)
void lcd_seg4x8_show_number2(u16 val); //显示数字(清屏&高位不显示)
void lcd_seg4x8_show_number_add(u16 val); //显示数字(追加方式)

//=================================================================================//
//                        		图标类显示接口                    				   //
//=================================================================================//
void lcd_seg4x8_show_icon(UI_LCD_SEG4X8_ICON icon); //显示单个图标(追加)
void lcd_seg4x8_flash_icon(UI_LCD_SEG4X8_ICON icon); //闪烁单个图标(追加)

//=================================================================================//
//                        		模式类类显示接口                    			   //
//=================================================================================//

//=================================================================================//
//                        		清屏类显示接口                    				   //
//=================================================================================//
void lcd_seg4x8_show_null(void); //清除所有显示(数字,字符串和图标)
void lcd_seg4x8_clear_string(void); //清除显示数字和字母
void lcd_seg4x8_clear_icon(UI_LCD_SEG4X8_ICON icon); //清除显示单个图标
void lcd_seg4x8_clear_all_icon(void); //清除显示所有图标



#endif /* #ifdef  UI_LCD_SEG4X8_TRUE_TABLE1 */

#endif /* #ifdef  LCD_4X8_EN*/

#endif /* #ifndef _LCD_SEG4X8_CFG_H_ */
