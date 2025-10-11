//*********************************************************************************//
// Module name : pmu_flag.h                                                            //
// Description : soft flag and efuse head file                                     //
// By Designer : longshusheng                                                      //
// Dat changed :                                                                   //
//*********************************************************************************//

#ifndef __PMU_FLAG__
#define __PMU_FLAG__


enum soft_flag_io_stage {
    SOFTFLAG_HIGH_RESISTANCE,
    SOFTFLAG_PU,
    SOFTFLAG_PD,

    SOFTFLAG_OUT0,
    SOFTFLAG_OUT0_HD0,
    SOFTFLAG_OUT0_HD,
    SOFTFLAG_OUT0_HD0_HD,

    SOFTFLAG_OUT1,
    SOFTFLAG_OUT1_HD0,
    SOFTFLAG_OUT1_HD,
    SOFTFLAG_OUT1_HD0_HD,

    SOFTFLAG_PU100K,
    SOFTFLAG_PU1M,
    SOFTFLAG_PD100K,
    SOFTFLAG_PD1M,
};

//p33 soft flag
struct soft_flag0_t {
    u8 wdt_dis: 1;
    u8 lvd_en: 1;
    u8 sfc_fast_boot: 1;
    u8 flash_power_keep: 1;
    u8 skip_flash_reset: 1;
    u8 sfc_flash_stable_delay_sel: 1;   //0: 0.5mS; 1: 1mS
    u8 flash_stable_delay_sel: 2;       //0: 0mS;  1: 2mS;  2:4mS; 3:6mS
};

struct soft_flag1_t {
    u8 usbdp: 4;
    u8 usbdm: 4;
};

struct soft_flag2_t {
    u8 pa0: 4;
    u8 disable_uart_upgrade: 1;
    u8 uart_key_port_pull_down: 1;
    u8 flash_spi_baud: 2;
};

struct soft_flag3_t {
    u8 pa9: 4;
    u8 pa10: 4;
};

struct soft_flag7_4_t {
    u32 chip_key : 32;
};

struct boot_soft_flag_t {
    u8 soff_wkup;
    union {
        struct soft_flag0_t boot_ctrl;
        u8 value;
    } flag0;
    union {
        struct soft_flag1_t boot_ctrl;
        u8 value;
    } flag1;
    union {
        struct soft_flag2_t boot_ctrl;
        u8 value;
    } flag2;
    union {
        struct soft_flag3_t boot_ctrl;
        u8 value;
    } flag3;
    union {
        struct soft_flag7_4_t boot_ctrl;
        u32 value;
    } flag7_4;
};

//*********************************************************************************//
//                                                                                 //
//                               end of this module                                //
//                                                                                 //
//*********************************************************************************//
#endif
