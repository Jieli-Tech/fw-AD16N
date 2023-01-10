/*********************************************************************************************
    *   Filename        : power_wakeup.h

    *   Description     :

    *   Author          : Sunlicheng

    *   Email           : Sunlicheng@zh-jieli.com

    *   Last modifiled  : 2022-01-07 11:30

    *   Copyright:(c)JIELI  2021-2030  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef ___POWER_WAKEUP_H___
#define ___POWER_WAKEUP_H___

//=========================唤醒原因==================================
enum WAKEUP_REASON {
    //P33 WAKEUP
    PWR_WK_REASON_PLUSE_CNT_OVERFLOW,   //pcnt唤醒
    PWR_WK_REASON_PORT_EDGE,			//数字io输入边沿唤醒
    PWR_WK_REASON_ANA_PORT_EDGE,			//模拟io输入边沿唤醒
    PWR_WK_REASON_VDDIO_LVD,			//vddio lvd唤醒
    PWR_WK_REASON_EDGE_INDEX0,			//p33 index0  io唤醒
    PWR_WK_REASON_EDGE_INDEX1,			//p33 index1  io唤醒
    PWR_WK_REASON_EDGE_INDEX2,			//p33 index2  io唤醒
    PWR_WK_REASON_EDGE_INDEX3,			//p33 index3  io唤醒
    PWR_WK_REASON_EDGE_INDEX4,     		//p33 index4  io唤醒
    PWR_WK_REASON_EDGE_INDEX5,          //p33 index5  io唤醒
    PWR_WK_REASON_EDGE_INDEX6,          //p33 index6  io唤醒
    PWR_WK_REASON_EDGE_INDEX7,          //p33 index7  io唤醒
    PWR_WK_REASON_EDGE_INDEX8,          //p33 index8  io唤醒
    PWR_WK_REASON_EDGE_INDEX9,          //p33 index9  io唤醒
    PWR_WK_REASON_EDGE_INDEX10,         //p33 index10 io唤醒
    PWR_WK_REASON_EDGE_INDEX11,         //p33 index11 io唤醒

    PWR_WK_REASON_P_ANA_EDGE_VBATDT,       //p33 vbat detect唤醒
    PWR_WK_REASON_N_ANA_EDGE_VBATDT,       //p33 vbat detect唤醒
    PWR_WK_REASON_P_ANA_EDGE_VBATCH,       //p33 vbat bpwr比较器唤醒
    PWR_WK_REASON_N_ANA_EDGE_VBATCH,       //p33 vbat bpwr比较器唤醒
    PWR_WK_REASON_P_ANA_EDGE_VPWRDT,       //p33 vpwr detect唤醒
    PWR_WK_REASON_N_ANA_EDGE_VPWRDT,       //p33 vpwr detect唤醒
};

//=========================唤醒参数配置==================================

struct wakeup_param {
    //数字io输入
    const struct port_wakeup *port[MAX_WAKEUP_PORT];
    //模拟io输入
    const struct port_wakeup *aport[MAX_WAKEUP_ANA_PORT];
};

//=========================唤醒接口==================================
void power_wakeup_index_enable(u8 index, u8 enable);
void power_wakeup_gpio_enable(u8 gpio, u8 enable);
void power_wakeup_gpio_edge(u8 gpio, POWER_WKUP_EDGE edge);

void power_awakeup_index_enable(u8 index, u8 enable);
void power_awakeup_gpio_enable(u8 gpio, u8 enable);
void power_awakeup_gpio_edge(u8 gpio, POWER_WKUP_EDGE edge);

void power_wakeup_set_callback(void (*wakeup_callback)(u8 index, u8 gpio));
void power_awakeup_set_callback(void (*wakeup_callback)(u8 index, u8 gpio, POWER_WKUP_EDGE edge));

void port_edge_wkup_set_callback_by_index(u8 index, void (*wakeup_callback)(u8 index, u8 gpio));
void aport_edge_wkup_set_callback_by_index(u8 index, void (*wakeup_callback)(u8 index, u8 gpio, POWER_WKUP_EDGE edge));
void lvd_wkup_set_callback(void (*wakeup_callback)(void));

void power_wakeup_init(const struct wakeup_param *param);

u8 is_wakeup_source(enum WAKEUP_REASON index);

void power_wakeup_reason_dump();



#endif

