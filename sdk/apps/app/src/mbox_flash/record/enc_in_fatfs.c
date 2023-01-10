#pragma bss_seg(".enc_in_fat.data.bss")
#pragma data_seg(".enc_in_fat.data")
#pragma const_seg(".enc_in_fat.text.const")
#pragma code_seg(".enc_in_fat.text")
#pragma str_literal_override(".enc_in_fat.text.const")

#include "record_mode.h"
#include "device.h"
#include "device_app.h"
#include "vfs.h"
#include "msg.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[rec]"
#include "log.h"

#define REC_FOLDER_PATH     "/JL_REC"
#define REC_FILE_NAME       "AD16****.mp3"
static char path_buff[25];

static void numtostr(char *str, u32 num, u8 bitnum)
{
    while (bitnum --) {
        *str-- = (num % 10) + '0';
        num /= 10;
    }
}
int fatfs_enc_file_create(Encode_Control *obj)
{
    int err = 0;
    u32 last_num = 0;
    u32 total_rec_file = 0;

    obj->device = device_open(obj->dev_index);
    if (NULL == obj->device) {
        log_info("device_open null!\n");
        return E_DEV_NULL;
    }
    err = fs_mount(&obj->pfs, obj->device, "fat");
    if (err) {
        log_info("vfs_mount err:0x%x\n", err);
        device_close(obj->dev_index);
        return err;
    }
    memset(&path_buff[0], 0, sizeof(path_buff));
    strcat(path_buff, REC_FOLDER_PATH);
    strcat(path_buff, "/");
    strcat(path_buff, REC_FILE_NAME);
    fs_get_encfolder_info(obj->pfs, (char *)REC_FOLDER_PATH, "MP3", &last_num, &total_rec_file);
    if (total_rec_file == 0) {
        fs_mk_dir(obj->pfs, (char *)REC_FOLDER_PATH, 0);
    }
    /* last_num++; */
    char *file_num_ptr = strchr(path_buff, '.') - 1;

__fat_rec_file_name:
    if (last_num > 9999) {
        last_num = 0;
    }
    numtostr(file_num_ptr, last_num, 4);
    /* log_info("path_buff:%s\n", path_buff); */

    err = fs_openbypath(obj->pfs, &obj->pfile, path_buff);
    if (0 == err) {
        log_info("file AD16%d exist!\n", last_num);
        fs_file_close(&obj->pfile);
        last_num++;
        goto __fat_rec_file_name;
    } else {
        log_info("REC FILE NAME:%s \n", path_buff);
        err = fs_createfile(obj->pfs, &obj->pfile, (u32 *)&path_buff[0]);
        if (err) {
            log_info("open file error !!! \n");
            fs_fs_close(&obj->pfs);
            device_close(obj->dev_index);
            return err;
        }
    }

    obj->rec_file_path = &path_buff[0];
    return 0;
}

dec_obj *fatfs_enc_file_decode(Encode_Control *obj, u16 dec_type)
{
    int err = 0;
    if (NULL == obj->rec_file_path) {
        return NULL;
    }

    obj->device = device_open(obj->dev_index);
    if (NULL == obj->device) {
        log_info("device_open null!\n");
        return NULL;
    }
    err = fs_mount(&obj->pfs, obj->device, "fat");
    if (err) {
        log_info("vfs_mount err:0x%x\n", err);
        device_close(obj->dev_index);
        return NULL;
    }

    log_info("OPEN FILE:%s\n", obj->rec_file_path);
    err = fs_openbypath(obj->pfs, &obj->pfile, (const char *)obj->rec_file_path);
    if (err) {
        log_info("fs_openbypath err:0x%x\n", err);
        fs_fs_close(&obj->pfs);
        device_close(obj->dev_index);
        return NULL;
    }

    return decoder_io(obj->pfile, dec_type, NULL, 0);
}

