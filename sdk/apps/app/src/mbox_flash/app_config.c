#include "includes.h"
#include "app_config.h"
#include "audio_adc.h"
#include "audio_dac.h"
#include "gpio.h"
//中断优先级
//系统使用到的
const int IRQ_IRTMR_IP   = 6;
const int IRQ_AUDIO_IP   = 5;
const int IRQ_DECODER_IP = 1;
const int IRQ_WFILE_IP   = 1;
const int IRQ_ADC_IP     = 3;
const int IRQ_ENCODER_IP = 0;
const int IRQ_TICKTMR_IP = 3;
const int IRQ_USB_IP	 = 3;
const int IRQ_SD_IP		 = 3;
const int IRQ_STREAM_IP  = 4;
const int IRQ_SPEAKER_IP = 1;
const int IRQ_LEDC_IP    = 1;
const int IRQ_SLCD_IP    = 2;
//系统还未使用到的
const int IRQ_UART0_IP   = 3;
const int IRQ_UART1_IP   = 3;
const int IRQ_ALINK0_IP  = 3;

//内存管理malloc内部断言
const char MM_ASSERT    = TRUE;
//内核异常打印
const u8 config_asser = 1;

//开启异常处理模块的lite打印版本(为了省代码空间,下面的控制变量必须逐级开启增加打印)
//异常打印模块使能
const u8 config_exception_enable = 1;
//1.仅报显示触发单元的寄存器，报错后可以去到对应的异常单元取消对应的使能进行查看或者查看对应的emu文档
const u8 config_exception_unit_lite_lite_enable  = 1;
//1.仅报触发单元的id信息，报错后可以去到对应的异常单元取消对应的使能进行查看或者查看对应的emu文档
const u8 config_exception_unit_lite_enable       = 1;
//1.不报dev信息，报错后可以去到对应的异常单元取消对应的使能进行查看
const u8 config_exception_dev_lite_enable        = 1;

const u8 config_spi_code_user_cache = 1;//sfc放code区

const u8 config_audio_adc_enable = 1;
#if AUDIO_EQ_ENABLE
const u8 config_audio_eq_enable = 1;
#else
const u8 config_audio_eq_enable = 0;
#endif
#if (DECODER_MIDI_EN || DECODER_MIDI_KEYBOARD_EN)
//midi主轨选择方式
const int MAINTRACK_USE_CHN = 0;    //0:用track号来区分  1:用channel号来区分。
const int MAX_DEC_PLAYER_CNT = 8;   //midi乐谱解码最大同时发声的key数,范围[1,31]
const int MAX_CTR_PLAYER_CNT = 15;  //midi琴最大同时发声的key数,范围[1,31]
const int NOTE_OFF_TRIGGER = 0;     //midi琴note_off time传0时，是否产生回调音符结束 1：不回调 0：回调
#endif
#if RTC_EN
const int config_rtc_enable = 1;
#else
const int config_rtc_enable = 0;
#endif
/*********************usb slave config************************************/
const bool config_usbslave_ctl_mic = 1;
/*********************usb slave config end********************************/

/*********************set cache way num********************************/
const u8 cache_way_num = CPU_USE_CACHE_WAY_NUMBER;

/*************audio analog config****************************************
 * 以下配置，作用为系统音频 模块开机默认配置
 * */
// 0 :vcm不会外挂电容；1：vcm会外挂电容；
const bool config_vcm_cap_addon = 0;

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
/****************audio config end************************************************/

/*************docoder mge aechite****************************************/
bool const config_decoder_auto_mutex = 1;

/***************低功耗保持LCD显示**********************/
#if POWERDOWN_KEEP_LCD
const u8 powerdown_lcd_on = 1;
#else
const u8 powerdown_lcd_on = 0;
#endif
/**
 * @brief Bluetooth Controller Log
 */
/*-----------------------------------------------------------*/
const char libs_debug AT(.LOG_TAG_CONST) = TRUE; //打印总开关

#define  CONFIG_DEBUG_LIBS(X)   (X & libs_debug)

const char log_tag_const_i_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_KEYM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_KEYM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_KEYM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_MUGRD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_MUGRD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_MUGRD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_e_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_P33 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_P33 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_P33 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

/* const char log_tag_const_i_SOFI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0); */
/* const char log_tag_const_d_SOFI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0); */
/* const char log_tag_const_e_SOFI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0); */


/* const char log_tag_const_i_UTD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1); */
/* const char log_tag_const_d_UTD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0); */
/* const char log_tag_const_e_UTD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0); */
/* const char log_tag_const_c_UTD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1); */


/* const char log_tag_const_i_LBUF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1); */
/* const char log_tag_const_d_LBUF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0); */
/* const char log_tag_const_e_LBUF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0); */

const char log_tag_const_i_FAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_FAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_FAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_c_FAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_NORM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_NORM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_NORM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_NORM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_FLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_FLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_FLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_FLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_SPI1 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_SPI1 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_SPI1 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_SPI1 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_SPI1_TEST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_SPI1_TEST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_SPI1_TEST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_EEPROM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_EEPROM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_EEPROM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_EEPROM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_HEAP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_HEAP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_HEAP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_c_HEAP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_CPU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_CPU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_CPU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_c_CPU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_c_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_CHARGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_CHARGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_CHARGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_CHARGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
