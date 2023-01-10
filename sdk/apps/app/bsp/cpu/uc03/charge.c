#include "gpio.h"
#include "charge.h"
#include "asm/power_interface.h"
#include "efuse.h"
#include "saradc.h"
#include "app_config.h"

#define LOG_TAG_CONST       CHARGE
#define LOG_TAG             "[charge]"
#include "log.h"

#define TCFG_CHARGE_FULL_FILTER     10 // *2 ms
#define TCFG_VPWR_ON_FILTER         50 // *2 ms
#define TCFG_VPWR_OFF_FILTER        100// *2 ms
#define TCFG_VPWR_PULLDOWN_LEV      CHARGE_PULLDOWN_50K
#define TCFG_VPWR_PULLDOWN_EN       1

typedef struct _CHARGE_VAR {
    u32 vbat_voltage;
    u8 trickle_flag : 1;
    u8 vbat_isr : 1;
    u8 vpwr_isr : 1;
    u8 init : 1;
    u8 full_flag : 1;
} CHARGE_VAR;

#define __this 	(&charge_var)
static CHARGE_VAR charge_var;

#define BIT_VPWR_IN			1
#define BIT_VPWR_OFF		2

#if (TCFG_CHARGE_FULL_V >= CHARGE_FULL_V_4237)
#define INIT_P3_CHG_CON0    (BIT(4) | BIT(2))
#else
#define INIT_P3_CHG_CON0    (BIT(4))
#endif
#define INIT_P3_CHG_CON2    (TCFG_CHARGE_FULL_MA << 4)
#if TCFG_VPWR_PULLDOWN_EN
#define INIT_P3_VPWR_CON0   (BIT(0))
#define INIT_P3_VPWR_CON1   (TCFG_VPWR_PULLDOWN_LEV)
#endif

static void charge_dump(void)
{
    log_info("P3_CHG_CON0: 0x%x\n", P33_CON_GET(P3_CHG_CON0));
    log_info("P3_CHG_CON1: 0x%x\n", P33_CON_GET(P3_CHG_CON1));
    log_info("P3_CHG_CON2: 0x%x\n", P33_CON_GET(P3_CHG_CON2));
    log_info("P3_CHG_CON3: 0x%x\n", P33_CON_GET(P3_CHG_CON3));
    log_info("P3_VPWR_CON0: 0x%x\n", P33_CON_GET(P3_VPWR_CON0));
    log_info("P3_VPWR_CON1: 0x%x\n", P33_CON_GET(P3_VPWR_CON1));
}

/*----------------------------------------------------*/
/**@brief    获取VPWR和插入电压（约1V）比的大小
   @param    无
   @return   0:VPWR<1V 1:VPWR>1v
   @note     该接口直接读取寄存器,没有经过软件滤波
*/
/*----------------------------------------------------*/
u8 charge_get_vpwr_online_hw(void)
{
    return LDO5V_DET_GET();
}

/*----------------------------------------------------*/
/**@brief    获取VPWR和VBAT比的大小
   @param    无
   @return   0:VPWR<VBAT 1:VPWR>VBAT
   @note     该接口直接读取寄存器,没有经过软件滤波
*/
/*----------------------------------------------------*/
u8 charge_get_lvcmp_det(void)
{
    return LVCMP_DET_GET();
}

/*----------------------------------------------------*/
/**@brief    设置当前电池电压
   @param    无
   @return   无
   @note     APP定时获取到电池电量,设置到充电模块
*/
/*----------------------------------------------------*/
void charge_set_vbat_voltage(u32 voltage)
{
    __this->vbat_voltage = voltage;
}

