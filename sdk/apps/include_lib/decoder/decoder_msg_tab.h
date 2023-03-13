#ifndef __DECODER_MSG_TAB_H__
#define __DECODER_MSG_TAB_H__


#include "typedef.h"
#include "app_modules.h"

enum {
#if DECODER_F1A_EN
    INDEX_F1A1 = 0,
#if (MAX_F1A_CHANNEL > 1)
    INDEX_F1A2,
#endif
#endif
#if DECODER_UMP3_EN
    INDEX_UMP3,
#endif
#if DECODER_A_EN
    INDEX_A,
#endif
#if DECODER_MIDI_EN
    INDEX_MIDI,
#endif
#if DECODER_MIDI_KEYBOARD_EN
    INDEX_MIDI_CTRL,
#endif
#if DECODER_WAV_EN
    INDEX_WAV,
#endif
#if DECODER_MP3_ST_EN
    INDEX_MP3_ST,
#endif
    INDEX_E_SPEED = 12,
    INDEX_E_EQ = 13,
};

#define BIT_SPEED  BIT(INDEX_E_SPEED)
#define BIT_EQ     BIT(INDEX_E_EQ)

#if DECODER_MP3_ST_EN
#include "mp3_standard_api.h"
#define MP3_ST_HLD      (u32)&dec_mp3_st_hld
#define MP3_ST_LST      MP3_ST_HLD,
#define MP3_ST_API      (u32)mp3_st_decode_api,
#define MP3_ST_MUT_TAB  (u32)mp3_st_buff_api,
#define BIT_MP3_ST      BIT(INDEX_MP3_ST)
#else
#define MP3_ST_HLD  (u32)NULL
#define MP3_ST_LST
#define MP3_ST_API
#define MP3_ST_MUT_TAB
#define BIT_MP3_ST    0
#endif

#if DECODER_UMP3_EN
#define UMP3_HLD      (u32)&dec_ump3_hld
#define UMP3_LST      UMP3_HLD,
#define UMP3_API      (u32)ump3_decode_api,
#define UMP3_MUT_TAB  (u32)ump3_buff_api,
#define BIT_UMP3      BIT(INDEX_UMP3)
#else
#define UMP3_HLD  (u32)NULL
#define UMP3_LST
#define UMP3_API
#define UMP3_MUT_TAB
#define BIT_UMP3    0
#endif

#if DECODER_WAV_EN
#include "wav_api.h"
#define WAV_HLD      	(u32)&dec_wav_hld
#define WAV_LST      	WAV_HLD,
#define WAV_API      	(u32)wav_decode_api,
#define WAV_MUT_TAB  	(u32)wav_buff_api,
#define BIT_WAV			BIT(INDEX_WAV)
#else
#define WAV_HLD  		(u32)NULL
#define WAV_LST
#define WAV_API
#define WAV_MUT_TAB
#define BIT_WAV			0
#endif


#if DECODER_F1A_EN
#define F1A1_LST      F1A1_HLD,
#define F1A1_API      (u32)f1a_decode_api_1,
#define F1A1_MUT_TAB  (u32)f1a_1_buff_api,

#if (1 == MAX_F1A_CHANNEL)
#define F1A2_LST
#define F1A2_API
#define F1A2_MUT_TAB
#define F1A2_HLD
#else
#define F1A2_LST       F1A2_HLD,
#define F1A2_API      (u32)f1a_decode_api_2,
#define F1A2_MUT_TAB  (u32)f1a_2_buff_api,
#define F1A2_HLD  (u32)&dec_f1a_hld[1]
#endif

#define F1A1_HLD  (u32)&dec_f1a_hld[0]
#define BIT_F1A1   BIT(INDEX_F1A1)
#define BIT_F1A2   BIT(INDEX_F1A2)
#else
#define F1A1_LST
#define F1A1_API
#define F1A1_MUT_TAB
#define F1A2_LST
#define F1A2_API
#define F1A2_MUT_TAB
#define F1A1_HLD  (u32)NULL
#define F1A2_HLD  (u32)NULL
#define BIT_F1A1   0
#define BIT_F1A2   0
//
#endif

#if DECODER_MIDI_EN
#include "midi_api.h"
#define MIDI_LST      MIDI_HLD,
#define MIDI_API      (u32)midi_decode_api,
#define MIDI_MUT_TAB  (u32)midi_buff_api,
#define MIDI_HLD      (u32)&dec_midi_hld
#define BIT_MIDI      BIT(INDEX_MIDI)
#else
#define MIDI_LST
#define MIDI_API
#define MIDI_MUT_TAB
#define MIDI_HLD  (u32)NULL
#define BIT_MIDI   0
#endif

#if DECODER_MIDI_KEYBOARD_EN
#include "midi_ctrl.h"
#define MIDI_CTRL_LST      MIDI_CTRL_HLD,
#define MIDI_CTRL_API      (u32)midi_ctrl_decode_api,
#define MIDI_CTRL_MUT_TAB  (u32)midi_ctrl_buff_api,
#define MIDI_CTRL_HLD      (u32)&dec_midi_ctrl_hld
#define BIT_MIDI_CTRL      BIT(INDEX_MIDI_CTRL)
#else
#define MIDI_CTRL_LST
#define MIDI_CTRL_API
#define MIDI_CTRL_MUT_TAB
#define MIDI_CTRL_HLD      (u32)NULL
#define BIT_MIDI_CTRL      0
#endif

#if DECODER_A_EN
#define A_LST      A_HLD,
#define A_API      (u32)a_decode_api,
#define A_MUT_TAB  (u32)a_buff_api,
#define A_HLD      (u32)&dec_a_hld
#define BIT_A      BIT(INDEX_A)
#else
#define A_LST
#define A_API
#define A_MUT_TAB
#define A_HLD     (u32)NULL
#define BIT_A      0
#endif


// #define MAX_F1A_CHANNEL 1
// extern const u8 a_evt[10];
// extern const u8 f1a_evt[MAX_F1A_CHANNEL][10];
// extern const u8 midi_evt[10];
// extern const u8 ump3_evt[10];
// extern const u8 mp3_st_evt[10];
// extern const u8 wav_evt[10];




#endif


