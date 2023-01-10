#include "asm/power/p33_app.h"
#include "asm/power/p33.h"
#include "app_power_mg.h"
#include "saradc.h"
#include "key.h"
#include "msg.h"
#include "clock.h"
#include "charge.h"

#define LOG_TAG_CONST     NORM
#define LOG_TAG           "[normal]"
#include "log.h"

static u16 lvd_warning_voltage;  //低电检测电压，比lvd电压大300mV
static u16 vbat_voltage;

static void lvd_warning_init(void)
{
    u16 lvd_voltage = 2100 + 100 * ((P3_VLVD_CON >> 3) & 0x7);
    lvd_warning_voltage = lvd_voltage + 300;
    /* log_info("lvd_warning_voltage : %d\n", lvd_warning_voltage); */
}

void app_power_init(void)
{
    low_power_warning_init();
    saradc_add_sample_ch(ADC_CH_PMU_1_4_VBAT);
    app_power_scan();
}

u16 app_power_get_vbat(void)
{
    return vbat_voltage;
}

void app_power_scan(void)
{
    static u16 low_power_cnt = 0;
    u16 vol = saradc_get_vbat_voltage();
    if (0 == vol) {
        return;
    }

    /* log_info("vbat voltage : %d", vol); */
    if ((vol <= LOW_POWER_VOL) && (0 == charge_get_lvcmp_det())) {
        //充电时不进入低功耗
        low_power_cnt++;
        if (low_power_cnt == 10) {
            log_error(LOW_POWER_LOG);
            post_msg(1, MSG_POWER_OFF);
        }
    } else {
        low_power_cnt = 0;
    }

    vbat_voltage = vol;
}

