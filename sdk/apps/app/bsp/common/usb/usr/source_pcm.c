#include "usb/usr/usb_mic_interface.h"
#include "decoder_mge.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[src_pcm]"
#include "log.h"

u32 source_data_pcm(void *source, u32 out_sr)
{
    if (NULL == source) {
        return 0;
    }
    dec_obj *p_recv_dec_obj = source;
    sound_stream_obj *p_stream = p_recv_dec_obj->p_file;
    if (NULL == p_stream) {
        return 0;
    }
    void *p_ibuf = p_stream->p_ibuf;
    u16 br = p_recv_dec_obj->br;

    if ((NULL == p_ibuf) || (0 == br)) {
        return 0;
    }
    /* 获取ibuf信息 */
    u32 ibuf_data = cbuf_get_data_size(p_ibuf);
    /* printf("ibuf:%d %d, sr out:%d\n", ibuf_data, ibuf_space, out_sr); */

    /* 考虑误差！！！ */
    out_sr /= 1000;

    return (out_sr * 16 * ibuf_data) / br;
}

u32 source_space_pcm(void *source, u32 out_sr)
{
    if (NULL == source) {
        return 0;
    }
    dec_obj *p_recv_dec_obj = source;
    sound_stream_obj *p_stream = p_recv_dec_obj->p_file;
    if (NULL == p_stream) {
        return 0;
    }
    void *p_ibuf = p_stream->p_ibuf;
    u16 br = p_recv_dec_obj->br;
    if ((NULL == p_ibuf) || (0 == br)) {
        return 0;
    }

    /* 获取ibuf信息 */
    u32 ibuf_space = cbuf_get_space(p_ibuf);
    /* printf("ibuf:%d %d, sr out:%d\n", ibuf_data, ibuf_space, out_sr); */

    /* 考虑误差！！！ */
    out_sr /= 1000;

    return (out_sr * 16 * ibuf_space) / br;
}
