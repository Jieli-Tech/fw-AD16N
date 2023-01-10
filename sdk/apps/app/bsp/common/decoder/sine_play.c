#include "sine_play.h"
#include "audio_dac_api.h"

#if D_HAS_KEY_VOICE

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[sine]"
#include "log.h"

//sr=96000,freq=1500 :
/* AT_RAM */
const signed short sine_tab[] = {
    0,
    3211,
    6392,
    9511,
    12539,
    15446,
    18204,
    20787,
    23169,
    25329,
    27244,
    28897,
    30272,
    31356,
    32137,
    32609,
    32767,
    32609,
    32137,
    31356,
    30272,
    28897,
    27244,
    25329,
    23169,
    20787,
    18204,
    15446,
    12539,
    9511,
    6392,
    3211,
    0,
    -3211,
    -6392,
    -9511,
    -12539,
    -15446,
    -18204,
    -20787,
    -23169,
    -25329,
    -27244,
    -28897,
    -30272,
    -31356,
    -32137,
    -32609,
    -32767,
    -32609,
    -32137,
    -31356,
    -30272,
    -28897,
    -27244,
    -25329,
    -23169,
    -20787,
    -18204,
    -15446,
    -12539,
    -9511,
    -6392,
    -3211,
};
#define SINE_SR         96000
#define SINE_TAB_LEN    (sizeof(sine_tab)/ sizeof(sine_tab[0]))
#define SINE_MAX_CNT    100
#define SINE_ENABLE     BIT(0)

#define KEY_VOICE_TAB_LEN 32

sine_obj g_sine_obj;

signed short voice_tab[KEY_VOICE_TAB_LEN];
void sine_voice_init(void)
{
    memset(&g_sine_obj, 0, sizeof(g_sine_obj));
    /* tab_init(&g_sine_obj.obj , &sine_tab[0], sizeof(sine_tab)); */
    /* g_sine_obj.obj.enable = 0; */
    /* g_sine_obj.sr = 0; */

}

void sine_voice_kick(void)
{
    /* log_info("key voice kick!\n"); */
    u32 dac_sr = dac_sr_read();
    if (0 == dac_sr) {
        log_info("key voice sr err\n");
        return;
    }
    if (dac_sr != g_sine_obj.sr) {
        u32 step = SINE_SR / dac_sr;
        u32 err = SINE_SR % dac_sr;
        log_info("cur dac sr %d %d\n", dac_sr, g_sine_obj.sr);
        log_info("sine step %d ; 0x%x\n", step, err);
        if (0 != err) {
            log_info("sr err %d!!", err);
        }
        u32 size = 0;
        for (u32 i = 0; i < SINE_TAB_LEN; i += step) {
            if (size >= KEY_VOICE_TAB_LEN) {
                break;
            }
            s32 vol = 10;
            voice_tab[size] = vol * sine_tab[i] / 32;
            size++;
        }
        tab_init(&g_sine_obj.obj, &voice_tab[0], size);
        g_sine_obj.sr = dac_sr;
    }

    g_sine_obj.obj.cnt = SINE_MAX_CNT;
    g_sine_obj.enable |= SINE_ENABLE;
}

AT(.audio_isr_text)
u32 sine_read(void *buff, u32 len)
{
    u32 rlen = len;
    /* log_char('r'); */
    if (SINE_ENABLE & g_sine_obj.enable) {
        /* log_char('R'); */
        /* log_info("g_sine_obj.obj.cnt %d\n", g_sine_obj.obj.cnt); */
        len = tab_read(buff, &g_sine_obj.obj, len);
        if (0 != len) {
            g_sine_obj.enable &= ~SINE_ENABLE;
        }
    }
    return rlen - len;
}


#endif


