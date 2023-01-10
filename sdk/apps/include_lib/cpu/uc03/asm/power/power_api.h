/*********************************************************************************************
    *   Filename        : power_hw.c

    *   Description     :

    *   Author          : Sunlicheng

    *   Email           : Sunlicheng@zh-jieli.com

    *   Last modifiled  : 2022-01-07 11:30

    *   Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __POWER_API_H__
#define __POWER_API_H__

#define AT_VOLATILE_RAM_CODE       SEC(.power.text.cache.L1)

extern const u8 config_otp_enable;
extern const u8 config_flash_enable;
extern const u8 config_soff_enable;
extern const u8 config_pdown_enable;
extern const u8 config_tmr_enable;
extern const u8 config_record_wkup;
extern const u8 config_record_reset;
extern const u8 config_max_wakeup_port;

#define SLEEP_EN                            BIT(1)

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
};

struct soft_flag0_t {
    u8 lvd_en: 1;
    u8 sfc_fast_boot: 1;
    u8 flash_boot: 1;
    u8 only_boot_port_a: 1;
    u8 sfpg_keep: 1;
    u8 do_flash_reset: 1;
    u8 flash_stable_delay_sel: 2;
};

struct soft_flag1_t {
    u8 usbdp: 2;
    u8 usbdm: 2;
    u8 ldoin: 3;
    u8 uart_key_port: 1;
};

struct boot_soft_flag_t {
    union {
        struct soft_flag0_t boot_ctrl;
        u8 value;
    } flag0;
    union {
        struct soft_flag1_t io_keep;
        u8 value;
    } flag1;
};

enum {
    OSC_TYPE_LRC = 0,
    OSC_TYPE_RTC,
    OSC_TYPE_BT_OSC,
};

struct low_power_param {
    u8 config;
    u8 osc_type;

    //=================
    u8 rtc_clk;
    u32 btosc_hz;
    u32 osc_delay_us;
    u8 vddiow_lev;
    u8 vddiom_lev;
    u8 flash_pg_keep;
    u8 vddio_keep;
    u8 vdc13_keep;
    u8 vdc13_cap_en;
};

typedef u8(*idle_handler_t)(void);

struct lp_target {
    char *name;
    idle_handler_t is_idle;
};

#define REGISTER_LP_TARGET(target) \
        const struct lp_target target sec_used(.lp_target)

extern const struct lp_target lp_target_begin[];
extern const struct lp_target lp_target_end[];

#define list_for_each_lp_target(p) \
    for (p = lp_target_begin; p < lp_target_end; p++)

struct low_power_operation {

    const char *name;

    u32(*get_timeout)(void *priv);

    void (*suspend_probe)(void *priv);

    void (*suspend_post)(void *priv, u32 usec);

    void (*resume)(void *priv, u32 usec);

    void (*resume_post)(void *priv, u32 usec);

    void (*off_probe)(void *priv);

    void (*off_post)(void *priv, u32 usec);

    void (*on)(void *priv);
};

void P33_ANA_DUMP();

void p33_init();

void power_set_callback(u8 mode, void (*powerdown_enter)(u8 step), void (*powerdown_exit)(u32), void (*soft_poweroff_enter)(void));

void power_init(const struct low_power_param *param);

u8 low_power_sys_request(u32 sleep_t);

void *low_power_sys_get(void *priv, const struct low_power_operation *ops);

void *low_power_get(void *priv, const struct low_power_operation *ops);

void low_power_put(void *priv);

void low_power_sys_put(void *priv);

void power_set_soft_poweroff(void);

void lvd_reset_en();

void lvd_wkup_en();

bool is_flash_pg_keep();

#endif
