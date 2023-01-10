#pragma bss_seg(".midi_dec_mode.data.bss")
#pragma data_seg(".midi_dec_mode.data")
#pragma const_seg(".midi_dec_mode.text.const")
#pragma code_seg(".midi_dec_mode.text")
#pragma str_literal_override(".midi_dec_mode.text.const")

#include "midi_dec_mode.h"
#include "midi_api.h"

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
#include "simple_play_file.h"
#include "vm.h"
#include "app.h"

#include "decoder_api.h"
#include "decoder_msg_tab.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[midi_dec]"
#include "log.h"

static play_control midi_pctl[2]    AT(.midi_buf);
static u8 app_midi_mode             AT(.midi_buf);

static const char *const dir_midi_tab[] = {
    "/dir_midi",
};

static const char *const dir_tab_a[] = {
    "/dir_a",
};

void midi_decode_app(void)
{
    vm_write(VM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    int msg[2], err;
    key_table_sel(midi_dec_key_msg_filter);
    decoder_init();

    err = midi_decode_cfg_init();
    if (err) {
        log_info("midi_decode_init fail!\n");
        work_mode++;
        return;
    }

    memset(&midi_pctl[0], 0, sizeof(midi_pctl));
    midi_pctl[0].dev_index   = INNER_FLASH_RO;
    midi_pctl[0].findex      = 1;
    midi_pctl[0].dec_type    = BIT_MIDI;
    midi_pctl[0].pdir        = (void *)&dir_midi_tab[0];
    midi_pctl[0].dir_total   = sizeof(dir_midi_tab) / 4;
    simple_dev_fs_mount(&midi_pctl[0]);
    post_msg(1, MSG_PLAY_FILE1);

    midi_pctl[1].dev_index   = INNER_FLASH_RO;
    midi_pctl[1].findex      = 1;
    midi_pctl[1].loop        = 255;
    midi_pctl[1].dec_type    = BIT_A;
    midi_pctl[1].pdir        = (void *)&dir_tab_a[0];
    midi_pctl[1].dir_total   = sizeof(dir_tab_a) / 4;
    simple_dev_fs_mount(&midi_pctl[1]);
    /* post_msg(1, MSG_A_PLAY); */

    while (1) {
        err = get_msg(2, &msg[0]);
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }

        switch (msg[0]) {
        case MSG_MIDI_MODE_SWITCH:
            if (CMD_MIDI_CTRL_MODE_0 == app_midi_mode) {
                log_info("SWITCH_TO_OKON\n");
                app_midi_mode = CMD_MIDI_CTRL_MODE_1;
            } else {
                log_info("SWITCH_TO_NORM\n");
                app_midi_mode = CMD_MIDI_CTRL_MODE_0;
            }
            midi_decode_set_mode(app_midi_mode);
            break;
        case MSG_MIDI_OKON_GOON:
            if (CMD_MIDI_CTRL_MODE_1 == app_midi_mode) {
                log_info("a");
                midi_decode_okon_goon();
            }
            break;

        case MSG_PLAY_FILE1:
__midi_dec_play_file_entry:
            log_info("MSG_PLAY_FILE1\n");
            if (0 != simple_play_file_byindex(&midi_pctl[0])) {
                post_msg(1, MSG_NEXT_FILE);
            }
            break;
        case MSG_PP:
            log_info("MSG_PP\n");
            decoder_pause(midi_pctl[0].p_dec_obj);
            break;
        case MSG_PREV_FILE:
            log_info("MSG_PREV_FILE\n");
            midi_pctl[0].findex--;
            if (midi_pctl[0].findex == 0) {
                midi_pctl[0].findex = midi_pctl[0].ftotal;
            }
            goto __midi_dec_play_file_entry;
            break;
        case MSG_NEXT_FILE:
            log_info("MSG_NEXT_FILE\n");
            midi_pctl[0].findex++;
            if (midi_pctl[0].findex > midi_pctl[0].ftotal) {
                midi_pctl[0].findex = 1;
            }
            goto __midi_dec_play_file_entry;
            break;
        case MSG_MIDI_FILE_ERR:
        case MSG_MIDI_FILE_END:
            log_info("FILE END OR ERR\n");
            decoder_stop(midi_pctl[0].p_dec_obj, NEED_WAIT, 0);
            post_msg(1, MSG_NEXT_FILE);
            break;

        case MSG_A_PLAY:
            log_info("MSG_A_PLAY\n");
            simple_play_file_byindex(&midi_pctl[1]);
            break;
        case MSG_A_FILE_END:
        case MSG_A_FILE_ERR:
            log_info("A FILE END OR ERR\n");
            decoder_stop(midi_pctl[1].p_dec_obj, NEED_WAIT, 0);
            break;
        case MSG_A_LOOP:
            log_info("-loop\n");
            break;

        case MSG_CHANGE_WORK_MODE:
            goto __midi_decode_app_exit;
        case MSG_500MS:
            UI_menu(MENU_MAIN);
            if ((MUSIC_PLAY != get_decoder_status(midi_pctl[0].p_dec_obj)) && \
                (MUSIC_PLAY != get_decoder_status(midi_pctl[1].p_dec_obj))) {
                app_powerdown_deal(0);
            } else {
                app_powerdown_deal(1);
            }
        default:
            ap_handle_hotkey(msg[0]);
            break;
        }
    }
__midi_decode_app_exit:
    decoder_stop(midi_pctl[0].p_dec_obj, NEED_WAIT, 0);
    simple_dev_fs_close(&midi_pctl[0]);
    decoder_stop(midi_pctl[1].p_dec_obj, NEED_WAIT, 0);
    simple_dev_fs_close(&midi_pctl[1]);
    key_table_sel(NULL);
}

