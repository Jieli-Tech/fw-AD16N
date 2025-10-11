#ifndef __P33_ACCESS_H__
#define __P33_ACCESS_H__

//
//
//					for p33_operation
//
//
//
/***************************************************************/

#include "p33_interface.h"

void P33_CON_SET(u16 addr, u8 start, u8 len, u8 data);

#define P33_CON_GET(sfr)    (p33_rx_1byte(sfr))

#if 1

#define p33_fast_access(reg, data, en)           \
{ 												 \
    if (en) {                                    \
		p33_or_1byte(reg, (data));               \
    } else {                                     \
		p33_and_1byte(reg, (u8)~(data));             \
    }                                            \
}

#else

#define p33_fast_access(reg, data, en)         \
{                                              \
	if (en) {                                  \
       	reg |= (data);                         \
	} else {                                   \
		reg &= ~(data);                        \
    }                                          \
}

#endif


#endif
