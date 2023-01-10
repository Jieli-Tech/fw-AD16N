#ifndef __TAB_READ_H__
#define __TAB_READ_H__

#include "typedef.h"

typedef struct _rtab_obj {
    void  *tab;
    u32   size;
    u32   offset;
    u32   cnt;
} rtab_obj;

void tab_init(rtab_obj *stab, void *tab, u32 size);
u32 tab_read(void *buff, rtab_obj *stab, u32 len);


#endif


