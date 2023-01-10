#ifndef _MP2_ENCODE_API_H_
#define _MP2_ENCODE_API_H_

#include "audio_enc_api.h"

#if 1//def WIN32
#define AT_MP2(x)
#define AT_MP2_CODE
#define AT_MP2_CONST
#define AT_MP2_SPARSE_CODE
#define AT_MP2_SPARSE_CONST
#else
#define AT_MP2(x)		__attribute__((section(#x)))
#define AT_MP2_CODE		AT_MP2(.mp2_encode_code)
#define AT_MP2_CONST		AT_MP2(.mp2_encode_const)
#define AT_MP2_SPARSE_CODE	AT_MP2(.mp2_encode_sparse_code)
#define AT_MP2_SPARSE_CONST AT_MP2(.mp2_encode_sparse_const)
#endif

extern ENC_OPS *get_mp2_ops();

#define get_mp2standard_ops get_mp2_ops

#endif

