
#ifndef __BD49__
#define __BD49__

//===============================================================================//
//
//      sfr define
//
//===============================================================================//

#define hs_base            0x3ee0000
#define ls_base            0x3ed0000
#define as_base            0x3e10000
#define wl_base            0x3e00000

#define __RW               volatile       // read write
#define __RO               volatile const // only read
#define __WO               volatile       // only write

#define __u8               unsigned int   // u8  to u32 special for struct
#define __u16              unsigned int   // u16 to u32 special for struct
#define __u32              unsigned int

#define __s8(x)            char(x); char(reserved_1_##x); char(reserved_2_##x); char(reserved_3_##x)
#define __s16(x)           short(x); short(reserved_1_##x)
#define __s32(x)           int(x)

#define map_adr(grp, adr)  ((64 * grp + adr) * 4)     // grp(0x0-0xff), adr(0x0-0x3f)

//===============================================================================//
//
//      high speed sfr address define
//
//===============================================================================//

//............. 0x0000 - 0x00ff............ for hemu
typedef struct {
    __RW __u32 WREN;
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 MSG0;
    __RW __u32 MSG1;
    __RW __u32 MSG2;
    __RW __u32 MSG3;
    __RO __u32 ID;
} JL_HEMU_TypeDef;

#define JL_HEMU_BASE                    (hs_base + map_adr(0x00, 0x00))
#define JL_HEMU                         ((JL_HEMU_TypeDef    *)JL_HEMU_BASE)

//............. 0x0100 - 0x01ff............ for hsb_mbist
//typedef struct {
//    __RW __u32 CON;
//    __RW __u32 SEL;
//    __RW __u32 BEG;
//    __RW __u32 END;
//    __RW __u32 DAT_VLD0;
//    __RW __u32 DAT_VLD1;
//    __RW __u32 DAT_VLD2;
//    __RW __u32 DAT_VLD3;
//} JL_HMBIST_TypeDef;
//
//#define JL_HMBIST_BASE                  (hs_base + map_adr(0x01, 0x00))
//#define JL_HMBIST                       ((JL_HMBIST_TypeDef *)JL_HMBIST_BASE)

//............. 0x0200 - 0x02ff............ for sfc
typedef struct {
    __RW __u32 CON;
    __RW __u32 USER_CODE;
    __RW __u32 OFFSET_ADR;
    __RW __u32 DEC_KEY;
    __RW __u32 NDEC_SADR;
    __RW __u32 NDEC_EADR;
    __RW __u32 CLK_CON;
} JL_SFC_TypeDef;

#define JL_SFC_BASE                     (hs_base + map_adr(0x02, 0x00))
#define JL_SFC                          ((JL_SFC_TypeDef    *)JL_SFC_BASE)

//............. 0x0300 - 0x03ff............
typedef struct {
    __RW __u16 MODE_CON;
    __RW __u16 PROT_CON;
} JL_MODE_TypeDef;

#define JL_MODE_BASE                    (hs_base + map_adr(0x03, 0x00))
#define JL_MODE                         ((JL_MODE_TypeDef     *)JL_MODE_BASE)

//............. 0x0400 - 0x04ff............ for syscfg
typedef struct {
    __RW __u32 RST_SRC;
} JL_RST_TypeDef;

#define JL_RST_BASE                     (hs_base + map_adr(0x04, 0x00))
#define JL_RST                          ((JL_RST_TypeDef        *)JL_RST_BASE)

typedef struct {
    __RW __u32 PWR_CON;
    __RW __u32 SYS_CON0;
    __RW __u32 SYS_CON1;
    __RW __u32 HSB_DIV;
    __RW __u32 HSB_SEL;
    __RW __u32 LSB_SEL;
    __RW __u32 STD_CON0;
    __RW __u32 STD_CON1;
    __RW __u32 PRP_CON0;
    __RW __u32 PRP_CON1;
    __RW __u32 PRP_CON2;
    __RW __u32 PRP_TCON;
} JL_CLOCK_TypeDef;

#define JL_CLOCK_BASE                   (hs_base + map_adr(0x04, 0x01))
#define JL_CLOCK                        ((JL_CLOCK_TypeDef      *)JL_CLOCK_BASE)

//............. 0x0500 - 0x05ff............ for pll0_ctl
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 NR;
    __RW __u32 TRIM_CON0;
    __RW __u32 TRIM_CON1;
    __RW __u32 TRIM_PND;
    __RW __u32 FRQ_CNT;
    __RW __u32 FRC_SCA;
} JL_PLL0_TypeDef;

#define JL_PLL0_BASE                   (hs_base + map_adr(0x05, 0x00))
#define JL_PLL0                        ((JL_PLL0_TypeDef      *)JL_PLL0_BASE)

//............. 0x0d00 - 0x0dff............ for src
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 IDAT_ADR;
    __RW __u32 IDAT_LEN;
    __RW __u32 ODAT_ADR;
    __RW __u32 ODAT_LEN;
    __RW __u32 FLTB_ADR;
    __WO __u32 ODAT_ADR_START;
    __WO __u32 ODAT_ADR_END;
    __WO __u32 STOP_FLAG;
    __RW __u32 INSR;
    __RW __u32 OUTSR;
    __RW __u32 PHASE;
    __RW __u32 COEF;
} JL_SRC_TypeDef;

#define JL_SRC_BASE                  (hs_base + map_adr(0x06, 0x00))
#define JL_SRC                       ((JL_SRC_TypeDef			*)JL_SRC_BASE)


//===============================================================================/
//
//      low speed sfr address define
//
//===============================================================================//

//............. 0x0000 - 0x00ff............
typedef struct {
    __RW __u32 WREN;
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 MSG0;
    __RW __u32 MSG1;
    __RW __u32 MSG2;
    __RW __u32 MSG3;
    __RO __u32 ID;
} JL_LEMU_TypeDef;

#define JL_LEMU_BASE                    (ls_base + map_adr(0x00, 0x00))
#define JL_LEMU                         ((JL_LEMU_TypeDef    *)JL_LEMU_BASE)

//............. 0x0100 - 0x01ff............ for pmu_ctl
typedef struct {
    __RW __u32 PMU_CON;
    __RW __u32 SPI_CON;
    __RW __u32 SPI_DAT;
    __RO __u32 WKUP_SRC;
} JL_PMU_TypeDef;

