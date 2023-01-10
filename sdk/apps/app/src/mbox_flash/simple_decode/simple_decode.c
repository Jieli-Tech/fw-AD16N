#pragma bss_seg(".simple_dec_mode.data.bss")
#pragma data_seg(".simple_dec_mode.data")
#pragma const_seg(".simple_dec_mode.text.const")
#pragma code_seg(".simple_dec_mode.text")
#pragma str_literal_override(".simple_dec_mode.text.const")

#include "simple_decode.h"
#include "common.h"
#include "msg.h"
#include "app.h"
#include "vfs.h"
#include "vm.h"
#include "hot_msg.h"
#include "circular_buf.h"
#include "jiffies.h"
#include "tick_timer_driver.h"
#include "device_app.h"
#include "ui_api.h"

#include "decoder_api.h"
#include "decoder_msg_tab.h"
#include "audio_dac_api.h"

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[simple]"
#include "log.h"

#if SIMPLE_DEC_EN

static play_control dec_pctl[2] AT(.mode_smpl_dec_data);

#define INR_DIR_NUM   5
static const char *const dir_inr_tab[INR_DIR_NUM] = {
    "/dir_song",
    "/dir_eng",
    "/dir_poetry",
    "/dir_story",
    "/dir_bin_f1x",
};
static dp_buff inr_dec_dp[INR_DIR_NUM] AT(.mode_smpl_dec_data);
static const u8 dir_inr_vm_tab[INR_DIR_NUM] = {
    VM_INDEX_SONG,
    VM_INDEX_ENG,
    VM_INDEX_POETRY,
    VM_INDEX_STORY,
    VM_INDEX_F1X,
};
static const char *const dir_tab_a[] = {
    "/dir_a",
};

#if TFG_EXT_FLASH_EN
#define EXT_DIR_NUM   1
static const char *const dir_ext_tab[EXT_DIR_NUM] = {
    "/",
};
static dp_buff ext_dec_dp[EXT_DIR_NUM] AT(.mode_smpl_dec_data);
static const char dir_ext_vm_tab[EXT_DIR_NUM] = {
    VM_INDEX_EXT_SONG,
};
#endif

