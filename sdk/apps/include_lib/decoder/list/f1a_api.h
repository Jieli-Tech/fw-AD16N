#ifndef __F1A_API_H__
#define __F1A_API_H__
#include "if_decoder_ctrl.h"
#include "decoder_mge.h"

u32 f1a_decode_api_1(void *p_file, void **p_pdec, void *p_dp_buf);
u32 f1a_decode_api_2(void *p_file, void **p_pdec, void *p_dp_buf);

u32 f1a_1_buff_api(dec_buf *p_dec_buf);
u32 f1a_2_buff_api(dec_buf *p_dec_buf);


void *f1x_play_api(void *pfile, u8 *loop_tab, u32 size, u8 index, u32 addr);

extern dec_obj dec_f1a_hld[];


#include "app_modules.h"
extern const u8 f1a_evt[MAX_F1A_CHANNEL][10];

extern const int F1A_MASKROM_TAB;



#endif