#define JL_PMU_BASE                     (ls_base + map_adr(0x01, 0x00))
#define JL_PMU                          ((JL_PMU_TypeDef        *)JL_PMU_BASE)

//............. 0x0200 - 0x02ff............ for timer
typedef struct {
    __RW __u32 CON;
    __RW __u32 CNT;
    __RW __u32 PRD;
    __RW __u32 PWM;
} JL_TIMER_TypeDef;

#define JL_TIMER0_BASE                  (ls_base + map_adr(0x02, 4*0))
#define JL_TIMER1_BASE                  (ls_base + map_adr(0x02, 4*1))
#define JL_TIMER2_BASE                  (ls_base + map_adr(0x02, 4*2))
#define JL_TIMER3_BASE                  (ls_base + map_adr(0x02, 4*3))

#define JL_TIMER0                       ((JL_TIMER_TypeDef     *)JL_TIMER0_BASE)
#define JL_TIMER1                       ((JL_TIMER_TypeDef     *)JL_TIMER1_BASE)
#define JL_TIMER2                       ((JL_TIMER_TypeDef     *)JL_TIMER2_BASE)
#define JL_TIMER3                       ((JL_TIMER_TypeDef     *)JL_TIMER3_BASE)

//............. 0x0300 - 0x05ff............ for uart
//
typedef struct {
    __RW __u16 CON0;
    __RW __u16 CON1;
    __RW __u16 CON2;
    __RW __u16 BAUD;
    __RW __u8  BUF;
    __RW __u32 OTCNT;
    __RW __u32 TXADR;
    __WO __u16 TXCNT;
    __RW __u32 RXSADR;
    __RW __u32 RXEADR;
    __RW __u32 RXCNT;
    __RO __u16 HRXCNT;
    __RO __u16 RX_ERR_CNT;
} JL_UART_TypeDef;

#define JL_UART0_BASE                   (ls_base + map_adr(0x03, 0x00))
#define JL_UART1_BASE                   (ls_base + map_adr(0x04, 0x00))
#define JL_UART2_BASE                   (ls_base + map_adr(0x05, 0x00))

#define JL_UART0                        ((JL_UART_TypeDef       *)JL_UART0_BASE)
#define JL_UART1                        ((JL_UART_TypeDef       *)JL_UART1_BASE)
#define JL_UART2                        ((JL_UART_TypeDef       *)JL_UART2_BASE)


//............. 0x0600 - 0x06ff............ for mcpwm
typedef struct {
    __RW __u32 TMR0_CON;
    __RW __u32 TMR0_CNT;
    __RW __u32 TMR0_PR;
    __RW __u32 TMR1_CON;
    __RW __u32 TMR1_CNT;
    __RW __u32 TMR1_PR;
    __RW __u32 TMR2_CON;
    __RW __u32 TMR2_CNT;
    __RW __u32 TMR2_PR;
    __RW __u32 TMR3_CON;
    __RW __u32 TMR3_CNT;
    __RW __u32 TMR3_PR;
    __RW __u32 TMR4_CON;
    __RW __u32 TMR4_CNT;
    __RW __u32 TMR4_PR;
    __RW __u32 TMR5_CON;
    __RW __u32 TMR5_CNT;
    __RW __u32 TMR5_PR;
    __RW __u32 TMR6_CON;
    __RW __u32 TMR6_CNT;
    __RW __u32 TMR6_PR;
    __RW __u32 TMR7_CON;
    __RW __u32 TMR7_CNT;
    __RW __u32 TMR7_PR;
    __RW __u32 FPIN_CON;
    __RW __u32 CH0_CON0;
    __RW __u32 CH0_CON1;
    __RW __u32 CH0_CMPH;
    __RW __u32 CH0_CMPL;
    __RW __u32 CH1_CON0;
    __RW __u32 CH1_CON1;
    __RW __u32 CH1_CMPH;
    __RW __u32 CH1_CMPL;
    __RW __u32 CH2_CON0;
    __RW __u32 CH2_CON1;
    __RW __u32 CH2_CMPH;
    __RW __u32 CH2_CMPL;
    __RW __u32 CH3_CON0;
    __RW __u32 CH3_CON1;
    __RW __u32 CH3_CMPH;
    __RW __u32 CH3_CMPL;
    __RW __u32 CH4_CON0;
    __RW __u32 CH4_CON1;
    __RW __u32 CH4_CMPH;
    __RW __u32 CH4_CMPL;
    __RW __u32 CH5_CON0;
    __RW __u32 CH5_CON1;
    __RW __u32 CH5_CMPH;
    __RW __u32 CH5_CMPL;
    __RW __u32 CH6_CON0;
    __RW __u32 CH6_CON1;
    __RW __u32 CH6_CMPH;
    __RW __u32 CH6_CMPL;
    __RW __u32 CH7_CON0;
    __RW __u32 CH7_CON1;
    __RW __u32 CH7_CMPH;
    __RW __u32 CH7_CMPL;
    __RW __u32 MCPWM_CON0;
} JL_MCPWM_TypeDef;

#define JL_MCPWM_BASE                   (ls_base + map_adr(0x06, 0x00))
#define JL_MCPWM                        ((JL_MCPWM_TypeDef     *)JL_MCPWM_BASE)


//............. 0x0700 - 0x08ff............
typedef struct {
    __RW __u32 CON;
    __RW __u32 BAUD;
    __RW __u32 BUF;
    __WO __u32 ADR;
    __RW __u32 CNT;
    __RW __u32 CON1;
} JL_SPI_TypeDef;

#define JL_SPI0_BASE                    (ls_base + map_adr(0x07, 0x00))
#define JL_SPI1_BASE                    (ls_base + map_adr(0x08, 0x00))
#define JL_SPI2_BASE                    (ls_base + map_adr(0x15, 0x00))

#define JL_SPI0                         ((JL_SPI_TypeDef      *)JL_SPI0_BASE)
#define JL_SPI1                         ((JL_SPI_TypeDef      *)JL_SPI1_BASE)
#define JL_SPI2                         ((JL_SPI_TypeDef      *)JL_SPI2_BASE)


