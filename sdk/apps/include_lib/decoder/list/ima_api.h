#ifndef __ADPCM_API_H__
#define __ADPCM_API_H__
#include "if_decoder_ctrl.h"

#include "decoder_mge.h"
extern dec_obj dec_ima_hld;

#define IMA_DEC_INPUT_MAX_SIZE    (64 * 2)
#define IMA_DEC_OUTPUT_MAX_SIZE   (32 * 2)

extern const u8 ima_evt[10];
u32 ima_decode_api(void *strm, void **p_dec, void *p_dp_buf);
u32 ima_buff_api(dec_buf *p_dec_buf);
void set_ima_input_goon_cb(int (*ima_goon_cb)(void *));
u32 set_ima_Headerstate_api(u32 sr, u32 br, int (*ima_goon_cb)(void *));
#endif


