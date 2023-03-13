#include "includes.h"
#include "key_drv_io.h"
#include "app_config.h"
#include "asm/power/p33.h"
#include "common.h"
#include "key_drv_matrix.h"

#include "lcd_seg4x8_driver.h"

#if LCD_4X8_EN
#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[LCDC]"
#include "log.h"

struct ui_lcd_seg4x8_env {
    u8 init;
    LCD_SEG4X8_VAR lcd_seg_var;
    u8 mode;
    const struct lcd_seg4x8_platform_data *user_data;
};

static u32 *SEGx_DAT[] = {
    (u32 *)(&(JL_LCDC->SEG0_DAT)),
    (u32 *)(&(JL_LCDC->SEG1_DAT)),
    (u32 *)(&(JL_LCDC->SEG2_DAT)),
    (u32 *)(&(JL_LCDC->SEG3_DAT)),
    (u32 *)(&(JL_LCDC->SEG4_DAT)),
    (u32 *)(&(JL_LCDC->SEG5_DAT)),
};

static struct ui_lcd_seg4x8_env _lcd_seg_env = {0};
#define __this 		(&_lcd_seg_env)


#define LED_A   BIT(0)
#define LED_B	BIT(1)
#define LED_C	BIT(2)
#define LED_D	BIT(3)
#define LED_E	BIT(4)
#define LED_F	BIT(5)
#define LED_G	BIT(6)
#define LED_H	BIT(7)
//数字'0' ~ '9'显示段码表
static const  u8 LCD_SEG_NUMBER_2_SEG[10] = {
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F), 		 //'0'
    (u8)(LED_B | LED_C), 										 //'1'
    (u8)(LED_A | LED_B | LED_D | LED_E | LED_G), 				 //'2'
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_G),  				 //'3'
    (u8)(LED_B | LED_C | LED_F | LED_G),						 //'4'
    (u8)(LED_A | LED_C | LED_D | LED_F | LED_G), 				 //'5'
    (u8)(LED_A | LED_C | LED_D | LED_E | LED_F | LED_G), 		 //'6'
    (u8)(LED_A | LED_B | LED_C), 								 //'7'
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F | LED_G), //'8'
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_F | LED_G), 		 //'9'
};

//字母'A' ~ 'Z'显示段码表
static const  u8 LCD_SEG_LARGE_LETTER_2_SEG[26] = {
    0x77, 0x40, 0x39, 0x3f, 0x79, ///<ABCDE
    0x71, 0x40, 0x76, 0x06, 0x40, ///<FGHIJ
    0x40, 0x38, 0x40, 0x37, 0x3f, ///<KLMNO
    0x73, 0x40, 0x50, 0x6d, 0x78, ///<PQRST
    0x3e, 0x3e, 0x40, 0x76, 0x40, ///<UVWXY
    0x40 ///<Z
};

//字母'a' ~ 'z'显示段码表
static const  u8 LCD_SEG_SMALL_LETTER_2_SEG[26] = {
    0x77, 0x7c, 0x58, 0x5e, 0x79, ///<abcde
    0x71, 0x40, 0x40, 0x40, 0x40, ///<fghij
    0x40, 0x38, 0x40, 0x54, 0x5c, ///<klmno
    0x73, 0x67, 0x50, 0x40, 0x78, ///<pqrst
    0x3e, 0x3e, 0x40, 0x40, 0x40, ///<uvwxy
    0x40 ///<z
};

//=================================================================================//
//                        		与IC芯片相关配置 								   //
//=================================================================================//
struct HW_PIN2SEG {
    enum HW_SEG_INDEX seg_index;
    u8 pin_index;
};

struct HW_PIN2COM {
    enum HW_COM_INDEX com_index;
    u8 pin_index;
};