//............. 0x0900 - 0x0aff............for
typedef struct {
    __RW __u32 CON    ;
    __RW __u32 PND    ;
    __RW __u32 TX_BUF ;
    __RW __u32 TASK   ;
    __RO __u32 RX_BUF ;
    __RW __u32 ADDR   ;
    __RW __u32 BAUD   ;
    __RW __u32 TSU    ;
    __RW __u32 THD    ;
    __RO __u32 DBG    ;
    __RW __u32 TRIG_EN;
} JL_IIC_TypeDef;

#define JL_IIC0_BASE                     (ls_base + map_adr(0x09, 0x00))

#define JL_IIC0                          ((JL_IIC_TypeDef       *)JL_IIC0_BASE)

//............. 0x0b00 - 0x0bff............for gpadc
typedef struct {
    __RW __u32 ADC_CON;
    __RW __u32 ANA_CON;
    __RO __u32 RES;
    __RW __u32 DMA_CON0;
    __RW __u32 DMA_CON1;
    __RW __u32 DMA_CON2;
    __RO __u32 DMA_CNT;
    __RW __u32 DMA_BADR;
} JL_ADC_TypeDef;

#define JL_ADC_BASE                     (ls_base + map_adr(0x0b, 0x00))
#define JL_ADC                          ((JL_ADC_TypeDef       *)JL_ADC_BASE)

//............. 0x0c00 - 0x0cff............for irflt
typedef struct {
    __RW __u32 RFLT_CON;
} JL_IR_TypeDef;

#define JL_IR_BASE                      (ls_base + map_adr(0x0c, 0x00))
#define JL_IR                           ((JL_IR_TypeDef         *)JL_IR_BASE)

//............. 0x0d00 - 0x0dff............for pcnt
typedef struct {
    __RW __u32 CON;
    __RW __u32 VAL;
} JL_PCNT_TypeDef;

#define JL_PCNT_BASE                    (ls_base + map_adr(0x0d, 0x00))
#define JL_PCNT                         ((JL_PCNT_TypeDef       *)JL_PCNT_BASE)


//............. 0x0e00 - 0x0eff............
typedef struct {
    __RW __u32 CON;
    __RO __u32 NUM;
} JL_GPCNT_TypeDef;

#define JL_GPCNT_BASE                   (ls_base + map_adr(0x0e, 0x00))
#define JL_GPCNT                        ((JL_GPCNT_TypeDef     *)JL_GPCNT_BASE)

//............. 0x0f00 - 0x0fff............
typedef struct {
    __WO __u32 FIFO;
    __RW __u32 REG;
} JL_CRC_TypeDef;

#define JL_CRC_BASE                     (ls_base + map_adr(0x0f, 0x00))
#define JL_CRC                          ((JL_CRC_TypeDef       *)JL_CRC_BASE)

//............. 0x1000 - 0x10ff............
typedef struct {
    __RW __u8  ENCCON ;
    __WO __u16 ENCKEY ;
    __WO __u16 ENCADR ;
} JL_PERIENC_TypeDef;

#define JL_PERIENC_BASE                 (ls_base + map_adr(0x10, 0x00))
#define JL_PERIENC                      ((JL_PERIENC_TypeDef *)JL_PERIENC_BASE)
//............. 0x1100 - 0x11ff............
typedef struct {
    __RO __u32 R64L;
    __RO __u32 R64H;
} JL_RAND_TypeDef;

#define JL_RAND_BASE                    (ls_base + map_adr(0x11, 0x00))
#define JL_RAND                         ((JL_RAND_TypeDef   *)JL_RAND_BASE)

//............. 0x1200 - 0x12ff............
//typedef struct {
//    __RW __u8  CON;
//    __RW __u8  DAT;
//    __RW __u8  SMP;
//    __RW __u8  DBE;
//} JL_QDEC_TypeDef;
//
//#define JL_QDEC0_BASE                   (ls_base + map_adr(0x012, 0x00))
//#define JL_QDEC0                        ((JL_QDEC_TypeDef       *)JL_QDEC0_BASE)

//............. 0x1300 - 0x13ff............ for pap
//typedef struct {
//        __RW __u32 CON ;
//        __RW __u32 DAT0;
//        __RW __u32 DAT1;
//        __RW __u32 BUF ;
//        __RW __u32 ADR ;
//        __RW __u32 CNT ;
//} JL_PAP_TypeDef;
//
//#define JL_PAP_BASE                     (ls_base + map_adr(0x13, 0x00))
//#define JL_PAP                          ((JL_PAP_TypeDef          *)JL_PAP_BASE)

//............. 0x1400 - 0x14ff............

//............. 0x1500 - 0x15ff............ for lsb peri(spi0/sd0)

//............. 0x1600 - 0x17ff............


//............. 0x1400 - 0x14ff............
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __WO __u32 CPTR;
    __WO __u32 DPTR;
    __RW __u32 CTU_CON;
    __WO __u32 CTU_CNT;
    __RW __u32 BRK;
} JL_SD_TypeDef;

#define JL_SD0_BASE                     (ls_base + map_adr(0x14, 0x00))
#define JL_SD0                          ((JL_SD_TypeDef        *)JL_SD0_BASE)


typedef struct {
    __RW __u32 SADR;
    __RW __u32 DADR;
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RO __u32 RESERVED[4];
} JL_UDMA_CH;

typedef struct {
    JL_UDMA_CH JL_UDMA_CH0;
    JL_UDMA_CH JL_UDMA_CH1;
    JL_UDMA_CH JL_UDMA_CH2;
    JL_UDMA_CH JL_UDMA_CH3;
} JL_UDMA_TypeDef;

#define JL_UDMA_BASE                  (ls_base + map_adr(0x16, 0x00))
#define JL_UDMA                       ((JL_UDMA_TypeDef *)JL_UDMA_BASE)

//............. 0x1800 - 0x18ff............

typedef struct {
    __RW __u32 CON0;
    __RW __u32 SEL0;
    __RW __u32 SEL1;
    __RO __u32 RESERVED[5];
} JL_DMAGEN_CH;

