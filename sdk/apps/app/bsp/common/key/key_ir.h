#ifndef _KEY_IR_H
#define _KEY_IR_H
#include "app_config.h"
#if KEY_IR_EN
#include "key.h"
#include "gpio.h"
#include "ir_decoder.h"

u8 irkey_get_value(void);
void ir_key_init(void);
extern const key_interface_t key_ir_info;
#endif
#endif


