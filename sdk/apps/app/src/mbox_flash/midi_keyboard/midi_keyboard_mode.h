#ifndef __MIDI_KEYBOARD_MODE_H__
#define __MIDI_KEYBOARD_MODE_H__

#include "typedef.h"
#include "key.h"
#include "decoder_api.h"

void midi_keyboard_app(void);
extern u16 midi_keyboard_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
static u32 midi_ctrl_melody_trigger(void *priv, u8 key, u8 vel);
static u32 midi_ctrl_melody_stop_trigger(void *priv, u8 key);
static void midi_on_off_callback_init(dec_obj *obj, u32(*melody_callback)(void *, u8, u8), u32(*melody_stop_callback)(void *, u8));
#endif
