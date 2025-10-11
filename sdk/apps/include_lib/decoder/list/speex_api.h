#ifndef __SPEEX_API_H__
#define __SPEEX_API_H__
#include "if_decoder_ctrl.h"

#include "decoder_mge.h"
extern dec_obj dec_speex_hld;

#define SPEEX_DEC_INPUT_MAX_SIZE    (64 * 2)
#define SPEEX_DEC_OUTPUT_MAX_SIZE   (32 * 2)

extern const u8 speex_evt[10];
u32 speex_decode_api(void *strm, void **p_dec, void *p_dp_buf);
u32 speex_buff_api(dec_buf *p_dec_buf);
u32 set_speex_Headerstate_api(u32 sr, u32 br, int (*opus_goon_cb)(void *));

#endif