typedef struct {
    JL_DMAGEN_CH JL_DMAGEN_CH0;
    JL_DMAGEN_CH JL_DMAGEN_CH1;
    JL_DMAGEN_CH JL_DMAGEN_CH2;
    JL_DMAGEN_CH JL_DMAGEN_CH3;
} JL_DMAGEN_TypeDef;

#define JL_DMAGEN_BASE               (ls_base + map_adr(0x18, 0x00))
#define JL_DMAGEN                    ((JL_DMAGEN_TypeDef *)JL_DMAGEN_BASE)

//............. 0x1900 - 0x19ff............
typedef struct {
    __WO __u32 CON;
    __RW __u32 NUM;
} JL_LRCT_TypeDef;

#define JL_LRCT_BASE                    (ls_base + map_adr(0x19, 0x00))
#define JL_LRCT                         ((JL_LRCT_TypeDef     *)JL_LRCT_BASE)


//............. 0x1900 - 0x19ff............
//typedef struct {
//    __RW __u8  CON;
//    __RW __u8  DAT;
//    __RW __u8  SMP;
//} JL_RDEC_TypeDef;

//#define JL_RDEC0_BASE                   (ls_base + map_adr(0x19, 0x00))
//#define JL_RDEC0                        ((JL_RDEC_TypeDef       *)JL_RDEC0_BASE)

//............. 0x1a00 - 0x1aff............ for led
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 BRI_PRDL;
    __RW __u32 BRI_PRDH;
    __RW __u32 BRI_DUTY0L;
    __RW __u32 BRI_DUTY0H;
    __RW __u32 BRI_DUTY1L;
    __RW __u32 BRI_DUTY1H;
    __RW __u32 PRD_DIVL;
    __RW __u32 DUTY0;
    __RW __u32 DUTY1;
    __RW __u32 DUTY2;
    __RW __u32 DUTY3;
    __RO __u32 CNT_RD;
    __RW __u32 CON4;
    __WO __u32 CNT_SYNC;
    __RW __u32 CNT_DEC;
} JL_PLED_TypeDef;

#define JL_PLED_BASE                    (ls_base + map_adr(0x1a, 0x00))
#define JL_PLED                         ((JL_PLED_TypeDef      *)JL_PLED_BASE)


typedef struct {
    /* 0  r*/    __RW __u32 CON0;
    /* 1  r*/    __RW __u32 CMD0;
    /* 2   */    __RW __u32 STATUS;
    /* 3  r*/    __RW __u32 PND;
    /* 4  r*/    __RW __u32 ACF;
    /* 5  r*/    __RW __u32 AMF;
    /* 6  r*/    __RW __u32 BTIME0;
    /* 7  r*/    __RW __u32 BTIME1;
    /* 8   */    __RW __u32 RESERVED0;
    /* 9   */    __RW __u32 RESERVED1;
    /*10  r*/    __RW __u32 TIDH;
    /*11  r*/    __RW __u32 TIDL;
    /*12  r*/    __RW __u32 TXB1;
    /*13  r*/    __RW __u32 TXB2;
    /*14  r*/    __RW __u32 TXB3;
    /*15  r*/    __RW __u32 TXB4;
    /*16  r*/    __RW __u32 TXB5;
    /*17  r*/    __RW __u32 TXB6;
    /*18  r*/    __RW __u32 TXB7;
    /*19  r*/    __RW __u32 TXB8;
    /*20   */    __RW __u32 RIDH;
    /*21   */    __RW __u32 RIDL;
    /*22   */    __RW __u32 RXB1;
    /*23   */    __RW __u32 RXB2;
    /*24   */    __RW __u32 RXB3;
    /*25   */    __RW __u32 RXB4;
    /*26   */    __RW __u32 RXB5;
    /*27   */    __RW __u32 RXB6;
    /*28   */    __RW __u32 RXB7;
    /*29   */    __RW __u32 RXB8;
    /*30  r*/    __RW __u32 BAUDH;
    /*31  r*/    __RW __u32 CLKDIV;
} JL_CAN_TypeDef;
#define JL_CAN_BASE                   (ls_base + map_adr(0x1b,0x00))
#define JL_CAN                        ((JL_CAN_TypeDef     *)JL_CAN_BASE)
typedef struct {
    /* 0   */    __RW __u32 CON0;
    /* 1   */    __RW __u32 CMD0;
    /* 2   */    __RW __u32 STATUS;
    /* 3   */    __RW __u32 PND;
    /* 4   */    __RW __u32 IE;
    /* 5   */    __RW __u32 RESERVED0;
    /* 6   */    __RW __u32 BTIME0;
    /* 7   */    __RW __u32 BTIME1;
    /* 8   */    __RW __u32 RESERVED1;
    /* 9   */    __RW __u32 RESERVED2;
    /*10   */    __RW __u32 RESERVED3;
    /*11   */    __RW __u32 ALC;
    /*12   */    __RW __u32 ECC;
    /*13   */    __RW __u32 EWLR;
    /*14   */    __RW __u32 RXERR;
    /*15   */    __RW __u32 TXERR;
    /*16   */    __RW __u32 FF;
    /*17   */    __RW __u32 BUF0;
    /*18   */    __RW __u32 BUF1;
    /*19   */    __RW __u32 BUF2;
    /*20   */    __RW __u32 BUF3;
    /*21   */    __RW __u32 BUF4;
    /*22   */    __RW __u32 BUF5;
    /*23   */    __RW __u32 BUF6;
    /*24   */    __RW __u32 BUF7;
    /*25   */    __RW __u32 BUF8;
    /*26   */    __RW __u32 BUF9;
    /*27   */    __RW __u32 BUF10;
    /*28   */    __RW __u32 BUF11;
    /*29   */    __RW __u32 RMC;
    /*30   */    __RW __u32 BAUDH;
    /*31   */    __RW __u32 CLKDIV;
} JL_ECAN_TypeDef;
#define JL_ECAN_BASE                  (ls_base + map_adr(0x1b,0x00))
#define JL_ECAN                       ((JL_ECAN_TypeDef     *)JL_ECAN_BASE)

typedef struct {
    /* 0   */    __RW __u32 CON;
    /* 1   */    __RW __u32 ADR;
    /* 2   */    __RW __u32 RXCNT;
    /* 3   */    __RO __u32 HRCNT;
} JL_CANDMA_TypeDef;
#define JL_CANDMA_BASE                (ls_base + map_adr(0x1c,0x00))
#define JL_CANDMA                     ((JL_CANDMA_TypeDef     *)JL_CANDMA_BASE)