/*----------------------------------------------------*/
/**@brief    设置恒流充电电流档位
   @param    参考charge.h充电电流档位
   @return   无
   @note     该接口用于改变充电电流档位,使用场景有:
             1、电池电压小时(<3V),用小电流充电(20mA)
                电池电压大时(>3V),用配置的电流充电(charge_mA)
             2、温度升高时,降低充电电流,减小发热
*/
/*----------------------------------------------------*/
void charge_set_mA(u8 charge_mA)
{
    static u8 charge_mA_old = 0xff;
    if (charge_mA_old != charge_mA) {
        charge_mA_old = charge_mA;
        if (charge_mA & BIT(4)) {
            //CHG_TRICKLE_EN(1);
            //CHGI_TURBO_EN(0);
            P3_CHG_CON0 |=  BIT(3);
            P3_CHG_CON0 &= ~BIT(5);
        } else if (charge_mA & BIT(5)) {
            //CHG_TRICKLE_EN(0);
            //CHGI_TURBO_EN(1);
            P3_CHG_CON0 &= ~BIT(3);
            P3_CHG_CON0 |=  BIT(5);
        } else {
            //CHG_TRICKLE_EN(0);
            //CHGI_TURBO_EN(1);
            P3_CHG_CON0 &= ~(BIT(3) | BIT(5));
        }
        /* CHARGE_mA_SEL(charge_mA & 0xf); */
        P3_CHG_CON1 &= ~0xf0;
        P3_CHG_CON1 |= ((charge_mA & 0xf) << 4);
    }
}

/*----------------------------------------------------*/
/**@brief    开始充电
   @param    无
   @return   无
   @note     当VPWR电压大于电池电压时,可进行充电
*/
/*----------------------------------------------------*/
void charge_start(void)
{
    u8 check_full = 0;
    log_info("%s", __func__);

    __this->full_flag = 0;
    if (__this->vbat_voltage < CHARGE_CCVOL_V) {
        charge_set_mA(TCFG_CHARGE_TRICKLE_MA);
        __this->trickle_flag = 1;
    } else {
        check_full = 1;
        __this->trickle_flag = 0;
        charge_set_mA(TCFG_CHARGE_MA);
        power_awakeup_gpio_enable(IO_CHGFL_DET, 1);
    }

    //CHARGE_EN(1);
    //CHARGE_GO(1);
    P3_CHG_CON0 |= BIT(1) | BIT(0);

    //开启充电时,充满标记为1时,主动检测一次是否充满
    if (check_full && CHARGE_FULL_FLAG_GET()) {
        __this->vbat_isr = 1;
    }
}

/*----------------------------------------------------*/
/**@brief    停止充电
   @param    无
   @return   无
   @note     当VPWR电压小于于电池电压时,可停止充电
*/
/*----------------------------------------------------*/
void charge_close(void)
{
    log_info("%s", __func__);
    /* CHARGE_EN(0); */
    /* CHARGE_GO(0); */
    P3_CHG_CON0 &= ~(BIT(1) | BIT(0));
    __this->trickle_flag = 0;
    __this->full_flag = 0;
    power_awakeup_gpio_enable(IO_CHGFL_DET, 0);
}

/*----------------------------------------------------*/
/**@brief    充电中,电池电量查询
             根据电池电压切换涓流/恒流充电
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
static void charge_vbat_detect(void)
{
    static u16 det_cnt = 0;

    //1s检测一次时间是否到
    det_cnt++;
    if (det_cnt < 500) {
        return;
    }
    det_cnt = 0;

    //判断涓流转恒流,检测电池电压,是否大于CHARGE_CCVOL_V
    if (IS_CHARGE_EN() && __this->trickle_flag) {
        log_info("vbat_voltage: %d\n", __this->vbat_voltage);
        if (__this->vbat_voltage > CHARGE_CCVOL_V) {
            __this->trickle_flag = 0;
            charge_set_mA(TCFG_CHARGE_MA);
            power_awakeup_gpio_enable(IO_CHGFL_DET, 1);
            __this->vbat_isr = 1;
        }
    }
#if RECHARGE_ENABLE
    //如果充满了,则检测是否需要复充
    if (__this->full_flag && (__this->vbat_voltage < RECHARGE_V)) {
        log_info("recharge!");
        charge_start();
    }
#endif
}

/*----------------------------------------------------*/
/**@brief    充满检测
   @param    无
   @return   无
   @note     当充电电流小于配置的充满电流时,充电判满
*/
/*----------------------------------------------------*/
static void charge_full_detect(void)
{
    static u16 charge_full_cnt = 0;

    if (__this->vbat_isr == 0) {
        return;
    }

    if (IS_CHARGE_EN() == 0) {
        __this->vbat_isr = 0;
        return;
    }

    if (CHARGE_FULL_FILTER_GET()) {
        if (CHARGE_FULL_FLAG_GET() && LVCMP_DET_GET()) {
            /* putchar('F'); */
            if (charge_full_cnt < TCFG_CHARGE_FULL_FILTER) {
                charge_full_cnt++;
            } else {
                charge_full_cnt = 0;
                log_info("Charge Full!\r\n");
                charge_close();
                __this->vbat_isr = 0;
                __this->full_flag = 1;
            }
        } else {
            /* putchar('K'); */
            charge_full_cnt = 0;
        }
    } else {
        charge_full_cnt = 0;
        __this->vbat_isr = 0;
    }
}

