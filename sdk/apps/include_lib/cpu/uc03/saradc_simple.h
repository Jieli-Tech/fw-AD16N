#ifndef __SARADC_SIMPLE_H__
#define __SARADC_SIMPLE_H__
#include "typedef.h"
// CH_TPYE
#define ADC_IO_TYPE			(0 << 16)
#define ADC_PMU_TYPE		(1 << 16)

// IO_CH_SEL
#define ADC_CH_PA0          ((0x00 << 8) | ADC_IO_TYPE)
#define ADC_CH_PA4          ((0x01 << 8) | ADC_IO_TYPE)
#define ADC_CH_PA6          ((0x02 << 8) | ADC_IO_TYPE)
#define ADC_CH_PA8          ((0x03 << 8) | ADC_IO_TYPE)
#define ADC_CH_PA10         ((0x04 << 8) | ADC_IO_TYPE)
#define ADC_CH_PB0          ((0x05 << 8) | ADC_IO_TYPE)
#define ADC_CH_PB2          ((0x06 << 8) | ADC_IO_TYPE)
#define ADC_CH_PB4          ((0x07 << 8) | ADC_IO_TYPE)
#define ADC_CH_PB6          ((0x08 << 8) | ADC_IO_TYPE)
#define ADC_CH_PB7          ((0x09 << 8) | ADC_IO_TYPE)
#define ADC_CH_PB8          ((0x0a << 8) | ADC_IO_TYPE)
#define ADC_CH_PC0          ((0x0b << 8) | ADC_IO_TYPE)
#define ADC_CH_PC1          ((0x0c << 8) | ADC_IO_TYPE)
#define ADC_CH_PC3          ((0x0d << 8) | ADC_IO_TYPE)
#define ADC_CH_USBDP        ((0x0e << 8) | ADC_IO_TYPE)
#define ADC_CH_USBDM        ((0x0f << 8) | ADC_IO_TYPE)

// PMU_CH_SEL
#define ADC_CH_MVBG08       ((0x00 << 1) | ADC_PMU_TYPE)
#define ADC_CH_PROGI        ((0x02 << 1) | ADC_PMU_TYPE)
#define ADC_CH_PROGF        ((0x03 << 1) | ADC_PMU_TYPE)
#define ADC_CH_VTMP         ((0x04 << 1) | ADC_PMU_TYPE)
#define ADC_CH_1_4_VPWR     ((0x05 << 1) | ADC_PMU_TYPE)
#define ADC_CH_1_4_VBAT     ((0x06 << 1) | ADC_PMU_TYPE)
#define ADC_CH_1_2_VBAT     ((0x07 << 1) | ADC_PMU_TYPE)
#define ADC_CH_DCVD         ((0x0a << 1) | ADC_PMU_TYPE)
#define ADC_CH_DVDD         ((0x0b << 1) | ADC_PMU_TYPE)
#define ADC_CH_WVDD         ((0x0c << 1) | ADC_PMU_TYPE)

void saradc_init(void);
u32 saradc_get_value(u32 chn);
u32 get_iochn_ad_value(u32 io_chn);
u32 get_pmuchn_ad_value(u32 pmu_chn);
u32 saradc_get_vbat_voltage(void);
#endif
