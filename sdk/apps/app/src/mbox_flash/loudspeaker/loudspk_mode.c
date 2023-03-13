#pragma bss_seg(".loudspk_mode.data.bss")
#pragma data_seg(".loudspk_mode.data")
#pragma const_seg(".loudspk_mode.text.const")
#pragma code_seg(".loudspk_mode.text")
#pragma str_literal_override(".loudspk_mode.text.const")

#include "loudspk_mode.h"
#include "common.h"
#include "msg.h"
#include "hot_msg.h"
#include "ui_api.h"
#include "circular_buf.h"
#include "jiffies.h"
#include "pa_mute.h"
#include "vm_api.h"

#include "sound_mge.h"
#include "audio_dac_api.h"
#include "audio_adc_api.h"
#include "audio_adc.h"

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[loud_spk]"
#include "log.h"

#define LOUDSPK_SR  48000

void loudspeaker_app(void)
{
    vm_write(VM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    key_table_sel(loudspk_key_msg_filter);

    u32 dac_sr = dac_sr_read();
    dac_sr_api(LOUDSPK_SR);
    audio_adc_speaker_start(LOUDSPK_SR);

    int msg[2];
    u32 err;
    u8 mute = 0;
    while (1) {
        err = get_msg(2, &msg[0]);
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }

        switch (msg[0]) {
        case MSG_PP:
            if (mute == 0) {
                log_info("MUTE\n");
                mute = 1;
                dac_mute(1);
                pa_mute(1);
            } else {
                log_info("UNMUTE\n");
                mute = 0;
                dac_mute(0);
                pa_mute(0);
            }
            break;
        case MSG_CHANGE_WORK_MODE:
            goto __loudspk_app_exit;
        case MSG_500MS:
            UI_menu(MENU_MAIN);
        default:
            ap_handle_hotkey(msg[0]);
            break;
        }
    }
__loudspk_app_exit:
    audio_adc_speaker_reless();
    dac_sr_api(dac_sr);
    key_table_sel(NULL);
}
