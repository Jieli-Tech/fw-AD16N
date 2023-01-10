/*********************************************************************************************
    *   Filename        : typedef.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 10:53

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include "asm_type.h"
#include "cpu.h"
// #include "sfr.h"
#include "string.h"


#define _WEAK_	        __attribute__((weak))
#define _WEAKREF_	    __attribute__((weakref))
#ifdef __GNUC__
#define sec_used(x)     __attribute__((section(#x),used))
#define SEC_USED(x)     __attribute__((section(#x),used))
#define sec(x)          __attribute__((section(#x)))
#define SEC(x)          __attribute__((section(#x)))
#define AT(x)           __attribute__((section(#x)))
#define SET(x)          __attribute__((x))
#define _GNU_PACKED_	__attribute__((packed))
#define _INLINE_        __attribute__((always_inline))
#define _NOINLINE_        __attribute__((noinline))
#define ALIGNED(x)      __attribute__((aligned(x)))
#define __weak          __attribute__((weak))
#define QLZ(x)         /* __attribute__((section(#x)))*/
//#define SET_INTERRUPT   __attribute__((interrupt("SWITCH")))
#define SET_INTERRUPT   __attribute__((interrupt("")))
#define ___interrupt    SET(interrupt(""))
#else
#define sec_used(x)
#define sec(x)
#define AT(x)
#define SET(x)
#define _GNU_PACKED_
#endif

//#define     LITTLE_ENDIAN
//
//#ifdef LITTLE_ENDIAN
//#define ntohl(x) (u32)((x>>24)|((x>>8)&0xff00)|(x<<24)|((x&0xff00)<<8))
//#define ntoh(x) (u16)((x>>8&0x00ff)|x<<8&0xff00)
//#define NTOH(x) (x) = ntoh(x)
//#define NTOHL(x) (x) = ntohl(x)
//#define LD_WORD(ptr)        (u16)(*(u16*)(u8*)(ptr))
//#define LD_DWORD(ptr)        (u32)(*(u32*)(u8*)(ptr))
//#define ST_WORD(ptr,val)    *(u16*)(u8*)(ptr)=(u16)(val)
//#define ST_DWORD(ptr,val)    *(u32*)(u8*)(ptr)=(u32)(val)
//#else
//#define ntohl(x) (x)
//#define ntoh(x) (x)
//#define NTOH(x) (x) = ntoh(x)
//#define NTOHL(x) (x) = ntohl(x)
//#endif

// section definitions
// #define AT_POWER        	AT(.poweroff_text)
// #define AT_AUDIO        	AT(.audio_text)
//
#define AT_RAM 				AT(.common.text.cache.L1)
// #define AT_VM 				AT(.vm)
#define AT_SPI_CODE         AT(.spi_code)
#define AT_SPI_CODE_RAM     AT_RAM
// #define AT_ADC_CODE         AT(.adc_code)
// #define AT_ENC_ISR_CODE     AT(.enc_isr_code)
// #define AT_WFIL_ISR_CODE    AT(.efil_isr_code)
//#define AT_POWER_DOWN		AT(.powerdown_text)
//#define AT_POWER_FLASH		AT(.poweroff_flash)
//#define AT_WPC              AT(.wpc_code)
// #define AT_SPI_CODE         AT(.vm_sfc.text.cache.L2)

#define FALSE    0
#define TRUE    1
#define false    0
#define true    1

#ifndef NULL
#define NULL    0
#endif // NULL

#define _banked_func
#define _xdata
// #define _data
#define _root
#define _no_init
#define my_memset memset
#define my_memcpy memcpy
#define _static_  static

#define     BIT(n)              (1UL << (n))
#define     BitSET(REG,POS)     ((REG) |= (1L << (POS)))
#define     BitCLR(REG,POS)     ((REG) &= (~(1L<< (POS))))
#define     BitXOR(REG,POS)     ((REG) ^= (~(1L << (POS))))
#define     BitCHK_1(REG,POS)   (((REG) & (1L << (POS))) == (1L << (POS)))
#define     BitCHK_0(REG,POS)   (((REG) & (1L << (POS))) == 0x00)
#define     testBit(REG,POS)    ((REG) & (1L << (POS)))

#define     clrBit(x,y)         (x) &= ~(1L << (y))
#define     setBit(x,y)         (x) |= (1L << (y))


#define readb(addr)   *((unsigned char*)(addr))
#define readw(addr)   *((unsigned short *)(addr))
#define readl(addr)   *((unsigned long*)(addr))

#define writeb(addr, val)  *((unsigned char*)(addr)) = (val)
#define writew(addr, val)  *((unsigned short *)(addr)) = (u16)(val)
#define writel(addr, val)  *((unsigned long*)(addr)) = (val)

// #define READ_BT_8( buffer, pos) ( ((u8) buffer[pos]))
// #define READ_BT_16( buffer, pos) ( ((u16) buffer[pos]) | (((u16)buffer[pos+1]) << 8))
// #define READ_BT_24( buffer, pos) ( ((u32) buffer[pos]) | (((u32)buffer[pos+1]) << 8) | (((u32)buffer[pos+2]) << 16))
// #define READ_BT_32( buffer, pos) ( ((u32) buffer[pos]) | (((u32)buffer[pos+1]) << 8) | (((u32)buffer[pos+2]) << 16) | (((u32) buffer[pos+3])) << 24)

// #define READ_NET_16( buffer, pos) ( ((u16) buffer[pos+1]) | (((u16)buffer[pos  ]) << 8))
// #define READ_NET_32( buffer, pos) ( ((u32) buffer[pos+3]) | (((u32)buffer[pos+2]) << 8) | (((u32)buffer[pos+1]) << 16) | (((u32) buffer[pos])) << 24)


#define ALIGN_4BYTE(size)   ((size+3)&0xfffffffc)


#define CPU_BIG_EADIN

#if defined(CPU_BIG_EADIN)
#define __cpu_u16(lo, hi)  ((lo)|((hi)<<8))

#elif defined(CPU_LITTLE_EADIN)
#define __cpu_u16(lo, hi)  ((hi)|((lo)<<8))
#else
#error "undefine cpu eadin"
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define ARRAY_SIZE(array)  (sizeof(array)/sizeof(array[0]))
#define ARRAY_AND_SIZE(array)   array, (sizeof(array)/sizeof(array[0]))

#define SIZEOF_ALIN(var, al)  (((((u32)var)+(al)-1)/(al))*(al))

extern int xprintf(const char *format, ...);   //fix warning
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#include "cpu.h"
#define SFR(sfr, start, len, dat) (sfr = (sfr & ~((~(0xffffffff << (len))) << (start))) | (((dat) & (~(0xffffffff << (len)))) << (start)))

enum {
    RUN_FLASH_MODE,
    RUN_OTP_MODE,
};


#define PA_H_L(x) do{JL_PORTA->DIR &= ~BIT(x);JL_PORTA->OUT ^= BIT(x);}while(0);
#define PA_H(x)   do{JL_PORTA->DIR &= ~BIT(x);JL_PORTA->OUT |= BIT(x);}while(0);
#define PA_L(x)   do{JL_PORTA->DIR &= ~BIT(x);JL_PORTA->OUT &= ~BIT(x);}while(0);

typedef u32 UINT;

#endif



