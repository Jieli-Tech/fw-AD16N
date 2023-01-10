
#ifndef __UC03_P33__
#define __UC03_P33__

#define P33_ACCESS(x) (*(volatile u32 *)(0xf00000 + 0x0000 + x*4))
#define RTC_ACCESS(x) (*(volatile u32 *)(0xf00000 + 0x1000 + x*4))

//===========
//===============================================================================//
//
//
//
//===============================================================================//

//===============================================================================//
//
//      p33 vddio
//
//===============================================================================//
//............. 0X0000 - 0X000F............ for analog control
#define P3_ANA_FLOW                   P33_ACCESS((0x00))
#define P3_WVD_CON0                   P33_ACCESS((0x01))
#define P3_ANA_CON2                   P33_ACCESS((0x02))
#define P3_VBG_CON0                   P33_ACCESS((0x03))

#define P3_IOV_CON0                   P33_ACCESS((0x05))
#define P3_IOV_CON1                   P33_ACCESS((0x06))


#define P3_DVD_CON0                   P33_ACCESS((0x09))
#define P3_ANA_CON10                  P33_ACCESS((0x0a))
#define P3_DCV_CON0                   P33_ACCESS((0x0b))
#define P3_ANA_CON12                  P33_ACCESS((0x0c))
#define P3_ANA_CON13                  P33_ACCESS((0x0d))
#define P3_PMU_ADC0                   P33_ACCESS((0x0e))
#define P3_PMU_ADC1                   P33_ACCESS((0x0f))

//............. 0X0010 - 0X001F............ for analog others
#define P3_ANA_MFIX                   P33_ACCESS((0x10))
#define P3_RST_FLAG                   P33_ACCESS((0x11))
#define P3_RST_SRC                    P33_ACCESS((0x12))
#define P3_LRC_CON0                   P33_ACCESS((0x13))
#define P3_LRC_CON1                   P33_ACCESS((0x14))
#define P3_RST_CON0                   P33_ACCESS((0x15))
#define P3_ANA_KEEP                   P33_ACCESS((0x16))
#define P3_VLD_KEEP                   P33_ACCESS((0x17))
#define P3_ANA_KEEP1                  P33_ACCESS((0x18))
#define P3_ANA_READ                   P33_ACCESS((0x19))
#define P3_CHG_CON0                   P33_ACCESS((0x1a))
#define P3_CHG_CON1                   P33_ACCESS((0x1b))
#define P3_CHG_CON2                   P33_ACCESS((0x1c))
#define P3_CHG_CON3                   P33_ACCESS((0x1d))
#define P3_VPWR_CON0                  P33_ACCESS((0x1e))
#define P3_VPWR_CON1                  P33_ACCESS((0x1f))

//............. 0X0020 - 0X002F............ for
#define P3_EFUSE_CON0                 P33_ACCESS((0x20))
#define P3_EFUSE_CON1                 P33_ACCESS((0x21))
#define P3_EFUSE_RDAT                 P33_ACCESS((0x22))
#define P3_EFUSE_PU_DAT0              P33_ACCESS((0x23))
#define P3_EFUSE_PU_DAT1              P33_ACCESS((0x24))
#define P3_OSL_CON                    P33_ACCESS((0x25))
#define P3_CLK_CON0                   P33_ACCESS((0x26))
#define P3_PR_PWR                     P33_ACCESS((0x27))

#define P3_WKUP_SRC                   P33_ACCESS((0x28))
#define P3_WKUP_SUB                   P33_ACCESS((0x29))
#define P3_ANA_FLOW1                  P33_ACCESS((0x2a))

#define P3_VLVD_CON                   P33_ACCESS((0x2d))
#define P3_VLVD_FLT                   P33_ACCESS((0x2e))

#define P3_LS_P11                     P33_ACCESS((0x2f))

//............. 0X0030 - 0X003F............ for
#define P3_PMU_CON0                   P33_ACCESS((0x30))
#define P3_PMU_CON1                   P33_ACCESS((0x31))

#define P3_PMU_CON4                   P33_ACCESS((0x34))
#define P3_PMU_CON5                   P33_ACCESS((0x35))
#define P3_LP_PRP0                    P33_ACCESS((0x36))
#define P3_LP_PRP1                    P33_ACCESS((0x37))
#define P3_LP_STB0_STB1               P33_ACCESS((0x38))
#define P3_LP_STB2_STB3               P33_ACCESS((0x39))
#define P3_LP_STB4_STB5               P33_ACCESS((0x3a))
#define P3_LP_STB6                    P33_ACCESS((0x3b))
#define P3_WDT_CON                    P33_ACCESS((0x3c))
#define P3_OTP_CON                    P33_ACCESS((0x3d))

