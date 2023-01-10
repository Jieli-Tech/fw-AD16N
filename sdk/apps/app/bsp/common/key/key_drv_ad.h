#ifndef __KEY_DRV_AD_H__
#define __KEY_DRV_AD_H__
#include "key.h"
#include "gpio.h"
#include "config.h"
#include "clock.h"
#include "app_config.h"
#include "saradc.h"

#define ADC_CH_NONE         0Xff
#define ADC_VALUE_NONE      0XFFFF
#define ADC_MAX_CLK         1000000L

extern const key_interface_t key_ad_info;
void ad_key_init(void);
u8 get_adkey_value(void);
#endif
