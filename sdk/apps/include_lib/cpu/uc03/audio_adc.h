#ifndef __AU_ADC_H__
#define __AU_ADC_H__

#include "typedef.h"
// #include "audio_analog.h"




#define AU_ADC_ASYNC_PND     BIT(23)
#define AU_ADC_ASYNC_PND_CLR BIT(22)
#define AU_ADC_ASYNC_IE      BIT(21)
#define AU_ADC_24BIT         BIT(20)
#define AU_ADC_UDE           BIT(11)
#define AU_ADC_CIC_48M       BIT(9)
#define AU_ADC_FIFO_REST     BIT(8)
#define AU_ADC_PND           BIT(7)
#define AU_ADC_PND_CLR       BIT(6)
#define AU_ADC_IE            BIT(5)
#define AU_ADC_EN            BIT(4)

#define AU_ADC_CHE_BS        16
#define AU_ADC_CHE_LEN       (19 -16 +1)

#define AU_ADC_HPSET_BS      12
#define AU_ADC_HPSET_LEN     (15 - 12 + 1)
#define AU_ADC_HPSET_DEFAULT (14 << 12)

#define AU_ADC_SR_BS         0
#define AU_ADC_SR            0xf
#define AU_ADC_SR_LEN        (3 - 0 + 1)

#define AU_ADC_CHE           (1<<16)



#define AUDIO_ADC_SP_BITS     16
#define AUDIO_ADC_PACKET_SIZE (48*5)
#define AUDIO_ADC_SP_SIZE     (AUDIO_ADC_SP_BITS / 8)
#define AUDIO_ADC_SP_PNS      (AUDIO_ADC_PACKET_SIZE / 3)

#if (16 == AUDIO_ADC_SP_BITS)
#define AUDIO_ADC_CON_DEFAULT  ( AU_ADC_IE | AU_ADC_PND_CLR | AU_ADC_UDE | AU_ADC_CHE | AU_ADC_HPSET_DEFAULT)
#endif
#if (24 == AUDIO_ADC_SP_BITS)
#define AUDIO_ADC_CON_DEFAULT  ( AU_ADC_IE | AU_ADC_24BIT | AU_ADC_PND_CLR | AU_ADC_UDE | AU_ADC_CHE | AU_ADC_HPSET_DEFAULT)
#endif

#define auadc_cic_gain0(n)   (( n & 0x3f) << 16)
#define auadc_cic_gain1(n)   (( n & 0x3f) << 22)


typedef struct __AUDIO_ADC_CTRL_HDL {
    void *buf;
    u16 pns;
    u16 sp_total;
    u8  sp_size;
} AUDIO_ADC_CTRL_HDL;


void audio_adc_isr(void);
bool audio_adc_enable(void);
void audio_adc_disable(void);
u32 read_audio_adc_sr(void);
u32 get_audio_adc_sr(u32 sr);
void set_audio_adc_sr(u32 sr);
u32 audio_adc_phy_init(AUDIO_ADC_CTRL_HDL *p_adc, u32 sr, u32 con, u32 throw);
void audio_adc_phy_off(void);

void audio_adc_analog_init();
void audio_adc_analog_off();

void auadc_analog_variate_init(void);

#define MACRO_MIC_RS_INSIDE     0
#define MACRO_MIC_RS_OUTSIDE    1
#define MACRO_MIC_DIFF          2
typedef enum __AUMIC_RS_MODE {
    mic_rs_inside = 0,  //使用内部MICLDO+内部电阻
    mic_rs_outside = 1, //PA0硬脚输出MICLDO+外部电阻
    mic_rs_null = 3,    //外置电源+外置电阻
} AUDIO_MIC_RS_MODE;
extern AUDIO_MIC_RS_MODE const audio_adc_mic_rs_mode;

