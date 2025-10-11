#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "audio_adc.h"
#include "stream_frame.h"
#include "usb/usr/usb_mic_interface.h"
#include "usb/usr/auadc_2_usbmic.h"
#include "uac_sync.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[UMIC]"
#include "log.h"

extern const bool config_usbslave_ctl_mic;
sound_out_obj mic_out_sound SEC(.uac_var);

cbuffer_t mic_out_cbuf SEC(.uac_var);
u16 mic_out_obuf[AUDIO_ADC_PACKET_SIZE * 2] SEC(.uac_var);

cbuffer_t mic_run_cbuf SEC(.uac_var);
u16 mic_run_obuf[AUDIO_ADC_PACKET_SIZE * 4] SEC(.uac_var);

sound_out_obj *auadc_mic_open(uac_mic_read *p_uac_read)
{
    u32 sr        = p_uac_read->self.sr;
    u32 frame_len = p_uac_read->self.frame_len;
    u32 ch        = p_uac_read->self.ch;
    void *p_src = NULL;
    u32 err = 0;
    log_info("AUADC MIC OPEN");
    cbuf_init(&mic_out_cbuf, &mic_out_obuf[0], sizeof(mic_out_obuf));
    sound_out_init(&mic_out_sound, (void *)&mic_out_cbuf, 0);

    cbuf_init(&mic_run_cbuf, &mic_run_obuf[0], sizeof(mic_run_obuf));
    sound_out_obj *p_curr_sound;
    log_info("usmo ---- 000");
    p_curr_sound = &mic_out_sound;
    log_info("usmo ---- 001");

#if TCFG_MIC_SRC_ENABLE
    ///*
    p_curr_sound = link_src_sound(
                       p_curr_sound,
                       &mic_run_cbuf,
                       (void **)&p_src,
                       sr,
                       sr,
                       1
                   );
    p_uac_read->p_src = p_src;
    /* uac_sync_init(&uac_mic_sync, sr); */
    //*/
#endif
    log_info("usmo ---- 002");
    void *kick = NULL;

    if (&mic_out_sound != p_curr_sound) {
        log_info("usmo ---- 003");
        kick = regist_stream_channel(&mic_out_sound, NULL);

    }
    /* p_curr_sound = &uac_read.mic_out_sound; */
    log_info("usmo ---- 004");
    regist_audio_adc_channel((void *)&mic_out_sound, NULL, kick);
    log_info("usmo ---- 005");
    if (config_usbslave_ctl_mic) {
        log_info("usmo ---- 006");
        err = audio_adc_init_api(sr, AUDIO_ADC_MIC, 0);
    }
    if (0 == err) {
        if (config_usbslave_ctl_mic) {
            audio_adc_enable();
        }
        p_curr_sound->enable |= B_DEC_RUN_EN;
        mic_out_sound.enable |= B_DEC_RUN_EN;
    } else {
        log_info("err : 0x%x", err);
    }

    return p_curr_sound;
}

void auadc_mic_close(void **ppsrc)
{
    if (mic_out_sound.enable & B_DEC_RUN_EN) {
        mic_out_sound.enable &= ~B_DEC_RUN_EN;
        unregist_audio_adc_channel(&mic_out_sound);
        unregist_stream_channel(&mic_out_sound);
        if (NULL != *ppsrc) {
            src_reless((void **)ppsrc);
        }
        memset(&mic_out_sound, 0, sizeof(mic_out_sound));
    }
    if (config_usbslave_ctl_mic) {
        log_info("usmc ---- 001");
        audio_adc_off_api();
    } else {
        log_info("usmc ---- 002");
        /* audio_adc_disable(); */
    }
}

