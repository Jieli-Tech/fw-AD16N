#ifndef __A_API_H__
#define __A_API_H__
#include "if_decoder_ctrl.h"

#define A_DEC_INPUT_MAX_SIZE    (64 * 2)
#define A_DEC_OUTPUT_MAX_SIZE   (32 * 2)

#include "decoder_mge.h"
extern dec_obj dec_a_hld ;

extern const u8 a_evt[10];
u32 a_decode_api(void *strm, void **p_dec, void *p_dp_buf);
u32 a_buff_api(dec_buf *p_dec_buf);
u32 get_a_dp_buff_size(void);
void clear_a_dp_buff(void);
#endif


