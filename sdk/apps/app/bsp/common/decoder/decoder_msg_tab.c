

#pragma bss_seg(".decoder_msg_tab.data.bss")
#pragma data_seg(".decoder_msg_tab.data")
#pragma const_seg(".decoder_msg_tab.text.const")
#pragma code_seg(".decoder_msg_tab.text")
#pragma str_literal_override(".decoder_msg_tab.text.const")

#include "app_modules.h"
#include "typedef.h"
#include "msg.h"
#include "decoder_msg_tab.h"


#if DECODER_A_EN
const u8 a_evt[10] = {
    EVENT_A_END,
    0xff,
    0xff,
    EVENT_A_ERR,
    0xff,
    0xff,
    0xff,
    0xff,
    EVENT_A_END,
    EVENT_A_LOOP,
};
#endif

#if DECODER_F1A_EN
const u8 f1a_evt[MAX_F1A_CHANNEL][10] = {
    {
        EVENT_F1A1_END,
        0xff,
        0xff,
        EVENT_F1A1_ERR,
        0xff,
        0xff,
        0xff,
        0xff,
        EVENT_F1A1_END,
        EVENT_F1A1_LOOP,
    },
#if (MAX_F1A_CHANNEL>1)
    {
        EVENT_F1A2_END,
        0xff,
        0xff,
        EVENT_F1A2_ERR,
        0xff,
        0xff,
        0xff,
        0xff,
        EVENT_F1A2_END,
        EVENT_F1A2_LOOP,
    },
#endif
};
#endif

#if (DECODER_MIDI_EN || DECODER_MIDI_KEYBOARD_EN)
const u8 midi_evt[10] = {
    EVENT_MIDI_END,
    0xff,
    0xff,
    EVENT_MIDI_ERR,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
};
#endif

#if DECODER_UMP3_EN
const u8 ump3_evt[10] = {
    EVENT_MP3_END,
    0xff,
    0xff,
    EVENT_MP3_ERR,
    0xff,
    0xff,
    0xff,
    0xff,
    EVENT_MP3_END,
    EVENT_MP3_LOOP,
};
#endif

#if DECODER_MP3_ST_EN
const u8 mp3_st_evt[10] = {
    EVENT_MP3_END,
    0xff,
    0xff,
    EVENT_MP3_ERR,
    0xff,
    0xff,
    0xff,
    0xff,
    EVENT_MP3_END,
    EVENT_MP3_LOOP,
};
#endif

#if DECODER_WAV_EN
const u8 wav_evt[10] = {
    EVENT_WAV_END,
    0xff,
    0xff,
    EVENT_WAV_ERR,
    0xff,
    0xff,
    0xff,
    0xff,
    EVENT_WAV_END,
    EVENT_WAV_LOOP,
};
#endif



