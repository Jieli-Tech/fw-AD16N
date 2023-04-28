/*********************************************************************************************
    *   Filename        : p33_app.h

    *   Description     : HW Sfr Layer

    *   Author          : Sunlicheng

    *   Email           : Sunlicheng@zh-jieli.com

    *   Last modifiled  : 2022-01-07 11:30

    *   Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __P33_APP_H__
#define __P33_APP_H__

#include "typedef.h"

//ROM
u8 p33_buf(u8 buf);

// void p33_xor_1byte(u16 addr, u8 data0);
#define p33_xor_1byte(addr, data0)      (*((volatile u8 *)&addr + 0x300*4)  = data0)
// #define p33_xor_1byte(addr, data0)      addr ^= (data0)

// void p33_and_1byte(u16 addr, u8 data0);
#define p33_and_1byte(addr, data0)      (*((volatile u8 *)&addr + 0x100*4)  = (data0))
//#define p33_and_1byte(addr, data0)      addr &= (data0)

// void p33_or_1byte(u16 addr, u8 data0);
#define p33_or_1byte(addr, data0)       (*((volatile u8 *)&addr + 0x200*4)  = data0)
// #define p33_or_1byte(addr, data0)       addr |= (data0)

// void p33_tx_1byte(u16 addr, u8 data0);
#define p33_tx_1byte(addr, data0)       addr = data0

// u8 p33_rx_1byte(u16 addr);
#define p33_rx_1byte(addr)              addr

#define P33_CON_SET(sfr, start, len, data)  (sfr = (sfr & ~((~(0xff << (len))) << (start))) | \
	 (((data) & (~(0xff << (len)))) << (start)))

#define P33_CON_GET(sfr)    sfr

#define P33_ANA_CHECK(reg) (((reg & reg##_MASK) == reg##_RV) ? 1:0)

#if 1

#define p33_fast_access(reg, data, en)           \
{ 												 \
    if (en) {                                    \
		p33_or_1byte(reg, (u8)(data));               \
    } else {                                     \
		p33_and_1byte(reg, (u8)~(data));             \
    }                                            \
}

#else

#define p33_fast_access(reg, data, en)         \
{                                              \
	if (en) {                                  \
       	reg |= (data);                         \
	} else {                                   \
		reg &= ~(data);                        \
    }                                          \
}

#endif

//
//
//					for p33_analog
//
//
//
/**************************P3_ANA_FLOW*******************************/
#define RC_250K_EN(en)				p33_fast_access(P3_ANA_FLOW, BIT(7), en)

#define MVBG_EN(en)					p33_fast_access(P3_ANA_FLOW, BIT(6), en)

#define MVIO_VBAT_EN(en)			p33_fast_access(P3_ANA_FLOW, BIT(5), en)

#define MVIO_VPWR_EN(en)			p33_fast_access(P3_ANA_FLOW, BIT(4), en)

#define DCVD_LDO_EN(en)				p33_fast_access(P3_ANA_FLOW, BIT(2), en)

#define DVDD_EN(en)					p33_fast_access(P3_ANA_FLOW, BIT(0), en)

/**************************P3_ANA_KEEP*******************************/
#define RC_250K_KEEP(keep)		p33_fast_access(P3_ANA_KEEP, BIT(7), keep)

#define MAIN_BG_KEEP(keep)		p33_fast_access(P3_ANA_KEEP, BIT(6), keep)

#define MVIO_VBAT_KEEP(keep)	p33_fast_access(P3_ANA_KEEP, BIT(5), keep)

#define MVIO_VPWR_KEEP(keep)	p33_fast_access(P3_ANA_KEEP, BIT(5), keep)

#define DCVDD_KEEP(keep)		p33_fast_access(P3_ANA_KEEP, BIT(2), keep)

#define DVDD_KEEP(keep)			p33_fast_access(P3_ANA_KEEP, BIT(0), keep)

#define P33_SET_ANA_KEEP(keep)	p33_fast_access(P3_ANA_KEEP, BIT(0), keep)

/**************************P3_IVS_SET*******************************/
#define WVDD_SHORT_DVDD()			p33_fast_access(P3_IVS_SET, BIT(4), 1)

