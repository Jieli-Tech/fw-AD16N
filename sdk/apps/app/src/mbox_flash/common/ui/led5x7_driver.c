
#pragma bss_seg(".led_ui.data.bss")
#pragma data_seg(".led_ui.data")
#pragma const_seg(".led_ui.text.const")
#pragma code_seg(".led_ui.text")
#pragma str_literal_override(".led_ui.text.const")
/*--------------------------------------------------------------------------*/
/**@file    LED.c
   @brief   LED 模块驱动接口函数
   @details
   @author  bingquan Cai
   @date    2012-8-30
   @note    AC109N
*/
/*----------------------------------------------------------------------------*/
#include "ui_api.h"
#include "led5x7_driver.h"
#include "music_play.h"
#include "play_file.h"
#include "break_point.h"
#include "msg.h"
#include "ui_common.h"
#include "music_play.h"
#include "audio_dac_api.h"
#include "device_app.h"
#include "audio_eq_api.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

LED5X7_VAR LED5X7_var;
/* bool Sys_HalfSec; */

const u8 LED_NUMBER[10] = {
    /*0*/
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F),
    /*1*/
    (u8)(LED_B | LED_C),
    /*2*/
    (u8)(LED_A | LED_B | LED_D | LED_E | LED_G),
    /*3*/
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_G),
    /*4*/
    (u8)(LED_B | LED_C | LED_F | LED_G),
    /*5*/
    (u8)(LED_A | LED_C | LED_D | LED_F | LED_G),
    /*6*/
    (u8)(LED_A | LED_C | LED_D | LED_E | LED_F | LED_G),
    /*7*/
    (u8)(LED_A | LED_B | LED_C),
    /*8*/
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F | LED_G),
    /*9*/
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_F | LED_G),
};

const u8 LED_LARGE_LETTER[26] = {
    0x77, 0x40, 0x39, 0x3f, 0x79, ///<ABCDE
    0x71, 0x40, 0x76, 0x06, 0x40, ///<FGHIJ
    0x40, 0x38, 0x40, 0x37, 0x3f, ///<KLMNO
    0x73, 0x40, 0x50, 0x6d, 0x40, ///<PQRST
    0x3e, 0x3e, 0x40, 0x76, 0x40, ///<UVWXY
    0x40///<Z
};

const u8 LED_SMALL_LETTER[26] = {
    0x77, 0x7c, 0x58, 0x5e, 0x79, ///<abcde
    0x71, 0x40, 0x40, 0x40, 0x40, ///<fghij
    0x40, 0x38, 0x40, 0x54, 0x5c, ///<klmno
    0x73, 0x67, 0x50, 0x40, 0x40, ///<pqrst
    0x3e, 0x3e, 0x40, 0x40, 0x40, ///<uvwxy
    0x40///<z
};

/* 通过测试 7断的真值表
   0    1    2     3     4     5     6
0  X    1A   1B    1E    SD   播放   X
1  1F   X    2A    2B    2E   2D     X
2  1G   2F   X     :     3B   ||     MP3
3  1C   2G   3F    X     3C   4E     X
4  1D   2C   3G    3A    X    4C     4G
5  3D   U    3E    4D    4F   X      4B
6  X    X    MHz    X    X    4A     X

      A
     ---
  F | G | B
     ---
  E |   | C
     ---
      D
*/

// 7断数码管转换表
u8 led_7[35][2] = {

//pinH, pinL
    {0, 1}, //1A
    {0, 2}, //1B
    {3, 0}, //1C
    {4, 0}, //1D
    {0, 3}, //1E
    {1, 0}, //1F
    {2, 0}, //1G

    {1, 2}, //2A
    {1, 3}, //2B
    {4, 1}, //2C
    {1, 5}, //2D
    {1, 4}, //2E
    {2, 1}, //2F
    {3, 1}, //2G

    {4, 3}, //3A
    {2, 4}, //3B
    {3, 4}, //3C
    {5, 0}, //3D
    {5, 2}, //3E
    {3, 2}, //3F
    {4, 2}, //3G

    {6, 5}, //4A
    {5, 6}, //4B
    {4, 5}, //4C
    {5, 3}, //4D
    {3, 5}, //4E
    {5, 4}, //4F
    {4, 6}, //4G

    {2, 5},//LED_PLAY
    {0, 5},//LED_PAUSE
    {5, 1},//USB
    {0, 4},//SD
    {2, 3},//2POINT
//	{6, 4},//DOT
    {6, 2},//FM
    {2, 6},//MP3

};
#if 0
//按位于 查表
u8 bit_table[8] AT(.ram_led) = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
};
#endif

