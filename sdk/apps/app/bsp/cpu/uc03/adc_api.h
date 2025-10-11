#ifndef __ADC_API_H__
#define __ADC_API_H__
//br29
#include "typedef.h"
#include "asm/power_interface.h"

#define ADC_SAMPLE_BITS     10

#define ADC_CH_MASK_TYPE_SEL	0xffff0000
#define ADC_CH_MASK_CH_SEL	    0x0000ffff

#define ADC_CH_TYPE_PMU    	(0x0<<16)
#define ADC_CH_TYPE_AUDIO  	(0x1<<16)
#define ADC_CH_TYPE_PLL  	(0x2<<16)
#define ADC_CH_TYPE_X32K  	(0x3<<16)
#define ADC_CH_TYPE_X12M  	(0x4<<16)
#define ADC_CH_TYPE_IO		(0x10<<16)

#define ADC_CH_PMU_VBG  	    (ADC_CH_TYPE_PMU | 0x0)//MVBG/WVBG
// #define ADC_CH_PMU_          (ADC_CH_TYPE_PMU | 0x1)
#define ADC_CH_PMU_PROGI	    (ADC_CH_TYPE_PMU | 0x2)
#define ADC_CH_PMU_PROGF	    (ADC_CH_TYPE_PMU | 0x3)
#define ADC_CH_PMU_VTEMP	    (ADC_CH_TYPE_PMU | 0x4)
#define ADC_CH_PMU_VPWR_4 	    (ADC_CH_TYPE_PMU | 0x5) // 1/4VPWR
#define ADC_CH_PMU_VBAT_4 	    (ADC_CH_TYPE_PMU | 0x6) // 1/4VBAT
#define ADC_CH_PMU_VBAT_2 	    (ADC_CH_TYPE_PMU | 0x7) // 1/2VBAT
// #define ADC_CH_PMU_ 	        (ADC_CH_TYPE_PMU | 0x8)
// #define ADC_CH_PMU_  	    (ADC_CH_TYPE_PMU | 0x9)
#define ADC_CH_PMU_DCVD		    (ADC_CH_TYPE_PMU | 0xa)
#define ADC_CH_PMU_DVDD		    (ADC_CH_TYPE_PMU | 0xb)
#define ADC_CH_PMU_WVDD  	    (ADC_CH_TYPE_PMU | 0xc)
// #define ADC_CH_PMU_  	    (ADC_CH_TYPE_PMU | 0xd)
// #define ADC_CH_PMU_  	    (ADC_CH_TYPE_PMU | 0xe)
// #define ADC_CH_PMU_  	    (ADC_CH_TYPE_PMU | 0xf)
#define ADC_CH_AUDIO_	        (ADC_CH_TYPE_AUDIO | 0x0)
#define ADC_CH_PLL_	            (ADC_CH_TYPE_PLL | 0x0)
#define ADC_CH_X32K_		    (ADC_CH_TYPE_X32K | 0x0)
#define ADC_CH_X12M_		    (ADC_CH_TYPE_X12M | 0x0)
#define ADC_CH_IO_PA0       (ADC_CH_TYPE_IO | 0x0)
#define ADC_CH_IO_PA4       (ADC_CH_TYPE_IO | 0x1)
#define ADC_CH_IO_PA6       (ADC_CH_TYPE_IO | 0x2)
#define ADC_CH_IO_PA8       (ADC_CH_TYPE_IO | 0x3)
#define ADC_CH_IO_PA10      (ADC_CH_TYPE_IO | 0x4)
#define ADC_CH_IO_PB0       (ADC_CH_TYPE_IO | 0x5)
#define ADC_CH_IO_PB2       (ADC_CH_TYPE_IO | 0x6)
#define ADC_CH_IO_PB4       (ADC_CH_TYPE_IO | 0x7)
#define ADC_CH_IO_PB6       (ADC_CH_TYPE_IO | 0x8)
#define ADC_CH_IO_PB7       (ADC_CH_TYPE_IO | 0x9)
#define ADC_CH_IO_PB8       (ADC_CH_TYPE_IO | 0xa)
#define ADC_CH_IO_PC0       (ADC_CH_TYPE_IO | 0xb)
#define ADC_CH_IO_PC1       (ADC_CH_TYPE_IO | 0xc)
#define ADC_CH_IO_PC3       (ADC_CH_TYPE_IO | 0xd)
#define ADC_CH_IO_DP        (ADC_CH_TYPE_IO | 0xe)
#define ADC_CH_IO_DM        (ADC_CH_TYPE_IO | 0xf)

#define     ADC_VBG_CENTER  800 //VBG基准值
#define     ADC_VBG_TRIM_STEP     0   //
#define     ADC_VBG_DATA_WIDTH    0

enum AD_CH {
    AD_CH_PMU_VBG = ADC_CH_PMU_VBG,
    AD_CH_PMU_PROGI = ADC_CH_PMU_PROGI,
    AD_CH_PMU_PROGF,
    AD_CH_PMU_VTEMP,
    AD_CH_PMU_VPWR_4,
    AD_CH_PMU_VBAT_4,
    AD_CH_PMU_VBAT_2,
    AD_CH_PMU_DCVD = ADC_CH_PMU_DCVD,
    AD_CH_PMU_DVDD,
    AD_CH_PMU_WVDD,

    AD_CH_AUDIO = ADC_CH_TYPE_AUDIO, //防编译报错，该宏非法
    AD_CH_PLL = ADC_CH_PLL_,
    AD_CH_X32K = ADC_CH_X32K_,
    AD_CH_X12M = ADC_CH_X12M_,

