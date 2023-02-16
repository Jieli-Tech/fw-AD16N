#ifndef __AUDIO_DAC_H__
#define __AUDIO_DAC_H__

#include "typedef.h"

#define AUDAC_CHANNEL_TOTAL   3

/********* define for DAC_CON0**********************************/
#define A_DAC_PND          BIT(7)
#define A_DAC_CLR_PND      BIT(6)
#define A_DAC_IE           BIT(5)
#define A_DAC_EN           BIT(4)
#define A_DAC_DFIFOR       BIT(8)

#define A_DAC_FADE_DIS     BIT(10)
#define A_DIF_EN           BIT(21)
#define A_DAC_SP_24B       BIT(20)
#define A_DAC_CH3_EN       BIT(19)
#define A_DAC_CH2_EN       BIT(18)
#define A_DAC_CH1_EN       BIT(17)
#define A_DAC_CH0_EN       BIT(16)

#define A_DAC_DEM_EN       BIT(31)
#define A_DAC_ACDIT_1_1    (0<<28)
#define A_DAC_ACDIT_1_2    (1<<28)
#define A_DAC_ACDIT_1_4    (2<<28)
#define A_DAC_ACDIT_1_8    (3<<28)
#define A_DAC_ACDIT_1_16   (4<<28)
#define A_DAC_ACDIT_1_32   (5<<28)
#define A_DAC_ACDIT_1_64   (6<<28)
#define A_DAC_ACDIT_1_128  (7<<28)

//dac 去直流滤波器配置
#define DAC_CHPSET_14      (14<<12)

//dac 输出采样率
#define DAC_SR8000         (0b1110<<0)
#define DAC_SR11025        (0b1101<<0)
#define DAC_SR12000        (0b1100<<0)
#define DAC_SR16000        (0b1010<<0)
#define DAC_SR22050        (0b1001<<0)
#define DAC_SR24000        (0b1000<<0)
#define DAC_SR32000        (0b0110<<0)
#define DAC_SR44100        (0b0101<<0)
#define DAC_SR48000        (0b0100<<0)
#define DAC_SR64000        (0b0010<<0)
#define DAC_SR88200        (0b0001<<0)
#define DAC_SR96000        (0b0000<<0)
#define DAC_SRBITS         (0b1111<<0)

//audio dac work mode
#define A_DAC_MONO_L       A_DAC_CH0_EN
#define A_DAC_MONO_R       A_DAC_CH1_EN
#define A_DAC_STERO        (A_DAC_CH1_EN | A_DAC_CH0_EN)
#define A_DAC_DIFF         (A_DIF_EN | A_DAC_CH0_EN)

// audio dac 声道数
#define AUDAC_OUTPUT_MODE  A_DAC_STERO
#if (A_DAC_STERO != AUDAC_OUTPUT_MODE)
#define DAC_TRACK_NUMBER   1
#else
#define DAC_TRACK_NUMBER   2
#endif

// audio dac 样点位宽
#define AUDAC_BIT_WIDE     16   //24
#if AUDAC_BIT_WIDE == 16
#define A_DAC_SP_BIT     0
#endif
#if AUDAC_BIT_WIDE == 24
#define A_DAC_SP_BIT     A_DAC_SP_24B
#endif

#ifdef D_IS_FLASH_SYSTEM
#define AUDAC_SHIFT_BITS   0
#define __AUDAC_SR_TYPE      u32
#else
#define AUDAC_SHIFT_BITS   2
#define __AUDAC_SR_TYPE      u16

#endif

#define DAC_CON0_DEFAULT        ( \
        A_DAC_DEM_EN       | \
        A_DAC_ACDIT_1_2    | \
        DAC_CHPSET_14      | \
        A_DAC_IE           | \
        A_DAC_SP_BIT       | \
        A_DAC_CLR_PND      | \
        AUDAC_OUTPUT_MODE)


/********* define for DAC_CON1**********************************/
#define audac_fade_slow(n) ((n & 0xf) << 8)
#define audac_fade_step(n) ((n & 0xf) << 4)
#define AUDAC_SRC_EN        BIT(1)
#define AUDAC_VIL_INV       BIT(0)

/************************************************************************/

typedef enum {
    DAC_SOURCE = 0,
    ALINE_SOURCE,
} AUDIO_TYPE;

typedef struct _DAC_CTRL_HDL {
    void *buf;
    u32  con;
    u32  pns;     //dac中断门槛
    u16  sp_total;
    u16  sp_max_free;//填充数据后允许的最大Free空间，不够填零
    u8   sp_size;
} DAC_CTRL_HDL;

