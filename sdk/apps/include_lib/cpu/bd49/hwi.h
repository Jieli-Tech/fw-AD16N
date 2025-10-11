#ifndef HWI_H
#define HWI_H
#include "irq_vec.h"


//系统使用到的
extern const int IRQ_IRTMR_IP;
extern const int IRQ_AUDIO_IP;
extern const int IRQ_DECODER_IP;
extern const int IRQ_WFILE_IP;
extern const int IRQ_RF_QUEUE_IP;
extern const int IRQ_ADC_IP;
extern const int IRQ_ENCODER_IP;
extern const int IRQ_TICKTMR_IP;
extern const int IRQ_USB_IP;
extern const int IRQ_SD_IP;
extern const int IRQ_CTMU_IP;
extern const int IRQ_PPM_IP;
extern const int IRQ_PPS_IP;
extern const int IRQ_STREAM_IP;
extern const int IRQ_LEDC_IP;
extern const int IRQ_SLCD_IP;
extern const int IRQ_PMU_TMR1_IP;
//系统还未使用到的
extern const int IRQ_UART0_IP;
extern const int IRQ_UART1_IP;
extern const int IRQ_ALINK0_IP;

extern u32 _IRQ_MEM_ADDR[];

#define IRQ_MEM_ADDR        (_IRQ_MEM_ADDR)

void bit_clr_swi(unsigned char index);
void bit_set_swi(unsigned char index);


void interrupt_init();

// void irq_save(void);
void irq_save(u32 b_index_l, u32 b_index_h);
void irq_resume(void);
void irq_enable(u8 index);
// void irq_list_enable(u32 b_index);
//void irq_list_enable(u32 b_index_l, u32 b_index_h);
// void HWI_Install(unsigned char index, unsigned int isr, unsigned char priority);

void request_irq(u8 index, u8 priority, void (*handler)(void), u8 chip_id);
void unrequest_irq(u8 index);
void request_irq(u8 index, u8 priority, void (*handler)(void), u8 cpu_id);
u16 sys_timeout_add(void *priv, void (*func)(void *priv), u32 msec);
void sys_timeout_del(u16 t);
u16 sys_timer_add(void *priv, void (*func)(void *priv), u32 msec);
void sys_timer_del(u16 t);
int sys_timer_modify(u16 id, u32 msec);


#define HWI_Install(idx,hdl,ip)    request_irq(idx, ip, (void *)hdl, 0);

void bit_clr_ie(unsigned char index);
void bit_set_ie(unsigned char index);

void irq_unmask_set(u8 index);
void irq_unmask_disable(u8 index);

#define irq_disable(x)  bit_clr_ie(x)
// #define irq_enable(x)   bit_set_ie(x)

#ifdef IRQ_TIME_COUNT_EN
void irq_handler_enter(int irq);

void irq_handler_exit(int irq);

void irq_handler_times_dump();
#else

void lp_waiting(int *ptr, int pnd, int cpd, char inum);

#define irq_handler_enter(irq)      do { }while(0)
#define irq_handler_exit(irq)       do { }while(0)
#define irq_handler_times_dump()    do { }while(0)

#endif


static inline int core_num(void)
{
    return 0;
}
#endif