//............. 0x1b00 - 0x1bff............ for lcd
//typedef struct {
//    __RW __u32 CON0;
//    __RW __u32 CON1;
//    __RW __u32 SEG_IO_EN0;
//    __RW __u32 SEG_IO_EN1;
//    __RW __u32 SEG0_DAT;
//    __RW __u32 SEG1_DAT;
//    __RW __u32 SEG2_DAT;
//    __RW __u32 SEG3_DAT;
//    __RW __u32 SEG4_DAT;
//    __RW __u32 SEG5_DAT;
//} JL_LCDC_TypeDef;

//#define JL_LCDC_BASE                    (ls_base + map_adr(0x1b, 0x00))
//#define JL_LCDC                         ((JL_LCDC_TypeDef      *)JL_LCDC_BASE)

//............. 0x1c00 - 0x1cff............ for pps
//typedef struct {
//    __RW __u32 CON;
//    __RW __u32 CNT;
//    __RW __u32 CFG;
//    __RW __u32 VTRIM;
//    __RW __u32 TX_BASE;
//    __RW __u32 RX_BASE;
//} JL_PPS_TypeDef;
//
//#define JL_PPS_BASE                     (ls_base + map_adr(0x1c, 0x00))
//#define JL_PPS                          ((JL_PPS_TypeDef       *)JL_PPS_BASE)

//............. 0x1d00 - 0x1dff............ for lpctmu
typedef struct {
    __RW __u32 CON0 ;
    __RW __u32 CHEN ;
    __RW __u32 CNUM ;
    __RW __u32 PPRD ;
    __RW __u32 DPRD ;
    __RW __u32 ECON ;
    __RW __u32 EXEN ;
    __RW __u32 CHIS ;
    __RW __u32 CLKC ;
    __WO __u32 WCON ;
    __RW __u32 ANA0 ;
    __RW __u32 ANA1 ;
    __RO __u32 RES  ;
} JL_LPCTM_TypeDef;

#define JL_LPCTM0_BASE                   (ls_base + map_adr(0x1d, 0x00))
#define JL_LPCTM0                        ((JL_LPCTM_TypeDef       *)JL_LPCTM0_BASE)

//............. 0x1d00 - 0x1dff............ for ppm
//typedef struct {
//    __RW __u32 CON;
//    __RW __u32 STRAN_DAT;
//    __RW __u32 STRAN_SET;
//    __RW __u32 DMA_ADR;
//    __RW __u32 PPM_RD;
// } JL_PPM_TypeDef;

// #define JL_PPM_BASE                    (ls_base + map_adr(0x1d, 0x00))
// #define JL_PPM                         ((JL_PPM_TypeDef       *)JL_PPM_BASE)

//............. 0x1f00 - 0x1fff............ for wl uart (only FPGA)
typedef struct {
    __RW __u16 CON0;
    __RW __u16 CON1;
    __RW __u16 CON2;
    __RW __u16 BAUD;
    __RW __u8  BUF;
} JL_WL_UART_LITE_TypeDef;

#define JL_WL_UART_BASE                 (ls_base + map_adr(0x1f, 0x00))
#define JL_WL_UART                      ((JL_WL_UART_LITE_TypeDef *)JL_WL_UART_BASE)

//............. 0x2000 - 0x24ff............ for sie
typedef struct {
    __RW __u32 FADDR;
    __RW __u32 POWER;
    __RO __u32 INTRTX1;
    __RO __u32 INTRTX2;
    __RO __u32 INTRRX1;
    __RO __u32 INTRRX2;
    __RO __u32 INTRUSB;
    __RW __u32 INTRTX1E;
    __RW __u32 INTRTX2E;
    __RW __u32 INTRRX1E;
    __RW __u32 INTRRX2E;
    __RW __u32 INTRUSBE;
    __RO __u32 FRAME1;
    __RO __u32 FRAME2;
    __RO __u32 RESERVED14;
    __RW __u32 DEVCTL;
    __RO __u32 RESERVED10_0x16[0x16 - 0x10 + 1];
} JL_USB_SIE_TypeDef;

#define JL_USB_SIE_BASE                 (ls_base + map_adr(0x20, 0x00))
#define JL_USB_SIE                      ((JL_USB_SIE_TypeDef *)JL_USB_SIE_BASE)

typedef struct {
    __RO __u32 RESERVED0;
    __RW __u32 CSR0;
    __RO __u32 RESERVED2_5[5 - 1];
    __RO __u32 COUNT0;

} JL_USB_EP0_TypeDef;

#define JL_USB_EP0_BASE                 (ls_base + map_adr(0x20, 0x10))
#define JL_USB_EP0                      ((JL_USB_EP0_TypeDef *)JL_USB_EP0_BASE)

typedef struct {
    __RW __u32 TXMAXP;
    __RW __u32 TXCSR1;
    __RW __u32 TXCSR2;
    __RW __u32 RXMAXP;
    __RW __u32 RXCSR1;
    __RW __u32 RXCSR2;
    __RO __u32 RXCOUNT1;
    __RO __u32 RXCOUNT2;
    __RW __u32 TXTYPE;
    __RO __u32 TXINTERVAL;
    __RW __u32 RXTYPE;
    __RO __u32 RXINTERVAL;

} JL_USB_EP_TypeDef;

#define JL_USB_EP1_BASE                 (ls_base + map_adr(0x21, 0x10))
#define JL_USB_EP1                      ((JL_USB_EP_TypeDef *)JL_USB_EP1_BASE)

#define JL_USB_EP2_BASE                 (ls_base + map_adr(0x22, 0x10))
#define JL_USB_EP2                      ((JL_USB_EP_TypeDef *)JL_USB_EP2_BASE)

#define JL_USB_EP3_BASE                 (ls_base + map_adr(0x23, 0x10))
#define JL_USB_EP3                      ((JL_USB_EP_TypeDef *)JL_USB_EP3_BASE)