typedef enum __AUDIO_DAC_ANA_VOL {
    AUDAC_M12db13 = 0,
    AUDAC_M10db13 = 1,
    AUDAC_M8db13 = 2,
    AUDAC_M6db13 = 3,
    AUDAC_M4db13 = 4,
    AUDAC_M2db13 = 5,
    AUDAC_M0db13 = 6,
    AUDAC_1db87 = 7,
} AUDIO_DAC_ANA_VOL;

typedef enum __AUDIO_DAC_AUMUX_VOL {
    AUAMUX_M6DB = 0,
    AUAMUX_M4DB = 1,
    AUAMUX_M2DB = 2,
    AUAMUX_0DB = 3,
    AUAMUX_2DB = 4,
    AUAMUX_4DB = 5,
    AUAMUX_6DB = 6,
    AUAMUX_8DB = 7,
} AUDIO_DAC_AUMX_VOL;

extern AUDIO_DAC_ANA_VOL const audio_dac_analog_vol_l;
extern AUDIO_DAC_ANA_VOL const audio_dac_analog_vol_r;

void audio_dac_isr(void);
void dac_resource_init(const DAC_CTRL_HDL *ops);
void dac_phy_init(u32 sr_sel);
void dac_phy_off(void);
u32 dac_sr_lookup(u32 sr);
u32 dac_sr_read(void);
void dac_phy_vol(u16 dac_l, u16 dac_r);
u32 dac_sr_set(u32 sr);

//模拟通道函数
void amux1_analog_open(void);
void audac_analog_open(void);
//模拟增益函数
void audio_dac_analog_vol(AUDIO_DAC_ANA_VOL vol_l, AUDIO_DAC_ANA_VOL vol_r);
void audio_dac_get_analog_vol(AUDIO_DAC_ANA_VOL *vol_l, AUDIO_DAC_ANA_VOL *vol_r);
void audio_amux_analog_vol(AUDIO_DAC_AUMX_VOL vol_l, AUDIO_DAC_AUMX_VOL vol_r);

void audac_analog_off_first_step(void);
void audac_analog_off_last_step(void);

void audac_analog_open(void);
void audac_analog_variate_init(void);

typedef enum __AUDIO_VCMO_VOLTAGE {
    AUDAC_VCMO_0v6 = 0,
    AUDAC_VCMO_0v8 = 1,
    AUDAC_VCMO_1v0 = 2,
    AUDAC_VCMO_1v2 = 3,
} AUDIO_VCMO_VOLTAGE;

extern bool const audio_dac_vcmo0_enable;
extern bool const audio_dac_vcmo1_enable;
extern AUDIO_VCMO_VOLTAGE const audio_dac_vcmo0_voltage;
extern AUDIO_VCMO_VOLTAGE const audio_dac_vcmo1_voltage;

void set_audio_dac_vcom0(bool enable, AUDIO_VCMO_VOLTAGE voltage);
void set_audio_dac_vcom1(bool enable, AUDIO_VCMO_VOLTAGE voltage);

void audio_dac_lpf(bool mute, bool mute_pa);//1-mute 0-unmute, 当mute=1时 mute_pa表示要不要关闭DAC输出
void audio_dac_hplpa(bool enable);//1-输出打开 0-输出关闭

typedef enum __AUDIO_DAC_PNS {
    AUDAC_PNS_CLOSE = 0,
    AUDAC_PNS_10K   = 1,
    AUDAC_PNS_100K  = 2,
    AUDAC_PNS_1OONA = 4,
} AUDIO_DAC_PNS;
void audio_dac_pns(AUDIO_DAC_PNS pns);

typedef enum __AUDIO_VCMO_PNS {
    AUVCMO_PNS_CLOSE = 0,
    AUVCMO_PNS_10K   = 1,
    AUVCMO_PNS_100K  = 2,
    AUVCMO_PNS_100NA = 4,
} AUDIO_VCMO_PNS;
void audio_vcmo_pns(AUDIO_VCMO_PNS pns);

extern bool const audio_dac_lpf_lenable;
extern bool const audio_dac_lpf_renable;
extern bool const audio_dac_lpf_lmute;
extern bool const audio_dac_lpf_rmute;
extern u32 fifo_dac_fill(u8 *buf, u32 len, AUDIO_TYPE type);

#endif
