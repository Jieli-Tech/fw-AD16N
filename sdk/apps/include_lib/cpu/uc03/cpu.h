
#ifndef ASM_CPU_H
#define ASM_CPU_H


#include "uc03.h"
#include "typedef.h"
#include "csfr.h"

#ifndef __ASSEMBLY__

typedef unsigned char   		u8;
typedef char            		s8;
typedef unsigned short  		u16;
typedef signed short    		s16;
typedef unsigned int    		u32;
typedef signed int      		s32;
typedef unsigned long long 		u64;
typedef u32						FOURCC;
typedef long long               s64;
typedef unsigned long long      u64;


#endif


#define ___trig        (JL_FPGA->TRI = 1)//__asm__ volatile ("trigger")


#ifndef BIG_ENDIAN
#define BIG_ENDIAN 			0x3021
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 		0x4576
#endif
#define CPU_ENDIAN 			LITTLE_ENDIAN


#define CPU_CORE_NUM     1
#define q32DSP_num       CPU_CORE_NUM

#define  CPU_TASK_CLR(a)
#define  CPU_TASK_SW(a) 		\
    do { \
        extern void wdt_clear(); \
        q32DSP(0)->ILAT_SET |= BIT(7-a); \
        wdt_clear(); \
    } while (0)


#define  CPU_INT_NESTING 	2


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

static inline u32 rand32_2()
{
    return JL_RAND->R64H;
}

#endif




#ifndef __ASSEMBLY__ //assembly

#define __asm_csync() \
    do { \
		asm volatile("csync;"); \
    } while (0)

#include "irq.h"


#define arch_atomic_read(v)  \
	({ \
        __asm_csync(); \
		(*(volatile int *)&(v)->counter); \
	 })


void local_irq_enable();
void local_irq_disable();



#define arch_spin_trylock(lock) \
	do { \
        __asm_csync(); \
		while ((lock)->rwlock); \
		(lock)->rwlock = 1; \
	}while(0)

#define arch_spin_lock(lock) \
	do { \
        int ret = false; \
        __asm_csync(); \
		if (!(lock)->rwlock) { \
            ret = true; \
		    (lock)->rwlock = 1; \
        } \
        if (ret) \
            break; \
	}while(1)

#define arch_spin_unlock(lock) \
	do { \
		(lock)->rwlock = 0; \
	}while(0)




#define	CPU_SR_ALLOC() 	\
//	int flags

#define CPU_CRITICAL_ENTER()  \
	do { \
		local_irq_disable(); \
	}while(0)


#define CPU_CRITICAL_EXIT() \
	do { \
		local_irq_enable(); \
	}while(0)


/* static inline void cpu_reset(void) */
/* {                                  */
/*     extern void log_flush();       */

/*     log_flush();                   */
/* //    PWR_CON |= (1 << 4);         */
/*     while (1);                     */
/* }                                  */


#define ASSERT(a,...)   \
		do { \
			if(!(a)){ \
				log_error("ASSERT: "#a" "__VA_ARGS__); \
			} \
		}while(0);

#endif

char chip_versions(void);

#endif


