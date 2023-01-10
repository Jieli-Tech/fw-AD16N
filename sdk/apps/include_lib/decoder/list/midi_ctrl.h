#ifndef __MIDI_CTRL_H__
#define __MIDI_CTRL_H__
#include "if_decoder_ctrl.h"
#include "decoder_mge.h"
#include "MIDI_CTRL_API.h"

extern dec_obj dec_midi_ctrl_hld;
extern const u8 midi_evt[10];
extern const decoder_ops_t midi_ctrl_ops;
u32 midi_ctrl_decode_api(void *p_file, void **ppdec, void *p_dp_buf);
int midi_ctrl_cfg_init(void);
u32 midi_ctrl_buff_api(dec_buf *p_dec_buf);

void midi_error_play_end_cb(dec_obj *obj, u32 ret);
u32 midi_ctrl_dec_confing_api(dec_obj *obj, u32 cmd, void *parm);
u32 midi_ctrl_set_prog(dec_obj *obj, u8 prog, u8 chn);
u32 midi_ctrl_note_on(dec_obj *obj, u8 nkey, u8 nvel, u8 chn);
u32 midi_ctrl_note_off(dec_obj *obj, u8 nkey, u8 chn, u16 decay_time);
u32 midi_ctrl_vel_vibrate(dec_obj *obj, u8 nkey, u8 vel_step, u8 vel_rate, u8 chn);
u32 midi_ctrl_pitch_bend(dec_obj *obj, u16 pitch_val, u8 chn);
u8 *midi_ctrl_query_play_key(dec_obj *obj, u8 chn);

#endif
