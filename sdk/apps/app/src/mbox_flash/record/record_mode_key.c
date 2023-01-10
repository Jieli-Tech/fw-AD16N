#pragma bss_seg(".record_mode_key.data.bss")
#pragma data_seg(".record_mode_key.data")
#pragma const_seg(".record_mode_key.text.const")
#pragma code_seg(".record_mode_key.text")
#pragma str_literal_override(".record_mode_key.text.const")

#include "typedef.h"
#include "key.h"
#include "msg.h"
#include "app_config.h"

#if KEY_IO_EN
#define IOKEY_RECORD_SHORT_UP \
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

#define IOKEY_RECORD_LONG \
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

#define IOKEY_RECORD_HOLD \
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

#define IOKEY_RECORD_LONG_UP \
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
#define IOKEY_RECORD_DOUBLE_KICK \
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

#define IOKEY_RECORD_TRIPLE_KICK \
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

#define IOKEY_RECORD_SHORT \
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

const u16 iokey_msg_record_table[][IO_KEY_MAX_NUM] = {
    /*短按*/		{IOKEY_RECORD_SHORT},
    /*短按抬起*/	{IOKEY_RECORD_SHORT_UP},
    /*长按*/		{IOKEY_RECORD_LONG},
    /*连按*/		{IOKEY_RECORD_HOLD},
    /*长按抬起*/	{IOKEY_RECORD_LONG_UP},
#if (KEY_DOUBLE_CLICK_EN)
    /*双击*/		{IOKEY_RECORD_DOUBLE_KICK},
    /*三击*/        {IOKEY_RECORD_TRIPLE_KICK},
#endif
};
#endif

#if KEY_AD_EN
#define ADKEY_RECORD_SHORT_UP \
							/*00*/		MSG_RECODE_START,\
							/*01*/		MSG_PP,\
							/*02*/		MSG_REC_SPEED_EN,\
							/*03*/		MSG_VOL_DOWN,\
							/*04*/		MSG_VOL_UP,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_RECORD_LONG \
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

#define ADKEY_RECORD_HOLD \
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

#define ADKEY_RECORD_LONG_UP \
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
#define ADKEY_RECORD_DOUBLE_KICK \
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

#define ADKEY_RECORD_TRIPLE_KICK \
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

#define ADKEY_RECORD_SHORT \
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

const u16 adkey_msg_record_table[][AD_KEY_MAX_NUM] = {
    /*短按*/		{ADKEY_RECORD_SHORT},
    /*短按抬起*/	{ADKEY_RECORD_SHORT_UP},
    /*长按*/		{ADKEY_RECORD_LONG},
    /*连按*/		{ADKEY_RECORD_HOLD},
    /*长按抬起*/	{ADKEY_RECORD_LONG_UP},
#if (KEY_DOUBLE_CLICK_EN)
    /*双击*/		{ADKEY_RECORD_DOUBLE_KICK},
    /*双击*/		{ADKEY_RECORD_TRIPLE_KICK},
#endif
};
#endif

u16 record_key_msg_filter(u8 key_status, u8 key_num, u8 key_type)
{
    u16 msg = NO_MSG;
    switch (key_type) {
#if KEY_IO_EN
    case KEY_TYPE_IO:
        msg = iokey_msg_record_table[key_status][key_num];
        break;
#endif
#if KEY_AD_EN
    case KEY_TYPE_AD:
        msg = adkey_msg_record_table[key_status][key_num];
        break;
#endif
    default:
        break;
    }
    return msg;
}