void simple_decode_app(void)
{
    vm_write(VM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    int msg[2], err;
    key_table_sel(simple_dec_key_msg_filter);
    decoder_init();

    memset(&dec_pctl[0], 0, sizeof(dec_pctl));      //初始化dec_pctl[0]和dec_pctl[1]
    memset(&inr_dec_dp[0], 0, sizeof(inr_dec_dp));  //初始化dec_dp[0]和dec_dp[1]
#if TFG_EXT_FLASH_EN
    memset(&ext_dec_dp[0], 0, sizeof(ext_dec_dp));
#endif

    dec_pctl[0].dev_index   = INNER_FLASH_RO;
    dec_pctl[0].findex      = 1;
    dec_pctl[0].dec_type    = BIT_F1A1 | BIT_UMP3;
    dec_pctl[0].pdp         = &inr_dec_dp[0];
    dec_pctl[0].p_vm_tab    = (void *)&dir_inr_vm_tab[0];
    dec_pctl[0].pdir        = (void *)&dir_inr_tab[0];
    dec_pctl[0].dir_total   = sizeof(dir_inr_tab) / 4;
    simple_dev_fs_mount(&dec_pctl[0]);
#if MUSIC_BREAK_POINT_EN
    if (sizeof(dp_buff) == vm_read(dec_pctl[0].p_vm_tab[dec_pctl[0].dir_index], dec_pctl[0].pdp, sizeof(dp_buff))) {
        dp_buff *pdp = (dp_buff *)dec_pctl[0].pdp;
        dec_pctl[0].findex = pdp->findex;
    }
#endif

    dec_pctl[1].dev_index   = INNER_FLASH_RO;
    dec_pctl[1].findex      = 1;
    dec_pctl[1].loop        = 255;//A格式无缝循环不需要断点buff
    dec_pctl[1].dec_type    = BIT_A;
    dec_pctl[1].pdir        = (void *)&dir_tab_a[0];
    dec_pctl[1].dir_total   = sizeof(dir_tab_a) / 4;
    simple_dev_fs_mount(&dec_pctl[1]);

    post_msg(1, MSG_PLAY_FILE1);
    /* post_msg(1, MSG_A_PLAY); */

    while (1) {
        err = get_msg(2, &msg[0]);
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }

        switch (msg[0]) {
        case MSG_PLAY_FILE1:
            log_info("MSG_PLAY_FILE1\n");
__simple_dec_play_file_entry:
            err = simple_play_file_byindex(&dec_pctl[0]);
            if (0 == err) {
                dp_buff *dp = (dp_buff *)dec_pctl[0].pdp;
                dp->findex = dec_pctl[0].findex;
                clear_dp(dp);
#if MUSIC_BREAK_POINT_EN
                vm_write(dec_pctl[0].p_vm_tab[dec_pctl[0].dir_index], \
                         dec_pctl[0].pdp, \
                         sizeof(dp_buff));
#endif
            } else {
                post_msg(1, MSG_NEXT_FILE);
            }
            break;

        case MSG_PP:
            log_info("MSG_PP\n");
            decoder_pause(dec_pctl[0].p_dec_obj);
            break;
        case MSG_PREV_FILE:
            log_info("MSG_PREV_FILE\n");
            dec_pctl[0].findex--;
            if (dec_pctl[0].findex == 0) {
                dec_pctl[0].findex = dec_pctl[0].ftotal;
            }
            goto __simple_dec_play_file_entry;
            break;
        case MSG_NEXT_FILE:
            log_info("MSG_NEXT_FILE\n");
            dec_pctl[0].findex++;
            if (dec_pctl[0].findex > dec_pctl[0].ftotal) {
                dec_pctl[0].findex = 1;
            }
            goto __simple_dec_play_file_entry;
            break;

        case MSG_NEXT_DIR:
            log_info("MSG_NEXT_DIR\n");
            if (true == simple_next_dir(&dec_pctl[0])) {
                goto __simple_dec_play_file_entry;
            }
            break;

#if TFG_EXT_FLASH_EN
        case MSG_NEXT_DEVICE:
            if (true == simple_switch_device(&dec_pctl[0])) {
                goto __simple_dec_play_file_entry;
            }
            break;
#endif
        case MSG_F1A1_FILE_ERR:
        case MSG_MP3_FILE_ERR:
        case MSG_WAV_FILE_ERR:
        case MSG_F1A1_FILE_END:
        case MSG_MP3_FILE_END:
        case MSG_WAV_FILE_END:
            log_info("FILE END OR ERR\n");
            decoder_stop(dec_pctl[0].p_dec_obj, NEED_WAIT, 0);
            post_msg(1, MSG_NEXT_FILE);
            break;

        case MSG_A_PLAY:
            log_info("MSG_A_PLAY\n");
            simple_play_file_byindex(&dec_pctl[1]);
            break;
        case MSG_A_FILE_END:
        case MSG_A_FILE_ERR:
            log_info("A FILE END OR ERR\n");
            decoder_stop(dec_pctl[1].p_dec_obj, NEED_WAIT, 0);
            break;

        case MSG_F1A1_LOOP:
        case MSG_MP3_LOOP:
        case MSG_WAV_LOOP:
        case MSG_A_LOOP:
            log_info("-loop\n");
            break;

        case MSG_CHANGE_WORK_MODE:
            goto __simple_decode_exit;
        case MSG_500MS:
            UI_menu(MENU_MAIN);
            if ((MUSIC_PLAY != get_decoder_status(dec_pctl[0].p_dec_obj)) && \
                (MUSIC_PLAY != get_decoder_status(dec_pctl[1].p_dec_obj))) {
                app_powerdown_deal(0);
            } else {
                app_powerdown_deal(1);
            }
        default:
            ap_handle_hotkey(msg[0]);
            break;
        }
    }
__simple_decode_exit:
    decoder_stop(dec_pctl[0].p_dec_obj, NEED_WAIT, dec_pctl[0].pdp);
#if MUSIC_BREAK_POINT_EN
    vm_write(dec_pctl[0].p_vm_tab[dec_pctl[0].dir_index], dec_pctl[0].pdp, sizeof(dp_buff));
#endif
    simple_dev_fs_close(&dec_pctl[0]);

    decoder_stop(dec_pctl[1].p_dec_obj, NEED_WAIT, 0);
    simple_dev_fs_close(&dec_pctl[1]);
    key_table_sel(NULL);
}



