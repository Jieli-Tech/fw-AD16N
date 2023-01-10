#ifndef __SARADC_ISR_H__
#define __SARADC_ISR_H__
#include "typedef.h"

#define SARADC_ISR_MODE

#define ADC_MUX_IO          (0x1 << 8) //普通IO
#define ADC_MUX_AN          (0x2 << 8) //内部模拟信号
#define ADC_MUX_CAL         (0x3 << 8) //MUX计算因子
#define ADC_AN_PMU          (0x0 << 4) //内部PMU通道
#define ADC_AN_AUDIO        (0x1 << 4) //内部audio通道
#define ADC_AN_PLL          (0x2 << 4) //内部PLL通道
#define ADC_AN_X32K         (0x3 << 4) //内部时钟
#define ADC_AN_X12M         (0x4 << 4) //内部时钟
#define ADC_AN_CAL          (0x7 << 4) //内部通道计算因子

#define ADC_PMU_WBG04       (0x0 << 10) //内部电源WBG
#define ADC_PMU_MBG08       (0x1 << 10) //内部电源MBG
#define ADC_PMU_LVDBG       (0x2 << 10) //内部电源LVDBG
#define ADC_PMU_CAL         (0x3 << 10) //内部电源计算因子


//AD channel define
#define ADC_CH_PA0           (ADC_MUX_IO | 0x0)
#define ADC_CH_PA4           (ADC_MUX_IO | 0x1)
#define ADC_CH_PA6           (ADC_MUX_IO | 0x2)
#define ADC_CH_PA8           (ADC_MUX_IO | 0x3)
#define ADC_CH_PA10          (ADC_MUX_IO | 0x4)
#define ADC_CH_PB0           (ADC_MUX_IO | 0x5)
#define ADC_CH_PB2           (ADC_MUX_IO | 0x6)
#define ADC_CH_PB4           (ADC_MUX_IO | 0x7)
#define ADC_CH_PB6           (ADC_MUX_IO | 0x8)
#define ADC_CH_PB7           (ADC_MUX_IO | 0x9)
#define ADC_CH_PB8           (ADC_MUX_IO | 0xa)
#define ADC_CH_PC0           (ADC_MUX_IO | 0xb)
#define ADC_CH_PC1           (ADC_MUX_IO | 0xc)
#define ADC_CH_PC3           (ADC_MUX_IO | 0xd)
#define ADC_CH_USBDP            (ADC_MUX_IO | 0xe)
#define ADC_CH_USBDM            (ADC_MUX_IO | 0xf)


#define AD_VALUE_NONE       0Xff
#define AD_CH_NONE          0Xfe

//pmu channel define
#define ADC_CH_PMU_WBG04     (ADC_PMU_WBG04 | ADC_AN_PMU | ADC_MUX_AN | 0x0)
#define ADC_CH_PMU_MBG08     (ADC_PMU_MBG08 | ADC_AN_PMU | ADC_MUX_AN | 0x0)
#define ADC_CH_PMU_LVDBG     (ADC_PMU_LVDBG | ADC_AN_PMU | ADC_MUX_AN | 0x0)
#define ADC_CH_PMU_PROGI     (ADC_AN_PMU | ADC_MUX_AN | 0x2)
#define ADC_CH_PMU_PROGF     (ADC_AN_PMU | ADC_MUX_AN | 0x3)
#define ADC_CH_PMU_VTEMP     (ADC_AN_PMU | ADC_MUX_AN | 0x4)
#define ADC_CH_PMU_1_4_VPWR  (ADC_AN_PMU | ADC_MUX_AN | 0x5)
#define ADC_CH_PMU_1_4_VBAT  (ADC_AN_PMU | ADC_MUX_AN | 0x6)
#define ADC_CH_PMU_1_2_VBAT  (ADC_AN_PMU | ADC_MUX_AN | 0x7)
#define ADC_CH_PMU_DCVD      (ADC_AN_PMU | ADC_MUX_AN | 0xA)
#define ADC_CH_PMU_DVDD      (ADC_AN_PMU | ADC_MUX_AN | 0xB)
#define ADC_CH_PMU_WVDD      (ADC_AN_PMU | ADC_MUX_AN | 0xC)

#define AD_MAX_CH           28

void saradc_init(void);
u16 saradc_get_value(u16 ch);
u32 saradc_value2voltage(u32 adc_vbg, u32 adc_ch_val);
u32 saradc_get_vbat_voltage(void);

void saradc_add_sample_ch(u16 real_ch);
void saradc_remove_sample_ch(u16 real_ch);
void saradc_scan(void);
#endif
