#ifndef _APP_POWER_MG_H
#define _APP_POWER_MG_H
#include "typedef.h"

#define LVD_WARNING_FOR_LOW_POWER   1//1:LVD作为低电警戒线 0:固定值作为警戒线

#if LVD_WARNING_FOR_LOW_POWER
#define LOW_POWER_VOL               lvd_warning_voltage
#define LOW_POWER_LOG               "Waring!!! Vbat is near to lvd!\n"
#define low_power_warning_init      lvd_warning_init
#else
#define LOW_POWER_VOL               3300//3.3V
#define LOW_POWER_LOG               "low power\n"
#define low_power_warning_init
#endif

void app_power_init(void);
void app_power_scan(void);
u16 app_power_get_vbat(void);

#endif
