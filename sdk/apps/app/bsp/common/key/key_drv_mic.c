#include "key_drv_mic.h"
#include "app_config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[key_mic]"
#include "log.h"

#if KEY_MIC_EN

#include "adc_api.h"
#include "audio_adc.h"

/************************************耳机按键检测原理********************************************************************
 * 接法1: MICLDO --> MIC RS(内置电阻) --> VCMO1/0 --> KEY --> VCMO0/1; 即配置宏 TCFG_MIC_KEY_USED_TYPE = MIC_KEY_USED_VCMO
 * 接法2: MICLDO --> MIC RS(内置电阻) --> VCMO1/0 --> KEY --> GND; 即配置宏 TCFG_MIC_KEY_USED_TYPE = MIC_KEY_USED_GND
 * 接法3: MICLDO(PA0) --> MIC RS(外置电阻) --> KEY(连到AD口) --> GND;即配置宏 TCFG_MIC_KEY_USED_TYPE = MIC_KEY_USED_OUTSIDE
 *
 *                              MICLDO(MIC供电电压可配) -- AD值为adc_micldo
 *                                |
 *                              MIC RS(电阻阻值在app_config.h头文件配置)
 *                                |
 *                              MIC(内置电阻则会从VCMO0/1输出,外置电阻则和普通AD口连一起) -- AD值为adc_mic
 *                                |
 *                              KEY(按键,三个按键电阻默认为400/200/0)
 *                                |
 *                              公共端(GND/VCMO) -- AD值为adc_vcmo
 *
 * step1:读取MICLDO的AD值adc_micldo,读取按键的AD值mic_adc,读取公共端的AD值adc_vcmo(接地则设置为0)
 * step2:计算出整个路径上总的AD值(adc_micldo-adc_vcmo),根据按键的三个电阻可以计算出当对应按键按下时相对公共端的AD值
 *          adc_pp = PP_KEY_RES * (adc_micldo - adc_vcmo) / (mic_res + PP_KEY_RES)
 *          adc_up = UP_KEY_RES * (adc_micldo - adc_vcmo) / (mic_res + UP_KEY_RES)
 *          adc_down = DOWN_KEY_RES * (adc_micldo - adc_vcmo) / (mic_res + DOWN_KEY_RES)
 * step3:计算MIC(IO)相对于公共端的ad值,即(adc_mic - adc_vcmo)
 * step4:比较(adc_mic - adc_vcmo)与 adc_pp、adc_up、adc_down、(adc_micldo-adc_vcmo),ad值和谁接近就是谁被按下了。
************************************************************************************************************************/

#define MIC_KEY_PP      0x00
#define MIC_KEY_UP      0x01
#define MIC_KEY_DOWN    0x02

#define PP_KEY_RES      0       //PP按键阻值
#define UP_KEY_RES      200     //UP按键阻值
#define DOWN_KEY_RES    400     //DOWN按键阻值

#define POWER_ON_FITER  (2000/10)//开机2s后,删除一些通道不采集

static volatile u8 mic_key_en, power_on_flag;
static volatile u8 mic_key_vcmo;
static u8 power_on_cnt;
#if TCFG_EARPHONE_ONLINE_CHECK_EN
static u32 detect_filter;
#endif

/*----------------------------------------------------------------------------*/
/**@brief   按键去抖函数，输出稳定键值
   @param   key：键值
   @return  稳定按键
   @note    u8 key_filter(u8 key)
*/
/*----------------------------------------------------------------------------*/
static u8 key_filter(u8 key)
{
    static u8 used_key = NO_KEY;
    static u8 old_key;
    static u8 key_counter;

    if (old_key != key) {
        key_counter = 0;
        old_key = key;
    } else {
        key_counter++;
        if (key_counter == KEY_BASE_CNT) {
            used_key = key;
        }
    }
    return used_key;
}

/*----------------------------------------------------------------------------*/
/**@brief   MICkey获取按键值
   @param   void
   @param   void
   @return  key_num:mic按键号
   @note    u8 get_mickey_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 get_mickey_value(void)
{
    u8 key = NO_KEY;
    u16 adc_mic, adc_vcmo, adc_micldo;
    u16 diff_val, diff_pp, diff_up, diff_down, diff_mic, diff_min, mic_res;

    //开机1s后使能记录对应通道ADC,不在采集对应通道
    if (power_on_flag) {
        power_on_cnt++;
        if (power_on_cnt > POWER_ON_FITER) {
            power_on_flag = 0;
#if ((TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_VCMO) || (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_GND))
#if (TCFG_EARPHONE_ONLINE_CHECK_EN == DISABLE)
            adc_api_set_channel_status(AD_CH_AUDIO_MICLDO, ADC_STATUS_START);
#if (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_VCMO)
            if (mic_key_vcmo == 0) {
                adc_api_set_channel_status(AD_CH_AUDIO_VCMO0, ADC_STATUS_START);
            } else {
                adc_api_set_channel_status(AD_CH_AUDIO_VCMO1, ADC_STATUS_START);
            }
#endif
#endif
#else
            adc_api_set_channel_status(AD_CH_AUDIO_MICLDO, ADC_STATUS_START);
#endif
        }
    }

    if (mic_key_en == 0) {
        return NO_KEY;
    }

    adc_micldo = adc_api_get_value(AD_CH_AUDIO_MICLDO);

#if (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_OUTSIDE)
    adc_vcmo = 0;
    adc_mic = adc_api_get_value(TCFG_MIC_KEY_AD_CH);
#else
    if (mic_key_vcmo == 0) {
#if (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_VCMO)
        adc_vcmo = adc_api_get_value(AD_CH_AUDIO_VCMO0);
#else
        adc_vcmo = 0;
#endif
        adc_mic = adc_api_get_value(AD_CH_AUDIO_VCMO1);
    } else {
#if (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_VCMO)
        adc_vcmo = adc_api_get_value(AD_CH_AUDIO_VCMO1);
#else
        adc_vcmo = 0;
#endif
        adc_mic = adc_api_get_value(AD_CH_AUDIO_VCMO0);
    }
#endif

#if TCFG_EARPHONE_ONLINE_CHECK_EN
    if ((adc_mic + 5) >= adc_micldo) {
        detect_filter = 250;//可能拔出抖动,滤波500ms
    }
    if (detect_filter) {
        detect_filter--;
        return NO_KEY;
    }
#endif

    /* log_info("adc_micldo: %d, adc_mic: %d, adc_vcmo: %d, diff0: %d, diff1: %d\n", adc_micldo, adc_mic, adc_vcmo, adc_micldo - adc_vcmo, adc_mic - adc_vcmo); */