const u8 playmodestr[][5] = {
    " ALL",
#ifdef FOLDER_PLAY_EN
    "Fold",
#endif
    " ONE",
#ifdef RANDOM_PLAY_EN
    "rAnd",
#endif
};

const u8 menu_string[][5] = {
    "HI  ",
    "IdLE",
    "Lod ",
#ifdef USB_DEVICE_EN
    " PC ",
    " UP ",
    " dN ",
#endif
    " AUX",
#ifdef RTC_ALARM_EN
    "-AL-",
#endif
#ifdef REC_ENABLE
    " REC",
#endif
#ifdef REC_ENABLE
    " LSP",
#endif
};



/*----------------------------------------------------------------------------*/
/**@brief   LED5X7 状态位缓存清除函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_clear_icon(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_clear_icon(void)
{
    LED5X7_var.bFlashChar = 0;
    LED5X7_var.bFlashIcon = 0;
    LED5X7_var.bShowBuff[4] = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   LED5X7 显示坐标设置
   @param   x：显示横坐标
   @return  void
   @author  Change.tsai
   @note    void LED5X7_setX(u8 X)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_setX(u8 X)
{
    LED5X7_var.bCoordinateX = X;
}
/*----------------------------------------------------------------------------*/
/**@brief   LED清屏函数
   @param   x：显示横坐标
   @return  void
   @author  Change.tsai
   @note    void LED5X7_clear(void)
*/
/*----------------------------------------------------------------------------*/
AT(.led5x7.text.cache.L1)
void LED5X7_clear(void)
{
    JL_PORTA->OUT &= ~LED_PORT_ALL;
    JL_PORTA->DIR |= LED_PORT_ALL;
    JL_PORTA->PU0 &= ~LED_PORT_ALL;
    JL_PORTA->PD0 &= ~LED_PORT_ALL;
    JL_PORTA->HD0 &= ~LED_PORT_ALL;
    JL_PORTA->HD1 &= ~LED_PORT_ALL;
}

/*----------------------------------------------------------------------------*/
/**@brief   LED 清屏函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_null(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_null(void)
{
    LED5X7_clear_icon();
    LED5X7_var.bShowBuff[0] = 0;
    LED5X7_var.bShowBuff[1] = 0;
    LED5X7_var.bShowBuff[2] = 0;
    LED5X7_var.bShowBuff[3] = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   LED5X7 扫描函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_init(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_init(void)
{
    LED5X7_clear();
}
#if 0
/*----------------------------------------------------------------------------*/
/**@brief   LED 亮度设置
   @param   void
   @return  void
   @author  Change.tsai
   @note    void set_LED_brightness(u8 br)
*/
/*----------------------------------------------------------------------------*/
void set_LED_brightness(u8 br)
{

}

