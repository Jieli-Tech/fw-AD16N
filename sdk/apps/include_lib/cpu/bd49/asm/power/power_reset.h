#ifndef __POWER_RESET_H__
#define __POWER_RESET_H__

#include "asm/power/pmu_flag.h"
/*
 *复位原因包括两种
 1.系统复位源: p33 p11 主系统
 2.自定义复位源：唤醒、断言、异常等
 */
enum RST_REASON {
    /*主系统*/
    MSYS_P11_RST,						//P11复位
    MSYS_P33_RST,						//P33复位
    MSYS_DVDD2_POR_RST,					//DVDD2上电
    MSYS_DVDD_POR_RST,					//DVDD上电
    MSYS_SOFT_RST,						//主系统软件复位
    MSYS_P2M_RST,						//低功耗唤醒复位(p11数字复位msys，soff/poff)
    MSYS_LPMR_RST,						//低功耗唤醒复位(p33数字复位msys, soff)
    MSYS_POWER_RETURN,					//主系统未被复位

    /*P11*/
    P11_PVDD_POR_RST,					//pvdd上电
    P11_LPMR_RST,						//低功耗唤醒复位(softoff legacy)
    P11_P33_RST,						//p33复位
    P11_P33_POR_RST,					//p33电源复位
    P11_WDT_RST,						//看门狗复位
    P11_SOFT_RST,						//软件复位
    P11_MSYS_RST,						//主系统复位P11
    P11_WVDD_POR,						//P33电源复位了
    P11_POWER_RETURN,					//P11系统未被复位

    /*P33*/
    P33_VDDIO_POR_RST,					//vddio上电复位(电池/vpwr供电)
    P33_VDDIO_LVD_RST,					//vddio低压复位、上电复位(电池/vpwr供电)
    P33_WDT_RST,						//看门狗复位
    P33_VCM_RST,						//vcm高电平短接复位
    P33_MCLR_RST,						//mclr低电平短接复位
    P33_PPINR_RST,						//数字io输入长按复位
    P33_P11_RST,						//p11系统复位p33，rset_mask=0
    P33_MSYS_RST,						//msys系统复位p33，rset_mask=0
    P33_SOFT_RST,						//p33软件复位，一般软件复位指此系统复位源，所有系统会直接复位。
    P33_PPINR1_RST,						//模拟io输入长按复位，包括charge_full、vatch、ldoint、vabt_det
    P33_PPINR1_SOFT_RST,				//PINR1保护机制使能时，当软件清PINR PND1的操作发生在窗口外时产生的复位
    P33_POWER_RETURN,					//p33系统未被复位。

    /*RTC*/
    R3_VDDIO_RST,						//rtc vddio
    R3_SOFT_RST,						//rtc 软件复位
    R3_POWER_RETURN,				    //rtc未被复位

    P33_EXCEPTION_SOFT_RST,	    		//异常软件复位
    P33_ASSERT_SOFT_RST,				//断言软件复位
    P33_RESERVE_SOFT_RST,				//默认软件复位

    RST_REASON_RESERVE = 63,
};

enum RESET_FLAG {
    RESET_FLAG_NULL,
    EXCEPTION_FLAG,
    ASSERT_FLAG,
    UPDATE_FLAG,
    BT_FLAG,

    //for vendor
    RESET_FLAG_VENDOR = 0x80,

    RESET_FLAG_MAX = 0xff,
};
// App 或者 用户自定义复位命令按照如下格式，0~0x80 为系统保留
// #define  RESET_FLAG_demo1   RESET_FLAG_VENDOR | 0x01
// #define  RESET_FLAG_demo2   RESET_FLAG_VENDOR | 0x7e
//



/**
 * @brief  判断是否为指定复位源
 * @param  index: 复位源
 * @retval 1: 是     0：否
 */
bool is_reset_source(enum RST_REASON index);

/**
 * @brief  判断是否为指定系统软件复位源
    指定软件复位源为flag
    is_system_reset(flag)
    is_reset_source(flag+P33_EXCEPTION_SOFT_RST)

 * @param  index: 软件复位源
 * @retval 1: 是     0：否
 */
bool is_system_reset(enum RESET_FLAG flag);

/**
 * @brief  系统软复位，并指定复位信息
 * @param  flag：系统软复位源
 * @retval 1: 是     0：否
 */
void system_reset(enum RESET_FLAG flag);

/**
 * @brief  复位的时候锁存模拟模块状态，例如GPIO
 */
void latch_reset(struct boot_soft_flag_t *softflag);

/**
 * @brief 返回系统软复位源
 * @retval 软复位源的标号
 */
enum RESET_FLAG get_system_reset_val(void);

/* void p33_soft_reset(); */

#endif