//............. 0X0040 - 0X005F............ for PMU timer
#define P3_LP_RSC00                   P33_ACCESS((0x40))
#define P3_LP_RSC01                   P33_ACCESS((0x41))
#define P3_LP_RSC02                   P33_ACCESS((0x42))
#define P3_LP_RSC03                   P33_ACCESS((0x43))
#define P3_LP_PRD00                   P33_ACCESS((0x44))
#define P3_LP_PRD01                   P33_ACCESS((0x45))
#define P3_LP_PRD02                   P33_ACCESS((0x46))
#define P3_LP_PRD03                   P33_ACCESS((0x47))
#define P3_LP_RSC10                   P33_ACCESS((0x48))
#define P3_LP_RSC11                   P33_ACCESS((0x49))
#define P3_LP_RSC12                   P33_ACCESS((0x4a))
#define P3_LP_RSC13                   P33_ACCESS((0x4b))
#define P3_LP_PRD10                   P33_ACCESS((0x4c))
#define P3_LP_PRD11                   P33_ACCESS((0x4d))
#define P3_LP_PRD12                   P33_ACCESS((0x4e))
#define P3_LP_PRD13                   P33_ACCESS((0x4f))
//
#define P3_LP_TMR0_CLK                P33_ACCESS((0x54))

#define P3_LP_TMR0_CON                P33_ACCESS((0x58))
#define P3_LP_TMR1_CON                P33_ACCESS((0x59))


//............. 0X0060 - 0X006F............ for PMU control
#define P3_LP_CNTRD0                  P33_ACCESS((0x60))
#define P3_LP_CNTRD1                  P33_ACCESS((0x61))
#define P3_LP_CNT0                    P33_ACCESS((0x62))
#define P3_LP_CNT1                    P33_ACCESS((0x63))
#define P3_LP_CNT2                    P33_ACCESS((0x64))
#define P3_LP_CNT3                    P33_ACCESS((0x65))

#define P3_MSTM_RD                    P33_ACCESS((0x68))
#define P3_IVS_RD                     P33_ACCESS((0x69))
#define P3_IVS_SET                    P33_ACCESS((0x6a))
#define P3_IVS_CLR                    P33_ACCESS((0x6b))
#define P3_WLDO_AUTO                  P33_ACCESS((0x6c))
#define P3_WLDO06_AUTO                P33_ACCESS((0x6d))
#define P3_WLDO06_AUTO1               P33_ACCESS((0x6e))

//............. 0X0070 - 0X007F............ for analog level shifter control
#define P3_LS_IO_DLY                  P33_ACCESS((0x70))
#define P3_LS_IO_ROM                  P33_ACCESS((0x71))
#define P3_LS_FLASHA                  P33_ACCESS((0x72))
#define P3_LS_FLASHB                  P33_ACCESS((0x73))
#define P3_LS_PLL                     P33_ACCESS((0x74))
#define P3_LS_ADC                     P33_ACCESS((0x75))
#define P3_LS_AUDIO                   P33_ACCESS((0x76))
#define P3_LS_LCD                     P33_ACCESS((0x77))
#define P3_LS_XOSC12M                 P33_ACCESS((0x78))
#define P3_LS_CON9                    P33_ACCESS((0x79))
#define P3_LS_CONA                    P33_ACCESS((0x7a))
#define P3_LS_CONB                    P33_ACCESS((0x7b))
#define P3_LS_CONC                    P33_ACCESS((0x7c))
#define P3_LS_COND                    P33_ACCESS((0x7d))
#define P3_LS_CONE                    P33_ACCESS((0x7e))
#define P3_LS_CONF                    P33_ACCESS((0x7f))

//............. 0X0080 - 0X008F............ for port input select
#define P3_PORT_SEL0                  P33_ACCESS((0x80))
#define P3_PORT_SEL1                  P33_ACCESS((0x81))
#define P3_PORT_SEL2                  P33_ACCESS((0x82))
#define P3_PORT_SEL3                  P33_ACCESS((0x83))
#define P3_PORT_SEL4                  P33_ACCESS((0x84))
#define P3_PORT_SEL5                  P33_ACCESS((0x85))
#define P3_PORT_SEL6                  P33_ACCESS((0x86))
#define P3_PORT_SEL7                  P33_ACCESS((0x87))
#define P3_PORT_SEL8                  P33_ACCESS((0x88))
#define P3_PORT_SEL9                  P33_ACCESS((0x89))
#define P3_PORT_SEL10                 P33_ACCESS((0x8a))
#define P3_PORT_SEL11                 P33_ACCESS((0x8b))
#define P3_PORT_SEL12                 P33_ACCESS((0x8c))
#define P3_PORT_SEL13                 P33_ACCESS((0x8d))
#define P3_PORT_SEL14                 P33_ACCESS((0x8e))
#define P3_PORT_SEL15                 P33_ACCESS((0x8f))