#define WVDD_SHORT_DVDD_DISABLE()	p33_fast_access(P3_IVS_CLR, BIT(4), 1)

#define MSYS_TO_P33_RST_MASK(en)	p33_fast_access(P3_IVS_SET,  BIT(5), en)

/**************************P3_ANA_FLOW1*******************************/
#define DVD_POR_EN(en)				p33_fast_access(P3_ANA_FLOW1, BIT(5), en)

#define DVD_IFULL_EN(en)			p33_fast_access(P3_ANA_FLOW1, BIT(4), en)

#define MVIO_VABT_IFULLEN(en)		p33_fast_access(P3_ANA_FLOW1, BIT(3), en)

#define MVIO_VPWR_IFULLEN(en)		p33_fast_access(P3_ANA_FLOW1, BIT(2), en)

#define MVIO_VBAT_VLMT_EN(en)		p33_fast_access(P3_ANA_FLOW1, BIT(1), en)

#define MVIO_VPWR_VLMT_EN(en)		p33_fast_access(P3_ANA_FLOW1, BIT(0), en)

/**************************P3_ANA_KEEP1*******************************/
#define DVD_POR_KEEP(keep)				p33_fast_access(P3_ANA_KEEP1, BIT(5), keep)

#define DVD_IFULL_KEEP(keep)			p33_fast_access(P3_ANA_KEEP1, BIT(4), keep)

#define MVIO_VABT_IFULL_KEEP(keep)		p33_fast_access(P3_ANA_KEEP1, BIT(3), keep)

#define MVIO_VPWR_IFULL_KEEP(keep)		p33_fast_access(P3_ANA_KEEP1, BIT(2), keep)

#define MVIO_VBAT_VLMT_KEEP(keep)		p33_fast_access(P3_ANA_KEEP1, BIT(1), keep)

#define MVIO_VPWR_VLMT_KEEP(keep)		p33_fast_access(P3_ANA_KEEP1, BIT(0), keep)

/**************************P3_WVD_CON0*******************************/
#define WVDD_LOAD_EN(en)			p33_fast_access(P3_WVD_CON0, BIT(5), en)

#define WVDD_DCVDDSHORT_EN(en)		p33_fast_access(P3_WVD_CON0, BIT(4), en)

#define WVD_EN(en)					p33_fast_access(P3_WVD_CON0, BIT(3), en)

/**************************P3_ANA_CON2*******************************/
#define SWVLD_AUTO_EN(en)			p33_fast_access(P3_ANA_CON2, BIT(7), en)

#define LPMR_RST_DLY(en)			p33_fast_access(P3_ANA_CON2, BIT(5), en)

#define VCM_DET_EN(en)				p33_fast_access(P3_ANA_CON2, BIT(3), en)

/**************************P3_PMU_ADC0*******************************/
#define ADC_CHANNEL_SEL(sel)		P33_CON_SET(P3_PMU_ADC0, 1, 4, sel)

#define PMU_DET_EN(en)				p33_fast_access(P3_PMU_ADC0, BIT(0), en)

#define GET_PMU_TEST_OE()			(P33_CON_GET(P3_PMU_ADC0) & BIT(0))

/**************************P3_PMU_ADC1*******************************/
#define VBG_TEST_SEL(sel)			P33_CON_SET(P3_PMU_ADC1, 2, 2, sel)

#define VBG_TEST_EN(en)				p33_fast_access(P3_PMU_ADC1, BIT(1), en)

#define GET_VBG_TEST_EN()			(P33_CON_GET(P3_PMU_ADC1) & BIT(1) ? 1:0)

#define VBG_BUFFER_EN(en)			p33_fast_access(P3_PMU_ADC1, BIT(0), en)

#define GET_VBG_BUFFER_EN()			(P33_CON_GET(P3_PMU_ADC1) & BIT(0) ? 1:0)

/**************************P3_IOV_CON0*******************************/
#define VDDIOW_VOL_SEL(sel)				P33_CON_SET(P3_IOV_CON0, 4, 4, sel)

#define GET_VDDIOW_VOL()				((P33_CON_GET(P3_IOV_CON0) & 0xf0)>>4)

