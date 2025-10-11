#include "includes.h"
#include "app_config.h"
#include "audio_adc.h"
#include "audio_dac.h"
#include "gpio.h"
//中断优先级
//系统使用到的
const int IRQ_IRTMR_IP   = 6;
const int IRQ_AUDIO_IP   = 5;
const int IRQ_DECODER_IP = 0;
const int IRQ_WFILE_IP   = 2;
const int IRQ_RF_QUEUE_IP   = 2;
const int IRQ_ADC_IP     = 3;
const int IRQ_ENCODER_IP = 1;
const int IRQ_TICKTMR_IP = 3;
const int IRQ_USB_IP	 = 4;
const int IRQ_SD_IP		 = 3;
const int IRQ_CTMU_IP	 = 2;
const int IRQ_STREAM_IP  = 4;
const int IRQ_SPEAKER_IP = 1;
const int IRQ_LEDC_IP    = 1;
const int IRQ_SLCD_IP    = 2;
const int IRQ_PMU_TMR1_IP = 3;
const int IRQ_ALINK_IP	 = 3;
//系统还未使用到的
const int IRQ_UART0_IP   = 3;
const int IRQ_UART1_IP   = 3;
const int IRQ_ALINK0_IP  = 3;

//BT
const int IRQ_BT_TIMEBASE_IP  = 6;   //BT TIMEBASE
const int IRQ_BLE_EVENT_IP    = 6;   //BT RX_EVT
const int IRQ_BLE_RX_IP       = 6;   //BT RX
const int IRQ_BTSTACK_MSG_IP  = 3;   //BT STACK
const int IRQ_BREDR_IP        = 3;   //no use
const int IRQ_BT_RXMCH_IP     = 3;   //no use
const int IRQ_AES_IP          = 3;   //aes

//内存管理malloc内部断言
const char MM_ASSERT    = TRUE;
//内核异常打印
const u8 config_asser = 1;

const u8 config_spi_code_user_cache = 1;//sfc放code区

const u8 config_audio_adc_enable = 1;

#if (DECODER_MIDI_EN || DECODER_MIDI_KEYBOARD_EN)
//midi主轨选择方式
const int MAINTRACK_USE_CHN  = 0;    //0:用track号来区分  1:用channel号来区分。
const int MAX_DEC_PLAYER_CNT = 8;    //midi乐谱解码最大同时发声的key数,范围[1,31]
const int MAX_CTR_PLAYER_CNT = 8;    //midi琴最大同时发声的key数,范围[1,31]
const int NOTE_OFF_TRIGGER   = 0;    //midi琴note_off回调 1：time传0时，不会回调 0：time传0时，回调
const int MIDI_MAX_MARK_CNT  = 0;    //midi解码最大支持的mark数
//for midi_4byte
const int MIDI_OUT_CHANNEL = 2;          //midi输出声道1or2，即单声道或者双声道
const int MIDI_CTRL_OUT_CHANNEL = 2;     //midi琴输出声道1or2，即单声道或者双声道
const int MIDI_SAVE_DIV_ENBALE = 0;     //小节回调功能，会影响buf
#endif

#if RTC_EN
const int config_rtc_enable = 1;
#else
const int config_rtc_enable = 0;
#endif
/*********************usb slave config************************************/
const bool config_usbslave_ctl_mic = 1;
/*********************usb slave config end********************************/

// vcmo0/1 输出电压使能，以及响应的电压挡位设置；
/*************docoder mge aechite****************************************/
bool const config_decoder_auto_mutex = 1;


////////////////////////////update control///////////////////////////////////////////
const int CONFIG_UPDATE_STORAGE_DEV_EN = 1;
const int CONFIG_UPDATE_TESTBOX_UART_EN = 1;
const int CONFIG_UPDATE_APP_OTA_EN = 1;
const int CONFIG_UPDATE_TESTBOX_BLE_EN = 1;

/////////////////////////////////////////////////////////////////////////////////////

const u8 config_no_osc_enable = 0;//bd49//ini:PLL=LRC;

/////////////////////////////////////////////////////////////////////////////////////
#if ((defined(DECODER_JLA_LW_EN) && (DECODER_JLA_LW_EN)) || (defined(ENCODER_JLA_LW_EN) && (ENCODER_JLA_LW_EN)))
const int JLA_LW_BITSTREAM_WEIGHT_TAB[8] = { -2, 0, 0, 0, 0, 0, 0, 1 };
/*----------------------------------------------------------------------------*/
/**@note    jla_lw_br_level
   @brief   码率压缩档位:0 ~ 1
   @note1   0档位：适用于32k采样率(10ms)、16k采样率(20ms)
   @note2   1档位：适用于16k采样率(10ms)
   @note3   编解码需要采用同样的档位
*/
/*----------------------------------------------------------------------------*/
const u8 jla_lw_br_level = 0;
#endif

/**
 * @brief Bluetooth Controller Log
 */
/*-----------------------------------------------------------*/
const char libs_debug AT(.LOG_TAG_CONST) = TRUE; //打印总开关

#define  CONFIG_DEBUG_LIBS(X)   (X & libs_debug)

const char log_tag_const_i_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

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

const char log_tag_const_i_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


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

const char log_tag_const_i_LCDC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_LCDC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_LCDC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_LCDC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

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

const char log_tag_const_i_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_DMA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_DMA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_DMA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_DMA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

/* BT */
const char log_tag_const_i_LL_ISO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_LL_ISO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_LL_ISO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_LL_ISO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_DEV_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_DEV_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_DEV_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_DEV_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_SYS_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_SYS_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_SYS_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_SYS_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0) ;
const char log_tag_const_d_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_RCSP_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_RCSP_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_RCSP_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_RCSP_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

#define RCSP_API_LOG		0	// 用于JL_rcsp_api.c中的调试打印
#define RCSP_PACKET_LOG		1	// 用于JL_rcps_packet.c中的调试打印
#define RCSP_PROTOCOL_LOG	2	// 用于JL_rcsp_protocol.c中的调试打印
#define SPP_USER_LOG		3	// 用于打印spp_user.c中的打印，app使用spp交互时需要用到
int app_info_debug_enable = 0 \
                            /*	| BIT(RCSP_API_LOG) \ */
                            /*	| BIT(RCSP_PACKET_LOG) \ */
                            /*	| BIT(RCSP_PROTOCOL_LOG) \ */
                            /*	| BIT(SPP_USER_LOG) \ */
                            ;
