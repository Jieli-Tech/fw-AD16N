#ifndef __UMP3_API_H__
#define __UMP3_API_H__
#include "if_decoder_ctrl.h"

#include "decoder_mge.h"
extern dec_obj dec_ump3_hld ;
extern const u8 ump3_evt[10];
u32 ump3_decode_api(void *p_file, void **p_dec, void *p_dp_buf);
u32 ump3_buff_api(dec_buf *p_dec_buf);
#endif