enum {
    VDDIOW_VOL_21V,
    VDDIOW_VOL_22V,
    VDDIOW_VOL_23V,
    VDDIOW_VOL_24V,
    VDDIOW_VOL_25V,
    VDDIOW_VOL_26V,
    VDDIOW_VOL_27V,
    VDDIOW_VOL_28V,
    VDDIOW_VOL_29V,
    VDDIOW_VOL_30V,
    VDDIOW_VOL_31V,
    VDDIOW_VOL_32V,
    VDDIOW_VOL_33V,
    VDDIOW_VOL_34V,
    VDDIOW_VOL_35V,
    VDDIOW_VOL_36V,
};
#define VDDIOM_VOL_SEL(sel)				P33_CON_SET(P3_IOV_CON0, 0, 4, sel)

#define GET_VDDIOM_VOL()				(P33_CON_GET(P3_IOV_CON0) & 0x0f)

enum {
    VDDIOM_VOL_21V,
    VDDIOM_VOL_22V,
    VDDIOM_VOL_23V,
    VDDIOM_VOL_24V,
    VDDIOM_VOL_25V,
    VDDIOM_VOL_26V,
    VDDIOM_VOL_27V,
    VDDIOM_VOL_28V,
    VDDIOM_VOL_29V,
    VDDIOM_VOL_30V,
    VDDIOM_VOL_31V,
    VDDIOM_VOL_32V,
    VDDIOM_VOL_33V,
    VDDIOM_VOL_34V,
    VDDIOM_VOL_35V,
    VDDIOM_VOL_36V,
};

/**************************P3_IOV_CON1*******************************/
#define MVIO_HD1_0(sel)				P33_CON_SET(P3_IOV_CON1, 0, 2, sel)

/**************************P3_DCV_CON0*******************************/
#define DCVD_DEOVSHOT_EN(en)		p33_fast_access(P3_DCV_CON0, BIT(7), en)

#define DCVD_CAP_EN(en)				p33_fast_access(P3_DCV_CON0, BIT(6), en)

#define DCVD_HD1_0(en)				P33_CON_SET(P3_DCV_CON0, 4, 2, en)

#define DCVD_S(sel)					P33_CON_SET(P3_DCV_CON0, 0, 3, sel)

/**************************P3_DVD_CON0*******************************/
#define SYSVDD_DEOVSHOT_EN(en)			p33_fast_access(P3_DVD_CON0, BIT(7), sel)

#define SYSVDD_CAP_EN(en)				p33_fast_access(P3_DVD_CON0, BIT(6), en)

#define SYSVDD_VOL_HD_SEL(en)				P33_CON_SET(P3_DVD_CON0, 4, 2, en)

enum {
    SYSVDD_VOL_SEL_096V = 0, //0.96v
    SYSVDD_VOL_SEL_099V,
    SYSVDD_VOL_SEL_102V,
    SYSVDD_VOL_SEL_105V,
    SYSVDD_VOL_SEL_108V,
    SYSVDD_VOL_SEL_111V,
    SYSVDD_VOL_SEL_114V,
    SYSVDD_VOL_SEL_117V,
    SYSVDD_VOL_SEL_120V,
    SYSVDD_VOL_SEL_123V,
    SYSVDD_VOL_SEL_126V,
    SYSVDD_VOL_SEL_129V,
    SYSVDD_VOL_SEL_132V,
    SYSVDD_VOL_SEL_135V,
    SYSVDD_VOL_SEL_138V,
    SYSVDD_VOL_SEL_141V,
};
#define SYSVDD_VOL_SEL(sel)				P33_CON_SET(P3_DVD_CON0, 0, 4, sel)

/**************************P3_PGSD_CON*******************************/
#define PGSD_SSEL(sel)				P33_CON_SET(P3_PGSD_CON, 2, 2, sel)

#define PGSD_SON(en)				p33_fast_access(P3_PGSD_CON, BIT(1), en)

#define PGSD_HON(en)				p33_fast_access(P3_PGSD_CON, BIT(0), en)