//............. 0x2500 - 0x25ff............
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __WO __u32 EP0_CNT;
    __WO __u32 EP1_CNT;
    __WO __u32 EP2_CNT;
    __WO __u32 EP3_CNT;
    __WO __u32 EP4_CNT;
    __WO __u32 EP0_ADR;
    __WO __u32 EP1_TADR;
    __WO __u32 EP1_RADR;
    __WO __u32 EP2_TADR;
    __WO __u32 EP2_RADR;
    __WO __u32 EP3_TADR;
    __WO __u32 EP3_RADR;
    __WO __u32 EP4_TADR;
    __WO __u32 EP4_RADR;
    __RW __u32 TXDLY_CON;
    __RW __u32 EP1_RLEN;
    __RW __u32 EP2_RLEN;
    __RW __u32 EP3_RLEN;
    __RW __u32 EP4_RLEN;
    __RW __u32 EP1_MTX_PRD;
    __RW __u32 EP1_MRX_PRD;
    __RO __u32 EP1_MTX_NUM;
    __RO __u32 EP1_MRX_NUM;
    __RW __u32 SOF_STA_CON;
} JL_USB_TypeDef;

#define JL_USB_BASE                     (ls_base + map_adr(0x25, 0x00))
#define JL_USB                          ((JL_USB_TypeDef       *)JL_USB_BASE)

//............. 0x3000 - 0x34ff............ for port
typedef struct {
    __RO __u32 IN;
    __RW __u32 OUT;
    __RW __u32 DIR;
    __RW __u32 DIE;
    __RW __u32 DIEH;
    __RW __u32 PU0;
    __RW __u32 PU1;
    __RW __u32 PD0;
    __RW __u32 PD1;
    __RW __u32 HD0;
    __RW __u32 HD1;
    __RW __u32 SPL;
    __RW __u32 CON; // usb phy only
    __WO __u32 SET;
    __WO __u32 CLR;
} JL_PORT_TypeDef;

#define JL_PORTA_BASE                   (ls_base + map_adr(0x30, 0x00))
#define JL_PORTA                        ((JL_PORT_TypeDef *)JL_PORTA_BASE)

#define JL_PORTB_BASE                   (ls_base + map_adr(0x31, 0x00))
#define JL_PORTB                        ((JL_PORT_TypeDef *)JL_PORTB_BASE)

//#define JL_PORTC_BASE                   (ls_base + map_adr(0x32, 0x00))
//#define JL_PORTC                        ((JL_PORT_TypeDef *)JL_PORTC_BASE)

//#define JL_PORTD_BASE                   (ls_base + map_adr(0x33, 0x00))
//#define JL_PORTD                        ((JL_PORT_TypeDef *)JL_PORTD_BASE)

#define JL_PORTF_BASE                   (ls_base + map_adr(0x34, 0x00))
#define JL_PORTF                        ((JL_PORT_TypeDef *)JL_PORTF_BASE)

#define JL_PORTUSB_BASE                 (ls_base + map_adr(0x35, 0x00))
#define JL_PORTUSB                      ((JL_PORT_TypeDef *)JL_PORTUSB_BASE)

//............. 0x3d00 - 0x3dff............ for port others
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
} JL_WAKEUP_TypeDef;

#define JL_WAKEUP_BASE                  (ls_base + map_adr(0x3e, 0x00))
#define JL_WAKEUP                       ((JL_WAKEUP_TypeDef   *)JL_WAKEUP_BASE)

typedef struct {
    __RW __u32 FSPG_CON;
    __RW __u32 IOMC0;
    __RW __u32 OCH_CON0;
    __RW __u32 ICH_CON0;
    __RW __u32 ICH_CON1;
    __RW __u32 ICH_CON2;
} JL_IOMC_TypeDef;

#define JL_IOMC_BASE                    (ls_base + map_adr(0x3e, 0x04))
#define JL_IOMC                         ((JL_IOMC_TypeDef     *)JL_IOMC_BASE)

//............. 0x4100 - 0x41ff............

//............. 0x4200 - 0x42ff............
typedef struct {
    __RW __u32 WLA_CON0 ;
    __RW __u32 WLA_CON1 ;
    __RW __u32 WLA_CON2 ;
    __RW __u32 WLA_CON3 ;
    __RW __u32 WLA_CON4 ;
    __RW __u32 WLA_CON5 ;
    __RW __u32 WLA_CON6 ;
    __RW __u32 WLA_CON7 ;
    __RW __u32 WLA_CON8 ;
    __RW __u32 WLA_CON9 ;
    __RW __u32 WLA_CON10;
    __RW __u32 WLA_CON11;
    __RW __u32 WLA_CON12;
    __RW __u32 WLA_CON13;
    __RW __u32 WLA_CON14;
    __RW __u32 WLA_CON15;
    __RW __u32 WLA_CON16;
    __RW __u32 WLA_CON17;
    __RW __u32 WLA_CON18;
    __RW __u32 WLA_CON19;
    __RW __u32 WLA_CON20;
    __RW __u32 WLA_CON21;
    __RW __u32 WLA_CON22;
    __RW __u32 WLA_CON23;
    __RW __u32 WLA_CON24;
    __RW __u32 WLA_CON25;
    __RW __u32 WLA_CON26;
    __RW __u32 WLA_CON27;
    __RW __u32 WLA_CON28;
    __RW __u32 WLA_CON29;
    __RO __u32 WLA_CON30;
    __RO __u32 WLA_CON31;
    __RO __u32 WLA_CON32;
    __RO __u32 WLA_CON33;
    __RO __u32 WLA_CON34;
    __RO __u32 WLA_CON35;
    __RO __u32 WLA_CON36;
    __RO __u32 WLA_CON37;
    __RO __u32 WLA_CON38;
    __RO __u32 WLA_CON39;
} JL_WLA_TypeDef;

#define JL_WLA_BASE                     (ls_base + map_adr(0x42, 0x00))
#define JL_WLA                          ((JL_WLA_TypeDef       *)JL_WLA_BASE)

//===============================================================================//
//
//      ass_sfr address define(TODO)
//
//===============================================================================//


