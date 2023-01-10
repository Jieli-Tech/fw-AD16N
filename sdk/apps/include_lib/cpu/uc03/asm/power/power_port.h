/*********************************************************************************************
    *   Filename        : power_hw.c

    *   Description     :

    *   Author          : Sunlicheng

    *   Email           : Sunlicheng@zh-jieli.com

    *   Last modifiled  : 2022-01-07 11:30

    *   Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __POWER_PORT_H__
#define __POWER_PORT_H__

enum {
    PORTA_GROUP = 0,
    PORTB_GROUP,
    PORTC_GROUP,
    PORTD_GROUP,
    PORTP_GROUP,
};

struct gpio_value {
    u16 gpioa;
    u16 gpiob;
    u16 gpioc;
    u16 gpiod;
    u16 gpiop;
    u16  gpiousb;
};

#define     _PORT(p)            JL_PORT##p
#define     SPI_PORT(p)         _PORT(p)

// | func\port |  A   |  B   |
// |-----------|------|------|
// | CS        | PD3  | PG4  |
// | CLK       | PD0  | PD0  |
// | DO(D0)    | PD1  | PD1  |
// | DI(D1)    | PD2  | PG3  |
// | WP(D2)    | xxx  | xxx  |
// | HOLD(D3)  | xxx  | xxx  |

#define get_sfc_port() (JL_IOMC->IOMC0 & 0x1)

////////////////////////////////////////////////////////////////////////////////
#define     PORT_SPI0_PWRA      D
#define     SPI0_PWRA           4

#define     PORT_SPI0_CSA       D
#define     SPI0_CSA            3

#define     PORT_SPI0_CLKA      D
#define     SPI0_CLKA           0

#define     PORT_SPI0_DOA       D
#define     SPI0_DOA            1

#define     PORT_SPI0_DIA       D
#define     SPI0_DIA            2

//#define     PORT_SPI0_D2A       D
//#define     SPI0_D2A         	5
//
//#define     PORT_SPI0_D3A       D
//#define     SPI0_D3A            6

#define		SPI0_PWR_A		IO_PORTD_04
#define		SPI0_CS_A		IO_PORTD_03
#define 	SPI0_CLK_A		IO_PORTD_00
#define 	SPI0_DO_D0_A	IO_PORTD_01
#define 	SPI0_DI_D1_A	IO_PORTD_02
//#define 	SPI0_WP_D2_A	IO_PORTD_05
//#define 	SPI0_HOLD_D3_A	IO_PORTD_06


////////////////////////////////////////////////////////////////////////////////
#define     PORT_SPI0_PWRB      C
#define     SPI0_PWRB           2

#define     PORT_SPI0_CSB       C
#define     SPI0_CSB            4

#define     PORT_SPI0_CLKB      C
#define     SPI0_CLKB           1

#define     PORT_SPI0_DOB       C
#define     SPI0_DOB            0

#define     PORT_SPI0_DIB       C
#define     SPI0_DIB            3

//#define     PORT_SPI0_D2B       G
//#define     SPI0_D2B   		 	2
//
//#define     PORT_SPI0_D3B       D
//#define     SPI0_D3B            6

#define		SPI0_PWR_B		IO_PORTC_02
#define		SPI0_CS_B		IO_PORTC_04
#define 	SPI0_CLK_B		IO_PORTC_01
#define 	SPI0_DO_D0_B	IO_PORTC_00
#define 	SPI0_DI_D1_B	IO_PORTC_03
//#define 	SPI0_WP_D2_B	IO_PORTG_02
//#define 	SPI0_HOLD_D3_B	IO_PORTD_06

////////////////////////////////////////////////////////////////////////

u8 get_sfc_bit_mode();
void port_init(void);
void port_protect(u16 *port_group, u32 port_num);

u8 get_boot_otp();

void sleep_gpio_protect(u32 gpio);
void sleep_enter_callback_common(void *priv);
void sleep_exit_callback_common(void *priv);

void soff_gpio_protect(u32 gpio);
void board_set_soft_poweroff_common(void *priv);

#endif