/**************************P3_CHG_CON0*******************************/
#define CHG_SSEL(sel)				p33_fast_access(P3_CHG_CON0, BIT(7), sel)

#define CHGI_TURBO_EN(en)			p33_fast_access(P3_CHG_CON0, BIT(5), en)

#define CHG_CCLOOP_EN(en)			p33_fast_access(P3_CHG_CON0, BIT(4), en)

#define CHG_TRICKLE_EN(en)			p33_fast_access(P3_CHG_CON0, BIT(3), en)

#define CHG_HV_MODE(en)				p33_fast_access(P3_CHG_CON0, BIT(2), en)

#define CHARGE_GO(en)				p33_fast_access(P3_CHG_CON0, BIT(1), en)

#define CHARGE_EN(en)				p33_fast_access(P3_CHG_CON0, BIT(0), en)

#define IS_CHARGE_EN()			    ((P33_CON_GET(P3_CHG_CON0) & BIT(0)) ? 1: 0)

/**************************P3_CHG_CON1*******************************/
#define CHARGE_mA_SEL(sel)			P33_CON_SET(P3_CHG_CON1, 4, 4, sel)

#define CHARGE_FULL_V_SEL(sel)		P33_CON_SET(P3_CHG_CON1, 0, 4, sel)

/**************************P3_CHG_CON2*******************************/
#define CHARGE_FULL_mA_SEL(sel)		P33_CON_SET(P3_CHG_CON2, 4, 2, sel)

#define CHG_DET_VOL_SEL(sel)		P33_CON_SET(P3_CHG_CON2, 1, 2, sel)

#define CHG_DET_EN(en)				p33_fast_access(P3_CHG_CON2, BIT(0), en)

/**************************P3_CHG_CON3*******************************/
#define CHGI_TRIM_SEL(sel)			P33_CON_SET(P3_CHG_CON3, 0, 4, sel)

/**************************P3_VPWR_CON0*******************************/
#define VPWR_IO_MODE(en)		    p33_fast_access(P3_VPWR_CON0, BIT(2), en)

#define PMU_LTMODE_EN(en)			p33_fast_access(P3_VPWR_CON0, BIT(1), en)

#define VPWR_LOAD_EN(en)			p33_fast_access(P3_VPWR_CON0, BIT(0), en)

/**************************P3_VPWR_CON1*******************************/
#define VPWR_LOAD_S(sel)			P33_CON_SET(P3_VPWR_CON1, 0, 2, sel)

/**************************P3_VLVD_CON*******************************/
#define VLVD_PND_CLR()       		p33_fast_access(P3_VLVD_CON, BIT(6), 1)

#define VLVD_PND()          		((P33_CON_GET(P3_VLVD_CON) & BIT(7)) ? 1 : 0)

#define VLVD_SEL(sel)				P33_CON_SET(P3_VLVD_CON,  3, 3, sel)

#define VLVD_OE(en)					p33_fast_access(P3_VLVD_CON,  BIT(2), en)

#define VLVD_EXPIN_EN(en)			p33_fast_access(P3_VLVD_CON,  BIT(1), en)

#define P33_VLVD_EN(en)				p33_fast_access(P3_VLVD_CON,  BIT(0), en)

#define P33_GET_VLVD_CON()			P33_CON_GET(P3_VLVD_CON)

/**************************P3_VLVD_FLT*******************************/
#define VLVD_FLT(flt)				P33_CON_SET(P3_VLVD_FLT,  0, 2, flt)

/**************************P3_RST_CON0*******************************/

#define VLVD_WKUP_EN(en)			p33_fast_access(P3_RST_CON0,  BIT(3), en)

#define VLVD_RST_EN(en)				p33_fast_access(P3_RST_CON0,  BIT(2), en)

#define FAST_UP_EN(en)				p33_fast_access(P3_RST_CON0,  BIT(1), en)

/**************************P3_LRC_CON0*******************************/
#define LRC_Hz_DEFAULT    (200 * 1000L)

