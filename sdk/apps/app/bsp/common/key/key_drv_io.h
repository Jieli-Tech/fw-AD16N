#ifndef __KEY_DRV_IO_H__
#define __KEY_DRV_IO_H__

#include "gpio.h"
#include "key.h"

#define IS_KEY0_DOWN()    	(!(JL_PORTA->IN & BIT(1)))
#define IS_KEY1_DOWN()    	(!(JL_PORTA->IN & BIT(2)))
#define IS_KEY2_DOWN()    	(!(JL_PORTA->IN & BIT(11)))
#define IS_KEY3_DOWN()    	(!(JL_PORTA->IN & BIT(15)))
// #define IS_KEY4_DOWN()    	(!(JL_PORTB->IN & BIT(4)))
#define IS_KEY4_DOWN()      0
#define IS_KEY5_DOWN()      0
#define IS_KEY6_DOWN()      0
#define IS_KEY7_DOWN()      0
// #define IS_KEY5_DOWN()    	(!(JL_PORTB->IN & BIT(5)))
// #define IS_KEY6_DOWN()    	(!(JL_PORTB->IN & BIT(6)))
// #define IS_KEY7_DOWN()    	(!(JL_PORTB->IN & BIT(7)))

#define KEY_INIT()        do{\
							/**key0 init*/\
							JL_PORTA->PU0 |= BIT(1),\
							JL_PORTA->PD0 &= ~BIT(1),\
							JL_PORTA->DIE |= BIT(1),\
							JL_PORTA->DIR |= BIT(1);\
							/**key1 init*/\
							JL_PORTA->PU0 |= BIT(2),\
							JL_PORTA->PD0 &= ~BIT(2),\
							JL_PORTA->DIE |= BIT(2),\
							JL_PORTA->DIR |= BIT(2);\
							/**key2 init*/\
							JL_PORTA->PU0 |= BIT(11),\
							JL_PORTA->PD0 &= ~BIT(11),\
							JL_PORTA->DIE |= BIT(11),\
							JL_PORTA->DIR |= BIT(11);\
							/**key3 init*/\
							JL_PORTA->PU0 |= BIT(15),\
							JL_PORTA->PD0 &= ~BIT(15),\
							JL_PORTA->DIE |= BIT(15),\
							JL_PORTA->DIR |= BIT(15);\
					    	}while(0)

extern const key_interface_t key_io_info;
void io_key_init(void);
u8 get_iokey_value(void);

#endif/*__KEY_DRV_IO_H__*/
