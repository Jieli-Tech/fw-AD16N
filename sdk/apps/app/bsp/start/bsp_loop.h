#ifndef __BSP_LOOP__
#define __BSP_LOOP__


#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "app_config.h"
typedef enum {
    B_EVENT_100MS = 0,

    B_NO_EVENT = 32,
} BSP_EVENT;

void bsp_event_init(void);
int bsp_post_event(BSP_EVENT be);
int bsp_loop(void);

#endif