#define LRC_CON0_INIT                                     \
        /*RC32K_RNPS_S0_33v              */     (0 << 7) |\
        /*RC32K_RPPS_S2_33v              */     (1 << 6) |\
        /*RC32K_RPPS_S1_33v              */     (0 << 5) |\
        /*RC32K_RPPS_S0_33v              */     (0 << 4) |\
        /*                               */     (0 << 3) |\
        /*                               */     (0 << 2) |\
        /*RC32K_RN_TRIM_33v              */     (0 << 1) |\
        /*RC32K_EN_33v                   */     (1 << 0)


/**************************P3_LRC_CON1*******************************/
#define LRC_CON1_INIT                                     \
        /*                               */     (0 << 7) |\
        /*                               */     (0 << 6) |\
        /*RC32K_LDO_S0_33v               */     (1 << 5) |\
        /*RC32K_CAP_S2_33v               */     (1 << 4) |\
        /*RC32K_CAP_S1_33v               */     (0 << 3) |\
        /*RC32K_CAP_S0_33v               */     (1 << 2) |\
        /*RC32K_RNPS_S2_33v              */     (0 << 1) |\
        /*RC32K_RNPS_S1_33v              */     (0 << 0)

/**************************P3_CLK_CON0*******************************/
#define PWM_CLK_SEL(sel)			P33_CON_SET(P3_CLK_CON0,  0, 2, sel)

/**************************P3_VLD_KEEP*******************************/
#define VLD_KEEP_RTC_WKUP(en)		p33_fast_access(P3_VLD_KEEP,  BIT(7), en)

#define VLD_KEEP_WDT_EXPT(en)		p33_fast_access(P3_VLD_KEEP,  BIT(6), en)

#define VLD_KEEP_VDD_LEL(en)		p33_fast_access(P3_VLD_KEEP,  BIT(5), en)

#define VLD_KEEP_SYS_RST(en)		p33_fast_access(P3_VLD_KEEP,  BIT(4), en)

#define VLD_KEEP_PWM_CLK(en)		p33_fast_access(P3_VLD_KEEP,  BIT(3), en)

#define PLL_RCLK_DIS(en)			p33_fast_access(P3_VLD_KEEP,  BIT(2), en)

#define VLD_KEEP_WKUP(en)			p33_fast_access(P3_VLD_KEEP,  BIT(1), en)

#define VLD_KEEP_CLK(en)			p33_fast_access(P3_VLD_KEEP,  BIT(0), en)

/************************P3_AWKUP_LEVEL*****************************/
#define CHARGE_FULL_FILTER_GET()	((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(2)) ? 1: 0)

#define LVCMP_DET_FILTER_GET()      ((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(1)) ? 1: 0)

#define VPWR_DET_FILTER_GET()       ((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(0)) ? 1: 0)

/**************************P3_ANA_READ*******************************/
#define CHARGE_FULL_FLAG_GET()		((P33_CON_GET(P3_ANA_READ) & BIT(0)) ? 1: 0)

#define LVCMP_DET_GET()			    ((P33_CON_GET(P3_ANA_READ) & BIT(1)) ? 1: 0)

#define LDO5V_DET_GET()			    ((P33_CON_GET(P3_ANA_READ) & BIT(2)) ? 1: 0)

#define VBAT_DET_GET()			    ((P33_CON_GET(P3_ANA_READ) & BIT(3)) ? 1: 0)


/**************************P3_OSL_CON*******************************/
#define X32K_TEST(en)		p33_fast_access(P3_OSL_CON,  BIT(4), en)

#define X32K_HC1_0(sel)		P33_CON_SET(P3_OSL_CON,  2, 2, sel)

#define X32K_OE(en)			p33_fast_access(P3_OSL_CON,  BIT(1), en)

#define X32K_EN(en)			p33_fast_access(P3_OSL_CON,  BIT(0), en)

/**************************P3_OTP_CON*******************************/
#define PENVDD2(en)			p33_fast_access(P3_OTP_CON,  BIT(0), en)

//
//					for Reset_source
//
//
//
/***************************JL_RESET******************************/
#define GET_MAIN_SYS_POWER_FLAG() 	((JL_CLOCK->PWR_CON & BIT(7)) ? 1 : 0)

#define MAIN_SYS_POWERUP_CLEAR() 	JL_CLOCK->PWR_CON |= BIT(6)

