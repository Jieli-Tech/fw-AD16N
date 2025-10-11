#ifndef __WAV_API_H__
#define __WAV_API_H__
#include "if_decoder_ctrl.h"

#include "decoder_mge.h"

extern dec_obj dec_wav_hld;
extern const u8 wav_evt[10];

u32 wav_decode_api(void *strm, void **p_dec, void *p_dp_buf);
void set_wav_repeat(void *work_buf, repeat_mode_flag *repeat_flag);
u32 wav_buff_api(dec_buf *p_dec_buf);

#endif


