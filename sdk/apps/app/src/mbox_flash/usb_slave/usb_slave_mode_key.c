
#pragma bss_seg(".usb_slave_mode_key.data.bss")
#pragma data_seg(".usb_slave_mode_key.data")
#pragma const_seg(".usb_slave_mode_key.text.const")
#pragma code_seg(".usb_slave_mode_key.text")
#pragma str_literal_override(".usb_slave_mode_key.text.const")

#include "typedef.h"
#include "key.h"
#include "msg.h"
#include "app_config.h"

#if KEY_IR_EN
#define IRFF00_USB_SLAVE_SHORT_UP		\
                                /*00*/    MSG_PP,\
							    /*01*/    MSG_NEXT_WORKMODE,\
								/*02*/    MSG_MUTE,\
								/*03*/    MSG_PP,\
								/*04*/    MSG_PREV_FILE,\
								/*05*/    MSG_NEXT_FILE,\
								/*06*/    MSG_EQ_SW,\
								/*07*/    MSG_VOL_DOWN,\
								/*08*/    MSG_VOL_UP,\
								/*09*/    MSG_0,\
                                /*10*/    MSG_NEXT_PLAYMODE,\
								/*11*/    MSG_NEXT_DEVICE,\
								/*12*/    MSG_1,\
								/*13*/    MSG_2,\
								/*14*/    MSG_3,\
								/*15*/    MSG_4,\
								/*16*/    MSG_5,\
								/*17*/    MSG_6,\
								/*18*/    MSG_7,\
								/*19*/    MSG_8,\
								/*20*/    MSG_9

#define IRFF00_USB_SLAVE_LONG		\
                                /*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
								/*02*/    NO_MSG,\
								/*03*/    NO_MSG,\
								/*04*/    MSG_MUSIC_FR,\
								/*05*/    MSG_MUSIC_FF,\
								/*06*/    NO_MSG,\
								/*07*/    MSG_VOL_DOWN,\
								/*08*/    MSG_VOL_UP,\
								/*09*/    NO_MSG,\
                                /*10*/    NO_MSG,\
								/*11*/    NO_MSG,\
								/*12*/    NO_MSG,\
								/*13*/    NO_MSG,\
								/*14*/    NO_MSG,\
								/*15*/    NO_MSG,\
								/*16*/    NO_MSG,\
								/*17*/    NO_MSG,\
								/*18*/    NO_MSG,\
								/*19*/    NO_MSG,\
								/*20*/    NO_MSG

#define IRFF00_USB_SLAVE_HOLD		\
                                /*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
								/*02*/    NO_MSG,\
								/*03*/    NO_MSG,\
								/*04*/    MSG_MUSIC_FR,\
								/*05*/    MSG_MUSIC_FF,\
								/*06*/    NO_MSG,\
								/*07*/    MSG_VOL_DOWN,\
								/*08*/    MSG_VOL_UP,\
								/*09*/    NO_MSG,\
                                /*10*/    NO_MSG,\
								/*11*/    NO_MSG,\
								/*12*/    NO_MSG,\
								/*13*/    NO_MSG,\
								/*14*/    NO_MSG,\
								/*15*/    NO_MSG,\
								/*16*/    NO_MSG,\
								/*17*/    NO_MSG,\
								/*18*/    NO_MSG,\
								/*19*/    NO_MSG,\
								/*20*/    NO_MSG


#define IRFF00_USB_SLAVE_LONG_UP	\
                                /*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
								/*02*/    NO_MSG,\
								/*03*/    NO_MSG,\
								/*04*/    NO_MSG,\
								/*05*/    NO_MSG,\
								/*06*/    NO_MSG,\
								/*07*/    NO_MSG,\
								/*08*/    NO_MSG,\
								/*09*/    NO_MSG,\
								/*10*/    NO_MSG,\
								/*11*/    NO_MSG,\
								/*12*/    NO_MSG,\
								/*13*/    NO_MSG,\
                                /*14*/    NO_MSG,\
								/*15*/    NO_MSG,\
								/*16*/    NO_MSG,\
								/*17*/    NO_MSG,\
								/*18*/    NO_MSG,\
								/*19*/    NO_MSG,\
								/*20*/    NO_MSG

