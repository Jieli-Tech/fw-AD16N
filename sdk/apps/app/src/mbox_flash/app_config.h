#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "app_modules.h"

#define ENABLE_THIS_MOUDLE				    1
#define DISABLE_THIS_MOUDLE				    0

#define ENABLE							    1
#define DISABLE							    0

#define NO_CONFIG_PORT						(-1)

/*---------系统时钟配置---------------------*/
#define TCFG_SYS_PLL_CLK                    96000000
#define TCFG_PLL_SEL                        PLL_D1p0_192M
#define TCFG_PLL_DIV                        PLL_DIV2
#define TCFG_HSB_DIV                        HSB_DIV1

/*---------Cache Configuration--------------*/
#define CPU_USE_CACHE_WAY_NUMBER            4//cache_way范围:2~4

/*---------UART Configuration---------------*/
#define TCFG_UART_TX_PORT  				    IO_PORTA_05     //串口打印发送脚配置
// #define TCFG_UART_RX_PORT  				    IO_PORTA_06     //串口打印接收脚配置
#define TCFG_UART_BAUDRATE  				1000000         //串口打印波特率配置

/*---------KEY Configuration----------------*/
#define KEY_DOUBLE_CLICK_EN		            DISABLE//使能按键多击
#define KEY_IO_EN         	                0//<IO按键使能
#define KEY_AD_EN				            1//<AD按键使能
#define KEY_MIC_EN                          0//<耳机按键使能
#define KEY_MATRIX_EN			            0//<矩阵按键使能
#define KEY_IR_EN				            0//<IR按键使能
#define KEY_TOUCH_EN			            0//<触摸按键使能
#define KEY_LPTOUCH_EN			            0
//AD KEY
#define AD_KEY_IO		                    IO_PORTA_08
//IR KEY
#define IR_KEY_IO			                IO_PORTA_06
// #define IR_KEY_IRQ_IDX  	                IRQ_TIME2_IDX
#define IR_KEY_TIMER_IDX		            0
//TOUCH KEY
#define TOUCH_KEY_IO_SEL		           {IO_PORTA_07,IO_PORTA_08}
//LPTOUCH KEY
#define LPCTMU_CH0_EN                       0//LPCTMU_CH0使能，固定引脚: PA01
#define LPCTMU_CH1_EN                       0//LPCTMU_CH1使能，固定引脚: PA02
#define LPCTMU_CH2_EN                       0//LPCTMU_CH2使能，固定引脚: PA03
#define LPCTMU_CH3_EN                       0//LPCTMU_CH3使能，固定引脚: PB00
#define LPCTMU_CH4_EN                       0//LPCTMU_CH4使能，固定引脚: PB01
#define LPCTMU_CH5_EN                       0//LPCTMU_CH5使能，固定引脚: PB02
#define LPCTMU_CH6_EN                       0//LPCTMU_CH6使能，固定引脚: PB03
#define LPCTMU_CH7_EN                       0//LPCTMU_CH7使能，固定引脚: PB04

/*---------KEY VOICE Configuration----------*/
#define D_HAS_KEY_VOICE   				    DISABLE

/*---------Power Wakeup IO------------------*/
#define TCFG_VDDIOM_LEVEL			        VDDIOM_VOL_32V
#define POWER_WAKEUP_IO				        AD_KEY_IO
#define POWER_WAKEUP_EDGE				    FALLING_EDGE

//*********************************************************************************//
//                                  低功耗配置                                     //
//*********************************************************************************//
#define TCFG_CONFIG_LOWPOWER_SEL          ENABLE

//*********************************************************************************//
//                                  软关机配置                                     //
//*********************************************************************************//
#define TCFG_SHUTDOWN_TIME         5000 //uint:ms

/*---------charge Configuration-------------*/
#define TCFG_CHARGE_ENABLE		            0//ENABLE
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
#define TCFG_CFG_TOOL_ENABLE                DISABLE//配置工具使能,需要在线调试eq时打开,默认关闭
#define TCFG_NULL_COMM                      0//不支持通信
#define TCFG_UART_COMM                      1//串口通信
#define TCFG_USB_COMM                       2//USB通信
#define TCFG_COMM_TYPE                      TCFG_USB_COMM//暂时只支持USB通信调音

/*---------RF PA----------------*/
#define RF_PA_EN     						DISABLE
#define RF_PA_POWER_SUPPLY      			DISABLE
#define RF_PA_POWER_SUPPLY_IO      			IO_PORTB_02
#define RF_PA_POWER_RX_IO      				IO_PORTA_13
#define RF_PA_POWER_TX_IO      				IO_PORTA_12