#define GET_MAIN_SYS_RST_SRC()   	(JL_RST->RST_SRC)

/***************************P3_RST_SRC******************************/
#define GET_P33_SYS_POWER_FLAG() 		((P33_CON_GET(P3_RST_FLAG) & BIT(5)) ? 1 : 0)

#define P33_SYS_POWERUP_CLEAR()  		p33_fast_access(P3_RST_FLAG, BIT(4), 1)

#define GET_P33_SYS_POWER_FLAG_ALL() 	((P33_CON_GET(P3_RST_FLAG) & BIT(7)) ? 1 : 0)

#define P33_SYS_POWERUP_CLEAR_ALL()  	p33_fast_access(P3_RST_FLAG, BIT(6), 1)

#define GET_P33_SYS_RST_SRC()				P33_CON_GET(P3_RST_SRC)

/************************P3_PR_PWR*****************************/
#define	P3_SOFT_RESET()				P33_CON_SET(P3_PR_PWR, 4, 2, 3)

/************************WLDO06_AUTO*****************************/
enum {
    WLDO_LEVEL_060V = 0,
    WLDO_LEVEL_065V,
    WLDO_LEVEL_070V,
    WLDO_LEVEL_075V,
    WLDO_LEVEL_080V,
    WLDO_LEVEL_085V,
    WLDO_LEVEL_090V,
    WLDO_LEVEL_095V,
    WLDO_LEVEL_100V,
    WLDO_LEVEL_105V,
    WLDO_LEVEL_110V,
    WLDO_LEVEL_115V,
    WLDO_LEVEL_120V,
    WLDO_LEVEL_125V,
    WLDO_LEVEL_130V,
    WLDO_LEVEL_135V,
};

#define pd_wldo_auto_init                     \
        /* WLDO PRD     3bit RW  */ ( 2<<5 )  | \
        /*              1bit RW  */ ( 0<<4 )  | \
        /*              2bit RW  */ ( 0<<2 )  | \
        /*              2bit RW  */ ( 0<<0 )

#define pd_wldo06_auto_init                     \
        /* RESERVED     3bit RW  */ ( 0<<5 )  | \
        /* WLDO EN      1bit RW  */ ( 0<<4 )  | \
        /* WLDO LEVEL   3bit RW  */ (12<<0 )

#define pd_wldo06_auto1_init                    \
        /* RESERVED     3bit RW  */ ( 0<<5 )  | \
        /* RESERVED     1bit RW  */ ( 0<<4 )  | \
        /* WLDO LVL LOW 3bit RW  */ ( 0<<0 )

#define P33_SET_WLDO_AUTO(val)		P33_CON_SET(P3_WLDO_AUTO, 0, 8, val)

#define P33_SET_WLDO06_AUTO(val)	P33_CON_SET(P3_WLDO06_AUTO, 0, 8, val)

#define P33_SET_WLDO06_AUTO1(val)	P33_CON_SET(P3_WLDO06_AUTO1, 0, 8, val)

#define WLDO06_EN(en)				p33_fast_access(P3_WLDO06_AUTO, BIT(4), en)

/*************************P3_LS_XX********************************/

#define DVDD_LS_ENTER_PDOWN()  		P3_LS_P11		= BIT(2);      \
									P3_LS_IO_DLY    = BIT(2);      \
    								P3_LS_IO_ROM    = BIT(2);      \
    								P3_LS_FLASHA    = BIT(2);      \
    								P3_LS_FLASHB    = BIT(2);      \
    								P3_LS_PLL       = BIT(2);      \
									P3_LS_ADC       = BIT(2);      \
    								P3_LS_AUDIO     = BIT(2);      \
    								P3_LS_LCD       = BIT(2);      \
    								P3_LS_XOSC12M   = BIT(2);      \
    								P3_LS_P11   = BIT(2) | BIT(1);    \
    								P3_LS_IO_DLY   = BIT(2) | BIT(1);    \
    								P3_LS_IO_ROM   = BIT(2) | BIT(1); 	 \
    								P3_LS_FLASHA   = BIT(2) | BIT(1); 	 \
    								P3_LS_FLASHB   = BIT(2) | BIT(1); 	 \
    								P3_LS_PLL      = BIT(2) | BIT(1); 	 \
    								P3_LS_ADC      = BIT(2) | BIT(1); 	 \
    								P3_LS_AUDIO    = BIT(2) | BIT(1); 	 \
    								P3_LS_LCD      = BIT(2) | BIT(1); 	 \
    								P3_LS_XOSC12M  = BIT(2) | BIT(1)