//............. 0X0090 - 0X009F............ for wkup
#define P3_WKUP_EN0                   P33_ACCESS((0x90))
#define P3_WKUP_EN1                   P33_ACCESS((0x91))
#define P3_WKUP_EDGE0                 P33_ACCESS((0x92))
#define P3_WKUP_EDGE1                 P33_ACCESS((0x93))
#define P3_WKUP_LEVEL0                P33_ACCESS((0x94))
#define P3_WKUP_LEVEL1                P33_ACCESS((0x95))
#define P3_WKUP_PND0                  P33_ACCESS((0x96))
#define P3_WKUP_PND1                  P33_ACCESS((0x97))
#define P3_WKUP_CPND0                 P33_ACCESS((0x98))
#define P3_WKUP_CPND1                 P33_ACCESS((0x99))

//............. 0X00A0 - 0X00AF............
#define P3_AWKUP_EN                   P33_ACCESS((0xa0))
#define P3_AWKUP_P_IE                 P33_ACCESS((0xa1))
#define P3_AWKUP_N_IE                 P33_ACCESS((0xa2))
#define P3_AWKUP_LEVEL                P33_ACCESS((0xa3))
#define P3_AWKUP_INSEL                P33_ACCESS((0xa4))
#define P3_AWKUP_P_PND                P33_ACCESS((0xa5))
#define P3_AWKUP_N_PND                P33_ACCESS((0xa6))
#define P3_AWKUP_P_CPND               P33_ACCESS((0xa7))
#define P3_AWKUP_N_CPND               P33_ACCESS((0xa8))

//............. 0X00B0 - 0X00BF............ for
#define P3_AWKUP_FLT0                 P33_ACCESS((0xb0))
#define P3_AWKUP_FLT1                 P33_ACCESS((0xb1))
#define P3_AWKUP_FLT2                 P33_ACCESS((0xb2))

#define P3_APORT_SEL0                 P33_ACCESS((0xb8))
#define P3_APORT_SEL1                 P33_ACCESS((0xb9))
#define P3_APORT_SEL2                 P33_ACCESS((0xba))

//............. 0X00C0 - 0X00CF............ for
#define P3_PGDR_CON0                  P33_ACCESS((0xc0))
#define P3_PGDR_CON1                  P33_ACCESS((0xc1))
#define P3_PGSD_CON                   P33_ACCESS((0xc2))
#define P3_PGFS_CON                   P33_ACCESS((0xc3))
#define P3_PINR_CON                   P33_ACCESS((0xc4))
#define P3_PINR_CON1                  P33_ACCESS((0xc5))
#define P3_PCNT_CON                   P33_ACCESS((0xc6))
#define P3_PCNT_VLUE                  P33_ACCESS((0xc7))
#define P3_PR_IN                      P33_ACCESS((0xc8))
#define P3_PR_OUT                     P33_ACCESS((0xc9))
#define P3_PR_DIR                     P33_ACCESS((0xca))
#define P3_PR_DIE                     P33_ACCESS((0xcb))
#define P3_PR_PU                      P33_ACCESS((0xcc))
#define P3_PR_PD                      P33_ACCESS((0xcd))
#define P3_PR_HD                      P33_ACCESS((0xce))


//............. 0X00D0 - 0X00FF............  48byte latch ram reserved
#define P3_RAM_00                     P33_ACCESS((0xd0))
#define P3_RAM_01                     P33_ACCESS((0xd1))
#define P3_RAM_02                     P33_ACCESS((0xd2))
#define P3_RAM_03                     P33_ACCESS((0xd3))
#define P3_RAM_04                     P33_ACCESS((0xd4))
#define P3_RAM_05                     P33_ACCESS((0xd5))
#define P3_RAM_06                     P33_ACCESS((0xd6))
#define P3_RAM_07                     P33_ACCESS((0xd7))
#define P3_RAM_08                     P33_ACCESS((0xd8))
#define P3_RAM_09                     P33_ACCESS((0xd9))
#define P3_RAM_0A                     P33_ACCESS((0xda))
#define P3_RAM_0B                     P33_ACCESS((0xdb))
#define P3_RAM_0C                     P33_ACCESS((0xdc))
#define P3_RAM_0D                     P33_ACCESS((0xdd))
#define P3_RAM_0E                     P33_ACCESS((0xde))
#define P3_RAM_0F                     P33_ACCESS((0xdf))

