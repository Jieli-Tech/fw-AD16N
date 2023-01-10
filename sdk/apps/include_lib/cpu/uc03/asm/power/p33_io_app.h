#ifndef __P33_IO_APP_H__
#define __P33_IO_APP_H__

struct port_wakeup {
    u8 pullup_down_enable;    //上下拉是否使能
    u8 edge;       			  //唤醒边沿条件
    u8 filter;				  //滤波参数，数字io输入没有滤波可配制
    u8 iomap;      			  //唤醒io
};

#define MAX_WAKEUP_PORT     3  //最大同时支持数字io输入个数
#define MAX_WAKEUP_ANA_PORT 3   //最大同时支持模拟io输入个数

typedef enum {
    RISING_EDGE = 0,
    FALLING_EDGE,
    BOTH_EDGE,
} POWER_WKUP_EDGE;

enum {
    P3_WKUP_SRC_PCNT_OVF = 0,
    P3_WKUP_SRC_PORT_EDGE,
    P3_WKUP_SRC_ANA_EDGE,
    P3_WKUP_SRC_VDDIO_LVD = 4,
};

enum {
    R3_WKUP_SRC_ALM = 0,
    R3_WKUP_SRC_FLAG,
    R3_WKUP_SRC_PORT = 3,
    R3_WKUP_SRC_256HZ,
    R3_WKUP_SRC_64HZ,
    R3_WKUP_SRC_2HZ,
    R3_WKUP_SRC_1HZ,
};

typedef enum {
    PORT_FLT_NULL = 0,
    PORT_FLT_256us,
    PORT_FLT_512us,
    PORT_FLT_1ms,
    PORT_FLT_2ms,
    PORT_FLT_4ms,
    PORT_FLT_8ms,
    PORT_FLT_16ms,
} POWER_WKUP_FLT;

#define PCNT_PND_CLR()		    	P33_CON_SET(P3_PCNT_CON, 6, 1, 1)

//en
#define P33_SET_WKUP_EN(data)		P33_CON_SET(P3_WKUP_EN0, 0, 8, data & 0xff); \
									P33_CON_SET(P3_WKUP_EN1, 0, 8, (data >> 8) & 0xff)

#define P33_OR_WKUP_EN(data)		p33_fast_access(P3_WKUP_EN0, data & 0xff, 1); \
									p33_fast_access(P3_WKUP_EN1, (data >> 8) & 0xff, 1)

#define P33_AND_WKUP_EN(data)		p33_fast_access(P3_WKUP_EN0, data & 0xff, 0); \
									p33_fast_access(P3_WKUP_EN1, (data >> 8) & 0xff, 0)

//edge
#define P33_SET_WKUP_EDGE(data)		P33_CON_SET(P3_WKUP_EDGE0, 0, 8, data & 0xff); \
									P33_CON_SET(P3_WKUP_EDGE1, 0, 8, (data >> 8) & 0xff)

#define P33_OR_WKUP_EDGE(data)		p33_fast_access(P3_WKUP_EDGE0, data & 0xff, 1); \
									p33_fast_access(P3_WKUP_EDGE1, (data >> 8), 1)

#define P33_AND_WKUP_EDGE(data)		p33_fast_access(P3_WKUP_EDGE0,  data & 0xff, 0); \
									p33_fast_access(P3_WKUP_EDGE1,  (data >> 8) & 0xff, 0)

//cpnd
#define P33_SET_WKUP_CPND(data)		p33_fast_access(P3_WKUP_CPND0, data & 0xff, 1); \
									p33_fast_access(P3_WKUP_CPND1, (data >> 8) & 0xff, 1)

//pnd
#define P33_GET_WKUP_PND()			(P33_CON_GET(P3_WKUP_PND0) | (P33_CON_GET(P3_WKUP_PND1)<<8))

//akwup_en
#define P33_SET_AWKUP_EN(data)		P33_CON_SET(P3_AWKUP_EN, 0, 8, data & 0xff)

#define P33_OR_AWKUP_EN(data)		p33_fast_access(P3_AWKUP_EN, data & 0xff, 1)

//awkup_p_pnd
#define P33_GET_AWKUP_P_PND()		(P33_CON_GET(P3_AWKUP_P_PND))

//awkup_n_pnd
#define P33_GET_AWKUP_N_PND()		(P33_CON_GET(P3_AWKUP_N_PND))

//awkup_p_cpnd
#define P33_SET_AWKUP_P_CPND(data)	p33_fast_access(P3_AWKUP_P_CPND, data & 0xff, 1)

//awkup_n_cpnd
#define P33_SET_AWKUP_N_CPND(data)	p33_fast_access(P3_AWKUP_N_CPND, data & 0xff, 1)

//awkup_p_ie
#define P33_SET_AWKUP_P_IE(data)	P33_CON_SET(P3_AWKUP_P_IE, 0, 8, data & 0xff)
#define P33_OR_AWKUP_P_IE(data)		p33_fast_access(P3_AWKUP_P_IE, data & 0xff, 1)
#define P33_AND_AWKUP_P_IE(data)	p33_fast_access(P3_AWKUP_P_IE, data & 0xff, 0)

//awkup_n_ie
#define P33_SET_AWAKEUP_N_IE(data)	P33_CON_SET(P3_AWKUP_N_IE, 0, 8, data & 0xff)
#define P33_OR_AWKUP_N_IE(data)		p33_fast_access(P3_AWKUP_N_IE, data & 0xff, 1)
#define P33_AND_AWKUP_N_IE(data)	p33_fast_access(P3_AWKUP_N_IE, data & 0xff, 0)

#define CLEAN_GPIO_WAKEUP_PENDING() P33_SET_AWKUP_P_CPND(0xff); \
                                    P33_SET_AWKUP_N_CPND(0xff); \
                                    P33_SET_WKUP_CPND(0xffff);

#define CLEAN_P33_WKUP_PENDING()	P33_SET_AWKUP_P_CPND(0xff); \
    								P33_SET_AWKUP_N_CPND(0xff); \
									P33_SET_WKUP_CPND(0xffff);  \
									VLVD_PND_CLR();             \
									PCNT_PND_CLR()

#endif
