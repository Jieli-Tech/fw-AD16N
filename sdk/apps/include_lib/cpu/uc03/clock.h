/*********************************************************************************************
    *   Filename        : typedef.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 10:53

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _CLOCK_H_
#define _CLOCK_H_
#include "typedef.h"

typedef enum {
    //96M:
    PLL_D3p5_27p4M = 0x01,
    PLL_D2p5_38p4M,
    PLL_D2p0_48M,
    PLL_D1p5_64M,
    PLL_D1p0_96M,
    //144M:
    PLL_D3p5_41p1M = 0x10,
    PLL_D2p5_57p6M = 0x20,
    PLL_D2p0_72M = 0x30,
    PLL_D1p5_96M = 0x40,
    PLL_D1p0_144M = 0x50,
    //192M:
    PLL_D3p5_54p8M = 0x100,
    PLL_D2p5_76p8M = 0x200,
    PLL_D2p0_96M = 0x300,
    PLL_D1p5_128M = 0x400,
    PLL_D1p0_192M = 0x500,
    //240M:
    PLL_D3p5_68p6M = 0x1000,
    PLL_D2p5_96p0M = 0x2000,
    PLL_D2p0_120M = 0x3000,
    PLL_D1p5_160M = 0x4000,
    PLL_D1p0_240M = 0x5000,
} _PLL_SYS_SEL;

typedef enum {
    PLL_DIV1 = 0x0,
    PLL_DIV3,
    PLL_DIV5,
    PLL_DIV7,
    PLL_DIV2 = 0x4,
    PLL_DIV6,
    PLL_DIV10,
    PLL_DIV14,

    PLL_DIV4 = 0x8,
    PLL_DIV12,
    PLL_DIV20,
    PLL_DIV28,

    PLL_DIV8 = 0xc,
    PLL_DIV24,
    PLL_DIV40,
    PLL_DIV56,

} _PLL_DIV;

typedef enum {
    HSB_DIV1 = 0x0,
    HSB_DIV2,
    HSB_DIV3,
    HSB_DIV4,
    HSB_DIV5,
    HSB_DIV6,
    HSB_DIV7,
    HSB_DIV8,

} _HSB_CLK_DIV;

typedef enum {
    PLL_REF_SEL_LRC_200K = 0x0,
    PLL_REF_SEL_XOSC,
    PLL_REF_SEL_RESERVED,
    PLL_REF_SEL_PAT_CLK,
} _PLL_REF_SEL;

typedef enum {
    PLL_VCO_SEL_96M = 0x0,
    PLL_VCO_SEL_144M,
    PLL_VCO_SEL_192M,
    PLL_VCO_SEL_240M,
} _PLL_VCO_SEL;


#define PLL_EN(x)              SFR(JL_PLL0->PLL_CON0,  0,  1,  (x))
#define PLL_RST(x)             SFR(JL_PLL0->PLL_CON0,  1,  1,  (x))

#define PLL_96M_SEL(x)         SFR(JL_CLOCK->CLK_CON0, 5,  3,  (x))
#define STD_48M_SEL(x)         SFR(JL_CLOCK->CLK_CON0, 8,  1,  (x))
#define STD_12M_SEL(x)         SFR(JL_CLOCK->CLK_CON0, 9,  1,  (x))

#define     SYS_SRC_RC250K          0
#define     SYS_SRC_RC16M           1
#define     SYS_SRC_LCR_CLK         2
#define     SYS_SRC_XOSC12M_CLK     3
#define     SYS_SRC_EXT_CLK         4
#define     SYS_SRC_PLL_SYS_CLK     5
#define     SYS_SRC_RC1M            6

#define MHz_UNIT	1000000L
#define KHz_UNIT	1000L
extern void IcuPfetchRegion(u32 *beg, u32 len);
void pll_sel(u32 pll_clock, _PLL_DIV pll_div, _HSB_CLK_DIV pll_b_div);

u32 sys_clock_get(void);
int clk_get(const char *name);
void dump_clock_info();



void udelay(u32 us);
void mdelay(u32 ms);



#endif