#if (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_OUTSIDE)
    mic_res = TCFG_MIC_BIAS_OUTSIDE_RES;
#else
    mic_res = set_auadc_mic_bias_rs(AUMIC_RS_READ) + 600;//加上内部开关内阻0.6K
#endif

    diff_val = adc_mic - adc_vcmo;
    diff_mic = adc_micldo - adc_vcmo;
    diff_pp  = PP_KEY_RES * diff_mic / (PP_KEY_RES + mic_res);
    diff_up  = UP_KEY_RES * diff_mic / (UP_KEY_RES + mic_res);
    diff_down  = DOWN_KEY_RES * diff_mic / (DOWN_KEY_RES + mic_res);

    //判断无按键电平
    if (diff_val > (diff_mic / 2)) {
        goto __key_filter;
    }

    diff_mic = (diff_mic > diff_val) ? (diff_mic - diff_val) : (diff_val - diff_mic);
    diff_pp = (diff_pp > diff_val) ? (diff_pp - diff_val) : (diff_val - diff_pp);
    diff_up = (diff_up > diff_val) ? (diff_up - diff_val) : (diff_val - diff_up);
    diff_down = (diff_down > diff_val) ? (diff_down - diff_val) : (diff_val - diff_down);

    diff_min = diff_mic;
    if (diff_pp < diff_min) {
        diff_min = diff_pp;
        key = MIC_KEY_PP;
    }
    if (diff_up < diff_min) {
        diff_min = diff_up;
        key = MIC_KEY_UP;
    }
    if (diff_down < diff_min) {
        diff_min = diff_down;
        key = MIC_KEY_DOWN;
    }

__key_filter:
    /* log_info("diff_mic: %d, diff_pp: %d, diff_up: %d, diff_down: %d, key: %d\n", diff_mic, diff_pp, diff_up, diff_down, key); */
    return key_filter(key);
}

void mic_key_init(void)
{
#if ((TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_VCMO) || (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_GND))
#if (TCFG_EARPHONE_ONLINE_CHECK_EN == DISABLE)
    mic_key_vcmo = ((TCFG_VCMO0_ENABLE == 0) ? 1 : 0);
    adc_api_set_channel_status(AD_CH_AUDIO_MICLDO, ADC_STATUS_NORMAL);
    if (mic_key_vcmo == 0) {
#if (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_VCMO)
        adc_api_set_channel_status(AD_CH_AUDIO_VCMO0, ADC_STATUS_NORMAL);
#endif
        adc_api_set_channel_status(AD_CH_AUDIO_VCMO1, ADC_STATUS_NORMAL);
    } else {
#if (TCFG_MIC_KEY_USED_TYPE == MIC_KEY_USED_VCMO)
        adc_api_set_channel_status(AD_CH_AUDIO_VCMO1, ADC_STATUS_NORMAL);
#endif
        adc_api_set_channel_status(AD_CH_AUDIO_VCMO0, ADC_STATUS_NORMAL);
    }
#endif
#else
    //设置成AD口
    gpio_set_direction(TCFG_MIC_KEY_IO, 1);
    gpio_set_die(TCFG_MIC_KEY_IO, 0);
    gpio_set_pull_down(TCFG_MIC_KEY_IO, 0);
    gpio_set_pull_up(TCFG_MIC_KEY_IO, 0);

    adc_api_set_channel_status(TCFG_MIC_KEY_AD_CH, ADC_STATUS_NORMAL);
    adc_api_set_channel_status(AD_CH_AUDIO_MICLDO, ADC_STATUS_NORMAL);
#endif
#if TCFG_EARPHONE_ONLINE_CHECK_EN
    detect_filter = 0;
#endif
    power_on_flag = 1;
}

void mic_key_enable(u8 en)
{
    mic_key_en = en;
}

void mic_key_set_vcmo(u8 vcmo)
{
    mic_key_vcmo = vcmo;
}

//检测按键是否只支持单击
u8 mic_key_check_support_double_click(u8 key_value)
{
    if (key_value == MIC_KEY_PP) {
        return 1;//可响应多击按键
    }
    return 0;//不响应多击
}

const key_interface_t key_mic_info = {
    .key_type = KEY_TYPE_MIC,
    .key_init = mic_key_init,
    .key_get_value = get_mickey_value,
};

#endif/*KEY_IO_EN*/
