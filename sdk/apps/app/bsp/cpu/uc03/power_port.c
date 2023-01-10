#include "asm/power_interface.h"
#include "app_config.h"
#include "includes.h"
#include "gpio.h"
#include "efuse.h"
#include "usb/otg.h"
#include "usb/host/usb_host.h"

#define LOG_TAG_CONST       PMU
#define LOG_TAG             "[pmu]"
#include "log.h"

#pragma bss_seg(".power_api.data.bss")
#pragma data_seg(".power_api.data")
#pragma const_seg(".power_api.text.const")
#pragma code_seg(".power_api.text")
#pragma str_literal_override(".power_api.text.const")

static u8 gpiousb = 0x3;
static u32 usb_io_con ;
static u32 usb_con0;

/* cpu公共流程：
 * 请勿添加板级相关的流程，例如宏定义
 * 可以重写改流程
 * 所有io保持原状，除usb io
 */
void sleep_gpio_protect(u32 gpio)
{
    if (gpio == IO_PORT_DP) {
        gpiousb &= ~BIT(0);
    } else if (gpio == IO_PORT_DM) {
        gpiousb &= ~BIT(1);
    }
}

void sleep_enter_callback_common(void *priv)
{
    if (gpiousb) {
#if TCFG_UDISK_ENABLE
        if (usb_otg_online(0) == HOST_MODE) {
            usb_h_entry_suspend(0);
        } else
#endif
        {
            usb_io_con = JL_USB_IO->CON0;
            usb_con0 = JL_USB->CON0;
            JL_USB->CON0 = 0;
            usb_iomode(1);

            if (gpiousb & BIT(0)) {
                usb_port_clr(\
                             B_USB_DP_PU | \
                             B_USB_DP_PD | \
                             B_USB_DP_DIE | \
                             B_USB_DP_DIEH  \
                            );
                usb_port_set(\
                             B_USB_DP_DIR \
                            );
                /* gpio_set_pull_up(IO_PORT_DP, 0); */
                /* gpio_set_pull_down(IO_PORT_DP, 0); */
                /* gpio_set_direction(IO_PORT_DP, 1); */
                /* gpio_set_die(IO_PORT_DP, 0); */
                /* gpio_set_dieh(IO_PORT_DP, 0); */
            }

            if (gpiousb & BIT(1)) {
                usb_port_clr(\
                             B_USB_DM_PU | \
                             B_USB_DM_PD | \
                             B_USB_DM_DIE | \
                             B_USB_DM_DIEH  \
                            );
                usb_port_set(\
                             B_USB_DM_DIR \
                            );
                /* gpio_set_pull_up(IO_PORT_DM, 0); */
                /* gpio_set_pull_down(IO_PORT_DM, 0); */
                /* gpio_set_direction(IO_PORT_DM, 1); */
                /* gpio_set_die(IO_PORT_DM, 0); */
                /* gpio_set_dieh(IO_PORT_DM, 0); */
            }
        }
    }
}

void sleep_exit_callback_common(void *priv)
{
    gpiousb = 0x3;
#if TCFG_UDISK_ENABLE
    if (usb_otg_online(0) == HOST_MODE) {
        return;
    }
#endif
    JL_USB_IO->CON0 = usb_io_con;
    JL_USB->CON0 = usb_con0;
}

static struct gpio_value soff_gpio_config = {
    .gpioa = 0xffff,
    .gpiob = 0xffff,
    .gpioc = 0xffff,
    .gpiod = 0xffff,
    .gpiop = 0x1,//
    .gpiousb = 0x3,
};

void soff_gpio_protect(u32 gpio)
{
    if (gpio < IO_MAX_NUM) {
        port_protect((u16 *)&soff_gpio_config, gpio);
    } else if (gpio == IO_PORT_DP) {
        soff_gpio_config.gpiousb &= ~BIT(0);
    } else if (gpio == IO_PORT_DM) {
        soff_gpio_config.gpiousb &= ~BIT(1);
    }
}

/* cpu公共流程：
 * 请勿添加板级相关的流程，例如宏定义
 * 可以重写改流程
 * 释放除内置flash外的所有io
 */
