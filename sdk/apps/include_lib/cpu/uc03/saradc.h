#ifndef __SARADC_H__
#define __SARADC_H__
#include "typedef.h"

#ifdef D_IS_FLASH_SYSTEM
#include "saradc_isr.h"
#else
#include "saradc_simple.h"
#endif

#endif
