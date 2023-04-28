#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "app_modules.h"

#define ENABLE_THIS_MOUDLE				    1
#define DISABLE_THIS_MOUDLE				    0

#define ENABLE							    1
#define DISABLE							    0

#define NO_CONFIG_PORT						(-1)

/*---------系统时钟配置---------------------*/
#define TCFG_PLL_SEL                        PLL_D1p0_192M
#define TCFG_PLL_DIV                        PLL_DIV2
#define TCFG_HSB_DIV                        HSB_DIV1

/*---------Cache Configuration--------------*/
#define CPU_USE_CACHE_WAY_NUMBER            4//cache_way范围:2~4

/*---------UART Configuration---------------*/
#define TCFG_UART_TX_PORT  				    IO_PORTA_06     //串口打印发送脚配置
#define TCFG_UART_BAUDRATE  				1000000         //串口打印波特率配置

/*---------KEY Configuration----------------*/
#define KEY_DOUBLE_CLICK_EN		            DISABLE//使能按键多击
#define KEY_IO_EN         	                0//<IO按键使能
#define KEY_AD_EN				            1//<AD按键使能
#define KEY_MIC_EN                          0//<耳机按键使能
#define KEY_MATRIX_EN			            0//<矩阵按键使能
#define KEY_IR_EN				            0//<IR按键使能
#define KEY_TOUCH_EN			            0//<触摸按键使能
//AD KEY
#define AD_KEY_IO_SEL		                IO_PORTA_08
#define AD_KEY_CH_SEL				        ADC_CH_PA8
//IR KEY
#define IR_KEY_IO			                IO_PORTA_05
#define IR_KEY_IRQ_IDX  	                IRQ_TIME2_IDX
#define IR_KEY_TIMER		                JL_TIMER2
//TOUCH KEY
#define TOUCH_KEY_IO_SEL		           {IO_PORTA_07,IO_PORTB_10}
/*---------KEY VOICE Configuration----------*/
#define D_HAS_KEY_VOICE   				    DISABLE

/*---------Power Wakeup IO------------------*/
#define TCFG_VDDIOM_LEVEL			        VDDIOM_VOL_32V
#define POWER_WAKEUP_IO				        AD_KEY_IO_SEL
#define POWER_WAKEUP_EDGE				    FALLING_EDGE

/*---------RTC Configuration-------------*/
#if RTC_EN
#define RTC_CLK_SEL                         CLK_SEL_LRC//RTC时钟源选择,可选32k晶振或者LRC时钟
#else
#define RTC_CLK_SEL                         0//无RTC功能
#endif

/*---------charge Configuration-------------*/
#define TCFG_CHARGE_ENABLE		            ENABLE
#define TCFG_CHARGE_FULL_V					CHARGE_FULL_V_4199
#define TCFG_CHARGE_FULL_MA					CHARGE_FULL_mA_DIV10
#define TCFG_CHARGE_MA						CHARGE_mA_90
#define TCFG_CHARGE_TRICKLE_MA              CHARGE_mA_20

/*---------audio dac Configuration-------------*/
#define SR_DEFAULT                          32000

/*---------audio mic Configuration-------------*/
#define AMM_RS_INSIDE_ENABLE                0
#define AMM_RS_OUTSIDE_ENABLE               1
#define AMM_DIFF_ENABLE                     0

/*---------自动mute配置--------------------*/
#define TCFG_AUDIO_AUTO_MUTE_ENABLE         0//不支持
#define TCFG_AUTO_MUTE_ENERGY               5//自动mute能量阈值
#define TCFG_AUTO_MUTE_TIME                 1000//自动mute的时间
#define TCFG_AUTO_UNMUTE_ENERGY             10//自动解除mute能量阈值
#define TCFG_AUTO_UNMUTE_TIME               50//自动解除mute的时间

/*---------DAC数字音量设置淡入淡出配置-----*/
#define TCFG_DAC_VOL_FADE_EN                DISABLE//硬件DAC淡入淡出
#define DAC_SOFT_FADE_EN                    DISABLE//软件运算淡入淡出
//若为44.1K采样率,音量从最小0->调大最大16384耗时:((16384-0)/step)/(352.8K/(slow+1)) = 46.4ms / step * (slow+1)
//若为48K/32K采样率,音量从最小0->调大最大16384耗时:((16384-0)/step)/(384K/(slow+1)) = 42.7ms / step * (slow+1)
#define TCFG_DAC_VOL_FADE_STEP              1//0和1一样效果,配置范围0~15
#define TCFG_DAC_VOL_FADE_SLOW              15//配置范围0~15