//maskrom 使用到的io
void mask_io_cfg()
{
    struct boot_soft_flag_t soft_flag = {0};
    soft_flag.flag0.boot_ctrl.lvd_en = 1 ;

    if (config_flash_enable) {
        if (is_boot_otp() == 0) {
            soft_flag.flag0.boot_ctrl.sfc_fast_boot = 1 ;
            soft_flag.flag0.boot_ctrl.flash_boot = 1 ;
            soft_flag.flag0.boot_ctrl.only_boot_port_a = (get_sfc_port() == 0) ? 1 : 0;
            soft_flag.flag0.boot_ctrl.sfpg_keep = is_flash_pg_keep();
            soft_flag.flag0.boot_ctrl.do_flash_reset = 0;
            soft_flag.flag0.boot_ctrl.flash_stable_delay_sel = 0 ;
        }
    }

    soft_flag.flag1.io_keep.usbdp = SOFTFLAG_HIGH_RESISTANCE ;
    soft_flag.flag1.io_keep.usbdm = SOFTFLAG_PU ;
    soft_flag.flag1.io_keep.ldoin = SOFTFLAG_HIGH_RESISTANCE ;
    soft_flag.flag1.io_keep.uart_key_port = SOFTFLAG_HIGH_RESISTANCE ;

    P3_RAM_00 = soft_flag.flag0.value;
    P3_RAM_01 = soft_flag.flag1.value;
}

#define cio_index(i)      (i % IO_GROUP_NUM)
#define cio_0xffff(n,i)   ( \
		((n == (i / IO_GROUP_NUM)) ? 0xffff: 0) \
		)
#define b_cio_index(n,i)  (BIT(cio_index(i)) & cio_0xffff(n,i) )

#define FLASH_A_PORT(n)    (\
                        b_cio_index(n, SPI0_PWR_A)   | \
                        b_cio_index(n, SPI0_CS_A)    | \
                        b_cio_index(n, SPI0_CLK_A)   | \
                        b_cio_index(n, SPI0_DO_D0_A) | \
                        b_cio_index(n, SPI0_DI_D1_A) )

const u16 config_flasha_porta =  FLASH_A_PORT(0);
const u16 config_flasha_portb =  FLASH_A_PORT(1);
const u16 config_flasha_portc =  FLASH_A_PORT(2);
const u16 config_flasha_portd =  FLASH_A_PORT(3);
const u16 config_flasha_portp =  FLASH_A_PORT(4);

/* #define FLASH_A_4BIT_PORT(n)    (\ */
/* b_cio_index(n, SPI0_WP_D2_A) | \ */
/* b_cio_index(n, SPI0_HOLD_D3_A) */

#define FLASH_B_PORT(n)    (\
                        b_cio_index(n, SPI0_PWR_B)   | \
                        b_cio_index(n, SPI0_CS_B)    | \
                        b_cio_index(n, SPI0_CLK_B)   | \
                        b_cio_index(n, SPI0_DO_D0_B) | \
                        b_cio_index(n, SPI0_DI_D1_B) )

const u16 config_flashb_porta =  FLASH_B_PORT(0);
const u16 config_flashb_portb =  FLASH_B_PORT(1);
const u16 config_flashb_portc =  FLASH_B_PORT(2);
const u16 config_flashb_portd =  FLASH_B_PORT(3);
const u16 config_flashb_portp =  FLASH_B_PORT(4);

/* #define FLASH_A_4BIT_PORT(n)    (\ */
/* b_cio_index(n, SPI0_WP_D2_A) | \ */
/* b_cio_index(n, SPI0_HOLD_D3_A) */



#define protected_flash_io( a, b, c, d, p)   \
    if(0 != a)                                              \
    {                                                       \
        tmpa |= a;                       \
    }                                                       \
    if(0 != b)                                              \
    {                                                       \
        tmpb |= b;                       \
    }                                                       \
    if(0 != c)                                              \
    {                                                       \
        tmpc |= c;                       \
    }                                                       \
    if(0 != d)                                              \
    {                                                       \
        tmpd |= d;                       \
    }                                                       \
    if(0 != p)                                              \
    {                                                       \
        tmpp |= p;                       \
    }
static void gpio_hi_z(u8 n, u32 data)
{
    extern const u32 gpio_regs[];
    ((JL_PORT_TypeDef *)gpio_regs[n])->DIR  |= data;
    ((JL_PORT_TypeDef *)gpio_regs[n])->PU0  &= ~data;
    ((JL_PORT_TypeDef *)gpio_regs[n])->PD0  &= ~data;
    ((JL_PORT_TypeDef *)gpio_regs[n])->DIE  &= ~data;
    ((JL_PORT_TypeDef *)gpio_regs[n])->DIEH &= ~data;

}