//............. 0x0700 - 0x07ff............
//typedef struct {
//    __RW __u16 CON;
//    __RW __u8  SMR;
//    __RW __u32 ADR;
//    __RW __u32 LEN;
//    __RW __u16 DOR;
//    __RW __u32 CON1;
//
//} JL_PLNK_TypeDef;
//
//#define JL_PLNK_BASE                 (hs_base + map_adr(0x07, 0x00))
//#define JL_PLNK                      ((JL_PLNK_TypeDef     *)JL_PLNK_BASE)

//............. 0x0200 - 0x02ff............ for audio_top
typedef struct {
    __RW __u32(DAC_CON);
    __RW __u32(DAC_ADR);
    __RW __u16(DAC_LEN);
    __RW __u16(DAC_PNS);
    __RW __u16(DAC_HRP);
    __RW __u16(DAC_SWP);
    __RW __u16(DAC_SWN);
    __RW __u32(DAC_CON1);
    __RW __u32(DAC_VL0);
    __RW __u32(DAC_DIT);
    __RW __u32(DAC_TM0);
    __RW __u16(DAC_COP);
    __WO __u32(DAC_DCW);
    __RO __u32(DAC_DCR);

    __RW __u32(ADC_CON);
    __RW __u32(ADC_ADR);
    __RW __u16(ADC_LEN);
    __RW __u16(ADC_PNS);
    __RW __u16(ADC_HWP);
    __RW __u16(ADC_SRP);
    __RW __u16(ADC_SRN);
    __RW __u32(ADC_CON1);
    __RW __u16(ADC_COP);
    //__RW __u32 (MBG_CON);
    __WO __u32(ADC_DCW);
    __RO __u32(ADC_DCR);
} JL_AUDIO_TypeDef;

#define JL_AUDIO_BASE               (as_base + map_adr(0x02, 0x00))
#define JL_AUDIO                    ((JL_AUDIO_TypeDef   *)JL_AUDIO_BASE)

//............. 0x0300 - 0x03ff............ for apa_top
typedef struct {
    __RW __u32(APA_LEN);
    __RW __u32(APA_ADR);
    __RW __u32(APA_CON0);
    __RW __u32(APA_CON1);
    __RW __u32(APA_CON2);
    __RW __u32(APA_CON3);
    __RO __u32(APA_CON4);
    __RW __u32(APA_CON5);
    __RW __u32(APA_CON6);
    __RW __u32(APA_CON7);
} JL_APA_TypeDef;

#define JL_APA_BASE                 (as_base + map_adr(0x3, 0x00))
#define JL_APA                      ((JL_APA_TypeDef   *)JL_APA_BASE)

//............. 0x0400 - 0x04ff............
typedef struct {
    __RW __u16 CON0;
    __RW __u16 CON1;
    __RW __u8  CON2;
    __RW __u8  CON3;
    __WO __u32 ADR0;
    __WO __u32 ADR1;
    __WO __u32 ADR2;
    __WO __u32 ADR3;
    __WO __u16 LEN;
    __RW __u32 PNS;
    __RW __u32 HWPTR0;
    __RW __u32 HWPTR1;
    __RW __u32 HWPTR2;
    __RW __u32 HWPTR3;
    __RW __u32 SWPTR0;
    __RW __u32 SWPTR1;
    __RW __u32 SWPTR2;
    __RW __u32 SWPTR3;
    __RW __u32 SHN0;
    __RW __u32 SHN1;
    __RW __u32 SHN2;
    __RW __u32 SHN3;
} JL_ALNK_TypeDef;

#define JL_ALNK0_BASE                (as_base + map_adr(0x04, 0x00))
#define JL_ALNK0                     ((JL_ALNK_TypeDef    *)JL_ALNK0_BASE)

//............. 0x0500 - 0x05ff............ for ass_clkgen
typedef struct {
    __RW __u32 AUD_CON0;
    __RW __u32 AUD_CON1;
    __RW __u32 AUD_CON2;
    //__RW __u32 AUD_CON3;
} JL_AUD_TypeDef;

#define JL_AUD_BASE                 (as_base + map_adr(0x05, 0x00))
#define JL_AUD                      ((JL_AUD_TypeDef  *)JL_AUD_BASE)


//........... .. 0x0a00 - 0x0aff............ for audio_ana
typedef struct {
    __RW __u32 DAA_CON0;
    __RO __u32 DAA_CON7;
    __RW __u32 ADA_CON0;
    __RW __u32 ADA_CON1;
    __RW __u32 ADA_CON2;
    __RW __u32 ADA_CON3;
    __RW __u32 ADA_CON4;
    __RW __u32 ADDA_CON0;
} JL_ADDA_TypeDef;

#define JL_ADDA_BASE               (as_base + map_adr(0x01, 0x00))
#define JL_ADDA                    ((JL_ADDA_TypeDef       *)JL_ADDA_BASE)

//............. 0x0b00 - 0x0bff............ for ass_mbist
typedef struct {
    __RW __u32 CON;
    __RW __u32 SEL;
    __RW __u32 BEG;
    __RW __u32 END;
    __RW __u32 DAT_VLD0;
    __RW __u32 DAT_VLD1;
    __RW __u32 DAT_VLD2;
    __RW __u32 DAT_VLD3;
} JL_ASS_MBIST_TypeDef;

#define JL_ASS_MBIST_BASE             (as_base + map_adr(0x00, 0x00))
#define JL_ASS_MBIST                  ((JL_ASS_MBIST_TypeDef  *)JL_ASS_MBIST_BASE)

//............. 0x0c00 - 0x0cff............ for imd
//typedef struct {
//    __RW __u32 IMD_CON0;
//    __RW __u32 IMD_CON1;
//    __RW __u32 IMD_CON2;
//    __RW __u32 IMD_CON3;
//    __RW __u32 IMD_LD_START_ADR;
//    __RW __u32 IMD_DEBUG_PIXEL;
//    __RW __u32 IMD_IO_CFG;
//    __RW __u16 IMD_BAUD;
//    __RW __u32 IMDSPI_CON0;
//    __RW __u32 IMDSPI_BUF;
//    __RW __u32 IMDPAP_CON0;
//    __RW __u32 IMDPAP_PRD;
//    __RW __u32 IMDPAP_BUF;
//    __RW __u32 IMDRGB_CON0;
//    __RW __u32 IMDRGB_HPRD;
//    __RW __u32 IMDRGB_VPRD;
//} JL_IMD_TypeDef;
//
//#define JL_IMD_BASE                      (hs_base + map_adr(0x0c, 0x00))
//#define JL_IMD                           ((JL_IMD_TypeDef			*)JL_IMD_BASE)

