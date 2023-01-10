#pragma bss_seg(".audio_eq.data.bss")
#pragma data_seg(".audio_eq.data")
#pragma const_seg(".audio_eq.text.const")
#pragma code_seg(".audio_eq.text")
#pragma str_literal_override(".audio_eq.text.const")

#include "audio_eq_api.h"
#include "audio_dac_api.h"
#include "app_config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[eq]"
#include "log.h"

#if AUDIO_EQ_ENABLE

AUDIO_EQ_INFO g_eq_info;
sound_fade_obj g_sound_fade;

/* EQ频点，单位Hz */
const u16 eq_freq[EQ_SECTION_MAX] = {31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000};

/* EQ频点增益，单位dB */
const char eq_mode_normal[EQ_SECTION_MAX] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const char eq_mode_rock[EQ_SECTION_MAX] = {-2, 0, 2, 4, -2, -2, 0, 0, 4, 4};
const char eq_mode_pop[EQ_SECTION_MAX] = { 3, 1, 0, -2, -4, -4, -2, 0, 1, 1};
const u32 eq_type_tab[] = {
    (u32)eq_mode_normal,
    (u32)eq_mode_rock,
    (u32)eq_mode_pop,
};

static void EQ_run(short *in, short *out, int ch, int len)
{
    JL_EQ->CON1 = 0;
    JL_EQ->CON2 = 0;
    JL_EQ->CON3 = 0;
    /* SEQUENCE_DAT_IN */
    JL_EQ->CON1 = sizeof(short);
    JL_EQ->CON2 = (ch - 1) * sizeof(short);
    JL_EQ->CON3 = ch * sizeof(short);
    /* SEQUENCE_DAT_OUT */
    JL_EQ->CON1 |= (sizeof(short)) << 16;
    JL_EQ->CON2 |= ((ch - 1)  * sizeof(short)) << 16;
    JL_EQ->CON3 |= (ch * sizeof(short)) << 16;

    JL_EQ->DATAI_ADR = (u32)in;
    JL_EQ->DATAO_ADR = (u32)out;
    JL_EQ->DATA_LEN = len - 1;
    /* log_info("points_per_ch:%d", len); */

    JL_EQ->CON0 |= BIT(30);//clr pnd
    JL_EQ->CON0 |= BIT(1) | BIT(0);
    while ((JL_EQ->CON0 & BIT(31)) == 0);
    JL_EQ->CON0 |= BIT(30);
}

static void HWEQ_FILT_OPEN(char *newGAIN, int ch)
{
    float *tptr = (float *)EQ_COEFF_BASE;
    int i, j, k;
    for (j = 0; j < ch; j++) {
        for (i = 0; i < EQ_SECTION_MAX; i++) {
            if (eq_freq[i] >= (g_eq_info.sample_rate / 2 * 29491) >> 15) {//0.9 * sr / 2
                tptr[0] = 1;
                for (k = 1; k < 5; k++) {
                    tptr[k] = 0;
                }
            } else {
                design_pe_for_int((int)(eq_freq[i]), (int)g_eq_info.sample_rate, (int)(newGAIN[i] * (1 << 20)), 11744051, (float *)tptr);
            }
#if 0
            int *p = (int *)tptr;
            printf("%dHz  ", eq_freq[i]);
            for (int l = 0; l < 5; l++) {
                printf("0x%x, ", p[l]);
            }
            putchar('\n');
#endif
            tptr += 8;
        }
    }
}

static void EQ_init(char *newGAIN, int ch)
{
    JL_EQ->CON0 = 0;
    __asm__ volatile("csync");//避免以上写动作，总线未写完，就操作eq mem导致死机的问题

    float *tptr = (float *)(int)EQ_COEFF_BASE;
    memset((void *)EQ_COEFF_BASE, 0, 20 * 8 * sizeof(float));

    HWEQ_FILT_OPEN(newGAIN, ch);

    /* printf_buf((u8 *)EQ_COEFF_BASE, (sizeof(int) * 5 * EQ_SECTION_MAX)); */

    JL_EQ->CON0 = 1 << 1;
    JL_EQ->CON0 |= ((EQ_RUN_MODE << 2) | \
                    (EQ_IN_FORMAT << 4) | \
                    (EQ_OUT_FORMAT << 6) | \
                    ((ch - 1) << 8) | \
                    (EQ_SECTION_MAX - 1) << 16);
}

void audio_eq_run_api(void *data_in, void *data_out, u32 len, bool stereo)
{
    AUDIO_EQ_INFO *eq_info = &g_eq_info;
    if (!(eq_info->eq_enable & B_EQ_START)) {
        return;
    }

    u8 ch = eq_info->output_ch;
    u16 sp_len = len / 2 / ch;
    EQ_run((short *)data_in,    \
           (short *)data_out,  \
           ch,                 \
           sp_len);

    if (eq_info->eq_enable & B_EQ_FADE_OUT) {
        if (!sound_fade_out(&g_sound_fade, data_out, len, stereo)) {
            /* putchar('S'); */
            local_irq_disable();
            EQ_init((char *)eq_type_tab[eq_info->eq_mode], eq_info->output_ch);
            eq_info->eq_enable &= ~B_EQ_FADE_OUT;
            eq_info->eq_enable |= B_EQ_FADE_IN;
            local_irq_enable();
        }
        /* putchar('-'); */
    } else if (eq_info->eq_enable & B_EQ_FADE_IN) {
        if (sound_fade_in(&g_sound_fade, data_out, len, stereo)) {
            /* putchar('E'); */
            local_irq_disable();
            eq_info->eq_enable &= ~B_EQ_FADE_IN;
            local_irq_enable();
        }
        /* putchar('+'); */
    }
}

AUDIO_EQ_INFO *audio_eq_open_api(u8 ch, u16 sr)
{
    AUDIO_EQ_INFO *eq_info = &g_eq_info;
    if (eq_info->eq_enable & B_EQ_START) {
        return NULL;
    }
    eq_info->sample_rate = sr;
    eq_info->output_ch = ch;
    log_info("eq:%d\n", eq_info->eq_mode);

    local_irq_disable();
    EQ_init((char *)eq_type_tab[eq_info->eq_mode], eq_info->output_ch);
    local_irq_enable();

    sound_fade_init(&g_sound_fade, 128);
    eq_info->eq_enable |= B_EQ_START;
    return eq_info;
}

void audio_eq_switch_tab(void)
{
    AUDIO_EQ_INFO *eq_info = &g_eq_info;
    if (++eq_info->eq_mode >= ARRAY_SIZE(eq_type_tab)) {
        eq_info->eq_mode = 0;
    }
    log_info("eq:%d\n", eq_info->eq_mode);

#if 1//淡入淡出
    eq_info->eq_enable |= B_EQ_FADE_OUT;
#else//直接切换
    local_irq_disable();
    EQ_init((char *)eq_type_tab[eq_info->eq_mode], eq_info->output_ch);
    local_irq_enable();
#endif
}

void audio_eq_init_api(void)
{
    AUDIO_EQ_INFO *eq_info = &g_eq_info;
    memset((void *)eq_info, 0, sizeof(AUDIO_EQ_INFO));
}

void audio_eq_close_api(void)
{
    AUDIO_EQ_INFO *eq_info = &g_eq_info;
    eq_info->eq_enable = 0;
}

#endif
