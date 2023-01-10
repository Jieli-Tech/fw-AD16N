#ifndef ADPCM_CODE_API_H
#define ADPCM_CODE_API_H

#include "audio_enc_api.h"

extern ENC_OPS *get_ima_code_ops();
extern int need_ima_encbk_buf();		//断点信息大小
extern u32 get_ima_encbp_info(void *work_buf); //返回断点信息地址

#endif