//............. 0x1500 - 0x15ff............ for aes
typedef struct {
    __RW __u32 CON;
    __RW __u32 DATIN;
    __WO __u32 KEY;
    __RW __u32 ENCRES0;
    __RW __u32 ENCRES1;
    __RW __u32 ENCRES2;
    __RW __u32 ENCRES3;
    __WO __u32 NONCE;
    __WO __u8  HEADER;
    __WO __u32 SRCADR;
    __WO __u32 DSTADR;
    __WO __u32 CTCNT;
    __WO __u32 TAGLEN;
    __RO __u32 TAGRES0;
    __RO __u32 TAGRES1;
    __RO __u32 TAGRES2;
    __RO __u32 TAGRES3;
} JL_AES_TypeDef;

#define JL_AES_BASE               (hs_base + map_adr(0x15, 0x00))
#define JL_AES                    ((JL_AES_TypeDef *)JL_AES_BASE)


////............. 0x1700 - 0x17ff............ for hadc_top
//  typedef struct {
//      __RW __u32 (HADC_CON0);               /* 00 */
//      __RW __u32 (HADC_CON1);               /* 01 */
//      __RW __u32 (HADC_COP);                /* 02 */
//      __RW __u32 (HADC_LEN);                /* 03 */
//      __RW __u32 (HADC_ADR);                /* 04 */
//      __RO __u32 (HADC_OSA);                /* 05 */
//  } JL_HADC_TypeDef;
//
//  #define JL_HADC_BASE                  (hs_base + map_adr(0x17, 0x00))
//  #define JL_HADC                       ((JL_HADC_TypeDef   *)JL_HADC_BASE)

//............. 0x3100 - 0x31ff............ as_emu
typedef struct {
    __RW __u32 WREN;
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 MSG0;
    __RW __u32 MSG1;
    __RW __u32 MSG2;
    __RW __u32 MSG3;
    __RO __u32 ID;
} JL_AEMU_TypeDef;

#define JL_AEMU_BASE                    (as_base + map_adr(0x31, 0x00))
#define JL_AEMU                         ((JL_AEMU_TypeDef    *)JL_AEMU_BASE)


//===============================================================================//
//
//      wl_sfr address define(TODO)
//
//===============================================================================//

//............. 0x0e00 - 0x0eff............
//
//............. 0x0f00 - 0x0fff............

//............. 0x1000 - 0x10ff............ for wl_mbist
//typedef struct {
//    __RW __u32 CON;
//    __RW __u32 SEL;
//    __RW __u32 BEG;
//    __RW __u32 END;
//    __RW __u32 DAT_VLD0;
//    __RW __u32 DAT_VLD1;
//    __RW __u32 DAT_VLD2;
//    __RW __u32 DAT_VLD3;
//} JL_WL_MBIST_TypeDef;

//#define JL_WL_MBIST_BASE               (hs_base + map_adr(0x10, 0x00))
//#define JL_WL_MBIST                    ((JL_WL_MBIST_TypeDef *)JL_WL_MBIST_BASE)

//............. 0x1100 - 0x11ff............
//typedef struct {
//    __RW __u32 CON0;
//} JL_WL_AUD_TypeDef;
//
//#define JL_WL_AUD_BASE                  (hs_base + map_adr(0x11, 0x00))
//#define JL_WL_AUD                       ((JL_WL_AUD_TypeDef *)JL_WL_AUD_BASE)

//............. 0x1200 - 0x12ff............ for wireless
//typedef struct {
//    __RW __u32 CON0;
//    __RW __u32 CON3;
//    __RW __u32 LOFC_CON;
//    __RW __u32 LOFC_RES;
//    __RW __u32 DBG_CON;
//} JL_WL_TypeDef;
//
//#define JL_WL_BASE                      (hs_base + map_adr(0x12, 0x00))
//#define JL_WL                           ((JL_WL_TypeDef     *)JL_WL_BASE)

//............. 0x1300 - 0x13ff............
//typedef struct {
//    __RW __u32 CON;
//    __RW __u32 DABUF;
//    __RO __u32 DASPACE;
//    __RO __u32 OFFEND;
//    __RO __u32 OFFSUM;
//    __RO __u32 PCMNUM;
//    __RO __u32 BTTIME;
//    __WO __u32 CON1  ;
//    __RW __u32 CON2  ;
//    __WO __u32 BT_OFFSET;
//    __WO __u32 MIN_SLOT;
//    __RW __u32 OUTSR;
//    __WO __u32 INSR;
//    __RW __u32 CON3  ;
//} JL_SYNC_TypeDef;
//
//#define JL_SYNC_BASE                 (hs_base + map_adr(0x13, 0x00))
//#define JL_SYNC                      ((JL_SYNC_TypeDef *)JL_SYNC_BASE)

//===============================================================================//
//
//  wl_top
//
//===============================================================================//

typedef struct {
    __RW __u32 CON0;
} JL_WL_AUD_TypeDef;

#define JL_WL_AUD_BASE             (wl_base + map_adr(0x01, 0x00))
#define JL_WL_AUD                  ((JL_WL_AUD_TypeDef *)JL_WL_AUD_BASE)

typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON3;
    __RW __u32 LOFC_CON;
    __RW __u32 LOFC_RES;
    __RW __u32 DBG_CON;
} JL_WL_TypeDef;

#define JL_WL_BASE                 (wl_base + map_adr(0x01, 0x01))
#define JL_WL                      ((JL_WL_TypeDef     *)JL_WL_BASE)

typedef struct {
    __RW __u32 WREN;
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 MSG0;
    __RW __u32 MSG1;
    __RW __u32 MSG2;
    __RW __u32 MSG3;
    __RO __u32 ID;
} JL_WEMU_TypeDef;

#define JL_WEMU_BASE                (wl_base + map_adr(0x10, 0x00))
#define JL_WEMU                     ((JL_WEMU_TypeDef    *)JL_WEMU_BASE)

#include "io_omap.h"
#include "io_imap.h"

#endif

