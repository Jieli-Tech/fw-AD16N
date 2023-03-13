/* #pragma bss_seg(".rtc_mode_key.data.bss") */
/* #pragma data_seg(".rtc_mode_key.data") */
/* #pragma const_seg(".rtc_mode_key.text.const") */
/* #pragma code_seg(".rtc_mode_key.text") */
/* #pragma str_literal_override(".rtc_mode_key.text.const") */
/*  */
#include "typedef.h"
#include "key.h"
#include "msg.h"
#include "app_config.h"
#if KEY_AD_EN
#define ADKEY_RTC_SHORT_UP \
							/*00*/		MSG_READ_CLOCK,\
							/*01*/		MSG_WRITE_CLOCK,\
							/*02*/		MSG_READ_ALARM,\
							/*03*/		MSG_WRITE_ALARM,\
							/*04*/		MSG_TIME_WAKEUP,\
							/*05*/		MSG_ALARM,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_RTC_LONG \
							/*00*/		MSG_POWER_OFF,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		MSG_NEXT_WORKMODE,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_RTC_HOLD \
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

#define ADKEY_RTC_LONG_UP \
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
#define ADKEY_RTC_DOUBLE_KICK \
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

#define ADKEY_RTC_TRIPLE_KICK \
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

#define ADKEY_RTC_SHORT \
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

const u16 adkey_msg_rtc_table[][AD_KEY_MAX_NUM] = {
    /*短按*/		{ADKEY_RTC_SHORT},
    /*短按抬起*/	{ADKEY_RTC_SHORT_UP},
    /*长按*/		{ADKEY_RTC_LONG},
    /*连按*/		{ADKEY_RTC_HOLD},
    /*长按抬起*/	{ADKEY_RTC_LONG_UP},
#if (KEY_DOUBLE_CLICK_EN)
    /*双击*/		{ADKEY_RTC_DOUBLE_KICK},
    /*双击*/		{ADKEY_RTC_TRIPLE_KICK},
#endif
};
#endif

u16 rtc_key_msg_filter(u8 key_status, u8 key_num, u8 key_type)
{
    u16 msg = NO_MSG;
    switch (key_type) {
#if KEY_AD_EN
    case KEY_TYPE_AD:
        msg = adkey_msg_rtc_table[key_status][key_num];
        break;
#endif
    default:
        break;
    }
    return msg;
}

