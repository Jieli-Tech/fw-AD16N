#ifndef __POWER_WAKEUP_H__
#define __POWER_WAKEUP_H__

//=========================唤醒原因==================================
enum WAKEUP_REASON {
    PWR_WK_REASON_PLUSE_CNT_OVERFLOW,   	    //pcnt唤醒复位
    PWR_WK_REASON_P11,   					    //P11唤醒复位
    PWR_WK_REASON_LPCTMU,   				    //触摸唤醒复位
    PWR_WK_REASON_PORT_EDGE,				    //数字io输入边沿唤醒复位
    PWR_WK_REASON_ANA_EDGE,					    //模拟io输入边沿唤醒复位
    PWR_WK_REASON_VDDIO_LVD,				    //vddio lvd唤醒复位
    PWR_WK_REASON_WDT,				    		//vddio lvd看门狗唤醒复位

    PWR_WK_REASON_FALLING_EDGE_0,			//p33 index0  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_1,			//p33 index1  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_2,			//p33 index2  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_3,			//p33 index3  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_4,     	    //p33 index4  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_5,          //p33 index5  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_6,          //p33 index6  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_7,          //p33 index7  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_8,          //p33 index8  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_9,          //p33 index9  io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_10,         //p33 index10 io下降沿唤醒复位
    PWR_WK_REASON_FALLING_EDGE_11,         //p33 index11 io下降沿唤醒复位

    PWR_WK_REASON_RISING_EDGE_0,			//p33 index0  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_1,			//p33 index1  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_2,			//p33 index2  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_3,			//p33 index3  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_4,     	    //p33 index4  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_5,          	//p33 index5  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_6,          	//p33 index6  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_7,          	//p33 index7  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_8,          	//p33 index8  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_9,          	//p33 index9  io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_10,         	//p33 index10 io上升沿唤醒复位
    PWR_WK_REASON_RISING_EDGE_11,         	//p33 index11 io上升沿唤醒复位

    PWR_ANA_WK_REASON_FALLING_EDGE_LDOIN, 		//LDO5V下降沿唤醒复位
    PWR_ANA_WK_REASON_FALLING_EDGE_VBATCH,      //VBATCH下降沿唤醒复位
    PWR_ANA_WK_REASON_FALLINIG_EDGE_VBATDT,     //vbatdt下降沿唤醒复位
    PWR_ANA_WK_REASON_FALLINIG_EDGE_CHARGEFULL, //charge full下降沿唤醒复位

    PWR_ANA_WK_REASON_RISING_EDGE_LDOIN,        //LDO5V上降沿唤醒复位
    PWR_ANA_WK_REASON_RISING_EDGE_VBATCH,       //VBATCH上降沿唤醒复位
    PWR_ANA_WK_REASON_RISING_EDGE_VBATDT,     	//vbatdet上升沿唤醒复位
    PWR_ANA_WK_REASON_RISING_EDGE_CHARGEFULL,   //chargefull上升沿唤醒复位

    PINR_PND1_WKUP,

    PWR_RTC_WK_REASON_ALM, 					    //RTC闹钟唤醒复位
    PWR_RTC_WK_REASON_256HZ, 					//RTC 256Hz时基唤醒复位
    PWR_RTC_WK_REASON_64HZ, 					//RTC 64Hz时基唤醒复位
    PWR_RTC_WK_REASON_2HZ, 						//RTC 2Hz时基唤醒复位
    PWR_RTC_WK_REASON_1HZ, 						//RTC 1Hz时基唤醒复位

    PWR_WKUP_REASON_RESERVE = 63,

};

//
//
//                    platform_data
//
//
//
//******************************************************************
#define MAX_WAKEUP_PORT     12  //最大同时支持数字io输入个数
#define MAX_WAKEUP_ANA_PORT 0   //最大同时支持模拟io输入个数

typedef enum {
    RISING_EDGE = 0,
    FALLING_EDGE,
    BOTH_EDGE,
    DISABLE_EDGE,
} P33_IO_WKUP_EDGE;

typedef enum {
    PORT_FLT_DISABLE = 0,
    PORT_FLT_16us,
    PORT_FLT_128us,
    PORT_FLT_1ms,
    PORT_FLT_4ms,
} P33_IO_WKUP_FLT;

struct _p33_io_edge_wakeup_config {
    u32 gpio;      				//唤醒io
    u32 pullup_down_enable;    	//上下拉是否使能
    P33_IO_WKUP_FLT filter;	  //滤波参数，数字
    P33_IO_WKUP_EDGE edge; //唤醒边沿条件
    void (*callback)(P33_IO_WKUP_EDGE edge);
};

struct _p33_io_edge_wakeup_param {
    //数字io输入
    /* P33_IO_WKUP_FLT filter;	  //滤波参数，数字 */
};

struct _wakeup_param {
    struct _p33_io_edge_wakeup_param *p33_io_edge_wakeup_param_p;

};

//
//
//                    power_wakeup api
//
//
//
//******************************************************************

bool is_wakeup_source(enum WAKEUP_REASON index);

bool is_ldo5v_wakeup();

void p33_io_wakeup_port_init(struct _p33_io_edge_wakeup_config *config);
void p33_io_wakeup_port_uninit(u32 gpio);
void p33_io_wakeup_enable(u32 gpio, u32 enable);

#endif