#define P3_RAM_10                     P33_ACCESS((0xe0))
#define P3_RAM_11                     P33_ACCESS((0xe1))
#define P3_RAM_12                     P33_ACCESS((0xe2))
#define P3_RAM_13                     P33_ACCESS((0xe3))
#define P3_RAM_14                     P33_ACCESS((0xe4))
#define P3_RAM_15                     P33_ACCESS((0xe5))
#define P3_RAM_16                     P33_ACCESS((0xe6))
#define P3_RAM_17                     P33_ACCESS((0xe7))
//  #define P3_RAM_18                     P33_ACCESS((0xe8))
//  #define P3_RAM_19                     P33_ACCESS((0xe9))
//  #define P3_RAM_1A                     P33_ACCESS((0xea))
//  #define P3_RAM_1B                     P33_ACCESS((0xeb))
//  #define P3_RAM_1C                     P33_ACCESS((0xec))
//  #define P3_RAM_1D                     P33_ACCESS((0xed))
//  #define P3_RAM_1E                     P33_ACCESS((0xee))
//  #define P3_RAM_1F                     P33_ACCESS((0xef))
//
//  #define P3_RAM_20                     P33_ACCESS((0xf0))
//  #define P3_RAM_21                     P33_ACCESS((0xf1))
//  #define P3_RAM_22                     P33_ACCESS((0xf2))
//  #define P3_RAM_23                     P33_ACCESS((0xf3))
//  #define P3_RAM_24                     P33_ACCESS((0xf4))
//  #define P3_RAM_25                     P33_ACCESS((0xf5))
//  #define P3_RAM_26                     P33_ACCESS((0xf6))
//  #define P3_RAM_27                     P33_ACCESS((0xf7))
//  #define P3_RAM_28                     P33_ACCESS((0xf8))
//  #define P3_RAM_29                     P33_ACCESS((0xf9))
//  #define P3_RAM_2A                     P33_ACCESS((0xfa))
//  #define P3_RAM_2B                     P33_ACCESS((0xfb))
//  #define P3_RAM_2C                     P33_ACCESS((0xfc))
//  #define P3_RAM_2D                     P33_ACCESS((0xfd))
//  #define P3_RAM_2E                     P33_ACCESS((0xfe))
//  #define P3_RAM_2F                     P33_ACCESS((0xff)) //NOTE: key address for spi request
#define P3_KEY                        P33_ACCESS((0xff))

//===============================================================================//
//
//      p33 rtcvdd
//
//===============================================================================//

//............. 0X0080 - 0X008F............ for RTC
#define R3_ALM_CON                    RTC_ACCESS((0x80))

#define R3_RTC_CON0                   RTC_ACCESS((0x84))
#define R3_RTC_CON1                   RTC_ACCESS((0x85))
#define R3_RTC_DAT0                   RTC_ACCESS((0x86))
#define R3_RTC_DAT1                   RTC_ACCESS((0x87))
#define R3_RTC_DAT2                   RTC_ACCESS((0x88))
#define R3_RTC_DAT3                   RTC_ACCESS((0x89))
#define R3_RTC_DAT4                   RTC_ACCESS((0x8a))
#define R3_ALM_DAT0                   RTC_ACCESS((0x8b))
#define R3_ALM_DAT1                   RTC_ACCESS((0x8c))
#define R3_ALM_DAT2                   RTC_ACCESS((0x8d))
#define R3_ALM_DAT3                   RTC_ACCESS((0x8e))
#define R3_ALM_DAT4                   RTC_ACCESS((0x8f))

//............. 0X0090 - 0X009F............ for PORT control
#define R3_WKUP_EN                    RTC_ACCESS((0x90))
#define R3_WKUP_EDGE                  RTC_ACCESS((0x91))
#define R3_WKUP_CPND                  RTC_ACCESS((0x92))
#define R3_WKUP_PND                   RTC_ACCESS((0x93))
#define R3_WKUP_LEVEL                 RTC_ACCESS((0x94))

#define R3_PR_IN                      RTC_ACCESS((0x98))
#define R3_PR_OUT                     RTC_ACCESS((0x99))
#define R3_PR_DIR                     RTC_ACCESS((0x9a))
#define R3_PR_DIE                     RTC_ACCESS((0x9b))
#define R3_PR_PU                      RTC_ACCESS((0x9c))
#define R3_PR_PD                      RTC_ACCESS((0x9d))
#define R3_PR_HD                      RTC_ACCESS((0x9e))

//............. 0X00A0 - 0X00AF............ for system
#define R3_TIME_CON                   RTC_ACCESS((0xa0))
#define R3_TIME_CPND                  RTC_ACCESS((0xa1))
#define R3_TIME_PND                   RTC_ACCESS((0xa2))

#define R3_ADC_CON                    RTC_ACCESS((0xa4))
#define R3_OSL_CON                    RTC_ACCESS((0xa5))

#define R3_WKUP_SRC                   RTC_ACCESS((0xa8))
#define R3_RST_SRC                    RTC_ACCESS((0xa9))

#define R3_RST_CON                    RTC_ACCESS((0xab))
#define R3_CLK_CON                    RTC_ACCESS((0xac))

#include "p33_app.h"

#include "asm/power/p33_io_app.h"

#endif
