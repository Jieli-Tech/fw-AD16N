#ifndef __UMP3_ENCODER_H__
#define __UMP3_ENCODER_H__

#include "typedef.h"

#define UMP2_ENC_OUTPUT_MAX_SIZE     (256)//byte

u32 ump3_encode_api(void *p_file, void *input_func, void *output_func);
void ump3_enc_output_cb_sel(u32 ump3_enc_output_func(void *, u8 *, u16));




#endif

