
#ifndef __JLA_LW_API_H__
#define __JLA_LW_API_H__

#include "typedef.h"
#include "decoder/if_decoder_ctrl.h"
#include "decoder_mge.h"

extern dec_obj dec_jla_lw_hld;

u32 jla_lw_decoder_open(void *work_buf, const struct if_decoder_io *decoder_io, u8 *bk_point_ptr);
u32 jla_lw_decoder_run(void *work_buf, u32 type);

extern const u8 jla_lw_evt[10];
u32 jla_lw_buff_api(dec_buf *p_dec_buf);
u32 jla_lw_decode_api(void *strm, void **p_dec, void *p_dp_buf);

#endif