static const struct HW_PIN2SEG hw_pin2seg_mapping[32] = {
    {LCD_SEG0, IO_PORTA_00},
    {LCD_SEG1, IO_PORTA_01},
    {LCD_SEG2, IO_PORTA_02},
    {LCD_SEG3, IO_PORTA_03},
    {LCD_SEG4, IO_PORTA_04},
    {LCD_SEG5, IO_PORTA_05},
    {LCD_SEG6, IO_PORTA_06},
    {LCD_SEG7, IO_PORTA_07},
    {LCD_SEG8, IO_PORTA_08},
    {LCD_SEG9, IO_PORTA_09},
    {LCD_SEG10, IO_PORTA_10},
    {LCD_SEG11, IO_PORTA_11},
    {LCD_SEG12, IO_PORTA_12},
    {LCD_SEG13, IO_PORTA_13},
    {LCD_SEG14, IO_PORTA_14},
    {LCD_SEG15, IO_PORTA_15},

    {LCD_SEG16, IO_PORTB_00},
    {LCD_SEG17, IO_PORTB_01},
    {LCD_SEG18, IO_PORTB_02},
    {LCD_SEG19, IO_PORTB_03},
    {LCD_SEG20, IO_PORTB_04},
    {LCD_SEG21, IO_PORTB_05},
    {LCD_SEG22, IO_PORTB_06},
    {LCD_SEG23, IO_PORTB_07},
    {LCD_SEG24, IO_PORTB_08},
    {LCD_SEG25, IO_PORTB_09},

    {LCD_SEG26, IO_PORTC_00},

    {LCD_SEG27, IO_PORTD_00},
    {LCD_SEG28, IO_PORTD_01},
    {LCD_SEG29, IO_PORTD_02},
    {LCD_SEG30, IO_PORTD_03},
    {LCD_SEG31, IO_PORTD_04},
};

#define UC03_LCD_COM_SEL 'A'//'A', 'B'
#if (UC03_LCD_COM_SEL == 'A')
static const struct HW_PIN2COM hw_pin2com_mapping[6] = {
    {LCD_COM0, IO_PORTC_05},
    {LCD_COM1, IO_PORTC_04},
    {LCD_COM2, IO_PORTC_03},
    {LCD_COM3, IO_PORTC_02},
    {LCD_COM4, IO_PORTC_01},
    {LCD_COM5, IO_PORTC_00},
};
#else
static const struct HW_PIN2COM hw_pin2com_mapping[6] = {
    {LCD_COM0, IO_PORTA_07},
    {LCD_COM1, IO_PORTA_06},
    {LCD_COM2, IO_PORTA_05},
    {LCD_COM3, IO_PORTA_04},
    {LCD_COM4, IO_PORTA_03},
    {LCD_COM5, IO_PORTA_02},
};
#endif

static enum HW_COM_INDEX __match_com_index(u8 gpio)
{
    for (u8 i = 0; i < 6; i++) {
        if (hw_pin2com_mapping[i].pin_index == gpio) {
            return hw_pin2com_mapping[i].com_index;
        }
    }

    return 0;
}

static enum HW_SEG_INDEX __match_seg_index(u8 gpio)
{
    for (u8 i = 0; i < 32; i++) {
        if (hw_pin2seg_mapping[i].pin_index == gpio) {
            return hw_pin2seg_mapping[i].seg_index;
        }
    }

    return 0;
}

//LCD COM & SEG
static void __lcd_seg4x8_show_segN(enum HW_COM_INDEX com, enum HW_SEG_INDEX seg, u8 value)
{
    enum HW_COM_INDEX com_index;
    enum HW_SEG_INDEX seg_index;

    u32 *seg_dat;

    com_index = __match_com_index(__this->user_data->pin_cfg.pin_com[com]); //To CHIP connect
    seg_index = __match_seg_index(__this->user_data->pin_cfg.pin_seg[seg]); //To CHIP connect

    seg_dat = SEGx_DAT[com_index];

    if (value) {
        *seg_dat |= BIT(seg_index);
    } else {
        *seg_dat &= ~BIT(seg_index);
    }
}


static void __lcd_seg4x8_reflash_char(u8 index)
{
    //show char
    if (index >= 4) {
        return;
    }

    if (BIT(index) & __this->lcd_seg_var.bFlashChar) {
        return;
    }

    for (u8 i = 0; i < 7; i++) {
        if (__this->lcd_seg_var.bShowBuff[index] & BIT(i)) {
            __lcd_seg4x8_show_segN(lcd_seg4x8_seg2pin[i + index * 7].com_index,
                                   lcd_seg4x8_seg2pin[i + index * 7].seg_index, 1);
        } else {
            __lcd_seg4x8_show_segN(lcd_seg4x8_seg2pin[i + index * 7].com_index,
                                   lcd_seg4x8_seg2pin[i + index * 7].seg_index, 0);
        }
    }
}