#define MACRO_MIC_INPUT_PA1             0
#define MACRO_MIC_INPUT_PA2             1
#define MACRO_MIC_INPUT_DIFF_PA1_PA2    2
#define MACRO_MIC_INPUT_DIFF_PA2_PA1    3
typedef enum __AUMIC_INPUT_MODE {
    mic_input_pa1 = 0,          //PA1单端输入
    mic_input_pa2 = 1,          //PA2单端输入
    mic_input_diff_pa1_pa2 = 2, //PA1输入到P端,PA2输入到N
    mic_input_diff_pa2_pa1 = 3, //PA2输入到P端,PA1输入到N
} AUDIO_MIC_INPUT_MODE;
extern AUDIO_MIC_INPUT_MODE const audio_adc_mic_input_mode;

typedef enum __AUDIO_MICLDO_VS {
    AUMIC_2v4 = 0,
    AUMIC_2v6 = 1,
    AUMIC_2v8 = 2,
    AUMIC_3v0 = 3,
    AUMIC_3v1 = 4,
    AUMIC_3v2 = 5,
    AUMIC_3v3 = 6,
    AUMIC_3v4 = 7,
} AUDIO_MICLDO_VS;
extern AUDIO_MICLDO_VS const audio_adc_mic_ldo_vs;

typedef enum __AUDIO_MICBIAS_RS {
    AUMIC_0k5 = 1,
    AUMIC_1k0 = 2,
    AUMIC_1k5 = 3,
    AUMIC_2k0 = 4,
    AUMIC_2k5 = 5,
    AUMIC_3k0 = 6,
    AUMIC_3k5 = 7,
    AUMIC_4k0 = 8,
    AUMIC_4k5 = 9,
    AUMIC_5k0 = 0xa,
    AUMIC_6k0 = 0xb,
    AUMIC_7k0 = 0xc,
    AUMIC_8k0 = 0xd,
    AUMIC_9k0 = 0xe,
    AUMIC_10k = 0xf,
    AUMIC_RS_READ = 0xff,
} AUDIO_MICBIAS_RS;
extern AUDIO_MICBIAS_RS const audio_adc_mic_bias_rs;

typedef enum __AUDIO_MICIN_PORT {
    AUMIC_NULL = 0,
    AUMIC_N = 1,
    AUMIC_P = 2,
    // AUMIC_MUTE = 3,
} AUDIO_MICIN_PORT;

extern u32 const audio_adc_mic_bias_2_vcmo0;
extern u32 const audio_adc_mic_bias_2_vcmo1;

typedef enum __AUDIO_MICPGA_G {
    AUMIC_M8db = 0,
    AUMIC_M6db = 1,
    AUMIC_M4db = 2,
    AUMIC_M2db = 3,//单端1vpp 差分2vpp
    AUMIC_0db = 4,
    AUMIC_2db = 5,
    AUMIC_4db = 6,
    AUMIC_6db = 7,
    AUMIC_8db = 8,
    AUMIC_10db = 9,
    AUMIC_12db,//0.2vpp 0.4vpp
    AUMIC_14db,//0.158vpp 0.31vpp
    AUMIC_16db,
    AUMIC_18db = 0x0d,//0.1vpp 0.2vpp
    AUMIC_20db,
    AUMIC_22db,
    AUMIC_24db,
    AUMIC_26db = 0x11,
    AUMIC_28db,
    AUMIC_30db,
    AUMIC_32db,
} AUDIO_MICPGA_G;

u32 set_auadc_mic_bias_rs(AUDIO_MICBIAS_RS bias_rs);
void set_auadc_micldo_vs(AUDIO_MICLDO_VS ldo_vs);
void set_auadc_micin_pa1(AUDIO_MICIN_PORT pa1);
void set_auadc_micin_pa2(AUDIO_MICIN_PORT pa2);
void set_auadc_mic_bias_2_vcom0(bool flag);
void set_auadc_mic_bias_2_vcom1(bool flag);
void set_auadc_mic_pga_6db(bool enable);
void set_auadc_mic_pga(AUDIO_MICPGA_G pga);




#endif
