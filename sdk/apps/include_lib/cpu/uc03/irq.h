#ifndef __IRQ_API_H__
#define __IRQ_API_H__
#include "hwi.h"
#include "typedef.h"





void HWI_Uninstall(unsigned char index);

#define     CPU_INT_DIS local_irq_disable
#define     CPU_INT_EN  local_irq_enable

#define     OS_ENTER_CRITICAL   CPU_INT_DIS
#define     OS_EXIT_CRITICAL    CPU_INT_EN



void _OS_EXIT_CRITICAL(void);
// void _OS_ENTER_CRITICAL(u32 bit_list);
void _OS_ENTER_CRITICAL(u32 bit_list_l, u32 bit_list_h);
// void irq_index_tab_reg(void *tab, u32 max_cnt);
// u8 irq_index_to_prio(u8 idx);

#endif	/*	_AD100_H	*/

