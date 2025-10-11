#ifndef __REC_API_H__
#define __REC_API_H__

#include "typedef.h"
#include "audio_enc_api.h"
#include "sound_mge.h"
#include "audio_adc.h"


#define B_ENC_ENABLE  BIT(0)
#define B_ENC_STOP    BIT(1)
#define B_ENC_FULL    BIT(2)
#define B_ENC_INIT    BIT(3)

typedef struct _enc_obj {
    void *p_file;
    void *p_ibuf;
    void *p_obuf;
    void *enc_ops;
    void *p_dbuf;
    ENC_DATA_INFO info;
    volatile u32 enable;
    u32 indata_kick_size;
    void (*wait_output_empty)(void *);
} enc_obj;

void start_encode(void);


u16 enc_input(void *priv, s16 *buf, u8 channel, u16 len);
// u32 enc_output(void *priv, u8 *data, u16 len);

enc_obj *stop_encode_file(enc_obj *p_enc_obj, u32 dlen);
bool stop_encode_phy(enc_obj *obj, IS_WAIT wait);
enc_obj *encoder_io(u32(*fun)(void *, void *, void *), void *input_func, void *output_func, void *pfile);
void wfil_soft2_isr_hook(enc_obj *hdl);

void kick_encode_api(void *obj);
void enc_wfile_init(void);
void rec_cbuf_init(void *cbuf_t);

u32 regist_encode_channel(enc_obj *p_enc_obj);
u32 unregist_encode_channel(enc_obj *p_enc_obj);
bool is_encode_channel_empty(void);

#define  kick_encode_isr()   bit_set_swi(1)
#define  kick_wfile_isr()    bit_set_swi(2)


// #if FPGA
#ifdef FPGA
typedef struct _adc_obj {
    void *p_adc_cbuf;
    volatile u32 enable;
} adc_obj;

#define B_DAC_ENABLE  BIT(0)
extern adc_obj adc_hdl;
#define REC_ADC_CBUF  adc_hdl.p_adc_cbuf

#else
extern sound_out_obj *enc_in_sound;
// #define REC_ADC_CBUF  rec_sound.p_obuf
#define REC_ADC_CBUF  enc_in_sound->p_obuf
#endif

#endif