static void __lcd_seg4x8_reflash_string()
{
    u8 j = 0;
    for (j = 0; j < 4; j++) {
        __lcd_seg4x8_reflash_char(j);
    }
}

//刷新icon常亮显示
static void __lcd_seg4x8_reflash_icon()
{
    //show icon
    u8 i = 0;
    u8 j = 0;
    for (j = 0; j < 32; j++) {
        if (BIT(j) & __this->lcd_seg_var.bFlashIcon) {
            continue;
        }
        if (BIT(j) & __this->lcd_seg_var.bShowIcon) {
            for (i = 0; i < ARRAY_SIZE(lcd_seg4x8_icon_seg2pin); i++) { //lookup icon exist
                if (BIT(j) == lcd_seg4x8_icon_seg2pin[i].icon) {
                    //look up the seg2pin table, set the pin should be output 0
                    __lcd_seg4x8_show_segN(lcd_seg4x8_icon_seg2pin[i].seg2pin.com_index,
                                           lcd_seg4x8_icon_seg2pin[i].seg2pin.seg_index, 1);
                }
            }
        } else {
            for (i = 0; i < ARRAY_SIZE(lcd_seg4x8_icon_seg2pin); i++) { //lookup icon exist
                if (BIT(j) == lcd_seg4x8_icon_seg2pin[i].icon) {
                    //look up the seg2pin table, set the pin should be output 0
                    __lcd_seg4x8_show_segN(lcd_seg4x8_icon_seg2pin[i].seg2pin.com_index,
                                           lcd_seg4x8_icon_seg2pin[i].seg2pin.seg_index, 0);
                }
            }
        }
    }
}

//刷新icon闪烁显示
static void __lcd_seg4x8_flash_show_icon(u8 is_on)
{
    //show/off icon
    u8 i = 0;
    u8 j = 0;
    if (__this->lcd_seg_var.bFlashIcon) {
        for (j = 0; j < 32; j++) {
            if (BIT(j) & __this->lcd_seg_var.bFlashIcon) {
                for (i = 0; i < ARRAY_SIZE(lcd_seg4x8_icon_seg2pin); i++) { //lookup icon exist
                    if (BIT(j) == lcd_seg4x8_icon_seg2pin[i].icon) {
                        //look up the seg2pin table, set the pin should be output 0
                        if (is_on) {
                            __lcd_seg4x8_show_segN(lcd_seg4x8_icon_seg2pin[i].seg2pin.com_index,
                                                   lcd_seg4x8_icon_seg2pin[i].seg2pin.seg_index, 1);
                        } else {
                            __lcd_seg4x8_show_segN(lcd_seg4x8_icon_seg2pin[i].seg2pin.com_index,
                                                   lcd_seg4x8_icon_seg2pin[i].seg2pin.seg_index, 0);
                        }
                    }
                }
            }
        }
    }
}


static void __lcd_seg4x8_flash_show_char(u8 is_on)
{
    //show/off icon
    u8 i = 0;
    u8 j = 0;
    for (j = 0; j < 4; j++) {
        if (__this->lcd_seg_var.bFlashChar & BIT(j)) {
            if (is_on) {
                for (u8 i = 0; i < 7; i++) {
                    if (__this->lcd_seg_var.bShowBuff[j] & BIT(i)) {
                        __lcd_seg4x8_show_segN(lcd_seg4x8_seg2pin[i + j * 7].com_index,
                                               lcd_seg4x8_seg2pin[i + j * 7].seg_index, 1);
                    }
                }
            } else {
                for (u8 i = 0; i < 7; i++) {
                    __lcd_seg4x8_show_segN(lcd_seg4x8_seg2pin[i + j * 7].com_index,
                                           lcd_seg4x8_seg2pin[i + j * 7].seg_index, 0);
                }
            }
        }
    }
}

void __lcd_seg4x8_flash_show_timer_handle()
{
    static u8 flash_on = 0;
    if (__this->lcd_seg_var.bFlashIcon) {
        __lcd_seg4x8_flash_show_icon(flash_on);
    }
    if (__this->lcd_seg_var.bFlashChar) {
        __lcd_seg4x8_flash_show_char(flash_on);
    }
    flash_on = !flash_on;
}



