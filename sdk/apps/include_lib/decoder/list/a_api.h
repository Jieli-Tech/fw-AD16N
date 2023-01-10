#ifndef __A_API_H__
#define __A_API_H__
#include "if_decoder_ctrl.h"


#include "decoder_mge.h"
extern dec_obj dec_a_hld ;

extern const u8 a_evt[10];
u32 a_decode_api(void *p_file, void **p_dec, void *p_dp_buf);
u32 a_buff_api(dec_buf *p_dec_buf);
u32 get_a_dp_buff_size(void);
#endif