/*---------AUDIO_LINK----------------*/
#define ALINK_MODE_TYPE						ALINK_MD_IIS
#define ALINK_ROLE_TYPE						ALINK_ROLE_MASTER
#define ALINK_CLK_MODE_TYPE 				ALINK_CLK_FALL_UPDATE_RAISE_SAMPLE
#define ALINK_BIT_WIDE_TYPE 				ALINK_LEN_16BIT
#define ALINK_SCLK_PER_FRAME_TYPE			ALINK_FRAME_64SCLK
#define ALINK_DMA_LEN 						ALNK_BUF_POINTS_NUM * 2 * 2 * 2 //16bit : 128 * 2(CH) * 2(Byte)* 2(dual buffer
#define ALINK_SR_DEFAULT 					ALINK_SR_32000

#define ALINK_MAX_CHANNEL					4
#define ALINK_MCLK_IO 						IO_PORTA_02
#define ALINK_SCLK_IO 						IO_PORTA_10
#define ALINK_LCLK_IO 						IO_PORTA_09
#define ALINK_DATA_IO0 						IO_PORTA_14
#define ALINK_DATA_IO1 						IO_PORTA_15
#define ALINK_DATA_IO2 						NULL
#define ALINK_DATA_IO3 						NULL


/*---------------UPDATE---------------------*/
#define TFG_DEV_UPGRADE_SUPPORT             ENABLE
#define TFG_UPGRADE_FILE_NAME               "/update.ufw"

/*---------UI Configuration-----------------*/
#if RUN_APP_CUSTOM
#define LED_5X7			                    ENABLE
#else
#define LED_5X7			                    DISABLE
#endif
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

/*---------VM Version Configuration----------------*/
#define USE_NEW_VM      					1
#define USE_OLD_VM      					2
#define SYS_MEMORY_SELECT   				USE_NEW_VM

/*---------FLASH Configuration--------------*/
#if RUN_APP_CUSTOM
#define TFG_EXT_FLASH_EN				    ENABLE
#else
#define TFG_EXT_FLASH_EN				    DISABLE
#endif
#define TFG_SPI_HW_NUM					    1
#define TFG_SPI_UNIDIR_MODE_EN              DISABLE//外挂flash单线模式
#if TFG_SPI_UNIDIR_MODE_EN
#define TFG_SPI_WORK_MODE				    SPI_MODE_UNIDIR_1BIT
#define TFG_SPI_READ_DATA_WIDTH			    1
#else
#define TFG_SPI_WORK_MODE				    SPI_MODE_BIDIR_1BIT
#define TFG_SPI_READ_DATA_WIDTH			    SPI_MODE_BIDIR_1BIT
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
#if RUN_APP_CUSTOM
#define TFG_SD_EN				            ENABLE
#else
#define TFG_SD_EN				            DISABLE
#endif
#define TFG_SDPG_ENABLE                     DISABLE

/*---------USB Configuration----------------*/
#if ((HAS_USB_EN) && (0 == RUN_APP_RC))
#define TCFG_PC_ENABLE						ENABLE  //PC模块使能
#define TCFG_USB_MSD_CDROM_ENABLE           DISABLE
#define TCFG_USB_EXFLASH_UDISK_ENABLE       DISABLE  //外掛FLASH UDISK
#define TCFG_UDISK_ENABLE					ENABLE //U盘模块使能
#define POWERDOWN_UDISK_MODE_EN             ENABLE //U盘插入时可进入pdown
#define SD_CDROM_EN                         DISABLE
#else
#define TCFG_PC_ENABLE						DISABLE  //PC模块使能
#define TCFG_USB_MSD_CDROM_ENABLE           DISABLE
#define TCFG_USB_EXFLASH_UDISK_ENABLE       DISABLE  //外掛FLASH UDISK
#define TCFG_UDISK_ENABLE					DISABLE //U盘模块使能
#define POWERDOWN_UDISK_MODE_EN             DISABLE //U盘插入时是否进入pdown
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

#if TCFG_CFG_TOOL_ENABLE
#define  USB_DEVICE_CLASS_CONFIG            (CDC_CLASS|AUDIO_CLASS|HID_CLASS)  //在线EQ调试助手

#elif defined(CONFIG_APP_OTA_EN) && (CONFIG_APP_OTA_EN == 1)
#define  USB_DEVICE_CLASS_CONFIG            (AUDIO_CLASS|HID_CLASS|CUSTOM_HID_CLASS)  //ota升级

#else
#define  USB_DEVICE_CLASS_CONFIG            (AUDIO_CLASS|HID_CLASS|MASSSTORAGE_CLASS)  //默认配置usb从机模式支持的class
#endif



/*---------ICACHE RAM 相关配置-----------------*/
#define ICACHE_RAM_TO_RAM_ENABLE			DISABLE//1//ICACHE RAM用作普通RAM使能位
#define ICACHE_RAM_TO_RAM					8192//将多少ICACHE RAM用作普通RAM----配置大小：4K、8K

#endif
