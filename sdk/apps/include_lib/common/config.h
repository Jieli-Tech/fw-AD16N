#ifndef _CONFIG_
#define _CONFIG_

#include "includes.h"
#include "common.h"
#include "errno-base.h"
#include "uart.h"



#if UART_DEBUG
#define  DEBUG_EN
#endif

extern const char libs_debug;

#ifdef DEBUG_EN
#define log_init(x)           uart_init(x)
#define log_uninit()          uart_uninit()
#define log_u8hex(x)          put_u8hex(x)
#else
#define log_init(...)
#define log_uninit()
#define log_u8hex(...)
// #define log_buf(...)
#endif
#define log(...)


//< huayue add
#define     IO_DEBUG_0(i,x)       {JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT &= ~BIT(x);}
#define     IO_DEBUG_1(i,x)       {JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT |= BIT(x);}
#define     IO_DEBUG_TOGGLE(i,x)  {JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT ^= BIT(x);}

#define set_ie              bit_set_ie
#define clear_ie            bit_clr_ie

#define TRIGGER()           __asm__ volatile ("trigger")


#ifdef D_IS_FLASH_SYSTEM
#define _OTP_CONST_
#else
#define _OTP_CONST_  const
#endif


#endif
