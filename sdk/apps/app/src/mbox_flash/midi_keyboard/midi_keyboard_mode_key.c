#pragma bss_seg(".midi_keyboard_mode_key.data.bss")
#pragma data_seg(".midi_keyboard_mode_key.data")
#pragma const_seg(".midi_keyboard_mode_key.text.const")
#pragma code_seg(".midi_keyboard_mode_key.text")
#pragma str_literal_override(".midi_keyboard_mode_key.text.const")

#include "typedef.h"
#include "key.h"
#include "msg.h"
#include "app_config.h"

#if KEY_IO_EN
#define IOKEY_MIDI_KEYBOARD_SHORT_UP \
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

#define IOKEY_MIDI_KEYBOARD_LONG \
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

#define IOKEY_MIDI_KEYBOARD_HOLD \
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

#define IOKEY_MIDI_KEYBOARD_LONG_UP \
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
#define IOKEY_MIDI_KEYBOARD_DOUBLE_KICK \
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

#define IOKEY_MIDI_KEYBOARD_TRIPLE_KICK \
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

#define IOKEY_MIDI_KEYBOARD_SHORT \
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

const u16 iokey_msg_midi_keyboard_table[][IO_KEY_MAX_NUM] = {
    /*短按*/		{IOKEY_MIDI_KEYBOARD_SHORT},
    /*短按抬起*/	{IOKEY_MIDI_KEYBOARD_SHORT_UP},
    /*长按*/		{IOKEY_MIDI_KEYBOARD_LONG},
    /*连按*/		{IOKEY_MIDI_KEYBOARD_HOLD},
    /*长按抬起*/	{IOKEY_MIDI_KEYBOARD_LONG_UP},
#if (KEY_DOUBLE_CLICK_EN)
    /*双击*/		{IOKEY_MIDI_KEYBOARD_DOUBLE_KICK},
    /*三击*/        {IOKEY_MIDI_KEYBOARD_TRIPLE_KICK},
#endif
};
#endif

#if KEY_AD_EN
#define ADKEY_MIDI_KEYBOARD_SHORT_UP \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		MSG_MIDICTRL_PITCH_BEND_DOWN,\
							/*05*/		MSG_MIDICTRL_PITCH_BEND_UP,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_MIDI_KEYBOARD_LONG \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		MSG_MIDICTRL_CHANNAL_NEXT,\
							/*05*/		MSG_NEXT_WORKMODE,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define ADKEY_MIDI_KEYBOARD_HOLD \
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

#define ADKEY_MIDI_KEYBOARD_LONG_UP \
							/*00*/		MSG_MIDICTRL_NOTE_OFF_DO,\
							/*01*/		MSG_MIDICTRL_NOTE_OFF_RE,\
							/*02*/		MSG_MIDICTRL_NOTE_OFF_MI,\
							/*03*/		MSG_MIDICTRL_NOTE_OFF_FA,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#if (KEY_DOUBLE_CLICK_EN)
#define ADKEY_MIDI_KEYBOARD_DOUBLE_KICK \
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

#define ADKEY_MIDI_KEYBOARD_TRIPLE_KICK \
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

#define ADKEY_MIDI_KEYBOARD_SHORT \
							/*00*/		MSG_MIDICTRL_NOTE_ON_DO,\
							/*01*/		MSG_MIDICTRL_NOTE_ON_RE,\
							/*02*/		MSG_MIDICTRL_NOTE_ON_MI,\
							/*03*/		MSG_MIDICTRL_NOTE_ON_FA,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

const u16 adkey_msg_midi_keyboard_table[][AD_KEY_MAX_NUM] = {
    /*短按*/		{ADKEY_MIDI_KEYBOARD_SHORT},
    /*短按抬起*/	{ADKEY_MIDI_KEYBOARD_SHORT_UP},
    /*长按*/		{ADKEY_MIDI_KEYBOARD_LONG},
    /*连按*/		{ADKEY_MIDI_KEYBOARD_HOLD},
    /*长按抬起*/	{ADKEY_MIDI_KEYBOARD_LONG_UP},
#if (KEY_DOUBLE_CLICK_EN)
    /*双击*/		{ADKEY_MIDI_KEYBOARD_DOUBLE_KICK},
    /*双击*/		{ADKEY_MIDI_KEYBOARD_TRIPLE_KICK},
#endif
};
#endif

u16 midi_keyboard_key_msg_filter(u8 key_status, u8 key_num, u8 key_type)
{
    u16 msg = NO_MSG;
    switch (key_type) {
#if KEY_IO_EN
    case KEY_TYPE_IO:
        msg = iokey_msg_midi_keyboard_able[key_status][key_num];
        break;
#endif
#if KEY_AD_EN
    case KEY_TYPE_AD:
        msg = adkey_msg_midi_keyboard_table[key_status][key_num];
        break;
#endif
    default:
        break;
    }
    return msg;
}

