#pragma bss_seg(".softoff_mode.data.bss")
#pragma data_seg(".softoff_mode.data")
#pragma const_seg(".softoff_mode.text.const")
#pragma code_seg(".softoff_mode.text")
#pragma str_literal_override(".softoff_mode.text.const")

#include "softoff_mode.h"
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
#include "device.h"
#include "device_app.h"
#include "vm_api.h"
#include "app.h"
#include "power_api.h"

#include "decoder_api.h"
#include "decoder_msg_tab.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[low_pwr]"
#include "log.h"

void softoff_app(void)
{
    int msg[2], err;
    void *pfs = 0;
    void *pfile = 0;
    dec_obj *obj = 0;
    decoder_init();

    err = fs_mount(&pfs, (void *)NULL, (void *)NULL);
    if (err) {
        log_info("fs_mount err : 0x%x\n", err);
        goto __softoff_app_deal;
    }
    err = fs_openbypath(pfs, &pfile, "dir_notice/power_off.f1a");
    if (err) {
        log_info("fs_openbypath err : 0x%x\n", err);
        fs_fs_close(&pfs);
        goto __softoff_app_deal;
    }
    obj = decoder_io(pfile, BIT_F1A1, NULL, 0);
    if (NULL == obj) {
        log_info("fs_openbypath err : 0x%x\n", err);
        fs_fs_close(&pfs);
        fs_file_close(&pfile);
        goto __softoff_app_deal;
    }

    while (1) {
        err = get_msg(2, &msg[0]);
        bsp_loop();
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }

        switch (msg[0]) {
        case MSG_F1A1_FILE_END:
        case MSG_MP3_FILE_END:
        case MSG_WAV_FILE_END:
            decoder_stop(obj, NEED_WAIT, 0);
__softoff_app_deal:
            sys_softoff();
            break;
        case MSG_500MS:
            wdt_clear();
            break;
        }
    }
}
