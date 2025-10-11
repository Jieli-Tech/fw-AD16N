#pragma bss_seg(".encoder_api.data.bss")
#pragma data_seg(".encoder_api.data")
#pragma const_seg(".encoder_api.text.const")
#pragma code_seg(".encoder_api.text")
#pragma str_literal_override(".encoder_api.text.const")

#include "encoder_mge.h"
#include "encoder_file.h"
#include "audio_adc_api.h"
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "hwi.h"
/* #include "dev_manage.h" */
/* #include "fs_io.h" */
#include "vfs.h"
#include "circular_buf.h"
#include "a_encoder.h"
#include "mp3_encoder.h"
#include "clock.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[encoder_file]"
#include "log.h"

u32 enc_output(void *priv, u8 *data, u16 len)
{
    enc_obj *obj = priv;


    len = cbuf_write(obj->p_obuf, data, len);
    if (cbuf_get_data_size(obj->p_obuf) >=  256) {
        kick_wfile_isr();
    }

    if (0 != obj->indata_kick_size) {
        if (cbuf_get_data_size(obj->p_ibuf) >= obj->indata_kick_size) {
            kick_encode_isr();
        }
    }

    return len;
}

void enc_wait_stop_wfile(enc_obj *obj)
{
    log_info("stop encode C\n");
    u32 i = 0x10000;
    while (0 != cbuf_get_data_size(obj->p_obuf) && (0 != i)) {
        if (obj->enable & B_ENC_FULL) {
            break;
        }
        kick_wfile_isr();
        delay(100);
        i--;
    }
    HWI_Uninstall(IRQ_SOFT2_IDX);
}

enc_obj *encoder_file(u32(*fun)(void *, void *, void *), void *pfile)
{
    enc_wfile_init();
    enc_obj *obj = encoder_io(fun, enc_input, enc_output, pfile);
    if (NULL != obj) {
        obj->wait_output_empty = (void *)enc_wait_stop_wfile;
        obj->enable = B_ENC_ENABLE;
        start_encode();//adc_enable();
        return obj;
    } else {
        return NULL;
    }
}



