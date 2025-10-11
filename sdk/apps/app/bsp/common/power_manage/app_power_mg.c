#include "power_interface.h"
#include "app_power_mg.h"
#include "app_config.h"
#include "adc_api.h"
#include "key.h"
#include "msg.h"
#include "clock.h"

#define LOG_TAG_CONST     NORM
#define LOG_TAG           "[app_per_mg]"
#include "log.h"

static u16 lvd_warning_voltage;  //低电检测电压，比lvd电压大300mV
static u16 vbat_voltage;

#if TCFG_CHARGE_ENABLE
#include "charge.h"
#define CHARGE_IN_DET     charge_get_lvcmp_det()
#else
#define CHARGE_IN_DET     0
#endif

static void lvd_warning_init(void)
{
    extern u32 get_lvd_vol(void);
    u32 lvd_voltage = get_lvd_vol();
    lvd_warning_voltage = lvd_voltage + 300;
    log_info("lvd_warning_voltage : %d\n", lvd_warning_voltage);
}

void app_power_init(void)
{
    low_power_warning_init();
    adc_add_sample_ch(AD_CH_PMU_VBAT);
    app_power_scan();
}

u16 app_power_get_vbat(void)
{
    return vbat_voltage;
}

void app_power_scan(void)
{
    static u16 low_power_cnt = 0;
    u16 vol = adc_get_voltage(AD_CH_PMU_VBAT) * 4;
    if (0 == vol) {
        return;
    }

    /* log_info("vbat voltage : %d", vol); */
    if ((vol <= LOW_POWER_VOL) && (0 == CHARGE_IN_DET)) {
        //充电时不进入低功耗
        low_power_cnt++;
        if (low_power_cnt == (TCFG_SHUTDOWN_TIME / 500)) {
            log_error(LOW_POWER_LOG);
            post_msg(1, MSG_POWER_OFF);
        }
    } else {
        low_power_cnt = 0;
    }

    vbat_voltage = vol;
}

