
//===============================================================================//
//
//      output function define
//
//===============================================================================//
#define FO_GP_OCH0        ((0 <<2)|BIT(1))
#define FO_GP_OCH1        ((1 <<2)|BIT(1))
#define FO_GP_OCH2        ((2 <<2)|BIT(1))
#define FO_GP_OCH3        ((3 <<2)|BIT(1))
#define FO_GP_OCH4        ((4 <<2)|BIT(1))
#define FO_GP_OCH5        ((5 <<2)|BIT(1))
#define FO_GP_OCH6        ((6 <<2)|BIT(1))
#define FO_GP_OCH7        ((7 <<2)|BIT(1))
#define FO_SPI0_CLK        ((8 << 2)|BIT(1)|BIT(0))
#define FO_SPI0_DA0        ((9 <<2)|BIT(1)|BIT(0))
#define FO_SPI0_DA1        ((10 <<2)|BIT(1)|BIT(0))
#define FO_SPI1_CLK        ((11 <<2)|BIT(1)|BIT(0))
#define FO_SPI1_DA0        ((12 <<2)|BIT(1)|BIT(0))
#define FO_SPI1_DA1        ((13 <<2)|BIT(1)|BIT(0))
#define FO_SPI1_DA2        ((14 <<2)|BIT(1)|BIT(0))
#define FO_SPI1_DA3        ((15 <<2)|BIT(1)|BIT(0))
#define FO_SD0_CLK        ((16 <<2)|BIT(1)|BIT(0))
#define FO_SD0_CMD        ((17 <<2)|BIT(1)|BIT(0))
#define FO_SD0_DA0        ((18 <<2)|BIT(1)|BIT(0))
#define FO_IIC0_SCL        ((19 <<2)|BIT(1)|BIT(0))
#define FO_IIC0_SDA        ((20 <<2)|BIT(1)|BIT(0))
#define FO_UART0_TX        ((21 <<2)|BIT(1)|BIT(0))
#define FO_UART1_TX        ((22 <<2)|BIT(1)|BIT(0))
#define FO_UART1_RTS        ((23 <<2)|BIT(1)|BIT(0))
#define FO_MCPWM0_H        ((24 <<2)|BIT(1)|BIT(0))
#define FO_MCPWM1_H        ((25 <<2)|BIT(1)|BIT(0))
#define FO_MCPWM0_L        ((26 <<2)|BIT(1)|BIT(0))
#define FO_MCPWM1_L        ((27 <<2)|BIT(1)|BIT(0))

//===============================================================================//
//
//      IO output select sfr
//
//===============================================================================//
typedef struct {
    __RW __u8 PA0_OUT;
    __RW __u8 PA1_OUT;
    __RW __u8 PA2_OUT;
    __RW __u8 PA3_OUT;
    __RW __u8 PA4_OUT;
    __RW __u8 PA5_OUT;
    __RW __u8 PA6_OUT;
    __RW __u8 PA7_OUT;
    __RW __u8 PA8_OUT;
    __RW __u8 PA9_OUT;
    __RW __u8 PA10_OUT;
    __RW __u8 PA11_OUT;
    __RW __u8 PA12_OUT;
    __RW __u8 PA13_OUT;
    __RW __u8 PA14_OUT;
    __RW __u8 PA15_OUT;
    __RW __u8 PB0_OUT;
    __RW __u8 PB1_OUT;
    __RW __u8 PB2_OUT;
    __RW __u8 PB3_OUT;
    __RW __u8 PB4_OUT;
    __RW __u8 PB5_OUT;
    __RW __u8 PB6_OUT;
    __RW __u8 PB7_OUT;
    __RW __u8 PB8_OUT;
    __RW __u8 PB9_OUT;
    __RW __u8 PB10_OUT;
    __RW __u8 PC0_OUT;
    __RW __u8 PC1_OUT;
    __RW __u8 PC2_OUT;
    __RW __u8 PC3_OUT;
    __RW __u8 PC4_OUT;
    __RW __u8 PC5_OUT;
    __RW __u8 USBDP_OUT;
    __RW __u8 USBDM_OUT;
    __RW __u8 PP0_OUT;
} JL_OMAP_TypeDef;

#define JL_OMAP_BASE      (ls_base + map_adr(0x35, 0x00))
#define JL_OMAP           ((JL_OMAP_TypeDef   *)JL_OMAP_BASE)