    AD_CH_IO_PA0 = ADC_CH_IO_PA0,
    AD_CH_IO_PA4,
    AD_CH_IO_PA6,
    AD_CH_IO_PA8,
    AD_CH_IO_PA10,
    AD_CH_IO_PB0,
    AD_CH_IO_PB2,
    AD_CH_IO_PB4,
    AD_CH_IO_PB6,
    AD_CH_IO_PB7,
    AD_CH_IO_PB8,
    AD_CH_IO_PC0,
    AD_CH_IO_PC1,
    AD_CH_IO_PC3,
    AD_CH_IO_DP,
    AD_CH_IO_DM,

    AD_CH_IOVDD = 0xffffffff,
};

#define AD_CH_PMU_VBAT AD_CH_PMU_VPWR_4 //防编译报错

#define AD_CH_LDOREF    AD_CH_PMU_VBG
// #define AD_CH_VBAT      AD_CH_PMU_VBAT

// extern void adc_pmu_ch_select(u32 ch);
extern u32 efuse_get_gpadc_vbg_trim();
// extern void P33_CON_SET(u16 addr, u8 start, u8 len, u8 data);



#define AD_CH_IO_VBAT_PORT        0//IO_PORTA_02   //选择一个有ADC功能IO口采集vbat电压，电压不能超过 vddio
#define ENABLE_PREEMPTIVE_MODE 1    //阻塞式采集使能
#define AD_CH_PMU_VBG_TRIM_NUM  32  //初始化时，VBG通道校准的采样次数
#define PMU_CH_SAMPLE_PERIOD    500 //CPU模式采样周期默认值 单位：ms
#define ADC_MAX_CH  10  //采集队列支持的最大通道数

#define ADC_SENSE_ENABLE 0 //ADC差分放大器使能位


struct adc_info_t { //adc采集队列信息结构体
    u32 jiffies;
    u32 ch;
    union {
        u16 value;
        float voltage;
    } v;
    u16 adc_voltage_mode;
    u16 sample_period;
};
extern u8 cur_ch;  //adc采集队列当前编号
extern u8 adc_clk_div; //adc时钟分频系数
// extern u16 vbat_voltage; //电池电压值，默认一直刷新
// extern u16 vbg_value; //vbg adc原始值，默认一直刷新
extern struct adc_info_t adc_queue[ADC_MAX_CH + ENABLE_PREEMPTIVE_MODE];  //采集队列声明
extern u32 maskrom_get_jiffies();

float adc_value_update(enum AD_CH ch, float adc_value_old, float adc_value_new); //vbat, vbg 值更新
void adc_init(void); //adc初始化
u32 adc_get_next_ch();    //获取采集队列中下一个通道的队列编号
u32 adc_set_sample_period(enum AD_CH ch, u32 ms); //设置一个指定通道的采样周期
u32 adc_value_to_voltage(u32 adc_vbg, u32 adc_value);   //adc_value-->voltage   用传入的 vbg 计算
u32 adc_value_to_voltage_filter(u32 adc_value);   //adc_value-->voltage   用 vbg_value_array 数组均值计算

u32 adc_get_value(enum AD_CH ch);   //获取一个指定通道的原始值，从队列中获取
u32 adc_get_voltage(enum AD_CH ch); //获取一个指定通道的电压值，从队列中获取
u32 adc_get_value_blocking(enum AD_CH ch);    //阻塞式采集一个指定通道的adc原始值
u32 adc_get_value_blocking_filter(enum AD_CH ch, u32 sample_times);    //阻塞式采集一个指定通道的adc原始值,均值滤波
u32 adc_get_voltage_blocking(enum AD_CH ch);  //阻塞式采集一个指定通道的电压值（经过均值滤波处理）
u32 adc_cpu_mode_process(u32 adc_value);    //adc_isr 中断中，cpu模式的公共处理函数

void adc_update_vbg_value_restart(u8 cur_miovdd_level, u8 new_miovdd_level);//iovdd变化之后，重新计算vbg_value
u32 adc_get_vbg_voltage();
u32 adc_io2ch(int gpio);   //根据传入的GPIO，返回对应的ADC_CH
void adc_io_ch_set(enum AD_CH ch, u32 mode); //adc io通道 模式设置
void adc_internal_signal_to_io(u32 analog_ch, u16 adc_io); //将内部通道信号，接到IO口上，输出
u32 adc_add_sample_ch(enum AD_CH ch);   //添加一个指定的通道到采集队列
u32 adc_delete_ch(enum AD_CH ch);    //将一个指定的通道从采集队列中删除
void adc_sample(enum AD_CH ch, u32 ie); //启动一次cpu模式的adc采样
void adc_close();     //adc close
void adc_wait_enter_idle(); //等待adc进入空闲状态，才可进行阻塞式采集
void adc_set_enter_idle(); //设置 adc cpu模式为空闲状态
u16 adc_wait_pnd();   //cpu采集等待pnd
void adc_hw_init(void);    //adc初始化子函数
void adc_hw_uninit(void);
/* void adc_hw_enter_sleep(void); */
/* void adc_hw_exit_sleep(void); */
void adc_hw_enter_sleep(enum LOW_POWER_LEVEL lp_mode);
void adc_hw_exit_sleep(enum LOW_POWER_LEVEL lp_mode);

u32 adc_check_vbat_lowpower();  //兼容旧芯片，直接return 0
void adc_scan();  //定时函数，每 x ms启动一轮cpu模式采集
u32 adc_get_vbat_voltage();

#endif
