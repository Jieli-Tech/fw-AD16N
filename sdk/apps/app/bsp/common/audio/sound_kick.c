

#include "circular_buf.h"
#include "app_modules.h"
#include "sound_mge.h"
#include "sound_kick.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[sound_kick]"
#include "log.h"



AT(.audio_isr_text)
__attribute__((weak))
u32 audio_kick_size(sound_out_obj *psound)
{
    return cbuf_get_space(psound->p_obuf) * 2 / 3;
}

/* SEC(.docoder_mge.text.cache.L1) */
AT(.audio_isr_text)
void if_kick_decoder(sound_stream_obj *p_stream_in, sound_out_obj *psound, void *pkick)
{
    /* u32 kick_threshold = DAC_DECODER_KICK_SIZE; */
    u32 kick_threshold = audio_kick_size(psound);
    /* log_info("0"); */
    if (0 != psound->para) {
        if (cbuf_get_space(psound->p_obuf) > psound->para) {
            kick_threshold = psound->para;
        }
    }
    /* log_info("1"); */
    if (cbuf_get_data_size(psound->p_obuf) <= kick_threshold) {
        /* psound->enable |= B_DEC_KICK; */
        if (NULL != pkick) {
            void (*kick)(void *, void *);
            kick = pkick;
            /* log_info("2"); */
            kick(p_stream_in, psound);
        }
    }
    /* log_info("3"); */
}

AT(.audio_isr_text)
void sound_kick(void *sound_hld, void *pkick)
{
    sound_out_obj *psound = sound_hld;
    if (psound->enable & B_DEC_RUN_EN) {
        if (0 == (psound->enable & B_DEC_PAUSE)) {
            /* debug_putchar('K'); */
            if_kick_decoder(NULL, psound, pkick);
        }
    } else {
        if (0 == cbuf_get_data_size(psound->p_obuf)) {
            /* log_char('L'); */
            psound->enable &= ~B_DEC_OBUF_EN;
        } else {
            /* log_char('M'); */
        }
    }
}
