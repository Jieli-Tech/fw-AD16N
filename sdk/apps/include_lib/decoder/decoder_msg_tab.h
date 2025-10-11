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
#if DECODER_OPUS_EN
    INDEX_OPUS,
#endif
#if DECODER_IMA_EN
    INDEX_IMA,
#endif
#if DECODER_SPEEX_EN
    INDEX_SPEEX,
#endif
#if DECODER_SBC_EN
    INDEX_SBC,
#endif
#if DECODER_JLA_LW_EN
    INDEX_JLA_LW,
#endif

    INDEX_E_SPEED   = 12,
    INDEX_E_EQ      = 13,
    INDEX_E_SRC_FORCE = 14,
};

#define BIT_SPEED       BIT(INDEX_E_SPEED)
#define BIT_EQ          BIT(INDEX_E_EQ)
#define BIT_SRC_FORCE   BIT(INDEX_E_SRC_FORCE)

#if DECODER_MP3_ST_EN
#include "mp3_standard_api.h"
#define MP3_ST_HLD      (u32)&dec_mp3_st_hld
#define MP3_ST_LST      MP3_ST_HLD,
#define MP3_ST_API      (u32)mp3_st_decode_api,
#define MP3_ST_MUT_TAB  (u32)mp3_st_buff_api,
#define MP3_ST_PARM_SET (u32)NULL,
#define BIT_MP3_ST      BIT(INDEX_MP3_ST)
#else
#define MP3_ST_HLD  (u32)NULL
#define MP3_ST_LST
#define MP3_ST_API
#define MP3_ST_MUT_TAB
#define MP3_ST_PARM_SET
#define BIT_MP3_ST    0
#endif

#if DECODER_UMP3_EN
#include "ump3_api.h"
#define UMP3_HLD      (u32)&dec_ump3_hld
#define UMP3_LST      UMP3_HLD,
#define UMP3_API      (u32)ump3_decode_api,
#define UMP3_MUT_TAB  (u32)ump3_buff_api,
#define UMP3_PARM_SET (u32)set_ump2_Headerstate_api,
#define BIT_UMP3      BIT(INDEX_UMP3)
#else
#define UMP3_HLD  (u32)NULL
#define UMP3_LST
#define UMP3_API
#define UMP3_MUT_TAB
#define UMP3_PARM_SET
#define BIT_UMP3    0
#endif

#if DECODER_WAV_EN
#include "wav_api.h"
#define WAV_HLD      	(u32)&dec_wav_hld
#define WAV_LST      	WAV_HLD,
#define WAV_API      	(u32)wav_decode_api,
#define WAV_MUT_TAB  	(u32)wav_buff_api,
#define WAV_PARM_SET  	(u32)NULL,
#define BIT_WAV			BIT(INDEX_WAV)
#else
#define WAV_HLD  		(u32)NULL
#define WAV_LST
#define WAV_API
#define WAV_MUT_TAB
#define WAV_PARM_SET
#define BIT_WAV			0
#endif


#if DECODER_F1A_EN
#include "f1a_api.h"
#define F1A1_LST      F1A1_HLD,
#define F1A1_API      (u32)f1a_decode_api_1,
#define F1A1_MUT_TAB  (u32)f1a_1_buff_api,
#define F1A1_PARM_SET (u32)NULL,

#if (1 == MAX_F1A_CHANNEL)
#define F1A2_LST
#define F1A2_API
#define F1A2_MUT_TAB
#define F1A2_PARM_SET
#define F1A2_HLD
#else
#define F1A2_LST       F1A2_HLD,
#define F1A2_API      (u32)f1a_decode_api_2,
#define F1A2_MUT_TAB  (u32)f1a_2_buff_api,
#define F1A2_PARM_SET (u32)NULL,
#define F1A2_HLD  (u32)&dec_f1a_hld[1]
#endif

#define F1A1_HLD  (u32)&dec_f1a_hld[0]
#define BIT_F1A1   BIT(INDEX_F1A1)
#define BIT_F1A2   BIT(INDEX_F1A2)
#else
#define F1A1_LST
#define F1A1_API
#define F1A1_MUT_TAB
#define F1A1_PARM_SET
#define F1A2_LST
#define F1A2_API
#define F1A2_MUT_TAB
#define F1A2_PARM_SET
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
#define MIDI_PARM_SET (u32)NULL,
#define MIDI_HLD      (u32)&dec_midi_hld
#define BIT_MIDI      BIT(INDEX_MIDI)
#else
#define MIDI_LST
#define MIDI_API
#define MIDI_MUT_TAB
#define MIDI_PARM_SET
#define MIDI_HLD  (u32)NULL
#define BIT_MIDI   0
#endif