/*----------------------------------------------------*/
/**@brief    VPWR插拔检测
   @param    无
   @return   无
   @note     当VPWR电压不同时,改变检测状态,开启充电或关闭充电
*/
/*----------------------------------------------------*/
static void charge_vpwr_detect(void)
{
    static u32 vpwr_in_normal_cnt = 0;
    static u32 vpwr_off_cnt = 0;
    static u8  vpwr_status = BIT(BIT_VPWR_OFF);

    if (__this->vpwr_isr == 0) {
        return;
    }

    if (LVCMP_DET_GET()) {
        //VPWR > vbat
        /* putchar('X'); */
        if (vpwr_in_normal_cnt < TCFG_VPWR_ON_FILTER) {
            vpwr_in_normal_cnt++;
        } else {
            if (vpwr_status != BIT_VPWR_IN) {
                log_info("VPWR_IN\r\n");
                vpwr_status = BIT_VPWR_IN;
                charge_start();
            }
            goto __clr_exit;
        }
    } else if (LDO5V_DET_GET() == 0) {
        //VPWR < VBAT
        /* putchar('Q'); */
        if (vpwr_off_cnt < (TCFG_VPWR_OFF_FILTER)) {
            vpwr_off_cnt++;
        } else {
            if (vpwr_status != BIT_VPWR_OFF) {
                log_info("VPWR_OFF\r\n");
                vpwr_status = BIT_VPWR_OFF;
                charge_close();
            }
            goto __clr_exit;
        }
    }
    return;
__clr_exit:
    vpwr_off_cnt = 0;
    vpwr_in_normal_cnt = 0;
    __this->vpwr_isr = 0;
    return;
}

/*----------------------------------------------------*/
/**@brief    检测定时器2ms
   @param    无
   @return   无
   @note     调用VPWR插拔检测和充电时的充满检测
*/
/*----------------------------------------------------*/
void charge_timer_handle(void)
{
    if (!__this->init) {
        return;
    }
    charge_vpwr_detect();
    charge_full_detect();
    charge_vbat_detect();
}

/*----------------------------------------------------*/
/**@brief    进入低功耗时检测是否允许进入
   @param    无
   @return   0:禁止进低功耗 1:允许进低功耗
   @note     无
*/
/*----------------------------------------------------*/
u8 charge_check_is_idle(void)
{
    return ((__this->vpwr_isr == 0) && (__this->vbat_isr == 0) && (IS_CHARGE_EN() == 0));
}

