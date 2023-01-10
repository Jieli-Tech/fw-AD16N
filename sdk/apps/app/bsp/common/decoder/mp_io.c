

#pragma bss_seg(".mp_io.data.bss")
#pragma data_seg(".mp_io.data")
#pragma const_seg(".mp_io.text.const")
#pragma code_seg(".mp_io.text")
#pragma str_literal_override(".mp_io.text.const")

#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "sound_mge.h"
#include "circular_buf.h"
#include "decoder_mge.h"
#include "decoder_api.h"
#include "vfs.h"

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[mp_io]"
#include "log.h"

//#	io接口：
//读数接口:priv为传入的dec_mp3_hld，返回值是读到多少个byte
int mp_input(void *priv, u32 addr, void *buf, int len, u8 type)
{
    dec_obj *obj = priv;
    int rlen = 0;
    fs_seek(obj->p_file, addr, SEEK_SET);        //addr为相对文件起始位置的偏移，len为多少个byte
    rlen = fs_read(obj->p_file, buf, len);
    return rlen;
}

u32 mp_output(void *priv, void *data, int len) //出数接口,data是出数的起始地址，len是长度（byte），priv为传入的
{
#if 0
    u8 *p = (u8 *)data;
    static u32 acnt = 0;
    u32 olen = len ;
    u32 i;
    /* printf("\n-olen : %0x\n", olen); */
    for (i = 0; i < olen; i++) {
        acnt++;
        printf("%02x ", p[i]);
        if (0 == (acnt % 16)) {
            printf("\n");
        }
    }
#endif
    dec_obj *obj = priv;
    return sound_output(&obj->sound, data, len);
}

