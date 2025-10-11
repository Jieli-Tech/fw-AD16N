
//===============================================================================//
//
//      output function define
//
//===============================================================================//
#define FO_GP_OCH0        ((0 << 2)|BIT(1))
#define FO_GP_OCH1        ((1 << 2)|BIT(1))
#define FO_GP_OCH2        ((2 << 2)|BIT(1))
#define FO_GP_OCH3        ((3 << 2)|BIT(1))
#define FO_SPI1_CLK        ((4 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_DA0        ((5 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_DA1        ((6 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_DA2        ((7 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_DA3        ((8 << 2)|BIT(1)|BIT(0))
#define FO_SPI2_CLK        ((9 << 2)|BIT(1)|BIT(0))
#define FO_SPI2_DA0        ((10 << 2)|BIT(1)|BIT(0))
#define FO_SPI2_DA1        ((11 << 2)|BIT(1)|BIT(0))
#define FO_IIC0_SCL        ((12 << 2)|BIT(1)|BIT(0))
#define FO_IIC0_SDA        ((13 << 2)|BIT(1)|BIT(0))
#define FO_UART0_TX        ((14 << 2)|BIT(1)|BIT(0))
#define FO_UART1_TX        ((15 << 2)|BIT(1)|BIT(0))
#define FO_UART2_TX        ((16 << 2)|BIT(1)|BIT(0))
#define FO_SD0_CLK        ((17 << 2)|BIT(1)|BIT(0))
#define FO_SD0_CMD        ((18 << 2)|BIT(1)|BIT(0))
#define FO_SD0_DA0        ((19 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_MCLK        ((20 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_LRCK        ((21 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_SCLK        ((22 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_DAT0        ((23 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_DAT1        ((24 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_DAT2        ((25 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_DAT3        ((26 << 2)|BIT(1)|BIT(0))
#define FO_CAN_TX            ((27 << 2)|BIT(1)|BIT(0))
#define FO_CHAIN_OUT0        ((28 << 2)|BIT(1)|BIT(0))
#define FO_CHAIN_OUT1        ((29 << 2)|BIT(1)|BIT(0))
#define FO_CHAIN_OUT2        ((30 << 2)|BIT(1)|BIT(0))
#define FO_CHAIN_OUT3        ((31 << 2)|BIT(1)|BIT(0))

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
    __RW __u8 USBDP_OUT;
    __RW __u8 USBDM_OUT;
    __RW __u8 APA_PIN;
    __RW __u8 APA_NIN;
} JL_OMAP_TypeDef;

#define JL_OMAP_BASE      (ls_base + map_adr(0x36, 0x00))
#define JL_OMAP           ((JL_OMAP_TypeDef   *)JL_OMAP_BASE)


