#pragma bss_seg(".enc_in_norfs.data.bss")
#pragma data_seg(".enc_in_norfs.data")
#pragma const_seg(".enc_in_norfs.text.const")
#pragma code_seg(".enc_in_norfs.text")
#pragma str_literal_override(".enc_in_norfs.text.const")

#include "record_mode.h"
#include "device.h"
#include "device_app.h"
#include "vfs.h"
#include "msg.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[rec]"
#include "log.h"

int norfs_enc_file_create(Encode_Control *obj)
{
    int err = 0;

    obj->device = device_open(obj->dev_index);
    if (NULL == obj->device) {
        log_info("device_open null!\n");
        return E_DEV_NULL;
    }
    err = fs_mount(&obj->pfs, obj->device, "norfs");
    if (err) {
        log_info("vfs_mount err:0x%x\n", err);
        device_close(obj->dev_index);
        return err;
    }
    err = fs_createfile(obj->pfs, &obj->pfile, (u32 *)&obj->rec_file_index);
    if (err) {
        log_info("vfs_createfile err:0x%x\n", err);
        fs_fs_close(&obj->pfs);
        device_close(obj->dev_index);
        return err;
    }

    log_info("REC FILE INDEX : %d\n", obj->rec_file_index);
    return err;
}

dec_obj *norfs_enc_file_decode(Encode_Control *obj, u16 dec_type)
{
    int err = 0;

    obj->device = device_open(obj->dev_index);
    if (NULL == obj->device) {
        log_info("device_open null!\n");
        return NULL;
    }
    err = fs_mount(&obj->pfs, obj->device, "norfs");
    if (err) {
        log_info("vfs_mount err:0x%x\n", err);
        device_close(obj->dev_index);
        return NULL;
    }
    err = fs_ioctl(obj->pfs, FS_IOCTL_FS_INDEX, (int)&obj->rec_file_index);
    if (err) {
        log_info("vfs_ioctl err:0x%x\n", err);
        goto __norfs_enc_fs_exit;
    }
    err = fs_openbyindex(obj->pfs, &obj->pfile, obj->rec_file_index, NULL);
    if (err) {
        log_info("vfs_createfile err:0x%x\n", err);
        goto __norfs_enc_fs_exit;
    }

    return decoder_io(obj->pfile, dec_type, NULL, 0);

__norfs_enc_fs_exit:
    fs_fs_close(&obj->pfs);
    device_close(obj->dev_index);
    return NULL;
}