#if (KEY_DOUBLE_CLICK_EN)
#define IRFF00_USB_SLAVE_DOUBLE_KICK	\
                                /*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
								/*02*/    NO_MSG,\
								/*03*/    NO_MSG,\
								/*04*/    NO_MSG,\
								/*05*/    NO_MSG,\
								/*06*/    NO_MSG,\
								/*07*/    NO_MSG,\
								/*08*/    NO_MSG,\
								/*09*/    NO_MSG,\
								/*10*/    NO_MSG,\
								/*11*/    NO_MSG,\
								/*12*/    NO_MSG,\
								/*13*/    NO_MSG,\
                                /*14*/    NO_MSG,\
								/*15*/    NO_MSG,\
								/*16*/    NO_MSG,\
								/*17*/    NO_MSG,\
								/*18*/    NO_MSG,\
								/*19*/    NO_MSG,\
								/*20*/    NO_MSG

#define IRFF00_USB_SLAVE_TRIPLE_KICK	\
                                /*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
								/*02*/    NO_MSG,\
								/*03*/    NO_MSG,\
								/*04*/    NO_MSG,\
								/*05*/    NO_MSG,\
								/*06*/    NO_MSG,\
								/*07*/    NO_MSG,\
								/*08*/    NO_MSG,\
								/*09*/    NO_MSG,\
								/*10*/    NO_MSG,\
								/*11*/    NO_MSG,\
								/*12*/    NO_MSG,\
								/*13*/    NO_MSG,\
                                /*14*/    NO_MSG,\
								/*15*/    NO_MSG,\
								/*16*/    NO_MSG,\
								/*17*/    NO_MSG,\
								/*18*/    NO_MSG,\
								/*19*/    NO_MSG,\
								/*20*/    NO_MSG
#endif

#define IRFF00_USB_SLAVE_SHORT		\
                                /*00*/   NO_MSG,\
							    /*01*/   NO_MSG,\
								/*02*/   NO_MSG,\
								/*03*/   NO_MSG,\
								/*04*/   NO_MSG,\
								/*05*/   NO_MSG,\
								/*06*/   NO_MSG,\
								/*07*/   NO_MSG,\
								/*08*/   NO_MSG,\
								/*09*/   NO_MSG,\
                                /*10*/   NO_MSG,\
								/*11*/   NO_MSG,\
								/*12*/   NO_MSG,\
								/*13*/   NO_MSG,\
								/*14*/   NO_MSG,\
								/*15*/   NO_MSG,\
								/*16*/   NO_MSG,\
								/*17*/   NO_MSG,\
								/*18*/   NO_MSG,\
								/*19*/   NO_MSG,\
								/*20*/   NO_MSG

const u16 irff00_msg_usb_slave_table[][IR_KEY_MAX_NUM] = {
    /*??????*/	    {IRFF00_USB_SLAVE_SHORT},
    /*????????????*/	{IRFF00_USB_SLAVE_SHORT_UP},
    /*??????*/		{IRFF00_USB_SLAVE_LONG},
    /*??????*/		{IRFF00_USB_SLAVE_HOLD},
    /*????????????*/	{IRFF00_USB_SLAVE_LONG_UP},
#if (KEY_DOUBLE_CLICK_EN)
    /*??????*/		{IRFF00_USB_SLAVE_DOUBLE_KICK},
    /*??????*/		{IRFF00_USB_SLAVE_TRIPLE_KICK},
#endif
};
#endif

