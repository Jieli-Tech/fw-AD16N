#ifndef _FUSB_PLL_TRIM_H_
#define _FUSB_PLL_TRIM_H_

#include "typedef.h"

#define FUSB_PLL_TRIM   1
#define FUSB_PLL_DEVIATION  10  //频率偏差 ±n/1000, 例：10，正负偏差千分之十
#define FUSB_MAX_TH     (48 * (1000 + FUSB_PLL_DEVIATION))
#define FUSB_MIN_TH     (48 * (1000 - FUSB_PLL_DEVIATION))
enum {
    USB_TRIM_HAND,  //手动校准模式
    USB_TRIM_AUTO,  //full_speed自动校准模式
};

#define FUSB_TRIM_CON0      JL_PLL0->TRIM_CON0
#define FUSB_TRIM_CON1      JL_PLL0->TRIM_CON1
#define FUSB_TRIM_PND       JL_PLL0->TRIM_PND
#define FUSB_FRQ_CNT        JL_PLL0->FRQ_CNT
#define FUSB_FRQ_SCA        JL_PLL0->FRC_SCA
#define FUSB_PLL_CON0       JL_PLL0->PLL_CON0
#define FUSB_PLL_CON1       JL_PLL0->PLL_CON1
#define FUSB_PLL_NR         JL_PLL0->PLL_NR

u8 fusb_pll_trim(u8 mode, u16 trim_prd);

#endif
