#ifndef __SYS_TIMER_H_
#define __SYS_TIMER_H_

#include "typedef.h"
#include "list.h"
#include "jiffies.h"

extern volatile u32 jiffies_2ms;
#define SYS_TIMER_JIFFIES jiffies_2ms
// #define SYS_TIMER_JIFFIES jiffies

#define SYS_TIMER_JIFFIES_UNIT   2   //系统定时任务计时单位ms
#define sys_timer_msecs_to_jiffies(msec)		((msec)/SYS_TIMER_JIFFIES_UNIT)

struct sys_timer {
    struct list_head entry;
    void (*func)(void *priv);       /**< 定时结束后执行的处理函数 */
    void *priv;                     /**< 定时结束后执行的处理函数的输入参数 */
    // const char *owner;              #<{(|*< 注册当前定时任务的线程 |)}>#
    u32 jiffies;                    /**< 定时任务的未来执行时间（当前系统时间 + 定时时间），单位：ms */
    u32 msec: 24;                   /**< 定时时间，单位：ms */
    u32 del: 1;                     /**< 定时任务是否执行删除标志，置1后在下次任务调度会删除对应任务 */
    u32 timeout: 1;                 /**< 定时任务是否为timeout类型，置1为timeout类型，定时任务执行一次后将自动删除 */
    u16 id;                         /**< 定时任务在任务链表中被分配的序号，一般用于定时任务的手动删除或定时时间修改 */
    // u8 used;                        #<{(|*< 定时任务池中任务位置是否被占用, 当所有任务位置都被占用时需要通过malloc申请新的任务存放空间。|)}>#
};

void sys_timer_init(void);
void *get_system_timer_head();
u16 sys_timer_add(void *priv, void (*func)(void *priv), u32 msec);
u16 sys_timeout_add(void *priv, void (*func)(void *priv), u32 msec);
void sys_timer_del(u16 id);
void sys_timeout_del(u16 id);
int sys_timer_modify(u16 id, u32 msec);
void timer_task_scan();

void system_timer_remove(struct sys_timer *timer);
void system_timer_register(struct sys_timer *timer, u32 msec, void (*fun)(struct sys_timer *timer), u8 delay_do);
#endif

