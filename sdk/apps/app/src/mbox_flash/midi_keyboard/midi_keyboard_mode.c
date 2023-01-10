#pragma bss_seg(".midi_keyboard_mode.data.bss")
#pragma data_seg(".midi_keyboard_mode.data")
#pragma const_seg(".midi_keyboard_mode.text.const")
#pragma code_seg(".midi_keyboard_mode.text")
#pragma str_literal_override(".midi_keyboard_mode.text.const")

#include "midi_keyboard_mode.h"
#include "midi_ctrl.h"
#include "midi_prog.h"

#include "common.h"
#include "msg.h"
#include "ui_api.h"
#include "hot_msg.h"
#include "circular_buf.h"
#include "jiffies.h"
#include "pa_mute.h"
#include "device.h"
#include "device_app.h"
#include "vfs.h"
#include "music_play.h"
#include "play_file.h"
#include "music_device.h"
#include "MIDI_DEC_API.h"
#include "vm.h"
#include "app.h"

#include "decoder_api.h"
#include "decoder_msg_tab.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[midi_dec]"
#include "log.h"

#define MIDI_VIBRATO_ENABLE     0   //琴键颤音功能

static dec_obj *midi_keyboard_obj   AT(.midi_ctrl_buf);
static u16 midi_keyboard_idle_cnt   AT(.midi_ctrl_buf);
extern int midi_keyboard_function_switch;
/* demo示例音色库只有钢琴可以弹奏全key音色，其余乐器key范围只有[60, 71] */
const u8 Channal_Prog_Tab[CTRL_CHANNEL_NUM] = {
    /* 标准乐器0~8 */
    MIDI_PROG_ACOUSTIC_GRAND_PIANO,
    MIDI_PROG_MUSIC_BOX_CHROMATIC_PERCUSSION,
    MIDI_PROG_HARMONICA_ORGAN,
    MIDI_PROG_NYLON_ACOUSTIC_GUITAR,
    MIDI_PROG_ACOUSTIC_BASS,
    MIDI_PROG_VIOLIN_SOLO_STRINGS,
    MIDI_PROG_TRUMPET_BRASS,
    MIDI_PROG_SOPRANO_SAX_REED,
    MIDI_PROG_TAIKO_DRUM_PERCUSSIVE,
    /* 乐器号>=128的打击乐器,只能用通道9 */
    MIDI_PROG_PERCUSSION_STANDARD,
    /* 标准乐器10~15 */
    MIDI_PROG_TUBULAR_BELLS_CHROMATIC_PERCUSSION,
    MIDI_PROG_ORCHESTRAL_HARP_SOLO_STRINGS,
    MIDI_PROG_FLUTE_PIPE,
    MIDI_PROG_BANJO_ETHNIC,
    MIDI_PROG_SHANAI_ETHNIC,
    MIDI_PROG_TELEPHONE_RING_SOUND_EFFECTS,
};