#define DVDD_LS_EXIT_PDOWN()		P3_LS_P11      = 0;   \
									P3_LS_IO_DLY   = 0;   \
    								P3_LS_IO_ROM   = 0;   \
    								P3_LS_FLASHA   = 0;   \
    								P3_LS_FLASHB   = 0;   \
    								P3_LS_PLL      = 0;   \
									P3_LS_ADC      = 0;   \
    								P3_LS_AUDIO    = 0;   \
    								P3_LS_LCD      = 0;   \
    								P3_LS_XOSC12M  = 0


#define DVDD_LS_ENTER_SOFF()		P3_LS_IO_DLY   = BIT(0);      \
									P3_LS_IO_ROM   = BIT(0);      \
									P3_LS_FLASHA   = BIT(0);      \
									P3_LS_FLASHB   = BIT(0);      \
									P3_LS_PLL      = BIT(0);      \
									P3_LS_ADC      = BIT(0);      \
									P3_LS_AUDIO    = BIT(0);      \
									P3_LS_LCD      = BIT(0);      \
									P3_LS_XOSC12M  = BIT(0);      \
									P3_LS_IO_DLY   = BIT(0) | BIT(1);    \
									P3_LS_IO_ROM   = BIT(0) | BIT(1);    \
									P3_LS_FLASHA   = BIT(0) | BIT(1);    \
									P3_LS_FLASHB   = BIT(0) | BIT(1);    \
									P3_LS_PLL      = BIT(0) | BIT(1);    \
									P3_LS_ADC      = BIT(0) | BIT(1);    \
									P3_LS_AUDIO    = BIT(0) | BIT(1);    \
									P3_LS_LCD      = BIT(0) | BIT(1);    \
									P3_LS_XOSC12M  = BIT(0) | BIT(1)

#define DVDD_LS_EXIT_SOFF()			P3_LS_IO_DLY   = BIT(0);     \
									P3_LS_ADC      = BIT(0);     \
									P3_LS_AUDIO    = BIT(0);     \
									P3_LS_LCD      = BIT(0);     \
									P3_LS_XOSC12M  = BIT(0);     \
									P3_LS_IO_DLY   = 0;          \
									P3_LS_ADC      = 0;          \
									P3_LS_AUDIO    = 0;          \
									P3_LS_LCD      = 0;          \
									P3_LS_XOSC12M  = 0




//
//
//					for LP_TIMER0
//
//
//
/**************************P3_LP_TMR0*******************************/
#define P33_GET_LPTMR0_RSC()   	(P33_CON_GET(P3_LP_RSC00)<<24   |   \
							   	P33_CON_GET(P3_LP_RSC01) << 16 |   \
							   	P33_CON_GET(P3_LP_RSC02) << 8  |   \
							   	P33_CON_GET(P3_LP_RSC03))

#define P33_SET_LPTMR0_RSC(rsc)	p33_tx_1byte(P3_LP_RSC00, (rsc & 0xff<<24)>>24); \
								p33_tx_1byte(P3_LP_RSC01, (rsc & 0xff<<16)>>16); \
								p33_tx_1byte(P3_LP_RSC02, (rsc & 0xff<<8)>>8); \
								p33_tx_1byte(P3_LP_RSC03, (rsc & 0xff))

#define P33_GET_LPTMR0_PRD()   (P33_CON_GET(P3_LP_PRD00)<<24   |   \
							   P33_CON_GET(P3_LP_PRD01)<< 16 |   \
							   P33_CON_GET(P3_LP_PRD02)<< 8  |   \
							   P33_CON_GET(P3_LP_PRD03))