void board_set_soft_poweroff_common(void *priv)
{
    u8 port_sel = 0;
    u16 tmpa = 0;
    u16 tmpb = 0;
    u16 tmpc = 0;
    u16 tmpd = 0;
    u16 tmpp = 0;

    if (P3_PINR_CON & BIT(0)) {
        port_sel = P3_PORT_SEL0;
    }

    if (config_flash_enable) {

        //flash电源
        if (get_sfc_port() == 0) {

            protected_flash_io(
                config_flasha_porta,
                config_flasha_portb,
                config_flasha_portc,
                config_flasha_portd,
                config_flasha_portp);
#if 0
            if (get_sfc_bit_mode() == 4) {
                protected_flash_io(
                    config_flasha_porta_4bit,
                    config_flasha_portb_4bit,
                    config_flasha_portc_4bit,
                    config_flasha_portd_4bit,
                    config_flasha_portp_4bit);
            }
#endif
        } else {
            protected_flash_io(
                config_flashb_porta,
                config_flashb_portb,
                config_flashb_portc,
                config_flashb_portd,
                config_flashb_portp);
            /* soff_gpio_protect(SPI0_PWR_B); */
            /* soff_gpio_protect(SPI0_CS_B); */
            /* soff_gpio_protect(SPI0_CLK_B); */
            /* soff_gpio_protect(SPI0_DO_D0_B); */
            /* soff_gpio_protect(SPI0_DI_D1_B); */
#if 0
            if (get_sfc_bit_mode() == 4) {
                protected_flash_io(
                    config_flashb_porta_4bit,
                    config_flashb_portb_4bit,
                    config_flashb_portc_4bit,
                    config_flashb_portd_4bit,
                    config_flashb_portp_4bit);
            }
#endif
        }

    }

    if (port_sel) {
        u8 port_sel = P3_PORT_SEL0;
        if ((port_sel >= PA0_IN) && (port_sel <= PA15_IN)) {
            /* tmpa |=  BIT(port_sel - PA0_IN); */
            soff_gpio_protect(IO_PORTA_00 + port_sel - PA0_IN);
        } else if ((port_sel >= PB0_IN) && (port_sel <= PB10_IN)) {
            soff_gpio_protect(IO_PORTB_00 + port_sel - PB0_IN);
        } else if ((port_sel >= PC0_IN) && (port_sel <= PC4_IN)) {
            soff_gpio_protect(IO_PORTC_00 + port_sel - PC0_IN);
        } else if ((port_sel >= PP0_IN) && (port_sel <= PP0_IN)) {
            soff_gpio_protect(IO_PORTP_00 + port_sel - PP0_IN);
        } else if (port_sel == USBDP_IN) {
            soff_gpio_protect(IO_PORT_DP);
        } else if (port_sel == USBDM_IN) {
            soff_gpio_protect(IO_PORT_DM);
        }
    }

    mask_io_cfg();




    /* u32 gpio_dir(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op); */
    /* u32 gpio_set_pu(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op); */
    /* u32 gpio_set_pd(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op); */
    /* u32 gpio_die(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op); */
    /* u32 gpio_dieh(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op); */
    tmpa = soff_gpio_config.gpioa & (~tmpa);
    gpio_hi_z(0, tmpa & 0xffff);
    /* JL_PORTA->DIR  |= (tmpa & 0xffff); */
    /* JL_PORTA->PU0  &= ~(tmpa & 0xffff); */
    /* JL_PORTA->PD0  &= ~(tmpa & 0xffff); */
    /* JL_PORTA->DIE  &= ~(tmpa & 0xffff); */
    /* JL_PORTA->DIEH &= ~(tmpa & 0xffff); */
    /* gpio_dir(GPIOA,    0, 16,  soff_gpio_config.gpioa, GPIO_OR); */
    /* gpio_set_pu(GPIOA, 0, 16, ~soff_gpio_config.gpioa, GPIO_AND); */
    /* gpio_set_pd(GPIOA, 0, 16, ~soff_gpio_config.gpioa, GPIO_AND); */
    /* gpio_die(GPIOA,    0, 16, ~soff_gpio_config.gpioa, GPIO_AND); */
    /* gpio_dieh(GPIOA,   0, 16, ~soff_gpio_config.gpioa, GPIO_AND); */

    tmpb = soff_gpio_config.gpiob & (~tmpb);
    gpio_hi_z(1, tmpb & 0xffff);
    /* JL_PORTB->DIR  |= (tmpb & 0xffff); */
    /* JL_PORTB->PU0  &= ~(tmpb & 0xffff); */
    /* JL_PORTB->PD0  &= ~(tmpb & 0xffff); */
    /* JL_PORTB->DIE  &= ~(tmpb & 0xffff); */
    /* JL_PORTB->DIEH &= ~(tmpb & 0xffff); */
    /* gpio_dir(GPIOB,    0, 16,  soff_gpio_config.gpiob, GPIO_OR); */
    /* gpio_set_pu(GPIOB, 0, 16, ~soff_gpio_config.gpiob, GPIO_AND); */
    /* gpio_set_pd(GPIOB, 0, 16, ~soff_gpio_config.gpiob, GPIO_AND); */
    /* gpio_die(GPIOB,    0, 16, ~soff_gpio_config.gpiob, GPIO_AND); */
    /* gpio_dieh(GPIOB,   0, 16, ~soff_gpio_config.gpiob, GPIO_AND); */

    tmpc = soff_gpio_config.gpioc & (~tmpc);
    gpio_hi_z(2, tmpc & 0xffff);
    /* JL_PORTC->DIR  |= (tmpc & 0xffff); */
    /* JL_PORTC->PU0  &= ~(tmpc & 0xffff); */
    /* JL_PORTC->PD0  &= ~(tmpc & 0xffff); */
    /* JL_PORTC->DIE  &= ~(tmpc & 0xffff); */
    /* JL_PORTC->DIEH &= ~(tmpc & 0xffff); */
    /* gpio_dir(GPIOC,    0, 16,  soff_gpio_config.gpioc, GPIO_OR); */
    /* gpio_set_pu(GPIOC, 0, 16, ~soff_gpio_config.gpioc, GPIO_AND); */
    /* gpio_set_pd(GPIOC, 0, 16, ~soff_gpio_config.gpioc, GPIO_AND); */
    /* gpio_die(GPIOC,    0, 16, ~soff_gpio_config.gpioc, GPIO_AND); */
    /* gpio_dieh(GPIOC,   0, 16, ~soff_gpio_config.gpioc, GPIO_AND); */

    tmpd = soff_gpio_config.gpiod & (~tmpd);
    gpio_hi_z(3, tmpd & 0xffff);
    /* JL_PORTD->DIR  |= (tmpd & 0xffff); */
    /* JL_PORTD->PU0  &= ~(tmpd & 0xffff); */
    /* JL_PORTD->PD0  &= ~(tmpd & 0xffff); */
    /* JL_PORTD->DIE  &= ~(tmpd & 0xffff); */
    /* JL_PORTD->DIEH &= ~(tmpd & 0xffff); */
    /* gpio_dir(GPIOD,    0, 16,  soff_gpio_config.gpiod, GPIO_OR); */
    /* gpio_set_pu(GPIOD, 0, 16, ~soff_gpio_config.gpiod, GPIO_AND); */
    /* gpio_set_pd(GPIOD, 0, 16, ~soff_gpio_config.gpiod, GPIO_AND); */
    /* gpio_die(GPIOD,    0, 16, ~soff_gpio_config.gpiod, GPIO_AND); */
    /* gpio_dieh(GPIOD,   0, 16, ~soff_gpio_config.gpiod, GPIO_AND); */

    tmpp = soff_gpio_config.gpiop & (~tmpp);
    JL_PORTP->DIR  |= (tmpp & 0xffff);
    JL_PORTP->PU0  &= ~(tmpp & 0xffff);
    JL_PORTP->PD0  &= ~(tmpp & 0xffff);
    JL_PORTP->DIE  &= ~(tmpp & 0xffff);
    JL_PORTP->DIEH &= ~(tmpp & 0xffff);
    /* gpio_dir(GPIOP,    0, 1,   soff_gpio_config.gpiop, GPIO_OR); */
    /* gpio_set_pu(GPIOP, 0, 1,  ~soff_gpio_config.gpiop, GPIO_AND); */
    /* gpio_set_pd(GPIOP, 0, 1,  ~soff_gpio_config.gpiop, GPIO_AND); */
    /* gpio_die(GPIOP,    0, 1,  ~soff_gpio_config.gpiop, GPIO_AND); */
    /* gpio_dieh(GPIOP,   0, 1,  ~soff_gpio_config.gpiop, GPIO_AND); */

    if (soff_gpio_config.gpiousb) {
        usb_iomode(1);

        if (soff_gpio_config.gpiousb & BIT(0)) {

            usb_port_clr(\
                         B_USB_DP_PU | \
                         B_USB_DP_PD | \
                         B_USB_DP_DIE | \
                         B_USB_DP_DIEH  \
                        );
            usb_port_set(\
                         B_USB_DP_DIR \
                        );

            /* gpio_set_pull_up(IO_PORT_DP, 0); */
            /* gpio_set_pull_down(IO_PORT_DP, 0); */
            /* gpio_set_direction(IO_PORT_DP, 1); */
            /* gpio_set_die(IO_PORT_DP, 0); */
            /* gpio_set_dieh(IO_PORT_DP, 0); */
        }

        if (soff_gpio_config.gpiousb & BIT(1)) {
            usb_port_clr(\
                         B_USB_DM_PU | \
                         B_USB_DM_PD | \
                         B_USB_DM_DIE | \
                         B_USB_DM_DIEH  \
                        );
            usb_port_set(\
                         B_USB_DM_DIR \
                        );
            /* gpio_set_pull_up(IO_PORT_DM, 0); */
            /* gpio_set_pull_down(IO_PORT_DM, 0); */
            /* gpio_set_direction(IO_PORT_DM, 1); */
            /* gpio_set_die(IO_PORT_DM, 0); */
            /* gpio_set_dieh(IO_PORT_DM, 0); */
        }
    }
}


