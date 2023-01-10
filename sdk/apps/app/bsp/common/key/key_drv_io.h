#ifndef __KEY_DRV_IO_H__
#define __KEY_DRV_IO_H__

#include "gpio.h"
#include "key.h"

#define IS_KEY0_DOWN()    	(!(JL_PORTB->IN & BIT(0)))
#define IS_KEY1_DOWN()    	(!(JL_PORTB->IN & BIT(1)))
#define IS_KEY2_DOWN()    	(!(JL_PORTB->IN & BIT(2)))
#define IS_KEY3_DOWN()    	(!(JL_PORTB->IN & BIT(8)))
#define IS_KEY4_DOWN()    	(!(JL_PORTB->IN & BIT(4)))
#define IS_KEY5_DOWN()      0
#define IS_KEY6_DOWN()      0
// #define IS_KEY5_DOWN()    	(!(JL_PORTB->IN & BIT(5)))
// #define IS_KEY6_DOWN()    	(!(JL_PORTB->IN & BIT(6)))
#define IS_KEY7_DOWN()    	(!(JL_PORTB->IN & BIT(7)))

#define KEY_INIT()        do{\
							/**key0 init*/\
							JL_PORTB->PU0 |= BIT(0),\
							JL_PORTB->PD0 &= ~BIT(0),\
							JL_PORTB->DIE |= BIT(0),\
							JL_PORTB->DIR |= BIT(0);\
							/**key1 init*/\
							JL_PORTB->PU0 |= BIT(1),\
							JL_PORTB->PD0 &= ~BIT(1),\
							JL_PORTB->DIE |= BIT(1),\
							JL_PORTB->DIR |= BIT(1);\
							/**key2 init*/\
							JL_PORTB->PU0 |= BIT(2),\
							JL_PORTB->PD0 &= ~BIT(2),\
							JL_PORTB->DIE |= BIT(2),\
							JL_PORTB->DIR |= BIT(2);\
							/**key3 init*/\
							JL_PORTB->PU0 |= BIT(8),\
							JL_PORTB->PD0 &= ~BIT(8),\
							JL_PORTB->DIE |= BIT(8),\
							JL_PORTB->DIR |= BIT(8);\
							/**key4 init*/\
							JL_PORTB->PU0 |= BIT(4),\
							JL_PORTB->PD0 &= ~BIT(4),\
							JL_PORTB->DIE |= BIT(4),\
							JL_PORTB->DIR |= BIT(4);\
							/**key7 init*/\
							JL_PORTB->PU0 |= BIT(7),\
							JL_PORTB->PD0 &= ~BIT(7),\
							JL_PORTB->DIE |= BIT(7),\
							JL_PORTB->DIR |= BIT(7);\
					    	}while(0)

extern const key_interface_t key_io_info;
void io_key_init(void);
u8 get_iokey_value(void);

#endif/*__KEY_DRV_IO_H__*/
