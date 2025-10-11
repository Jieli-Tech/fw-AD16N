#ifndef __OPUS_API_H__
#define __OPUS_API_H__
#include "if_decoder_ctrl.h"

#define OPUS_DEC_INPUT_MAX_SIZE    (320)
#define OPUS_DEC_OUTPUT_MAX_SIZE   (320 * 2)

#include "decoder_mge.h"
extern dec_obj dec_opus_hld ;

extern const u8 opus_evt[10];
u32 opus_decode_init(void *strm, void **p_dec, void *p_dp_buf, void *input_func, void *output_func);
u32 opus_decode_api(void *strm, void **p_dec, void *p_dp_buf);
u32 opus_buff_api(dec_buf *p_dec_buf);
void set_opus_input_goon_cb(int (*opus_goon_cb)(void *));
u32 set_opus_Headerstate_api(u32 sr, u32 br, int (*opus_goon_cb)(void *));
#endif


