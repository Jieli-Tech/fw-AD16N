/*********************************************************************************************
    *   Filename        : power_reset.c

    *   Description     : 复位模块

    *   Author          : Sunlicheng

    *   Email           : Sunlicheng@zh-jieli.com

    *   Last modifiled  : 2022-01-07 11:30

    *   Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __POWER_RESET_H__
#define __POWER_RESET_H__

enum RST_REASON {
    /*主系统*/
    MSYS_P33_RST,
    MSYS_DVDD_POR_RST,
    MSYS_SOFT_RST,
    MSYS_PLM_RST,
    MSYS_POWER_RETURN,

    /*P33*/
    P33_VDDIO_POR_RST,
    P33_VDDIO_LVD_RST,
    P33_WDT_RST,
    P33_VCM_RST,
    P33_PPINR_RST,
    P33_PPINR1_RST,
    P33_MSYS_RST,
    P33_SOFT_RST,
    P33_POWER_RETURN,
    P33_POR_RTC_RST,

    /*SUB*/
    P33_EXCEPTION_SOFT_RST = 20,	    //异常软件复位
    P33_ASSERT_SOFT_RST,				//断言软件复位
};

void power_reset_source_dump();

u8 is_reset_source(enum RST_REASON index);

#endif
