#ifndef _CHARGE_H_
#define _CHARGE_H_

#include "typedef.h"
#include "device.h"

/*------充满电电压选择 4.041V-4.534V-------*/
//低压压电池配置0~15
#define CHARGE_FULL_V_4041		0
#define CHARGE_FULL_V_4061		1
#define CHARGE_FULL_V_4081		2
#define CHARGE_FULL_V_4101		3
#define CHARGE_FULL_V_4119		4
#define CHARGE_FULL_V_4139		5
#define CHARGE_FULL_V_4159		6
#define CHARGE_FULL_V_4179		7
#define CHARGE_FULL_V_4199		8
#define CHARGE_FULL_V_4219		9
#define CHARGE_FULL_V_4238		10
#define CHARGE_FULL_V_4258		11
#define CHARGE_FULL_V_4278		12
#define CHARGE_FULL_V_4298		13
#define CHARGE_FULL_V_4318		14
#define CHARGE_FULL_V_4338		15
//高压电池配置16~31
#define CHARGE_FULL_V_4237		16
#define CHARGE_FULL_V_4257		17
#define CHARGE_FULL_V_4275		18
#define CHARGE_FULL_V_4295		19
#define CHARGE_FULL_V_4315		20
#define CHARGE_FULL_V_4335		21
#define CHARGE_FULL_V_4354		22
#define CHARGE_FULL_V_4374		23
#define CHARGE_FULL_V_4394		24
#define CHARGE_FULL_V_4414		25
#define CHARGE_FULL_V_4434		26
#define CHARGE_FULL_V_4454		27
#define CHARGE_FULL_V_4474		28
#define CHARGE_FULL_V_4494		29
#define CHARGE_FULL_V_4514		30
#define CHARGE_FULL_V_4534		31
#define CHARGE_FULL_V_MAX       32
/*****************************************/

/*充满电电流选择 恒流充电的1/N*/
#define CHARGE_FULL_mA_DIV5		0   //恒流充电电流的1/5
#define CHARGE_FULL_mA_DIV10	1   //恒流充电电流的1/10
#define CHARGE_FULL_mA_DIV15 	2   //恒流充电电流的1/15
#define CHARGE_FULL_mA_DIVD20	3   //恒流充电电流的1/20

/*
 	充电电流选择
	恒流：20-180mA
    注意:当充电电流大于等于时CHARGE_mA_100时,判满电流选择CHARGE_FULL_mA_DIV10时,实际是CHARGE_FULL_mA_DIVD20
*/
//正常恒流充电档位
#define CHARGE_mA_20			0
#define CHARGE_mA_25			1
#define CHARGE_mA_30			2
#define CHARGE_mA_35			3
#define CHARGE_mA_40			4
#define CHARGE_mA_45			5
#define CHARGE_mA_50			6
#define CHARGE_mA_55			7
#define CHARGE_mA_60			8
#define CHARGE_mA_65			9
#define CHARGE_mA_70			10
#define CHARGE_mA_75			11
#define CHARGE_mA_80			12
#define CHARGE_mA_90			13
#define CHARGE_mA_100           14
#define CHARGE_mA_120           15
//涓流可设充电档位--需要更小的电流时设置
#define CHARGE_mA_2			    (BIT(4) | CHARGE_mA_20)
#define CHARGE_mA_2P5			(BIT(4) | CHARGE_mA_25)
#define CHARGE_mA_3		    	(BIT(4) | CHARGE_mA_30)
#define CHARGE_mA_3P5			(BIT(4) | CHARGE_mA_35)
#define CHARGE_mA_4             (BIT(4) | CHARGE_mA_40)
#define CHARGE_mA_4P5           (BIT(4) | CHARGE_mA_45)
#define CHARGE_mA_5             (BIT(4) | CHARGE_mA_50)
#define CHARGE_mA_5P5           (BIT(4) | CHARGE_mA_55)
#define CHARGE_mA_6	    		(BIT(4) | CHARGE_mA_60)
#define CHARGE_mA_6P5			(BIT(4) | CHARGE_mA_65)
#define CHARGE_mA_7 			(BIT(4) | CHARGE_mA_70)
#define CHARGE_mA_7P5			(BIT(4) | CHARGE_mA_75)
#define CHARGE_mA_8             (BIT(4) | CHARGE_mA_80)
#define CHARGE_mA_9             (BIT(4) | CHARGE_mA_90)
#define CHARGE_mA_10            (BIT(4) | CHARGE_mA_100)
#define CHARGE_mA_12            (BIT(4) | CHARGE_mA_120)
//增强恒流充电档位--需要更大的电流时设置(不建议使用后面的档位)
#define CHARGE_mA_130			(BIT(5) | CHARGE_mA_65) //计算判满电流时使用 65/CHARGE_FULL_mA_DIVx
#define CHARGE_mA_140           (BIT(5) | CHARGE_mA_70) //计算判满电流时使用 70/CHARGE_FULL_mA_DIVx
#define CHARGE_mA_150           (BIT(5) | CHARGE_mA_75) //计算判满电流时使用 75/CHARGE_FULL_mA_DIVx
#define CHARGE_mA_160           (BIT(5) | CHARGE_mA_80) //计算判满电流时使用 80/CHARGE_FULL_mA_DIVx
#define CHARGE_mA_180           (BIT(5) | CHARGE_mA_90) //计算判满电流时使用 90/CHARGE_FULL_mA_DIVx

/*
 	充电口下拉选择
	电阻 50k ~ 200k
*/
#define CHARGE_PULLDOWN_50K     0
#define CHARGE_PULLDOWN_100K    1
#define CHARGE_PULLDOWN_150K    2
#define CHARGE_PULLDOWN_200K    3

/*
 	涓流转恒流电压
*/
#define CHARGE_CCVOL_V			3000

/*
    复充电压设置
*/
#define RECHARGE_ENABLE         0
#define RECHARGE_V			    4000

extern u8 get_ldo5v_online_hw(void);
extern u8 get_lvcmp_det(void);
extern u8 charge_get_vpwr_online_hw(void);
extern u8 charge_get_lvcmp_det(void);
extern u8 charge_check_is_idle(void);
extern u8 charge_check_is_full(void);
extern void charge_timer_handle(void);
extern void charge_start(void);
extern void charge_close(void);
extern void charge_set_mA(u8 charge_mA);
extern void charge_set_vbat_voltage(u32 voltage);
extern void charge_check_and_set_pinr(u8 mode);
extern void vpwr_wakeup_isr(void);
extern void charge_wakeup_isr(void);
extern void charge_init(void);

#endif    //_CHARGE_H_

