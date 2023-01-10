#ifndef __AUDIO_EQ_API_H__
#define __AUDIO_EQ_API_H__
#include "typedef.h"
#include "hw_eq.h"
#include "sound_mge.h"

typedef struct __AUDIO_EQ_INFO {
    float *coeff;
    u16 sample_rate;
    u8 output_ch;
    u8 eq_mode;
    u8 eq_enable;
} AUDIO_EQ_INFO;

#define EQ_SECTION_MAX  10
#define EQ_COEFF_BASE   (0x2F0000)

#define EQ_RUN_MODE         NORMAL
#define EQ_IN_FORMAT        DATI_SHORT
#define EQ_OUT_FORMAT       DATO_SHORT
#define EQ_DATA_IN_MODE     SEQUENCE_DAT_IN
#define EQ_DATA_OUT_MODE    SEQUENCE_DAT_OUT

#define B_EQ_START          BIT(0)
#define B_EQ_FADE_IN        BIT(1)
#define B_EQ_FADE_OUT       BIT(2)

void audio_eq_init_api(void);
AUDIO_EQ_INFO *audio_eq_open_api(u8 ch, u16 sr);
void audio_eq_run_api(void *data_in, void *data_out, u32 len, bool stereo);
void audio_eq_close_api(void);
void audio_eq_switch_tab(void);
#endif
