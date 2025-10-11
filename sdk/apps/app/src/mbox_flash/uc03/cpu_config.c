#include "includes.h"
#include "app_config.h"
#include "audio_adc.h"
#include "audio_dac.h"
#include "app_modules.h"
#include "gpio.h"

//开启异常处理模块的lite打印版本(为了省代码空间,下面的控制变量必须逐级开启增加打印)
//异常打印模块使能
const u8 config_exception_enable = 1;
//1.仅报显示触发单元的寄存器，报错后可以去到对应的异常单元取消对应的使能进行查看或者查看对应的emu文档
const u8 config_exception_unit_lite_lite_enable  = 1;
//1.仅报触发单元的id信息，报错后可以去到对应的异常单元取消对应的使能进行查看或者查看对应的emu文档
const u8 config_exception_unit_lite_enable       = 1;
//1.不报dev信息，报错后可以去到对应的异常单元取消对应的使能进行查看
const u8 config_exception_dev_lite_enable        = 1;

/*********************set cache way num********************************/
const u8 cache_way_num = CPU_USE_CACHE_WAY_NUMBER;

/*************audio dac analog config************************************/
const bool config_vcm_cap_addon = 0;// 0 :vcm不会外挂电容；1：vcm会外挂电容；

/************update*************************************/
#if TFG_SDPG_ENABLE
const u8 dev_update_power_io = IO_PORTC_03;
#else
const u8 dev_update_power_io = -1;
#endif

//升级使用区域， 0:VM区, 1:eeprom区
const u8 dev_update_use_eeprom = 0;
//升级时是否保持IO状态
const u8 dev_update_keep_io_status = 0;
//ufw文件的vid要求：0:vid相同  1:vid不同
const u8 ufw_vid_need_to_be_different = 0;

/*************audio dac analog config*************************************
 * 以下配置，作用为系统音频 DAC 模块开机默认配置
 * */
// vcmo0/1 输出电压使能，以及响应的电压挡位设置；
bool const audio_dac_vcmo0_enable = 0;
bool const audio_dac_vcmo1_enable = 0;
AUDIO_VCMO_VOLTAGE const audio_dac_vcmo0_voltage = AUDAC_VCMO_1v2;
AUDIO_VCMO_VOLTAGE const audio_dac_vcmo1_voltage = AUDAC_VCMO_1v2;

//模拟增益挡位
AUDIO_DAC_ANA_VOL const audio_dac_analog_vol_l = AUDAC_1db87;
AUDIO_DAC_ANA_VOL const audio_dac_analog_vol_r = AUDAC_1db87;

// 左右声道音频输出声能
bool const audio_dac_lpf_lenable = 1;
bool const audio_dac_lpf_renable = 1;

// 左右声道静音使能
bool const audio_dac_lpf_lmute = 0;
bool const audio_dac_lpf_rmute = 0;

#if (DAC_TRACK_NUMBER == 1)
const bool config_dac_points_combine = 1;// 单声道融合左右声道数据 1:融合 0:不融合
/* 融合公式:L*Lgain/Totalgain + R*Rgain/Totalgain */
const u8 config_dac_points_Lgain = 128;
const u8 config_dac_points_Rgain = 128;
const u8 config_dac_points_Totalgain = 128;
#else
const bool config_dac_points_combine = 0;
#endif

/*************audio adc analog config***********************************
 * 以下配置，作用为系统音频 ADC 模块开机默认配置
 * */

u32 const audio_adc_con1 = auadc_cic_gain0(36) | auadc_cic_gain1(54);//gain0对应非44.1k,gain1对应44.1K

#if AMM_RS_INSIDE_ENABLE

u32 const audio_adc_mic_bias_2_vcmo0 = 0;      //only 0 or 1 //初始状态不开mic bias 2 vcom0/1
u32 const audio_adc_mic_bias_2_vcmo1 = 0;      //only 0 or 1 //初始状态不开mic bias 2 vcom0/1

AUDIO_MICBIAS_RS const audio_adc_mic_bias_rs = AUMIC_1k5;
AUDIO_MICLDO_VS const audio_adc_mic_ldo_vs = AUMIC_3v0;
AUDIO_MIC_RS_MODE const audio_adc_mic_rs_mode = mic_rs_inside;
AUDIO_MIC_INPUT_MODE const audio_adc_mic_input_mode = mic_input_pa1;
#endif

#if AMM_RS_OUTSIDE_ENABLE

u32 const audio_adc_mic_bias_2_vcmo0 = 0;      //only 0 or 1
u32 const audio_adc_mic_bias_2_vcmo1 = 0;      //only 0 or 1

AUDIO_MICBIAS_RS const audio_adc_mic_bias_rs = AUMIC_1k5;
AUDIO_MICLDO_VS const audio_adc_mic_ldo_vs = 0;
AUDIO_MIC_RS_MODE const audio_adc_mic_rs_mode = mic_rs_outside;
AUDIO_MIC_INPUT_MODE const audio_adc_mic_input_mode = mic_input_pa1;

#endif

#if AMM_DIFF_ENABLE//mic diff
u32 const audio_adc_mic_bias_2_vcmo0 = 0;      //only 0 or 1
u32 const audio_adc_mic_bias_2_vcmo1 = 0;      //only 0 or 1

AUDIO_MICBIAS_RS const audio_adc_mic_bias_rs = AUMIC_1k5;
AUDIO_MICLDO_VS const audio_adc_mic_ldo_vs = 0;
AUDIO_MIC_RS_MODE const audio_adc_mic_rs_mode = mic_rs_outside;
AUDIO_MIC_INPUT_MODE const audio_adc_mic_input_mode = mic_input_diff_pa1_pa2;
#endif

u32 const audio_adc_mic_pga_6db = 0;// 0 or 1
AUDIO_MICPGA_G const audio_adc_mic_pga_g = AUMIC_14db;
AUDIO_MICPGA_G const audio_adc_aux_pga_g = AUMIC_M2db;

AUDIO_AUX_INPUT_MODE const audio_adc_aux_input_mode = aux_input_pa1;
/****************audio config end************************************************/

/***************低功耗保持LCD显示**********************/
#if POWERDOWN_KEEP_LCD
const u8 powerdown_lcd_on = 1;
#else
const u8 powerdown_lcd_on = 0;
#endif

//是否支持syd文件系统直接读取根目录资源文件（如bin文件）
const u8 syd_read_root_doc = 0;