/*---------EQ Configuration-----------------*/
#define AUDIO_EQ_ENABLE                     ENABLE//eq总使能
#define TCFG_CFG_TOOL_ENABLE                DISABLE//配置工具使能,需要在线调试eq时打开,默认关闭
#define TCFG_NULL_COMM                      0//不支持通信
#define TCFG_UART_COMM                      1//串口通信
#define TCFG_USB_COMM                       2//USB通信
#define TCFG_COMM_TYPE                      TCFG_USB_COMM//暂时只支持USB通信调音

/*---------------UPDATE---------------------*/
#define TFG_DEV_UPGRADE_SUPPORT             ENABLE
#define TFG_UPGRADE_FILE_NAME               "/update.ufw"

/*---------UI Configuration-----------------*/
#define LED_5X7			                    ENABLE
#define LCD_4X8_EN                          0//ENABLE
#define KEY_MATRIX_LCD_REUSE_EN             0//矩阵按键与LCD复用宏
#if (LED_5X7)
#define UI_ENABLE                           ENABLE
#else
#define UI_ENABLE                           DISABLE
#endif
#if LCD_4X8_EN
#define POWERDOWN_KEEP_LCD                  ENABLE
#else
#define POWERDOWN_KEEP_LCD                  DISABLE
#endif
/*---------VM_SFC Configuration----------------*/
#define VM_SFC_ENABLE                       ENABLE

/*---------FLASH Configuration--------------*/
#define TFG_EXT_FLASH_EN				    ENABLE
#define TFG_SPI_HW_NUM					    1
#define TFG_SPI_UNIDIR_MODE_EN              DISABLE//外挂flash单线模式
#if TFG_SPI_UNIDIR_MODE_EN
#define TFG_SPI_WORK_MODE				    SPI_MODE_UNIDIR_1BIT
#define TFG_SPI_READ_DATA_WIDTH			    1
#else
#define TFG_SPI_WORK_MODE				    SPI_MODE_BIDIR_1BIT
#define TFG_SPI_READ_DATA_WIDTH			    2
#endif
#define TFG_SPI_CS_PORT_SEL				    IO_PORTB_00
#define TFG_SPI_CS_PORT         		    PB00
#define TFG_SPI_CLK_PORT_SEL			    IO_PORTB_01
#define TFG_SPI_CLK_PORT        		    PB01
#define TFG_SPI_DO_PORT_SEL				    IO_PORTB_02
#define TFG_SPI_DO_PORT         		    PB02
#define TFG_SPI_DI_PORT_SEL				    IO_PORTB_03
#define TFG_SPI_DI_PORT         		    PB03
#define SPI_SD_IO_REUSE                     0

/*---------SD Configuration-----------------*/
#define TFG_SD_EN				            ENABLE
#define TFG_SDPG_ENABLE                     DISABLE

/*---------USB Configuration----------------*/
#if HAS_USB_EN
#define TCFG_PC_ENABLE						ENABLE  //PC模块使能
#define TCFG_USB_MSD_CDROM_ENABLE           DISABLE
#define TCFG_USB_EXFLASH_UDISK_ENABLE       DISABLE  //外掛FLASH UDISK
#define TCFG_UDISK_ENABLE					ENABLE //U盘模块使能
#define SD_CDROM_EN                         DISABLE
#else
#define TCFG_PC_ENABLE						DISABLE  //PC模块使能
#define TCFG_USB_MSD_CDROM_ENABLE           DISABLE
#define TCFG_USB_EXFLASH_UDISK_ENABLE       DISABLE  //外掛FLASH UDISK
#define TCFG_UDISK_ENABLE					DISABLE //U盘模块使能
#define SD_CDROM_EN                         DISABLE
#endif

#if TCFG_USB_EXFLASH_UDISK_ENABLE
#define FLASH_CACHE_ENABLE                  1
#else
#define FLASH_CACHE_ENABLE                  0
#endif

#define TCFG_USB_PORT_CHARGE                DISABLE
#define TCFG_OTG_USB_DEV_EN                 BIT(0)  //USB0 = BIT(0)  USB1 = BIT(1)

#if TCFG_PC_ENABLE
#define USB_DEVICE_EN       //Enable USB SLAVE MODE
#endif
#if TCFG_UDISK_ENABLE
#define	USB_DISK_EN        //是否可以读U盘
#endif

#include "usb_std_class_def.h"
#include "usb_common_def.h"

#if TCFG_PC_ENABLE || TCFG_UDISK_ENABLE
#undef USB_DEVICE_CLASS_CONFIG
#if TCFG_CFG_TOOL_ENABLE
#define  USB_DEVICE_CLASS_CONFIG            (CDC_CLASS|AUDIO_CLASS|HID_CLASS)  //配置usb从机模式支持的class
#else
#define  USB_DEVICE_CLASS_CONFIG            (MASSSTORAGE_CLASS|AUDIO_CLASS|HID_CLASS)  //配置usb从机模式支持的class
#endif
#endif

#endif
