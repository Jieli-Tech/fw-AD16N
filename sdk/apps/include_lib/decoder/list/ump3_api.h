#ifndef __UMP3_API_H__
#define __UMP3_API_H__
#include "if_decoder_ctrl.h"
#include "lib_ump3.h"
#include "decoder_mge.h"
extern dec_obj dec_ump3_hld ;
extern const u8 ump3_evt[10];

#define UMP3_DEC_INPUT_MAX_SIZE     (128)
#define UMP3_DEC_OUTPUT_MAX_SIZE    (32 * 2)

u32 ump3_decode_init(void *strm, void **p_dec, void *p_dp_buf, void *input_func, void *output_func);
u32 ump3_decode_api(void *strm, void **p_dec, void *p_dp_buf);
u32 ump3_buff_api(dec_buf *p_dec_buf);
void ump3_mp_input_cb_sel(u32 ump3_mp_input_func(void *, u32, void *, int, u8));
u32 set_ump2_Headerstate_api(u32 sr, u32 br, int (*ump2_goon_cb)(void *));
#endif