/*----------------------------------------------------*/
/**@brief    充电口边沿唤醒回调接口
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void vpwr_wakeup_isr(void)
{
    __this->vpwr_isr = 1;
}

/*----------------------------------------------------*/
/**@brief    充满标记唤醒系统回调接口
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void charge_wakeup_isr(void)
{
    __this->vbat_isr = 1;
}

/*----------------------------------------------------*/
/**@brief    充电配置
   @param    无
   @return   无
   @note     根据efuse的trim值,设置充满电压寄存器;
             根据配置设置充满电流阈值;
             根据配置设置恒流充电电流
*/
/*----------------------------------------------------*/
static void charge_config(void)
{
#if (TCFG_CHARGE_FULL_V == CHARGE_FULL_V_4199)
    P3_CHG_CON1 |= (u8)efuse_get_vbat_trim_420();//4.20V对应的trim出来的实际档位
#elif (TCFG_CHARGE_FULL_V == CHARGE_FULL_V_4354)
    P3_CHG_CON1 |= (u8)efuse_get_vbat_trim_435();//4.35V对应的trim出来的实际档位
#else
    u8 charge_trim_val;
    u8 offset = 0;
    u8 charge_full_v_val = 0;
    if (TCFG_CHARGE_FULL_V < CHARGE_FULL_V_4237) {
        charge_trim_val = (u8)efuse_get_vbat_trim_420();//4.20V对应的trim出来的实际档位
        log_info("low charge_trim_val = %d\n", charge_trim_val);
        if (TCFG_CHARGE_FULL_V >= CHARGE_FULL_V_4199) {
            offset = TCFG_CHARGE_FULL_V - CHARGE_FULL_V_4199;
            charge_full_v_val = charge_trim_val + offset;
            if (charge_full_v_val > 0xf) {
                charge_full_v_val = 0xf;
            }
        } else {
            offset = CHARGE_FULL_V_4199 - TCFG_CHARGE_FULL_V;
            if (charge_trim_val >= offset) {
                charge_full_v_val = charge_trim_val - offset;
            } else {
                charge_full_v_val = 0;
            }
        }
    } else {
        charge_trim_val = (u8)efuse_get_vbat_trim_435();//4.35V对应的trim出来的实际档位
        log_info("high charge_trim_val = %d\n", charge_trim_val);
        if (TCFG_CHARGE_FULL_V >= CHARGE_FULL_V_4354) {
            offset = TCFG_CHARGE_FULL_V - CHARGE_FULL_V_4354;
            charge_full_v_val = charge_trim_val + offset;
            if (charge_full_v_val > 0xf) {
                charge_full_v_val = 0xf;
            }
        } else {
            offset = CHARGE_FULL_V_4354 - TCFG_CHARGE_FULL_V;
            if (charge_trim_val >= offset) {
                charge_full_v_val = charge_trim_val - offset;
            } else {
                charge_full_v_val = 0;
            }
        }
    }
    log_info("charge_full_v_val = %d\n", charge_full_v_val);
    /* CHARGE_FULL_V_SEL(charge_full_v_val); */
    P3_CHG_CON1 |= charge_full_v_val;
#endif
    /* CHGI_TRIM_SEL(efuse_get_charge_cur_trim()); */
    P3_CHG_CON3 |= efuse_get_charge_cur_trim();
    charge_set_mA(TCFG_CHARGE_TRICKLE_MA);
}

/*----------------------------------------------------*/
/**@brief    内置充电初始化
   @param    配置结构体
   @return   无
   @note     设置充电相关寄存器
*/
/*----------------------------------------------------*/
void charge_init(void)
{
    log_info("%s\r\n", __func__);
#ifdef SARADC_ISR_MODE
    saradc_add_sample_ch(ADC_CH_PMU_1_4_VBAT);
#endif
    power_awakeup_gpio_enable(IO_CHGFL_DET, 0);

    //初始化充电寄存器
    P3_CHG_CON0 = INIT_P3_CHG_CON0;
    P3_CHG_CON1 = 0;
    P3_CHG_CON2 = INIT_P3_CHG_CON2;
    P3_CHG_CON3 = 0;

    /*VPWR的100K下拉电阻使能*/
#if TCFG_VPWR_PULLDOWN_EN
    P3_VPWR_CON1 = TCFG_VPWR_PULLDOWN_LEV;
    P3_VPWR_CON0 = BIT(0);
#else
    P3_VPWR_CON1 = 0;
    P3_VPWR_CON0 = 0;
#endif

    charge_config();

    __this->init = 1;

    __this->vpwr_isr = 1;

    __this->full_flag = 0;

    __this->vbat_voltage = saradc_get_vbat_voltage();

    charge_dump();
}


