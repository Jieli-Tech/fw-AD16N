
#ifndef ASM_CPU_H
#define ASM_CPU_H


#include "bd49.h"
#include "typedef.h"
#include "csfr.h"
#include <stdarg.h>

#ifndef __ASSEMBLY__
#define _G_va_list __gnuc_va_list
typedef _G_va_list va_list;

typedef unsigned char   		u8, bool, BOOL, uint8_t;
typedef char            		s8;
typedef signed char             int8_t;
typedef unsigned short  		u16;
typedef signed short    		s16;
typedef unsigned int    		u32;
typedef signed int      		s32;
typedef unsigned long long 		u64;
typedef unsigned long long int	uint64_t;
typedef u32						FOURCC;
typedef long long               s64;
typedef unsigned long long      u64;


#endif



#ifndef BIG_ENDIAN
#define BIG_ENDIAN 			0x3021
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 		0x4576
#endif
#define CPU_ENDIAN 			LITTLE_ENDIAN

#define CPU_CORE_NUM     1


#ifndef __ASSEMBLY__

#if CPU_CORE_NUM > 1
static inline int current_cpu_id()
{
    unsigned id;
    asm volatile("%0 = cnum" : "=r"(id) ::);
    return id ;
}
#else
static inline int current_cpu_id()
{
    return 0;
}
#endif

static inline int cpu_in_irq()
{
    int flag;
    __asm__ volatile("%0 = icfg" : "=r"(flag));
    return flag & 0xff;
}

static inline int cpu_irq_disabled()
{
    int flag;
    __asm__ volatile("%0 = icfg" : "=r"(flag));
    return (flag & 0x300) != 0x300;
}



static inline u32 rand32()
{
    return JL_RAND->R64L;
}


#include "asm/power/power_reset.h"

/* static inline void cpu_reset(void) */
/* { */
/* void system_reset(enum RESET_FLAG flag); */
/* system_reset(RESET_FLAG_NULL); */
/* } */


#include "irq.h"

#ifndef EINVAL
#define EINVAL      22  /* Invalid argument */
#endif



extern void __local_irq_disable();
extern void __local_irq_enable();
extern int printf(const char *format, ...);

#define local_irq_disable __local_irq_disable
#define local_irq_enable  __local_irq_enable

#define	CPU_SR_ALLOC() 	\

#define __asm_csync() \
    do { \
		asm volatile("csync;"); \
    } while (0)



#define CPU_CRITICAL_ENTER()  local_irq_disable()

#define CPU_CRITICAL_EXIT()  local_irq_enable()

extern const u8 config_asser;
#define ASSERT(a,...)   \
		do { \
            if(!(a)){ \
                if(config_asser){\
                    if(!(a)){ \
                        printf("file:%s, line:%d", __FILE__, __LINE__); \
                        printf("ASSERT-FAILD: "#a" "__VA_ARGS__); \
                        system_reset(ASSERT_FLAG); \
                    } \
                }else{\
                    if(!(a)){ \
                        system_reset(ASSERT_FLAG); \
                    }\
                }\
            } \
		}while(0);

#define arch_atomic_read(v)  \
	({ \
        __asm_csync(); \
		(*(volatile int *)&(v)->counter); \
	 })

#endif //__ASSEMBLY__


#endif

