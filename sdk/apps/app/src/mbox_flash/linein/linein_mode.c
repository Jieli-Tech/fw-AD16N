#pragma bss_seg(".linein_mode.data.bss")
#pragma data_seg(".linein_mode.data")
#pragma const_seg(".linein_mode.text.const")
#pragma code_seg(".linein_mode.text")
#pragma str_literal_override(".linein_mode.text.const")

#include "linein_mode.h"
#include "common.h"
#include "msg.h"
#include "ui_api.h"
#include "hot_msg.h"
#include "circular_buf.h"
#include "jiffies.h"
#include "pa_mute.h"
#include "sys_memory.h"

#include "sound_mge.h"
#include "audio_dac_api.h"
#include "audio_adc_api.h"
#include "audio_adc.h"

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[aux]"
#include "log.h"

#if LINEIN_MODE_EN

/* #define LINEIN_MODE     ANALOG_LINEIN_MODE */
#define LINEIN_MODE     DIGITAL_LINEIN_MODE

#if (LINEIN_MODE == DIGITAL_LINEIN_MODE)
cbuffer_t cbuf_digital_linein        AT(.digital_linein_data);
u16 obuf_digital_linein[1024 / 2]    AT(.digital_linein_data);
sound_out_obj digital_linein_sound     AT(.digital_linein_data);
#endif

void linein_app(void)
{
    sysmem_write_api(SYSMEM_INDEX_SYSMODE, &work_mode, sizeof(work_mode));
    SET_UI_MAIN(MENU_AUX_MAIN);
    UI_menu(MENU_AUX_MAIN);
    key_table_sel(linein_key_msg_filter);

#if (LINEIN_MODE == DIGITAL_LINEIN_MODE)
    log_info("digital linein init!\n");
    memset(&digital_linein_sound, 0, sizeof(digital_linein_sound));
    cbuf_init(&cbuf_digital_linein, &obuf_digital_linein[0], sizeof(obuf_digital_linein));
    digital_linein_sound.p_obuf = &cbuf_digital_linein;
    digital_linein_sound.info &= ~B_STEREO;//单声道

    u32 sr = dac_sr_read();
    /* log_info("dac_sr %d\n", sr); */
    dac_sr_api(48000);
    audio_adc_init_api(48000, AUDIO_ADC_LINEIN, 0);//48k采样率
    regist_dac_channel(&digital_linein_sound, NULL);
    regist_audio_adc_channel(&digital_linein_sound, NULL);

    audio_adc_enable();
    digital_linein_sound.enable |= B_DEC_RUN_EN | B_DEC_FIRST;
#else
    log_info("analog linein init!\n");
    amux1_analog_open();
    audio_amux_analog_vol(3, 3);
#endif

    aux_test_audio();

    int msg[2];
    u32 err;
    u8 mute = 0;
    while (1) {
        err = get_msg(2, &msg[0]);
        bsp_loop();

        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }
        switch (msg[0]) {
        case MSG_PP:
            if (mute == 0) {
                log_info("MUTE\n");
                mute = 1;
                dac_mute(1);
                pa_mute(1);
            } else {
                log_info("UNMUTE\n");
                mute = 0;
                dac_mute(0);
                pa_mute(0);
            }
            break;
        case MSG_CHANGE_WORK_MODE:
            goto __linein_app_exit;
        case MSG_500MS:
            UI_menu(MENU_MAIN);
        default:
            ap_handle_hotkey(msg[0]);
            break;
        }
    }
__linein_app_exit:
    if (0 != mute) {
        dac_mute(0);
        pa_mute(0);
    }
#if (LINEIN_MODE == DIGITAL_LINEIN_MODE)
    digital_linein_sound.enable &= ~B_DEC_RUN_EN;
    audio_adc_disable();
    unregist_audio_adc_channel(&digital_linein_sound);
    unregist_dac_channel(&digital_linein_sound);
    audio_adc_off_api();
    dac_sr_api(sr);
#else
    audac_analog_open(1);
#endif
    SET_UI_MAIN(MENU_POWER_UP);
    UI_menu(MENU_POWER_UP);
    key_table_sel(NULL);
}

void aux_test_audio(void)
{
    log_info(" JL_AUDIO->DAC_CON    0x%x;", JL_AUDIO->DAC_CON);
    log_info(" JL_AUDIO->DAC_CON1   0x%x;", JL_AUDIO->DAC_CON1);
    log_info(" JL_AUDIO->DAC_ADR   0x%x;", JL_AUDIO->DAC_ADR);
    log_info(" JL_AUDIO->DAC_LEN   0x%x;\n", JL_AUDIO->DAC_LEN);
    log_info(" JL_AUDIO->ADC_CON    0x%x;", JL_AUDIO->ADC_CON);
    log_info(" JL_AUDIO->ADC_CON1   0x%x;", JL_AUDIO->ADC_CON1);
    log_info(" JL_AUDIO->ADC_ADR   0x%x;", JL_AUDIO->ADC_ADR);
    log_info(" JL_AUDIO->ADC_LEN   0x%x;\n", JL_AUDIO->ADC_LEN);

    log_info(" JL_ADDA->DAA_CON0    0x%x;",   JL_ADDA->DAA_CON0);
    log_info(" JL_ADDA->DAA_CON1    0x%x;",   JL_ADDA->DAA_CON1);
    log_info(" JL_ADDA->DAA_CON2    0x%x;",   JL_ADDA->DAA_CON2);
    log_info(" JL_ADDA->DAA_CON3    0x%x;",   JL_ADDA->DAA_CON3);
    log_info(" JL_ADDA->DAA_CON4    0x%x;",   JL_ADDA->DAA_CON4);
    log_info(" JL_ADDA->DAA_CON7    0x%x;\n",   JL_ADDA->DAA_CON7);
    log_info(" JL_ADDA->ADA_CON0    0x%x;",   JL_ADDA->ADA_CON0);
    log_info(" JL_ADDA->ADA_CON1    0x%x;",   JL_ADDA->ADA_CON1);
    log_info(" JL_ADDA->ADA_CON2    0x%x;",   JL_ADDA->ADA_CON2);
    log_info(" JL_ADDA->ADA_CON3    0x%x;",   JL_ADDA->ADA_CON3);
    log_info(" JL_ADDA->ADA_CON4    0x%x;",   JL_ADDA->ADA_CON4);
    log_info(" JL_ADDA->ADA_CON5    0x%x;",   JL_ADDA->ADA_CON5);
    log_info(" JL_ADDA->ADA_CON6    0x%x;",   JL_ADDA->ADA_CON6);
    log_info(" JL_ADDA->ADA_CON7    0x%x;",   JL_ADDA->ADA_CON7);
    log_info(" JL_ADDA->ADA_CON8    0x%x;",   JL_ADDA->ADA_CON8);
    log_info(" JL_ADDA->ADA_CON9    0x%x;\n",   JL_ADDA->ADA_CON9);

    log_info(" JL_ADDA->ADDA_CON0    0x%x;",   JL_ADDA->ADDA_CON0);
    log_info(" JL_ADDA->ADDA_CON1    0x%x;",   JL_ADDA->ADDA_CON1);
}

#endif