#if KEY_IO_EN
#define IOKEY_USB_SLAVE_SHORT_UP \
							/*00*/		MSG_PP,\
							/*01*/		MSG_PREV_FILE,\
							/*02*/		MSG_NEXT_FILE,\
							/*03*/		MSG_NEXT_WORKMODE,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_USB_SLAVE_LONG \
							/*00*/		NO_MSG,\
							/*01*/		MSG_VOL_DOWN,\
							/*02*/		MSG_VOL_UP,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_USB_SLAVE_HOLD \
							/*00*/		NO_MSG,\
							/*01*/		MSG_VOL_DOWN,\
							/*02*/		MSG_VOL_UP,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_USB_SLAVE_LONG_UP \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#if (KEY_DOUBLE_CLICK_EN)
#define IOKEY_USB_SLAVE_DOUBLE_KICK \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_USB_SLAVE_TRIPLE_KICK \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\


#endif

#define IOKEY_USB_SLAVE_SHORT \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

const u16 iokey_msg_usb_slave_table[][IO_KEY_MAX_NUM] = {
    /*??????*/		{IOKEY_USB_SLAVE_SHORT},
    /*????????????*/	{IOKEY_USB_SLAVE_SHORT_UP},
    /*??????*/		{IOKEY_USB_SLAVE_LONG},
    /*??????*/		{IOKEY_USB_SLAVE_HOLD},
    /*????????????*/	{IOKEY_USB_SLAVE_LONG_UP},
#if (KEY_DOUBLE_CLICK_EN)
    /*??????*/		{IOKEY_USB_SLAVE_DOUBLE_KICK},
    /*??????*/        {IOKEY_USB_SLAVE_TRIPLE_KICK},
#endif
};
#endif

#if KEY_AD_EN
#define ADKEY_USB_SLAVE_SHORT_UP \
							/*00*/		MSG_PP,\
							/*01*/		MSG_PREV_FILE,\
							/*02*/		MSG_NEXT_FILE,\
							/*03*/		MSG_VOL_DOWN,\
							/*04*/		MSG_VOL_UP,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_USB_SLAVE_LONG \
							/*00*/		MSG_POWER_OFF,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		MSG_VOL_DOWN,\
							/*04*/		MSG_VOL_UP,\
							/*05*/		MSG_NEXT_WORKMODE,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_USB_SLAVE_HOLD \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		MSG_VOL_DOWN,\
							/*04*/		MSG_VOL_UP,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_USB_SLAVE_LONG_UP \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#if (KEY_DOUBLE_CLICK_EN)
#define ADKEY_USB_SLAVE_DOUBLE_KICK \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_USB_SLAVE_TRIPLE_KICK \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#endif

#define ADKEY_USB_SLAVE_SHORT \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

const u16 adkey_msg_usb_slave_table[][AD_KEY_MAX_NUM] = {
    /*??????*/		{ADKEY_USB_SLAVE_SHORT},
    /*????????????*/	{ADKEY_USB_SLAVE_SHORT_UP},
    /*??????*/		{ADKEY_USB_SLAVE_LONG},
    /*??????*/		{ADKEY_USB_SLAVE_HOLD},
    /*????????????*/	{ADKEY_USB_SLAVE_LONG_UP},
#if (KEY_DOUBLE_CLICK_EN)
    /*??????*/		{ADKEY_USB_SLAVE_DOUBLE_KICK},
    /*??????*/		{ADKEY_USB_SLAVE_TRIPLE_KICK},
#endif
};
#endif

u16 usb_slave_key_msg_filter(u8 key_status, u8 key_num, u8 key_type)
{
    u16 msg = NO_MSG;
    switch (key_type) {
#if KEY_IO_EN
    case KEY_TYPE_IO:
        msg = iokey_msg_usb_slave_table[key_status][key_num];
        break;
#endif
#if KEY_AD_EN
    case KEY_TYPE_AD:
        msg = adkey_msg_usb_slave_table[key_status][key_num];
        break;
#endif
#if KEY_IR_EN
    case KEY_TYPE_IR:
        msg = irff00_msg_usb_slave_table[key_status][key_num];
        break;
#endif
    default:
        break;
    }

    return msg;
}

