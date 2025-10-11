#ifndef  __IRQ_VEC_H__
#define  __IRQ_VEC_H__

#define IRQ_EMUEXCPT_IDX        0
#define IRQ_EXCEPTION_IDX       1
#define IRQ_SYSCALL_IDX         2
#define IRQ_TICKTMR_IDX         3
#define IRQ_TIME0_IDX           4
#define IRQ_TIME1_IDX           5
#define IRQ_TIME2_IDX           6
#define IRQ_UART0_IDX           7
#define IRQ_UART1_IDX           8
#define IRQ_UART2_IDX      		9
#define IRQ_SPI0_IDX            10
#define IRQ_SPI1_IDX            11
#define IRQ_IIC0_IDX            12

#define IRQ_PORT_IDX            14
#define IRQ_GPADC_IDX           15
#define IRQ_GPCNT_IDX           16
#define IRQ_USB_SOF_IDX         17
#define IRQ_USB_CTRL_IDX        18
#define IRQ_TIME3_IDX           19
#define IRQ_MCPWM_TMR_IDX       20
#define IRQ_PMU_SOFT_IDX        21
#define IRQ_PMU_TMR0_IDX        22
#define IRQ_PMU_TMR1_IDX        23

#define IRQ_SD0_BRK_IDX         24
#define IRQ_SD0_IDX             25
#define IRQ_CTMU0_IDX           26

#define IRQ_MCPWM_CHX_IDX       27
#define IRQ_SPI2_IDX            28
#define IRQ_CAN_IDX             29
#define IRQ_PWM_LED_IDX         30

#define IRQ_UDMA_CH0_IDX        34
#define IRQ_UDMA_CH1_IDX        35
#define IRQ_UDMA_CH2_IDX        36
#define IRQ_UDMA_CH3_IDX        37
#define IRQ_AES_IDX             38
#define IRQ_SRC_IDX             39

#define IRQ_AUDIO_IDX           42
#define IRQ_APA_IDX             43
#define IRQ_ALINK_IDX           44

#define IRQ_BT_TIMEBASE_IDX     48
#define IRQ_BLE_RX_IDX          49
#define IRQ_BLE_EVENT_IDX       50
#define IRQ_BT_CLKN_IDX         51
#define IRQ_BREDR_IDX           52
#define IRQ_BT_RXMCH            53
#define IRQ_SYNC_IDX            54


#define IRQ_SOFT0_IDX           56 //解码
#define IRQ_SOFT1_IDX           57 //编码
#define IRQ_SOFT2_IDX           58 //录音写数据 / 队列
#define IRQ_SOFT3_IDX           59 //bt_stack
#define IRQ_SOFT4_IDX           60 //rcsp协议
#define IRQ_SOFT5_IDX           61 //stream_frame使用
#define IRQ_SOFT6_IDX           62
#define IRQ_SOFT7_IDX           63







#define MAX_IRQ_ENTRY_NUM       64





#endif