void __lcd_seg4x8_reflash_screen()
{
    __lcd_seg4x8_reflash_string();
    __lcd_seg4x8_reflash_icon();
}


static void __lcd_seg4x8_clear_screen()
{
    //clear all
    JL_LCDC->SEG0_DAT = 0;
    JL_LCDC->SEG1_DAT = 0;
    JL_LCDC->SEG2_DAT = 0;
    JL_LCDC->SEG3_DAT = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   lcd_seg 单个字符显示函数
  @param   chardata：显示字符
  @return  void
  @note    void __lcd_seg4x8_show_char(u8 chardata)
  @ display:
  	   ___    ___    ___	___
	  |___|  |___|  |___|  |___|
	  |___|  |___|  |___|  |___|
	 ---0------1------2------3----> X
 */
/*----------------------------------------------------------------------------*/
static void __lcd_seg4x8_show_char(u8 chardata)
{
    if (__this->lcd_seg_var.bCoordinateX >= 4) {
        __this->lcd_seg_var.bCoordinateX = 0; //or return
        //return ;
    }
    if ((chardata >= '0') && (chardata <= '9')) {
        __this->lcd_seg_var.bShowBuff[__this->lcd_seg_var.bCoordinateX++] = LCD_SEG_NUMBER_2_SEG[chardata - '0'];
    } else if ((chardata >= 'a') && (chardata <= 'z')) {
        __this->lcd_seg_var.bShowBuff[__this->lcd_seg_var.bCoordinateX++] = LCD_SEG_SMALL_LETTER_2_SEG[chardata - 'a'];
    } else if ((chardata >= 'A') && (chardata <= 'Z')) {
        __this->lcd_seg_var.bShowBuff[__this->lcd_seg_var.bCoordinateX++] = LCD_SEG_LARGE_LETTER_2_SEG[chardata - 'A'];
    } else if (chardata == ':') {
        __this->lcd_seg_var.bShowIcon |= LCD_SEG4X8_1POINT;
        __this->lcd_seg_var.bFlashIcon &= (~LCD_SEG4X8_1POINT);
    } else if (chardata == '.') {
        __this->lcd_seg_var.bShowIcon |= LCD_SEG4X8_2POINT;
        __this->lcd_seg_var.bFlashIcon &= (~LCD_SEG4X8_2POINT);
    } else if (chardata == ' ') {
        __this->lcd_seg_var.bShowBuff[__this->lcd_seg_var.bCoordinateX++] = 0;
    } else {
        __this->lcd_seg_var.bShowBuff[__this->lcd_seg_var.bCoordinateX++] = LED_G; //显示'-'
    }

    /* __lcd_seg4x8_reflash_char(__this->lcd_seg_var.bCoordinateX - 1); */
}

///////////////// API:
void lcd_seg4x8_setX(u8 X)
{
    __this->lcd_seg_var.bCoordinateX = X;
}

void lcd_seg4x8_clear_string(void)
{
    memset(__this->lcd_seg_var.bShowBuff, 0x00, 4);
    __this->lcd_seg_var.bFlashChar = 0;

    lcd_seg4x8_setX(0);
    __lcd_seg4x8_reflash_string();
}

void lcd_seg4x8_show_char(u8 chardata)
{
    __lcd_seg4x8_show_char(chardata);
}

void lcd_seg4x8_show_string(u8 *str)
{
    while (*str != '\0') {
        __lcd_seg4x8_show_char(*str++);
    }
}

void lcd_seg4x8_show_icon(UI_LCD_SEG4X8_ICON icon)
{
    __this->lcd_seg_var.bShowIcon |= icon;
    __this->lcd_seg_var.bFlashIcon &= (~icon); //stop display
    __lcd_seg4x8_reflash_icon();
}

void lcd_seg4x8_flash_icon(UI_LCD_SEG4X8_ICON icon)
{
    __this->lcd_seg_var.bFlashIcon |= icon;
    __this->lcd_seg_var.bShowIcon &= (~icon); //stop display
    __lcd_seg4x8_reflash_icon();
}

void lcd_seg4x8_flash_char_start(u8 index)
{
    if (index < 4) {
        __this->lcd_seg_var.bFlashChar |= BIT(index);
    }
}

void lcd_seg4x8_flash_char_stop(u8 index)
{
    if (index < 4) {
        __this->lcd_seg_var.bFlashChar &= ~BIT(index);
    }
}

void lcd_seg4x8_clear_icon(UI_LCD_SEG4X8_ICON icon)
{
    __this->lcd_seg_var.bShowIcon &= (~icon);
    __this->lcd_seg_var.bFlashIcon &= (~icon);
    __lcd_seg4x8_reflash_icon();
}

void lcd_seg4x8_clear_all_icon(void)
{
    __this->lcd_seg_var.bFlashIcon = 0;
    __this->lcd_seg_var.bShowIcon = 0;
    __lcd_seg4x8_reflash_icon();
}

void lcd_seg4x8_show_null(void)
{
    lcd_seg4x8_clear_string();
    lcd_seg4x8_clear_all_icon();
}


/*----------------------------------------------------------------------------*/
/**@brief   字符串显示函数, 默认左对齐, 从x = 0开始显示
  @param   *str：字符串的指针
  @return  void
  @note    void lcd_seg_show_string_left(u8 *str)
 */
/*----------------------------------------------------------------------------*/
void lcd_seg4x8_show_string_reset_x(u8 *str)
{
    lcd_seg4x8_clear_string();
    while (*str != '\0') {
        __lcd_seg4x8_show_char(*str++);
    }
    __lcd_seg4x8_reflash_screen();
}

void lcd_seg4x8_show_string_align_right(u8 *str)
{
    u8 cnt = 0;
    u8 *_str = str;
    while (*_str++ != '\0') {
        cnt++;
    }
    if (cnt > 4) {
        return;
    }
    lcd_seg4x8_clear_string();
    lcd_seg4x8_setX(4 - cnt);//right
    /* lcd_seg4x8_setX(0);//left */
    while (*str != '\0') {
        lcd_seg4x8_show_char(*str++);
    }
    __lcd_seg4x8_reflash_screen();
}

void lcd_seg4x8_show_string_align_left(u8 *str)
{
    lcd_seg4x8_show_string_reset_x(str);
}

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

/*----------------------------------------------------------------------------*/
/**@brief  获取一个4位十进制的数的各个位
   @param  i:输入的一个4位十进制的数
   @return 无
   @note   void itoa4(u8 i, u8 *buf)
*/
/*----------------------------------------------------------------------------*/
static void itoa4(u16 i, u8 *buf)
{
    buf[0] = asc_number[(i % 10000) / 1000]; //千
    buf[1] = asc_number[(i % 1000) / 100]; //百
    buf[2] = asc_number[(i % 100) / 10]; //十
    buf[3] = asc_number[i % 10]; //个
}

void lcd_seg4x8_show_number(u16 val)
{
    u8 tmp_buf[5] = {0};

    if (val > 9999) {
        lcd_seg4x8_show_icon(LCD_SEG4X8_1POINT);
    } else {
        lcd_seg4x8_clear_icon(LCD_SEG4X8_1POINT);
    }
    itoa4(val, tmp_buf);
    lcd_seg4x8_show_string_reset_x(&tmp_buf[0]);
}

//数字显示函数, 高位不显示0
void lcd_seg4x8_show_number2(u16 val)
{
    u8 i;
    u8 tmp_buf[5] = {0};

    if (val > 9999) {
        lcd_seg4x8_show_icon(LCD_SEG4X8_1POINT);
    } else {
        lcd_seg4x8_clear_icon(LCD_SEG4X8_1POINT);
    }

    itoa4(val, tmp_buf);
    for (i = 0; i < 3; i++) {
        if (tmp_buf[i] != '0') {
            break;
        }
    }
    lcd_seg4x8_show_string_align_right((u8 *) & (tmp_buf[i + 0]));
}

//数字显示函数(追加方式)
void lcd_seg4x8_show_number_add(u16 val)
{
    u8 i;
    u8 tmp_buf[5] = {0};

    if (__this->lcd_seg_var.bCoordinateX == 0) {
        if (val > 9999) {
            lcd_seg4x8_show_icon(LCD_SEG4X8_1POINT);
        } else {
            lcd_seg4x8_clear_icon(LCD_SEG4X8_1POINT);
        }
    }

    itoa4(val, tmp_buf);
    for (i = 0; i < 4; i++) {
        if (tmp_buf[i] != '0') {
            break;
        }
    }
    lcd_seg4x8_show_string((u8 *)&tmp_buf[i]);
}

static void lcd_con_dump()
{
    log_info("JL_LCDC->CON0 = 0x%x\n", JL_LCDC->CON0);
    log_info("JL_LCDC->CON1 = 0x%x\n", JL_LCDC->CON1);
    log_info("JL_LCDC->SEG_IO_EN0 = 0x%x\n", JL_LCDC->SEG_IO_EN0);
    log_info("JL_LCDC->SEG0_DAT = 0x%x\n", JL_LCDC->SEG0_DAT);
    log_info("JL_LCDC->SEG1_DAT = 0x%x\n", JL_LCDC->SEG1_DAT);
    log_info("JL_LCDC->SEG2_DAT = 0x%x\n", JL_LCDC->SEG2_DAT);
    log_info("JL_LCDC->SEG3_DAT = 0x%x\n", JL_LCDC->SEG3_DAT);
    log_info("JL_LCDC->SEG4_DAT = 0x%x\n", JL_LCDC->SEG4_DAT);
    log_info("JL_LCDC->SEG5_DAT = 0x%x\n", JL_LCDC->SEG5_DAT);
}

void lcd_seg4x8_seg_init()
{
    for (int j = 0; j < 8; j++) {
        gpio_set_pull_up(__this->user_data->pin_cfg.pin_seg[j], 0);
        gpio_set_pull_down(__this->user_data->pin_cfg.pin_seg[j], 0);
        gpio_set_direction(__this->user_data->pin_cfg.pin_seg[j], 1);
        gpio_set_die(__this->user_data->pin_cfg.pin_seg[j], 0);
        gpio_set_dieh(__this->user_data->pin_cfg.pin_seg[j], 0);
    }
}

void lcd_seg4x8_com_init()
{
    for (int i = 0; i < 4; i++) {
        gpio_set_pull_up(__this->user_data->pin_cfg.pin_com[i], 0);
        gpio_set_pull_down(__this->user_data->pin_cfg.pin_com[i], 0);
        gpio_set_direction(__this->user_data->pin_cfg.pin_com[i], 1);
        gpio_set_die(__this->user_data->pin_cfg.pin_com[i], 0);
        gpio_set_dieh(__this->user_data->pin_cfg.pin_com[i], 0);
    }
}
u32 seg_io_en = 0;
u8 lcdc_suspend()
{
    u8 i = 0;
    if ((JL_LCDC->CON0 & BIT(0)) == 0) { //LCD_EN
        seg_io_en = JL_LCDC->SEG_IO_EN0;
        JL_LCDC->SEG_IO_EN0 = 0;

#if KEY_MATRIX_LCD_REUSE_EN//复用
        matrix_key_suspend();
#endif
        return 1;
    }
    return 0;//err
}
void lcdc_release()
{
    u8 i = 0;
    if ((JL_LCDC->CON0 & BIT(0)) == 0) { //LCD_EN
#if KEY_MATRIX_LCD_REUSE_EN
        matrix_key_release();
#endif
        lcd_seg4x8_seg_init();

        JL_LCDC->SEG_IO_EN0 = seg_io_en;
        JL_LCDC->CON0 |= BIT(0);                //LCD_EN
        JL_LCDC->CON1 |= BIT(0);                //sw_kst
    }
}
#if KEY_MATRIX_LCD_REUSE_EN
static volatile u8 lcd_key_num = 0xff;
u8 get_lcd_matrix_key_value()
{
    return lcd_key_num;
}
#endif

/* u32 tick_ = 0; */
/* u8 key_num=0xff,key_show=0; */
//32k-->15ms,30
___interrupt
static void lcdc_isr(void)
{
    JL_LCDC->CON0 &= ~BIT(0);                //LCD_EN

    if (JL_LCDC->CON1 & BIT(15)) {
        JL_LCDC->CON1 |= BIT(14);
    }
    if (lcdc_suspend()) { //ok
#if KEY_MATRIX_LCD_REUSE_EN
        u8 _get_matrixkey_value(void);
        lcd_key_num = _get_matrixkey_value();
#endif
        lcdc_release();
    }

    //test
    /* if(lcd_key_num!=0xff){ */
    /*     key_show = lcd_key_num; */
    /* } */
    /* lcd_seg4x8_show_number(key_show*100+tick_%100); */

    /* JL_LCDC->CON0 |= BIT(0);                //LCD_EN */
    /* JL_LCDC->CON1 |= BIT(0);                //sw_kst */
}

/*----------------------------------------------------------------------------*/
/**@brief   LCD段码屏初始化
  @param   void
  @return  void
  @note    void lcd_seg4x8_init(const struct lcd_SEG4X8_platform_data *_data)
 */
/*----------------------------------------------------------------------------*/
void lcd_seg4x8_init(const struct lcd_seg4x8_platform_data *_data)
{
    if ((__this->init == true) || (_data == NULL)) {
        return;
    }

    memset(__this, 0x00, sizeof(struct ui_lcd_seg4x8_env));
    __this->user_data = _data;
    //CLK_CON3[13:11]
    //000b: disable;
    //001b: lrc_clk;
    //010b: rtcosc_clk;
    //011b: wclk_clk;
    //100b: lsb_clk;
    JL_LCDC->CON0 = 0;
    JL_LCDC->CON1 = 0;
    SFR(JL_CLOCK->CLK_CON3, 11, 3, 1);  //LCDC时钟源选择lrc 200(259)k
    SFR(JL_LCDC->CON1, 6, 2, 3);				//clk div
    /* SFR(JL_CLOCK->CLK_CON3, 11, 3, 2);  //LCDC时钟源选择rtc 32k */
    /* SFR(JL_LCDC->CON1, 6, 2, 0);				//clk div */
#if (UC03_LCD_COM_SEL == 'A')  //IO_PORT GROUP SEL
    JL_IOMC->IOMC0 &= ~BIT(5); //GROUP A
#else
    JL_IOMC->IOMC0 |= BIT(5);  //GROUP B
#endif
    lcd_seg4x8_seg_init();				//seg set
    lcd_seg4x8_com_init();				//com set
    //板级配置参数
    JL_LCDC->CON0 |= (_data->bias << 2);    //[3:2]:0x01:1/2 Bias。0x02:1/3。0x03:1/4
    /* JL_LCDC->CON0 |= (_data->hd_isel << 28);    //[28:29]: bias current selection 0~3:200na~500na*/
    JL_LCDC->CON0 |= (_data->vlcd << 4); 	//[6:4]: VLCDS

    //固定配置参数
    JL_LCDC->CON0 |= (1 << 7);                //[7]: 帧频率控制, 默认使用 32KHz / 64
    JL_LCDC->CON0 |= (0b01 << 12);          //[10:11]: CHGMOD一直用强充电模式

    JL_LCDC->CON0 |= (1 << 30);                //high drive EN0
    /* JL_LCDC->CON0 |= (1 << 31);                //high drive EN1 */
    /* JL_LCDC->CON0 |= (0b0000 << 8);      //[12:13]: CHGMOD交替充电模式下cycle */
    JL_LCDC->CON0 |= (LCD_SEG4X8_COM_NUMBER_4 << 14); 			//[15:14]: COM_NUMBER_3;

    //SEG使能
    JL_LCDC->SEG_IO_EN0 = 0;

    u8 i = 0;
    u8 j = 0;
    for (j = 0; j < 8; j++) {
        for (i = 0; i < ARRAY_SIZE(hw_pin2seg_mapping); i++) {
            if (_data->pin_cfg.pin_seg[j] == hw_pin2seg_mapping[i].pin_index) {
                JL_LCDC->SEG_IO_EN0 |= BIT(i);
                break;
            }
        }
    }

    __this->mode =  _data->ctu_en;						//0:断续推屏 1:连续推屏

    JL_LCDC->CON1 |= BIT(14);                //clr pnd
    if (!__this->mode) {
        request_irq(IRQ_SLCD_IDX, 2, lcdc_isr, 0);
        JL_LCDC->CON1 &= ~BIT(1);                //
        JL_LCDC->CON1 |= BIT(2);                //ie
    } else {
        JL_LCDC->CON1 &= ~BIT(2);                //ie
        JL_LCDC->CON1 |= BIT(1);                //
    }

    /* JL_LCDC->CON0 |= BIT(1);                //DOT_EN */
    JL_LCDC->CON0 |= BIT(0);                //LCD_EN

    JL_LCDC->CON1 |= BIT(0);                //sw_kst

    log_info("lcd_seg4x8_init\n");

    __this->init = true;
}

#if 0
//=================================================================================//
//                        		FOR TEST CODE 								       //
//=================================================================================//
// *INDENT-OFF*
LCD_SEG4X8_PLATFORM_DATA_BEGIN(lcd_seg4x8_test_data)
    /* .vlcd = LCD_VOLTAGE_3_3V, */
    /* .bias = LCD_BIAS_1_3, */
    .vlcd = LCD_SEG4X8_VOLTAGE_3_0V,
    .bias = LCD_SEG4X8_BIAS_1_3,
    .hd_isel = LCD_SEG4X8_HD_ISEL_200NA,
    .ctu_en = 0, //0:断续推屏(en ie) 1:连续推屏
    .pin_cfg.pin_com[0] = IO_PORTC_05,
    .pin_cfg.pin_com[1] = IO_PORTC_04,
    .pin_cfg.pin_com[2] = IO_PORTC_03,
    .pin_cfg.pin_com[3] = IO_PORTC_02,
    .pin_cfg.pin_seg[0] = IO_PORTA_00,
    .pin_cfg.pin_seg[1] = IO_PORTA_01,
    .pin_cfg.pin_seg[2] = IO_PORTA_02,
    .pin_cfg.pin_seg[3] = IO_PORTA_03,
    .pin_cfg.pin_seg[4] = IO_PORTA_04,
    .pin_cfg.pin_seg[5] = IO_PORTA_05,
    .pin_cfg.pin_seg[6] = IO_PORTA_06,
    .pin_cfg.pin_seg[7] = IO_PORTA_07,
    /* .pin_cfg.pin_seg[8] = IO_PORTA_09, */
LCD_SEG4X8_PLATFORM_DATA_END()
static u32 cnt = 0;
void lcd_seg4x8_test_show()
{
    if (cnt > 9999) {
        cnt = 0;
    }
    /* log_info("cnt = %d\n", cnt); */
    lcd_seg4x8_show_number(cnt);
    cnt += 10;
}
void lcd_seg4x8_test(void)
{
    lcd_seg4x8_init(&lcd_seg4x8_test_data);
    __lcd_seg4x8_clear_screen();
    lcd_seg4x8_show_number(999);
    lcd_seg4x8_show_icon(LCD_SEG4X8_1POINT);
    lcd_con_dump();
}

/***************************复用,测试******************************/
void lcd_seg_io_reuse_test_display()
{
    printf("---------------reuse test------------------\n");
    u32 cnt = 0;
    u32 tick_ = 0;
    u8 key_show = 0;
    JL_PORTB->OUT &= ~BIT(6);
    JL_PORTB->DIR &= ~BIT(6); //frame time

    lcd_seg4x8_init(&lcd_seg4x8_test_data);
    __lcd_seg4x8_clear_screen();
    lcd_seg4x8_show_number(2811);

    void wdt_close();
    wdt_close();
    log_info("test:%s(): %d", __func__, __LINE__);
    while (1) {
        tick_++;
        printf("+");
#if KEY_MATRIX_LCD_REUSE_EN
        if (lcd_key_num != 0xff) {
            printf("\nreuse key(%d)cnt:%d\n", lcd_key_num, cnt);
            key_show = lcd_key_num;
            /* lcd_seg4x8_show_number(cnt); */
            /* lcd_seg4x8_show_number2(key_show); */
            /* lcd_key_num=0xff; */
        }
#endif
        lcd_seg4x8_show_number2(key_show * 100 + tick_ / 10 % 100);
        /* lcd_seg4x8_show_number(key_show * 100 + tick_ / 10 % 100); */
        delay(199999);
        /* wdt_clear(); */
    }
}
#endif
#endif