#if DECODER_MIDI_KEYBOARD_EN
#include "midi_ctrl.h"
#define MIDI_CTRL_LST      MIDI_CTRL_HLD,
#define MIDI_CTRL_API      (u32)midi_ctrl_decode_api,
#define MIDI_CTRL_MUT_TAB  (u32)midi_ctrl_buff_api,
#define MIDI_CTRL_PARM_SET (u32)NULL,
#define MIDI_CTRL_HLD      (u32)&dec_midi_ctrl_hld
#define BIT_MIDI_CTRL      BIT(INDEX_MIDI_CTRL)
#else
#define MIDI_CTRL_LST
#define MIDI_CTRL_API
#define MIDI_CTRL_MUT_TAB
#define MIDI_CTRL_PARM_SET
#define MIDI_CTRL_HLD      (u32)NULL
#define BIT_MIDI_CTRL      0
#endif

#if DECODER_A_EN
#include "a_api.h"
#define A_LST      A_HLD,
#define A_API      (u32)a_decode_api,
#define A_MUT_TAB  (u32)a_buff_api,
#define A_PARM_SET (u32)NULL,
#define A_HLD      (u32)&dec_a_hld
#define BIT_A      BIT(INDEX_A)
#else
#define A_LST
#define A_API
#define A_MUT_TAB
#define A_PARM_SET
#define A_HLD     (u32)NULL
#define BIT_A      0
#endif

#if DECODER_OPUS_EN
#include "opus_api.h"
#define OPUS_LST      OPUS_HLD,
#define OPUS_API      (u32)opus_decode_api,
#define OPUS_MUT_TAB  (u32)opus_buff_api,
#define OPUS_PARM_SET (u32)set_opus_Headerstate_api,
#define OPUS_HLD      (u32)&dec_opus_hld
#define BIT_OPUS      BIT(INDEX_OPUS)
#else
#define OPUS_LST
#define OPUS_API
#define OPUS_MUT_TAB
#define OPUS_PARM_SET
#define OPUS_HLD     (u32)NULL
#define BIT_OPUS      0
#endif

#if DECODER_IMA_EN
#include "ima_api.h"
#define IMA_LST  	 IMA_HLD,
#define IMA_API      (u32)ima_decode_api,
#define IMA_MUT_TAB  (u32)ima_buff_api,
#define IMA_PARM_SET (u32)set_ima_Headerstate_api,
#define IMA_HLD      (u32)&dec_ima_hld
#define BIT_IMA      BIT(INDEX_IMA)
#else
#define IMA_LST
#define IMA_API
#define IMA_MUT_TAB
#define IMA_PARM_SET
#define IMA_HLD     (u32)NULL
#define BIT_IMA      0
#endif

#if DECODER_SPEEX_EN
#include "speex_api.h"
#define SPEEX_LST  	   SPEEX_HLD,
#define SPEEX_API      (u32)speex_decode_api,
#define SPEEX_MUT_TAB  (u32)speex_buff_api,
#define SPEEX_PARM_SET (u32)set_speex_Headerstate_api,
#define SPEEX_HLD      (u32)&dec_speex_hld
#define BIT_SPEEX      BIT(INDEX_SPEEX)
#else
#define SPEEX_LST
#define SPEEX_API
#define SPEEX_MUT_TAB
#define SPEEX_PARM_SET
#define SPEEX_HLD     (u32)NULL
#define BIT_SPEEX      0
#endif

#if DECODER_SBC_EN
#include "sbc_api.h"
#define SBC_LST  	 SBC_HLD,
#define SBC_API      (u32)sbc_decode_api,
#define SBC_MUT_TAB  (u32)sbc_buff_api,
#define SBC_PARM_SET (u32)NULL,
#define SBC_HLD      (u32)&dec_sbc_hld
#define BIT_SBC      BIT(INDEX_SBC)
#else
#define SBC_LST
#define SBC_API
#define SBC_MUT_TAB
#define SBC_PARM_SET
#define SBC_HLD     (u32)NULL
#define BIT_SBC      0
#endif

#if DECODER_JLA_LW_EN
#include "jla_lw_api.h"
#define JLA_LW_HLD      (u32)&dec_jla_lw_hld
#define JLA_LW_LST      JLA_LW_HLD,
#define JLA_LW_API      (u32)jla_lw_decode_api,
#define JLA_LW_MUT_TAB  (u32)jla_lw_buff_api,
#define JLA_LW_PARM_SET (u32)NULL,
#define BIT_JLA_LW      BIT(INDEX_JLA_LW)
#else
#define JLA_LW_HLD      (u32)NULL
#define JLA_LW_LST
#define JLA_LW_API
#define JLA_LW_MUT_TAB
#define JLA_LW_PARM_SET
#define BIT_JLA_LW      0
#endif

// #define MAX_F1A_CHANNEL 1
// extern const u8 a_evt[10];
// extern const u8 f1a_evt[MAX_F1A_CHANNEL][10];
// extern const u8 midi_evt[10];
// extern const u8 ump3_evt[10];
// extern const u8 mp3_st_evt[10];
// extern const u8 wav_evt[10];




#endif


