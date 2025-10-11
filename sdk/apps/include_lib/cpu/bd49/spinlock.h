#ifndef SYS_SPINLOCK_H
#define SYS_SPINLOCK_H

#include "typedef.h"
#include "cpu.h"
#include "irq.h"


struct __spinlock {
    volatile u8 rwlock;
    volatile u8 lock_cnt[1];
};

typedef struct __spinlock spinlock_t;


#define preempt_disable() \
	local_irq_disable()

#define preempt_enable() \
	local_irq_enable()




#define spin_acquire(lock) 	\
	do { \
	}while(0)


#define spin_release(lock) \
	do { \
	}while(0)




#define DEFINE_SPINLOCK(x) \
	spinlock_t x = { .rwlock = 0 }

void spin_lock_init(spinlock_t *lock);
void spin_lock(spinlock_t *lock);
void spin_unlock(spinlock_t *lock);

#if 0

#define spin_lock(lock) \
    do { \
        preempt_disable(); \
        if (!(T2_CON & (1<<0))) { \
            T2_CNT = 0; \
            T2_PRD = 120000000 / 10; \
            T2_CON = 1; \
        } \
        spin_lock_cnt[current_cpu_id()] = T2_CNT; \
        spin_acquire(lock); \
    } while (0)


#define spin_unlock(lock) \
    do { \
        u32 t = T2_CNT;\
        if(t < spin_lock_cnt[current_cpu_id()]) \
            t += T2_PRD - spin_lock_cnt[current_cpu_id()]; \
        else \
            t -= spin_lock_cnt[current_cpu_id()]; \
        spin_release(lock); \
        preempt_enable(); \
        if (t > 100000) { /*120000 == 1ms*/ \
            printf("???????spinlock: %d, %s\n", t, __func__); \
        } \
    } while(0)

#endif

/*#define spin_lock_irqsave(lock, flags) \
	do { \
		local_irq_save(flags); \
		spin_acquire((lock)); \
	}while(0)

#define spin_unlock_irqrestore(lock, flags) \
	do { \
		spin_release((lock)); \
		local_irq_restore(flags); \
	}while(0) */











#endif


