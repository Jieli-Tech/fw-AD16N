#ifndef __ADC_API_H__
#define __ADC_API_H__

#include "typedef.h"

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

#define ADC_AUDIO_SUB       (0x1 << 10) //音频子系统
#define ADC_AUDIO_CAL       (0x3 << 10) //音频子系统计算因子

//AD channel define
#define AD_CH_PA0           (ADC_MUX_IO | 0x0)
#define AD_CH_PA4           (ADC_MUX_IO | 0x1)
#define AD_CH_PA6           (ADC_MUX_IO | 0x2)
#define AD_CH_PA8           (ADC_MUX_IO | 0x3)
#define AD_CH_PA10          (ADC_MUX_IO | 0x4)
#define AD_CH_PB0           (ADC_MUX_IO | 0x5)
#define AD_CH_PB2           (ADC_MUX_IO | 0x6)
#define AD_CH_PB4           (ADC_MUX_IO | 0x7)
#define AD_CH_PB6           (ADC_MUX_IO | 0x8)
#define AD_CH_PB7           (ADC_MUX_IO | 0x9)
#define AD_CH_PB8           (ADC_MUX_IO | 0xa)
#define AD_CH_PC0           (ADC_MUX_IO | 0xb)
#define AD_CH_PC1           (ADC_MUX_IO | 0xc)
#define AD_CH_PC3           (ADC_MUX_IO | 0xd)
#define AD_CH_DP            (ADC_MUX_IO | 0xe)
#define AD_CH_DM            (ADC_MUX_IO | 0xf)

//pmu channel define
#define AD_CH_PMU_WBG04     (ADC_PMU_WBG04 | ADC_AN_PMU | ADC_MUX_AN | 0x0)
#define AD_CH_PMU_MBG08     (ADC_PMU_MBG08 | ADC_AN_PMU | ADC_MUX_AN | 0x0)
#define AD_CH_PMU_LVDBG     (ADC_PMU_LVDBG | ADC_AN_PMU | ADC_MUX_AN | 0x0)
#define AD_CH_PMU_PROGI     (ADC_AN_PMU | ADC_MUX_AN | 0x2)
#define AD_CH_PMU_PROGF     (ADC_AN_PMU | ADC_MUX_AN | 0x3)
#define AD_CH_PMU_VTEMP     (ADC_AN_PMU | ADC_MUX_AN | 0x4)
#define AD_CH_PMU_1_4_VPWR  (ADC_AN_PMU | ADC_MUX_AN | 0x5)
#define AD_CH_PMU_1_4_VBAT  (ADC_AN_PMU | ADC_MUX_AN | 0x6)
#define AD_CH_PMU_1_2_VBAT  (ADC_AN_PMU | ADC_MUX_AN | 0x7)
#define AD_CH_PMU_DCVD      (ADC_AN_PMU | ADC_MUX_AN | 0xA)
#define AD_CH_PMU_DVDD      (ADC_AN_PMU | ADC_MUX_AN | 0xB)
#define AD_CH_PMU_WVDD      (ADC_AN_PMU | ADC_MUX_AN | 0xC)

//audio channel define
#define AD_CH_AUDIO_AIN_A0  (ADC_AN_AUDIO | ADC_MUX_AN | 0x0)
#define AD_CH_AUDIO_AIN_A1  (ADC_AN_AUDIO | ADC_MUX_AN | 0x1)
#define AD_CH_AUDIO_MICLDO  (ADC_AN_AUDIO | ADC_MUX_AN | 0x2)
#define AD_CH_AUDIO_ADCVDD  (ADC_AN_AUDIO | ADC_MUX_AN | 0x3)
#define AD_CH_AUDIO_QTVDD   (ADC_AN_AUDIO | ADC_MUX_AN | 0x4)
#define AD_CH_AUDIO_QTREF   (ADC_AN_AUDIO | ADC_MUX_AN | 0x5)
#define AD_CH_AUDIO_VOP     (ADC_AN_AUDIO | ADC_MUX_AN | 0x6)
#define AD_CH_AUDIO_VON     (ADC_AN_AUDIO | ADC_MUX_AN | 0x7)

#define AD_CH_AUDIO_BG      (ADC_AUDIO_SUB | ADC_AN_AUDIO | ADC_MUX_AN | 0x0)
#define AD_CH_AUDIO_VCM     (ADC_AUDIO_SUB | ADC_AN_AUDIO | ADC_MUX_AN | 0x2)
#define AD_CH_AUDIO_DACL    (ADC_AUDIO_SUB | ADC_AN_AUDIO | ADC_MUX_AN | 0x3)
#define AD_CH_AUDIO_DACR    (ADC_AUDIO_SUB | ADC_AN_AUDIO | ADC_MUX_AN | 0x4)
#define AD_CH_AUDIO_DACVDD  (ADC_AUDIO_SUB | ADC_AN_AUDIO | ADC_MUX_AN | 0x5)
#define AD_CH_AUDIO_RTZVDD  (ADC_AUDIO_SUB | ADC_AN_AUDIO | ADC_MUX_AN | 0x6)
#define AD_CH_AUDIO_VCMO0   (ADC_AUDIO_SUB | ADC_AN_AUDIO | ADC_MUX_AN | 0x7)
#define AD_CH_AUDIO_VCMO1   (ADC_AUDIO_SUB | ADC_AN_AUDIO | ADC_MUX_AN | 0x8)

#define ADC_STATUS_STOP     0//停止该通道的采集
#define ADC_STATUS_NORMAL   1//正常状态,每次采集
#define ADC_STATUS_START    2//开始保存数据,等待电压稳定后开始采集
#define ADC_STATUS_KEEP     3//保持数据,保持数据后,不在采集该通道,返回固定值

void adc_api_init(void);
u16 adc_api_get_value(u16 ch);
u16 adc_api_get_voltage(u16 ch);
void adc_api_scan(void);
void adc_api_sample_vbg(void);
void adc_api_set_channel_status(u16 ch, u8 status);

#endif