void midi_keyboard_app(void)
{
    vm_write(VM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    int msg[2], err;
    u8 scale = 5;
    u8 key_offset = 0;
    u8 cur_channal = 0;
    u16 pitch_val = 256;
    midi_keyboard_idle_cnt = 0;

    key_table_sel((void *)midi_keyboard_key_msg_filter);
    decoder_init();

    err = midi_ctrl_cfg_init();
    if (err) {
        log_info("midi_keyboard_init fail!\n");
        work_mode++;
        return;
    }
    midi_keyboard_obj = decoder_io(NULL, BIT_MIDI_CTRL, NULL, 0);
    if (NULL == midi_keyboard_obj) {
        log_info("midi_keyboard init fail!\n");
        return;
    }

#if 0   //midi琴配置外部音量
    EX_CH_VOL_PARM vol_parm;
    for (int i = 0; i < CTRL_CHANNEL_NUM; i++) {
        vol_parm.cc_vol[i] = 4096;
    }
    vol_parm.ex_vol_use_chn = 1;
    midi_ctrl_dec_confing_api(midi_keyboard_obj, CMD_MIDI_SET_EX_VOL, &vol_parm);
    midi_keyboard_function_switch |= EX_VOL_ENABLE;
    midi_ctrl_dec_confing_api(midi_keyboard_obj, CMD_MIDI_SET_SWITCH, &midi_keyboard_function_switch);
#endif

    midi_on_off_callback_init(midi_keyboard_obj, \
                              midi_ctrl_melody_trigger, \
                              midi_ctrl_melody_stop_trigger);
    for (int chn_num = 0; chn_num < CTRL_CHANNEL_NUM; chn_num++) {
        if (9 == chn_num) {
            midi_ctrl_set_prog(midi_keyboard_obj, Channal_Prog_Tab[chn_num] - 128, chn_num);
        } else {
            midi_ctrl_set_prog(midi_keyboard_obj, Channal_Prog_Tab[chn_num], chn_num);
        }
    }
    midi_ctrl_note_on(midi_keyboard_obj, 60, 127, 0);

    while (1) {
        err = get_msg(2, &msg[0]);
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }

        switch (msg[0]) {
        case MSG_MIDICTRL_NOTE_ON_DO:
            log_info("Do 1");
            key_offset = 0;
            goto __midikey_note_on;
        case MSG_MIDICTRL_NOTE_ON_RE:
            log_info("Re 2");
            key_offset = 2;
            goto __midikey_note_on;
        case MSG_MIDICTRL_NOTE_ON_MI:
            log_info("Mi 3");
            key_offset = 4;
            goto __midikey_note_on;
        case MSG_MIDICTRL_NOTE_ON_FA:
            log_info("Fa 4");
            key_offset = 5;
__midikey_note_on:
            midi_ctrl_note_on(midi_keyboard_obj, \
                              key_offset + scale * 12, \
                              127, \
                              cur_channal);
#if MIDI_VIBRATO_ENABLE
            midi_ctrl_vel_vibrate(midi_keyboard_obj, \
                                  key_offset + scale * 12, \
                                  2, \
                                  9, \
                                  cur_channal);
#endif
            break;

        /* NOTE OFF 松开琴键，部分乐器需要NOTE_OFF才会停止发声 */
        case MSG_MIDICTRL_NOTE_OFF_DO:
            key_offset = 0;
            goto __midikey_note_off;
        case MSG_MIDICTRL_NOTE_OFF_RE:
            key_offset = 2;
            goto __midikey_note_off;
        case MSG_MIDICTRL_NOTE_OFF_MI:
            key_offset = 4;
            goto __midikey_note_off;
        case MSG_MIDICTRL_NOTE_OFF_FA:
            key_offset = 5;
__midikey_note_off:
            midi_ctrl_note_off(midi_keyboard_obj, \
                               key_offset + scale * 12, \
                               cur_channal, \
                               0);
            /* log_info("NOTE OFF"); */
            break;

        /* PITCH BEND 弯音轮 */
        case MSG_MIDICTRL_PITCH_BEND_DOWN:
            pitch_val -= 16;
            goto __midikeypitch_bend;
        case MSG_MIDICTRL_PITCH_BEND_UP:
            pitch_val += 16;
__midikeypitch_bend:
            midi_ctrl_pitch_bend(midi_keyboard_obj, pitch_val, cur_channal);
            log_info("Pitch Bend : %d\n", pitch_val);
            break;

        /* PROG CHANNAL SEL 乐器通道选择 */
        case MSG_MIDICTRL_CHANNAL_PREV:
            cur_channal--;
            if (cur_channal >= CTRL_CHANNEL_NUM) {
                cur_channal = CTRL_CHANNEL_NUM - 1;
            }
            goto __midikey_channal_sel;
        case MSG_MIDICTRL_CHANNAL_NEXT:
            cur_channal++;
            if (cur_channal >= CTRL_CHANNEL_NUM) {
                cur_channal = 0;
            }
__midikey_channal_sel:
            log_info("Channal:%d, Prog:%d\n", cur_channal, Channal_Prog_Tab[cur_channal]);
            break;

        case MSG_CHANGE_WORK_MODE:
            goto __midi_decode_app_exit;
        case MSG_500MS:
            UI_menu(MENU_MAIN);
            if (0 == midi_keyboard_idle_cnt) {
                app_powerdown_deal(0);
            } else {
                app_powerdown_deal(1);
            }
        /* midi_ctrl_query_play_key(midi_keyboard_obj, cur_channal); */
        default:
            ap_handle_hotkey(msg[0]);
            break;
        }
    }
__midi_decode_app_exit:
    decoder_stop(midi_keyboard_obj, NEED_WAIT, 0);
    key_table_sel(NULL);
}

/* 音符开始回调 */
static u32 midi_ctrl_melody_trigger(void *priv, u8 key, u8 vel)
{
    log_info("ON %d %d\n", key, vel);
    midi_keyboard_idle_cnt++;
    return 0;
}
/* 音符结束回调 */
static u32 midi_ctrl_melody_stop_trigger(void *priv, u8 key)
{
    log_info("OFF %d\n", key);
    midi_keyboard_idle_cnt--;
    if (0 == midi_keyboard_idle_cnt) {
        dec_obj *obj = (dec_obj *)priv;
        obj->sound.enable |= B_DEC_PAUSE;
    }
    return 0;
}

static void midi_on_off_callback_init(dec_obj *obj, u32(*melody_callback)(void *, u8, u8), u32(*melody_stop_callback)(void *, u8))
{
    audio_decoder_ops *ops = (audio_decoder_ops *)obj->dec_ops;

    EX_MELODY_STRUCT melody_parm;
    melody_parm.priv = obj;
    melody_parm.melody_trigger = melody_callback;
    ops->dec_confing(obj->p_dbuf, CMD_MIDI_MELODY_TRIGGER, &melody_parm);

    EX_MELODY_STOP_STRUCT melody_stop_parm;
    melody_stop_parm.priv = obj;
    melody_stop_parm.melody_stop_trigger = melody_stop_callback;
    ops->dec_confing(obj->p_dbuf, CMD_MIDI_STOP_MELODY_TRIGGER, &melody_stop_parm);

    midi_keyboard_function_switch |= MELODY_ENABLE | MELODY_STOP_ENABLE;
    ops->dec_confing(obj->p_dbuf, CMD_MIDI_SET_SWITCH, &midi_keyboard_function_switch);
}
