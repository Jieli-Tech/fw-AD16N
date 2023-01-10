#ifndef __MIDI_API_H__
#define __MIDI_API_H__
#include "if_decoder_ctrl.h"
#include "decoder_mge.h"
#include "MIDI_DEC_API.h"

extern dec_obj dec_midi_hld;
extern const u8 midi_evt[10];
u32 midi_decode_api(void *p_file, void **ppdec, void *p_dp_buf);
u32 midi_buff_api(dec_buf *p_dec_buf);
u32 midi_dec_confing_api(dec_obj *obj, u32 cmd, void *parm);
extern void midi_init_info(MIDI_INIT_STRUCT *init_info, u8 sr_index, u32 spi_pos_addr, int max_cnt);
int midi_decode_cfg_init(void);

#endif