#define P33_SET_LPTMR0_PRD(prd)	p33_tx_1byte(P3_LP_PRD00, (prd & 0xff<<24)>>24); \
								p33_tx_1byte(P3_LP_PRD01, (prd & 0xff<<16)>>16); \
								p33_tx_1byte(P3_LP_PRD02, (prd & 0xff<<8)>>8); \
								p33_tx_1byte(P3_LP_PRD03, (prd & 0xff))

#define P33_GET_LPTMR0_CNT()   (P33_CON_GET(P3_LP_CNT0)<<24  |   \
							   P33_CON_GET(P3_LP_CNT1)<< 16 |   \
							   P33_CON_GET(P3_LP_CNT2)<< 8  |   \
							   P33_CON_GET(P3_LP_CNT3))

#define P33_SET_LPTMR0_CLK(val)	p33_tx_1byte(P3_LP_TMR0_CLK, val)

#define P33_SET_LPTMR0_CON(val)	p33_tx_1byte(P3_LP_TMR0_CON, val)

#define P33_GET_LPTMR0_CON()	(P33_CON_GET(P3_LP_TMR0_CON))

#define P3_LP_TMR0_EN(en)		p33_fast_access(P3_LP_TMR0_CON, BIT(0), en)

#define P3_LP_TMR0_WKUP_PND()	(P33_CON_GET(P3_LP_TMR0_CON) & BIT(5) ? 1:0)

#define P3_LP_TMR0_WKUP_CPND()	p33_fast_access(P3_LP_TMR0_CON, BIT(4), 1)

#define P3_LP_TMR0_TOUT_PND()	(P33_CON_GET(P3_LP_TMR0_CON) & BIT(7) ? 1:0)

#define P3_LP_TMR0_TOUT_CPND()	p33_fast_access(P3_LP_TMR0_CON, BIT(6), 1)

#define P3_LP_TMR0_CPND()		P3_LP_TMR0_TOUT_CPND(); \
 								P3_LP_TMR0_WKUP_CPND()

//
//
//					for RTC
//
//
//
/**************************R3_WKUP_SRC*******************************/
#define P33_SET_R3_OSL_CON(val)	P33_CON_SET(R3_OSL_CON, 0, 8, val)

/*
 *-------------------R3_ALM_CON
 */
#define ALM_CLK_SEL(a)	P33_CON_SET(R3_ALM_CON, 2, 3, a)

#define ALM_ALMEN(a)	P33_CON_SET(R3_ALM_CON, 0, 1, a)

//Macro for CLK_SEL
// enum RTC_CLK {
//     CLK_SEL_32K = 1,
//     CLK_SEL_XOSC_DIV1,
//     CLK_SEL_XOSC_DIV2,
//     CLK_SEL_LRC,
// };

//RTC_CLK_SEL
#define CLK_SEL_32K         1
#define CLK_SEL_XOSC_DIV1   2
#define CLK_SEL_XOSC_DIV2   3
#define CLK_SEL_LRC         4


/*
 *-------------------R3_RTC_CON0
 */
#define RTC_RTC_KICK(a)		P33_CON_SET(R3_RTC_CON0, 4, 1, a)



/*
 *-------------------R3_TIME_CPND
 */
#define TIME_256HZ_CPND(a)	P33_CON_SET(R3_TIME_CPND, 0, 1, a)

#define TIME_64HZ_CPND(a)	P33_CON_SET(R3_TIME_CPND, 1, 1, a)

#define TIME_2HZ_CPND(a)	P33_CON_SET(R3_TIME_CPND, 2, 1, a)

#define TIME_1HZ_CPND(a)	P33_CON_SET(R3_TIME_CPND, 3, 1, a)


/*
 *-------------------R3_TIME_PND
 */
#define TIME_256HZ_CON(a)	P33_CON_SET(R3_TIME_CON, 0, 1, a)

#define TIME_64HZ_CON(a)	P33_CON_SET(R3_TIME_CON, 1, 1, a)

#define TIME_2HZ_CON(a)		P33_CON_SET(R3_TIME_CON, 2, 1, a)

#define TIME_1HZ_CON(a)		P33_CON_SET(R3_TIME_CON, 3, 1, a)



#endif
