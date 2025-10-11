#ifndef __SBC_API_H__
#define __SBC_API_H__
#include "if_decoder_ctrl.h"

#include "decoder_mge.h"
extern dec_obj dec_sbc_hld;

#define SBC_DEC_INPUT_MAX_SIZE    (64 * 2)
#define SBC_DEC_OUTPUT_MAX_SIZE   (32 * 2)

extern const u8 sbc_evt[10];
u32 sbc_decode_api(void *strm, void **p_dec, void *p_dp_buf);
u32 sbc_buff_api(dec_buf *p_dec_buf);
u32 set_sbc_Headerstate_api(u32 sr, u32 br, int (*ima_goon_cb)(void *));
#endif