static int midi_decode_set_mode(u8 midi_mode)
{
    dec_obj *obj = midi_pctl[0].p_dec_obj;
    if (NULL == obj) {
        return -1;
    }

    MIDI_PLAY_CTRL_MODE midi_play_ctrl_mode;
    midi_play_ctrl_mode.mode = midi_mode;

    midi_dec_confing_api(obj, CMD_MIDI_CTRL_MODE, (void *)&midi_mode);
    return 0;
}
static int midi_decode_okon_goon(void)
{
    dec_obj *obj = midi_pctl[0].p_dec_obj;
    if (NULL == obj) {
        return -1;
    }

    midi_dec_confing_api(obj, CMD_MIDI_GOON, (void *)NULL);
    return 0;
}

static u32 midi_timeDiv_trigger(void *priv)
{
    return 0;
}
static u32 midi_melody_trigger(void *priv, u8 key, u8 vel)
{
    return 0;
}
static u32 midi_melody_stop_trigger(void *priv, u8 key)
{
    return 0;
}
static u32 midi_beat_trigger(void *priv, u8 val1, u8 val2)
{
    return 0;
}
static u32 midi_mark_trigger(void *priv, u8 *val, u8 len)
{
    return 0;
}
static u32 wdt_clear_trigger(void *priv)
{
    /* wdt_clear(); */
    /* log_char('w'); */
    /* log_info("wdt_clear_trigger \n"); */
    return 64;
}

void midi_init_info(MIDI_INIT_STRUCT *init_parm, u8 sr_index, u32 spi_pos_addr, int max_cnt)
{
    log_info(">>>>>>>>>midi_dec information init<<<<<<<<<<\n");
    log_info("init_parm:0x%x\n", (u32)init_parm);

    ///初始化参数
    init_parm->init_info.sample_rate = sr_index;
    init_parm->init_info.spi_pos = (unsigned char *)spi_pos_addr;
    init_parm->init_info.player_t = (short)max_cnt;
    /* log_info("sr:%d addr:0x%x, max_cnt:%d\n", init_parm->init_info.sample_rate, (u32)init_parm->init_info.spi_pos, init_parm->init_info.player_t); */

    ///控制模式
    init_parm->mode_info.mode = CMD_MIDI_CTRL_MODE_0;

    ///节奏参数
    init_parm->tempo_info.tempo_val = 1042;
    for (u8 i = 0; i < CTRL_CHANNEL_NUM; i++) {
        init_parm->tempo_info.decay_val[i] = ((u16)31 << 11) | 1024;
    }
    init_parm->tempo_info.mute_threshold = (u16)1L << 29;

    ///midi外部音量初始化,置上EX_VOL_ENABLE生效
    for (int i = 0; i < CTRL_CHANNEL_NUM; i++) {
        init_parm->vol_info.cc_vol[i] = 4096;
    }
    init_parm->vol_info.ex_vol_use_chn = 0;

    ///主轨道乐器参数,置上SET_PROG_ENABLE生效
    init_parm->prog_info.prog = 0;
    init_parm->prog_info.replace_mode = 0;
    init_parm->prog_info.ex_vol = 1024;

    ///midi主轨道初始化
    init_parm->mainTrack_info.chn = 17;

    ///One Key One Note参数
    init_parm->okon_info.Melody_Key_Mode = CMD_MIDI_MELODY_KEY_0;
    init_parm->okon_info.OKON_Mode = CMD_MIDI_OKON_MODE_0;

    ///midi音符移半音设置,置上SEMITONE_ENABLE生效
    for (int i = 0; i < CTRL_CHANNEL_NUM; i++) {
        init_parm->semitone_info.key_diff[i] = 12;
    }

    ///OKON 解码副旋空转回调注册
    init_parm->wdt_clear.priv = NULL;
    init_parm->wdt_clear.count = 68;
    init_parm->wdt_clear.wdt_clear_trigger = wdt_clear_trigger;

    ///mark回调函数控制初始化,置上MARK_ENABLE生效
    init_parm->mark_info.priv = NULL;
    init_parm->mark_info.mark_trigger = midi_mark_trigger;

    ///melody回调函数,置上MELODY_ENABLE生效
    init_parm->moledy_info.priv = NULL;
    init_parm->moledy_info.melody_trigger = midi_melody_trigger;

    ///melody_stop回调函数,置上MELODY_STOP_ENABLE生效
    init_parm->moledy_stop_info.priv = NULL;
    init_parm->moledy_stop_info.melody_stop_trigger = midi_melody_stop_trigger;

    ///每拍回调参数,置上BEAT_TRIG_ENABLE生效
    init_parm->beat_info.priv = NULL;
    init_parm->beat_info.beat_trigger = midi_beat_trigger;

    ///小节回调参数,置上TIM_DIV_ENABLE生效
    init_parm->tmDiv_info.priv = NULL;
    init_parm->tmDiv_info.timeDiv_trigger = midi_timeDiv_trigger;

    ///使能控制
    init_parm->switch_info = MELODY_STOP_ENABLE | MARK_ENABLE | MELODY_ENABLE | TIM_DIV_ENABLE | BEAT_TRIG_ENABLE | MELODY_PLAY_ENABLE | EX_VOL_ENABLE;// | SEMITONE_ENABLE;

    app_midi_mode = init_parm->mode_info.mode;
}
