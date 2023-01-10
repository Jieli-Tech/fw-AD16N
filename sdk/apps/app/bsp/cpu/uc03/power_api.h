/*********************************************************************************************
    *   Filename        : power_api.h

    *   Description     : 低功耗所用接口及流程

    *   Author          : Sunlicheng

    *   Email           : Sunlicheng@zh-jieli.com

    *   Last modifiled  : 2021-01-08 09:50

    *   Copyright:(c)JIELI  2021-2029  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef ___POWER_API_H___
#define ___POWER_API_H___

void sys_power_init();

void sys_power_down(u32 usec);

void sys_softoff();

#endif
