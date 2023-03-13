#include "audio_dac_fade.h"
#include "audio_dac.h"
#include "wdt.h"
#include "audio_dac_api.h"
#include "app_config.h"
#include "audac_basic.h"
#include "tick_timer_driver.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[dac_fade]"
#include "log.h"

extern DAC_MANAGE dac_mge;
extern const u16 vol_tab[];

#if TCFG_DAC_VOL_FADE_EN


void hardware_fade_in(void)
{
    CPU_INT_DIS();
    dac_mge.flag &= ~B_DAC_FADE_OUT;
    dac_mge.flag &= ~B_DAC_FADE_EN;
    dac_mge.vol_l_phy = vol_tab[dac_mge.vol_l];
    dac_mge.vol_r_phy = vol_tab[dac_mge.vol_r];
    CPU_INT_EN();

    JL_AUDIO->DAC_CON &= ~BIT(10);
    dac_phy_vol(dac_mge.vol_l_phy, dac_mge.vol_r_phy);
    while (JL_AUDIO->DAC_CON & (0x01 << 11)) {
        /* putchar('b'); */
        wdt_clear();
    }
    JL_AUDIO->DAC_CON |= BIT(10);
}

void hardware_fade_out(void)
{
    CPU_INT_DIS();
    dac_mge.flag |= B_DAC_FADE_OUT;
    dac_mge.flag |= B_DAC_FADE_EN;
    dac_mge.vol_l_phy = 0;
    dac_mge.vol_r_phy = 0;
    CPU_INT_EN();

    JL_AUDIO->DAC_CON &= ~BIT(10);
    dac_phy_vol(dac_mge.vol_l_phy, dac_mge.vol_r_phy);
    while (JL_AUDIO->DAC_CON & (0x01 << 11)) {
        /* putchar('a'); */
        wdt_clear();
    }
    JL_AUDIO->DAC_CON |= BIT(10);
}

#endif

#if DAC_FADE_ENABLE

u16 dac_single_fade(u16 vol, u16 vol_phy, u16 *p_cnt)
{
    u16 target_vol = 0;
    u16 curr_vol = vol_phy;
    u16 t_fade_step;

    if (curr_vol > 100) {
        t_fade_step = (curr_vol + 1000) / 60;
    } else {
        t_fade_step =  10;
    }

    if (dac_mge.flag & B_DAC_FADE_OUT) {
        if (curr_vol > t_fade_step) {
            curr_vol -= t_fade_step;
        } else {
            curr_vol = 0;
        }
    } else {
        target_vol = vol_tab[vol];
        if (target_vol > curr_vol) {
            if (curr_vol < (target_vol - t_fade_step)) {
                curr_vol += t_fade_step;
            } else {
                curr_vol = target_vol;
            }

        } else {
            if (curr_vol > (target_vol + t_fade_step)) {
                curr_vol -= t_fade_step;
            } else {
                curr_vol = target_vol;
            }
        }

    }
    if (target_vol == curr_vol) {
        *p_cnt = 0;
    }
    return curr_vol;
}
/* #define DAC_FADE_STEP (16384 / 1638) */
void dac_fade(void)
{
    if (0 == (dac_mge.flag & B_DAC_FADE_EN)) {
        return;
    }
    if (dac_mge.flag & B_DAC_MUTE) {
        return;
    }
    /*
    static u16 cnt = 0;
    cnt++;
    u16 target_vol = 0;
    u16 curr_vol = dac_mge.vol_phy;
    u16 t_fade_step;
    if (curr_vol > 100) {
        t_fade_step = (curr_vol + 1000) / 60;
    } else {
        t_fade_step =  10;
    }

    if (dac_mge.flag & B_DAC_FADE_OUT) {
        if (curr_vol > t_fade_step) {
            curr_vol -= t_fade_step;
        } else {
            curr_vol = 0;
        }
    } else {
        target_vol = vol_tab[dac_mge.vol];
        if (target_vol > curr_vol) {
            if (curr_vol < (target_vol - t_fade_step)) {
                curr_vol += t_fade_step;
            } else {
                curr_vol = target_vol;
            }

        } else {
            if (curr_vol > (target_vol + t_fade_step)) {
                curr_vol -= t_fade_step;
            } else {
                curr_vol = target_vol;
            }
        }

    }
    dac_mge.vol_phy = curr_vol;

    [> log_info(" fade  %d %d \n",dac_mge.vol_phy, cnt); <]
    if (target_vol == dac_mge.vol_phy) {
        dac_mge.flag &= ~B_DAC_FADE_EN;
        cnt = 0;
    }
    */
    static u16 cnt_l = 0;
    static u16 cnt_r = 0;
    cnt_l++;
    dac_mge.vol_l_phy = dac_single_fade(dac_mge.vol_l, dac_mge.vol_l_phy, &cnt_l);
    cnt_r++;
    dac_mge.vol_r_phy = dac_single_fade(dac_mge.vol_r, dac_mge.vol_r_phy, &cnt_r);
    if ((0 == cnt_r) && (0 == cnt_l)) {
        dac_mge.flag &= ~B_DAC_FADE_EN;
    }
    dac_phy_vol(dac_mge.vol_l_phy, dac_mge.vol_r_phy);
}
void dac_fade_in(void)
{
#if DAC_FADE_ENABLE
    CPU_INT_DIS();
    /* log_info(" fade in  %d \n",dac_mge.vol_phy); */
    dac_mge.flag &= ~B_DAC_FADE_OUT;
    dac_mge.flag |= B_DAC_FADE_EN;
    CPU_INT_EN();
#endif
}

void dac_fade_out(u32 delay)
{
#if DAC_FADE_ENABLE
    CPU_INT_DIS();
    /* log_info(" fade out  %d \n",dac_mge.vol_phy); */
    dac_mge.flag |= B_DAC_FADE_OUT;
    dac_mge.flag |= B_DAC_FADE_EN;
    CPU_INT_EN();
    u32 to_cnt = 0;
    while (dac_mge.flag & B_DAC_FADE_EN) {
        wdt_clear();
        delay_10ms(1);
        to_cnt++;
        if (to_cnt > delay) {
            break;
        }
    }
#endif
}

#endif
