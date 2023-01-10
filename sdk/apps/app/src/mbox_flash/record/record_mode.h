#ifndef __RECODE_MODE_H__
#define __RECODE_MODE_H__

#include "typedef.h"
#include "key.h"
#include "decoder_api.h"

#define RECORD_ADC_SR_8K        8000
#define RECORD_ADC_SR_11K025    11025
#define RECORD_ADC_SR_12K       12000
#define RECORD_ADC_SR_16K       16000
#define RECORD_ADC_SR_22K05     22050
#define RECORD_ADC_SR_32K       32000
#define RECORD_ADC_SR_44K1      44100
#define RECORD_ADC_SR_48K       48000

typedef enum {
    ENC_NULL = 0,
    ENC_ING,
} ENC_STATUS;

typedef struct _Encode_Control {
    char fs_name[8];            //编码文件系统名
    void *device;               //编码设备句柄
    void *pfs;                  //编码文件系统句柄
    void *pfile;                //编码文件句柄
    union {
        u32 rec_file_index;     //nor_fs最新文件号
        char *rec_file_path;    //fat_fs最新文件路径
    };
    u8 dev_index;               //编码设备序号
    ENC_STATUS enc_status;      //编码器工作状态
} Encode_Control;

void record_app(void);
void encode_file_fs_close(Encode_Control *obj);
int norfs_enc_file_create(Encode_Control *obj);
dec_obj *norfs_enc_file_decode(Encode_Control *obj, u16 dec_type);
int fatfs_enc_file_create(Encode_Control *obj);
dec_obj *fatfs_enc_file_decode(Encode_Control *obj, u16 dec_type);
extern u16 record_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
static void encode_stop(Encode_Control *obj);
static int encode_start(Encode_Control *obj);
#endif