static bool simple_switch_device(play_control *ppctl)
{
    decoder_stop(ppctl->p_dec_obj, NEED_WAIT, ppctl->pdp);//记录音乐断点
#if MUSIC_BREAK_POINT_EN
    vm_write(ppctl->p_vm_tab[ppctl->dir_index], ppctl->pdp, sizeof(dp_buff));
#endif
    simple_dev_fs_close(ppctl);
    if (EXT_FLASH_RW == ppctl->dev_index) {
        log_info("SWITCH TO INNER_FLASH\n");
        memset(ppctl, 0, sizeof(play_control));
        ppctl->dev_index   = INNER_FLASH_RO;
        ppctl->findex      = 1;
        ppctl->dec_type    = BIT_F1A1 | BIT_UMP3;
        ppctl->pdp         = &inr_dec_dp[ppctl->dir_index];
        ppctl->p_vm_tab    = (void *)&dir_inr_vm_tab[0];
        ppctl->pdir        = (void *)&dir_inr_tab[0];
        ppctl->dir_total   = sizeof(dir_inr_tab) / 4;
    } else {
        log_info("SWITCH TO EXT_FLASH\n");
        memset(ppctl, 0, sizeof(play_control));
        ppctl->dev_index   = EXT_FLASH_RW;
        ppctl->findex      = 1;
        ppctl->dec_type    = BIT_F1A1 | BIT_UMP3;
        ppctl->pdp         = &ext_dec_dp[ppctl->dir_index];
        ppctl->p_vm_tab    = (void *)&dir_ext_vm_tab[0];
        ppctl->pdir        = (void *)&dir_ext_tab[0];
        ppctl->dir_total   = sizeof(dir_ext_tab) / 4;
    }
    if (simple_dev_fs_mount(ppctl)) {
        return false;
    }
#if MUSIC_BREAK_POINT_EN
    u32 ret = vm_read(ppctl->p_vm_tab[ppctl->dir_index], \
                      ppctl->pdp, \
                      sizeof(dp_buff));
    if (sizeof(dp_buff) == ret) {
        dp_buff *dp = (dp_buff *)ppctl->pdp;
        ppctl->findex = dp->findex;
    }
    log_info("next dev findex : %d\n", ppctl->findex);
#endif
    return true;
}

static bool simple_next_dir(play_control *ppctl)
{
    decoder_stop(ppctl->p_dec_obj, NEED_WAIT, ppctl->pdp);//记录音乐断点
#if MUSIC_BREAK_POINT_EN
    vm_write(ppctl->p_vm_tab[ppctl->dir_index], ppctl->pdp, sizeof(dp_buff));
#endif
    ppctl->dir_index++;
    if (ppctl->dir_index >= ppctl->dir_total) {
        ppctl->dir_index = 0;
    }
#if TFG_EXT_FLASH_EN
    if (EXT_FLASH_RW == ppctl->dev_index) {
        ppctl->pdp = &ext_dec_dp[ppctl->dir_index];
    } else
#endif
    {
        ppctl->pdp = &inr_dec_dp[ppctl->dir_index];
    }
#if MUSIC_BREAK_POINT_EN
    vm_read(ppctl->p_vm_tab[ppctl->dir_index], ppctl->pdp, sizeof(dp_buff));
#endif
    dp_buff *dp = (dp_buff *)ppctl->pdp;
    /* log_info("dp->findex : %d\n", dp->findex); */
    if (0 != dp->findex) {
        ppctl->findex = dp->findex;
    } else {
        ppctl->findex = 1;
    }
    return true;
}
#endif
