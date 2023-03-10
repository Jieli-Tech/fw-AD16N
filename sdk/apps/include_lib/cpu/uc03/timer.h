#ifndef  __TIMER_H__
#define  __TIMER_H__

enum {
    TIMER_SRC_LSB = 1,
    TIMER_SRC_RTC_OSC,
    TIMER_SRC_LRC_CLK,
    TIMER_SRC_RC_16M,
    TIMER_SRC_STD_48M,
    TIMER_SRC_STD_24M,
    TIMER_SRC_STD_12M,
    TIMER_SRC_XOSC_12M,
    TIMER_SRC_PAT_CLK = 14,
    TIMER_SRC_TMRX_CIN,
};

enum {
    TIMER_PRESCALE_1 = 0,
    TIMER_PRESCALE_4,
    TIMER_PRESCALE_16,
    TIMER_PRESCALE_64,
    TIMER_PRESCALE_2,
    TIMER_PRESCALE_8,
    TIMER_PRESCALE_32,
    TIMER_PRESCALE_128,
    TIMER_PRESCALE_256,
    TIMER_PRESCALE_1024,
    TIMER_PRESCALE_4096,
    TIMER_PRESCALE_16384,
    TIMER_PRESCALE_512,
    TIMER_PRESCALE_2048,
    TIMER_PRESCALE_8192,
    TIMER_PRESCALE_32768,
};


#endif  /*TIMER_H*/
