#ifndef __A_ENCODER_H__
#define __A_ENCODER_H__

#include "typedef.h"

#define A_ENC_INPUT_MAX_SIZE    (128 * 2)
#define A_ENC_OUTPUT_MAX_SIZE   (32 * 2)

u32 a_encode_api(void *p_file, void *input_func, void *output_func);




#endif

