#pragma bss_seg(".music_player.data.bss")
#pragma data_seg(".music_player.data")
#pragma const_seg(".music_player.text.const")
#pragma code_seg(".music_player.text")
#pragma str_literal_override(".music_player.text.const")

#include "simple_play_file.h"
#include "device.h"
#include "device_app.h"
#include "vfs.h"
#include "errno-base.h"
#include "sound_mge.h"
#include "sys_memory.h"

#include "decoder_api.h"
#include "decoder_msg_tab.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[midi_dec]"
#include "log.h"

u32 get_decoder_status(dec_obj *obj)
{
    if (NULL == obj) {
        return MUSIC_STOP;
    }

    u32 enable = obj->sound.enable;
    if (0 == (enable & B_DEC_RUN_EN)) {
        return MUSIC_STOP;
    }

    return (enable & B_DEC_PAUSE) ? MUSIC_STOP : MUSIC_PLAY;
}

u32 simple_dev_fs_mount(play_control *ppctl)
{
    ppctl->device = device_open(ppctl->dev_index);
    if ((NULL == ppctl->device) && (INNER_FLASH_RO != ppctl->dev_index)) {
        log_info("device open null!\n");
        return E_DEV_NULL;
    }
    u32 err = fs_mount(&ppctl->pfs, ppctl->device, "sydfile");
    if (err) {
        log_info("vfs_mount err:0x%x\n", err);
        return E_MOUNT;
    }

    return 0;
}

u32 simple_dev_fs_close(play_control *ppctl)
{
    fs_file_close(&ppctl->pfile);
    fs_fs_close(&ppctl->pfs);
    device_close(ppctl->dev_index);
    return 0;
}

u32 simple_play_file_byindex(play_control *ppctl)
{
    u32 err;
    dp_buff *pdp = ppctl->pdp;
    decoder_stop(ppctl->p_dec_obj, NO_WAIT, 0);

    sysmem_pre_erase_api();

    if (NULL != ppctl->pdir) {
        const char *dir = ((const char **)ppctl->pdir)[ppctl->dir_index];
        log_info("scan dir path : %s\n", dir);
        err = fs_openbypath(ppctl->pfs, &ppctl->pfile, dir);
        if (0 != err) {
            log_info("fs_openbypath err:0x%x\n", err);
            return E_OPENBYPATH;
        }
    }
    err = fs_openbyindex(ppctl->pfs, &ppctl->pfile, ppctl->findex, NULL);
    if (0 != err) {
        log_info("fs_openbyindex err:0x%x index:%d\n", err, ppctl->findex);
        return E_OPENBYINDEX;
    }
    if (0 != ppctl->loop) {
        clear_dp(pdp);
    }
    ppctl->p_dec_obj = decoder_io(ppctl->pfile, \
                                  ppctl->dec_type, \
                                  pdp, \
                                  ppctl->loop);
    if (NULL == ppctl->p_dec_obj) {
        return E_DECODER;
    }

    fs_ioctl(ppctl->pfile, FS_IOCTL_DIR_FILE_TOTAL, (int)&ppctl->ftotal);
    log_info("dev:0x%x findex:%d ftotal:%d loop:%d\n", ppctl->dev_index, ppctl->findex, ppctl->ftotal, ppctl->loop);

    return 0;
}