/*----------------------------------------------------------------------------*/
/**@brief   设置LED亮度渐暗
   @param   void
   @return  void
   @author  Change.tsai
   @note    void set_LED_fade_out(void)
*/
/*----------------------------------------------------------------------------*/
void set_LED_fade_out(void)
{
    if (LED5X7_var.bBrightness < 20) {
        LED5X7_var.bBrightness++;
        set_LED_brightness(23 - LED5X7_var.bBrightness);
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   设置LED亮度全亮
   @param   void
   @return  void
   @author  Change.tsai
   @note    void set_LED_all_on(void)
*/
/*----------------------------------------------------------------------------*/
void set_LED_all_on(void)
{
    set_LED_brightness(16);
    LED5X7_var.bBrightness = 0;
}
#endif
/*----------------------------------------------------------------------------*/
/**@brief   LED5X7 单个字符显示函数
   @param   chardata：显示字符
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_char(u8 chardata)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_char(u8 chardata)
{
    //必须保证传入的参数符合范围，程序不作判断
    //if ((chardata < ' ') || (chardata > '~') || (LED5X7_var.bCoordinateX > 4))
    //{
    //    return;
    //}
    if ((chardata >= '0') && (chardata <= '9')) {
        LED5X7_var.bShowBuff[LED5X7_var.bCoordinateX++] = LED_NUMBER[chardata - '0'];
    } else if ((chardata >= 'a') && (chardata <= 'z')) {
        LED5X7_var.bShowBuff[LED5X7_var.bCoordinateX++] = LED_SMALL_LETTER[chardata - 'a'];
    } else if ((chardata >= 'A') && (chardata <= 'Z')) {
        LED5X7_var.bShowBuff[LED5X7_var.bCoordinateX++] = LED_LARGE_LETTER[chardata - 'A'];
    } else if (chardata == ':') {
        LED_STATUS |= LED_2POINT;
    } else if (chardata == ' ') {
        LED5X7_var.bShowBuff[LED5X7_var.bCoordinateX++] = 0;
    } else { //if (chardata == '-')     //不可显示
        LED5X7_var.bShowBuff[LED5X7_var.bCoordinateX++] = BIT(6);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   LED5X7 字符串显示函数
   @param   *str：字符串的指针   offset：显示偏移量
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_string(u8 *str)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_string(u8 *str)
{
    while (0 != *str) {
        LED5X7_show_char(*str++);
    }
}


/*----------------------------------------------------------------------------*/
/** @brief:
    @param:
    @return:
    @author:Juntham
    @note:
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_string_menu(u8 menu)
{
    LED5X7_show_string((u8 *)menu_string[menu]);
}

/*----------------------------------------------------------------------------*/
/**@brief   Music模式 设备显示
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_dev(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_dev(void)
{
    /*Music Device type*/
    LED_STATUS &= ~(LED_SD | LED_USB);
    if (pctl[0].dev_index == SD0_INDEX) {
        LED_STATUS |= LED_SD;
    } else if (pctl[0].dev_index == UDISK_INDEX) {
        LED_STATUS |= LED_USB;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   Music 播放文件号显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_filenumber(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_filenumber(void)
{
    /*Music File Number info*/
    itoa4(pctl[0].findex);
    LED5X7_show_string((u8 *)bcd_number);
    LED5X7_show_dev();
}

void LED5X7_show_IR_number(void)
{
    /*IR File Number info*/
    extern u16 Input_Number;
    itoa4(Input_Number);
    LED5X7_show_string((u8 *)bcd_number);
}

/*----------------------------------------------------------------------------*/
/**@brief   Music模式 显示界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_music_main(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_music_main(void)
{
    u16 play_time;

    /*Music Play time info*/
    play_time = decoder_time(pctl[0].p_dec_obj);
    /* play_time = get_music_play_time(); */

    itoa2(play_time / 60);
    LED5X7_show_string((u8 *)bcd_number);

    itoa2(play_time % 60);
    LED5X7_show_string((u8 *)bcd_number);

    LED5X7_show_dev();
    LED_STATUS |= LED_2POINT | LED_MP3;
    u32 status = decoder_status(pctl[0].p_dec_obj);
    if (status & B_DEC_PAUSE) {
        LED_STATUS |= LED_PAUSE;
    } else {
        LED_STATUS |= LED_PLAY;
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   EQ显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_volume(void)
*/
/*----------------------------------------------------------------------------*/
#if AUDIO_EQ_ENABLE
void LED5X7_show_eq(void)
{
    LED5X7_show_string((u8 *)"Eq ");
    extern AUDIO_EQ_INFO g_eq_info;
    LED5X7_show_char(g_eq_info.eq_mode % 10 + '0');
}
#endif
/*----------------------------------------------------------------------------*/
/**@brief   循环模式显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_volume(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_playmode(void)
{
    LED5X7_show_string((u8 *)&playmodestr[pctl[0].play_mode][0]);
}

/*----------------------------------------------------------------------------*/
/**@brief   音量显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_volume(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_volume(void)
{
    LED5X7_show_string((u8 *)"V ");
    itoa2(dac_vol('r', 0));
    LED5X7_show_string((u8 *)bcd_number);
}
#if 0
/*----------------------------------------------------------------------------*/
/**@brief   FM 模式主界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_fm_main(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_fm_main(void)
{
    /*FM - Frequency*/
    itoa4(fm_mode_var.wFreq);
    if (fm_mode_var.wFreq <= 999) {
        bcd_number[0] = ' ';
    }

    LED5X7_show_string((u8 *)bcd_number);
    LED_STATUS |= LED_MHZ;
}
/*----------------------------------------------------------------------------*/
/**@brief   FM 模式主界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_fm_main(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_fm_station(void)
{
    /*FM - Station*/
    LED5X7_show_string((u8 *)"P ");
    itoa2(fm_mode_var.bFreChannel);
    LED5X7_show_string((u8 *)bcd_number);
}

/*----------------------------------------------------------------------------*/
/**@brief   RTC 显示界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_RTC_main(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_RTC_main(void)
{
#ifdef RTC_EN
    itoa2(curr_time.bHour);
    LED5X7_show_string((u8 *)bcd_number);
    itoa2(curr_time.bMin);
    LED5X7_show_string((u8 *)bcd_number);

    LED5X7_var.bFlashIcon |= LED_2POINT;
    LED_STATUS |= LED_2POINT;

    if (UI_var.bCurMenu == MENU_RTC_SET) {
        if (RTC_setting_var.bCoordinate) {
            LED5X7_var.bFlashChar = BIT(2) | BIT(3);
        } else {
            LED5X7_var.bFlashChar = BIT(0) | BIT(1);
        }
    } else {
        LED5X7_var.bFlashChar = 0;
    }
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief   Alarm 显示界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_show_alarm(void)
*/
/*----------------------------------------------------------------------------*/
void LED5X7_show_alarm(void)
{
#ifdef RTC_EN
    itoa2(curr_alarm.bHour);
    LED5X7_show_string((u8 *)bcd_number);
    itoa2(curr_alarm.bMin);
    LED5X7_show_string((u8 *)bcd_number);

    LED_STATUS |= LED_2POINT;

    if (RTC_setting_var.bCoordinate == 0) {
        LED5X7_var.bFlashChar = BIT(0) | BIT(1);
    } else if (RTC_setting_var.bCoordinate == 1) {
        LED5X7_var.bFlashChar = BIT(2) | BIT(3);
    } else if (RTC_setting_var.bCoordinate == 2) {
        LED5X7_var.bFlashChar |= 0xF;
    }

    /*Alarm info - Switch On/Off*/
    if (curr_alarm.bSw) {
        LED_STATUS |= LED_PLAY;
    }
#endif
}
#endif
AT(.led5x7.text.cache.L1)
void LED_drive7(void)
{
    u8 k, i, j, temp;
    k = 0;
    /*
        LED5X7_var.bShowBuff[0]=0;
        LED5X7_var.bShowBuff[1]=LED_NUMBER[8];
        LED5X7_var.bShowBuff[2]=0;
        LED5X7_var.bShowBuff[3]=0;
        LED5X7_var.bShowBuff[4]=0;
    */
    LED5X7_var.bShowBuff1[0] = 0;
    LED5X7_var.bShowBuff1[1] = 0;
    LED5X7_var.bShowBuff1[2] = 0;
    LED5X7_var.bShowBuff1[3] = 0;
    LED5X7_var.bShowBuff1[4] = 0;
    LED5X7_var.bShowBuff1[5] = 0;
    LED5X7_var.bShowBuff1[6] = 0;

    //mark by mingqiu,need rebuild
#if 0
    if ((mute) && (work_mode != REC_MIC_MODE) && (scan_mode == 0)) { //FM_SCAN_STOP
        if (work_mode == FM_RADIO_MODE) {
            LED5X7_var.bFlashIcon |= LED_MHZ;
        }
        LED5X7_var.bFlashChar = 0x0f;
    } else {
        if (work_mode == FM_RADIO_MODE) {
            LED5X7_var.bFlashIcon &= ~LED_MHZ;
        }
        LED5X7_var.bFlashChar = 0x00;
    }
#endif

    for (i = 0; i < 5; i++) {
        temp = LED5X7_var.bShowBuff[i];

        /* 字符位/图标闪烁处理,未用到 */
        /* if (Sys_HalfSec) { */
        /*     if ((LED5X7_var.bFlashIcon) && (i == 4)) { */
        /*         temp = LED_STATUS & (~LED5X7_var.bFlashIcon); */
        /*     } else if (LED5X7_var.bFlashChar & BIT(i)) { */
        /*         temp = 0x0; */
        /*     } */
        /* } */

        for (j = 0; j < 7; j++) {
            if (temp & BIT(j)) {
                LED5X7_var.bShowBuff1[led_7[k][0]] |= BIT(led_7[k][1]);
            }
            k++;
        }
    }
}



/*----------------------------------------------------------------------------*/
/**@brief   LED扫描函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED5X7_scan(void)
*/
/*----------------------------------------------------------------------------*/
/* AT(.led5x7.text.cache.L1) */

u16 led_port_tab[] = {
    LED_PORT0,
    LED_PORT1,
    LED_PORT2,
    LED_PORT3,
    LED_PORT4,
    LED_PORT5,
    LED_PORT6,
};


void LED5X7_scan(void)
{
    static u8 cnt;

    u8 seg;
    u32 tmp = 0;
    LED_drive7();

    seg = LED5X7_var.bShowBuff1[cnt];

    LED5X7_clear();

#if 1
    tmp = led_port_tab[cnt];

    JL_PORTA->OUT |= tmp;
    JL_PORTA->HD0 |= tmp;
    JL_PORTA->DIR &= ~tmp;

    tmp = 0;
    for (u32 i = 0 ; i < 7; i++) {
        if (seg & BIT(i)) {
            tmp |= led_port_tab[i];
        }
    }
    JL_PORTA->OUT &= ~tmp;//LED_PORT0;
    JL_PORTA->DIR &= ~tmp;

    cnt++;
    if (cnt >= ARRAY_SIZE(led_port_tab)) {
        cnt = 0;
    }

#else
    switch (cnt) {
    case 0:
        JL_PORTA->OUT |= LED_PORT0;
        JL_PORTA->HD0 |= LED_PORT0;
        JL_PORTA->DIR &= ~LED_PORT0;
        break;

    case 1:
        JL_PORTA->OUT |= LED_PORT1;
        JL_PORTA->HD0 |= LED_PORT1;
        JL_PORTA->DIR &= ~LED_PORT1;
        break;

    case 2:
        JL_PORTA->OUT |= LED_PORT2;
        JL_PORTA->HD0 |= LED_PORT2;
        JL_PORTA->DIR &= ~LED_PORT2;
        break;

    case 3:
        JL_PORTA->OUT |= LED_PORT3;
        JL_PORTA->HD0 |= LED_PORT3;
        JL_PORTA->DIR &= ~LED_PORT3;
        break;

    case 4:
        JL_PORTA->OUT |= LED_PORT4;
        JL_PORTA->HD0 |= LED_PORT4;
        JL_PORTA->DIR &= ~LED_PORT4;
        break;

    case 5:
        JL_PORTA->OUT |= LED_PORT5;
        JL_PORTA->HD0 |= LED_PORT5;
        JL_PORTA->DIR &= ~LED_PORT5;
        break;

    case 6:
        JL_PORTA->OUT |= LED_PORT6;
        JL_PORTA->HD0 |= LED_PORT6;
        JL_PORTA->DIR &= ~LED_PORT6;
        break;

    default :
        break;
    }


    if (seg & BIT(0)) {
        JL_PORTA->OUT &= ~LED_PORT0;
        JL_PORTA->DIR &= ~LED_PORT0;
    }

    if (seg & BIT(1)) {
        JL_PORTA->OUT &= ~LED_PORT1;
        JL_PORTA->DIR &= ~LED_PORT1;
    }

    if (seg & BIT(2)) {
        JL_PORTA->OUT &= ~LED_PORT2;
        JL_PORTA->DIR &= ~LED_PORT2;
    }

    if (seg & BIT(3)) {
        JL_PORTA->OUT &= ~LED_PORT3;
        JL_PORTA->DIR &= ~LED_PORT3;
    }

    if (seg & BIT(4)) {
        JL_PORTA->OUT &= ~LED_PORT4;
        JL_PORTA->DIR &= ~LED_PORT4;
    }

    if (seg & BIT(5)) {
        JL_PORTA->OUT &= ~LED_PORT5;
        JL_PORTA->DIR &= ~LED_PORT5;
    }

    if (seg & BIT(6)) {
        JL_PORTA->OUT &= ~LED_PORT6;
        JL_PORTA->DIR &= ~LED_PORT6;
    }

    cnt++;
    cnt = (cnt >= 7) ? 0 : cnt;

#endif
}

#if 0
void led_test(void)
{
    LED5X7_init();
    LED5X7_show_string_menu(0);
    while (1) {
        wdt_clear();
    }
}
#endif
