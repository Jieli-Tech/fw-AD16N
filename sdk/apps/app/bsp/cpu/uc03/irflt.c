#include "cpu.h"
#include "config.h"
#include "gpio.h"
#include "irflt.h"
#include "app_config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[irflt]"
#include "log.h"

static IR_CODE ir_code;       ///<红外遥控信息
volatile u8 ir_busy;

#define IRGPIO  IR_KEY_IO
#define IRIDX   IR_KEY_IRQ_IDX
#define IRTMR   IR_KEY_TIMER

#define timer_Prd   (timer_Hz / (1000 * timer_Div)) //1ms cnt次数
#define timer_Hz    12000000L
#define timer_Div   64
/*----------------------------------------------------------------------------*/
/**@brief   红外timer中断服务函数
   @param   void
   @param   void
   @return  void
   @note    void irtmr_ir_isr(void)
*/
/*----------------------------------------------------------------------------*/
___interrupt
static void irtmr_ir_isr(void)
{
    IRTMR->CON |= BIT(14);
    IRTMR->CNT = 0;

    u16 bCap1 = IRTMR->PRD;
    u8 cap = bCap1 / timer_Prd;

    /* log_info("ir isr verify"); */
    /* log_info("cnt = %d, cap = 0x%x", cnt++, cap); */
    if (cap <= 1) {
        ir_code.wData >>= 1;
        ir_code.bState++;
        ir_code.boverflow = 0;
    } else if (cap == 2) {
        ir_code.wData >>= 1;
        ir_code.bState++;
        ir_code.wData |= 0x8000;
        ir_code.boverflow = 0;
    }
    /*13ms-Sync*/
    else if ((cap == 13) && (ir_code.boverflow < 8)) {
        ir_code.bState = 0;
        ir_busy = 1;
    } else if ((cap < 8) && (ir_code.boverflow < 5)) {
        ir_code.bState = 0;
    } else if ((cap > 110) && (ir_code.boverflow > 53)) {
        ir_code.bState = 0;
    } else if ((cap > 20) && (ir_code.boverflow > 53)) { //溢出情况下 （12M 48M）
        ir_code.bState = 0;
    } else {
        ir_code.boverflow = 0;
    }

    if (ir_code.bState == 16) {
        ir_code.wUserCode = ir_code.wData;
    }
    if (ir_code.bState == 32) {
        /* log_info("[0x%X]\n", ir_code.wData); */
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   红外timer初始化
   @param   void
   @param   void
   @return  void
   @note    void set_ir_clk(void)
*/
/*----------------------------------------------------------------------------*/
static void set_ir_clk(void)
{
    IRTMR->CON = BIT(14);
    IRTMR->PRD = 0;
    IRTMR->CNT = 0;

    request_irq(IRIDX, IRQ_IRTMR_IP, irtmr_ir_isr, 0);
    IRTMR->CON |= (7 << 10)/*std_12m (lsb/2以下)*/ | \
                  (3 << 4)/*12m div 64*/ | \
                  (1 << 2)/*irflt out*/ | \
                  (3 << 0)/*falling edge capture*/;
}

/*----------------------------------------------------------------------------*/
/**@brief   红外io初始化
   @param   void
   @param   void
   @return  void
   @note    void ir_input_io_sel(u32 port)
*/
/*----------------------------------------------------------------------------*/
static void ir_input_io_sel(u32 port)
{
    //选择irflt input channel1输入
    gpio_set_direction(port, 1);
    gpio_set_die(port, 1);
    gpio_ich_sel_input_signal(port, INPUT_CH_SIGNAL_IRFLT, INPUT_CH_TYPE_GP_ICH1);
}

/*----------------------------------------------------------------------------*/
/**@brief   红外模块初始化
   @param   void
   @param   void
   @return  void
   @note    void irflt_config(void)
*/
/*----------------------------------------------------------------------------*/
static void irflt_config(void)
{
    //std12m  256/12=21.3us
    JL_IR->RFLT_CON = 0;
    JL_IR->RFLT_CON |= (irflt_div_256 << 4) | \
                       (irflt_std_12m << 2) | \
                       (1 << 0);
    set_ir_clk();
}

static void log_irflt_info()
{
    log_info("JL_IR->RFLT_CON = 0x%x\n", JL_IR->RFLT_CON);
    log_info("IR_TIMER->CON   = 0x%x\n", IRTMR->CON);
}

void ir_timeout(void)
{
    ir_code.boverflow++;
    if (ir_code.boverflow > 10) { //100ms
        ir_code.boverflow = 10;
        ir_code.bState = 0;
        ir_busy = 0;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   ir按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ir_key_init(void)
*/
/*----------------------------------------------------------------------------*/
int irflt_init(void)
{
    log_info("ir key init >>>\n");
    ir_input_io_sel(IRGPIO);
    irflt_config();
    log_irflt_info();
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   获取ir按键值
   @param   void
   @param   void
   @return  void
   @note    u8 get_irkey_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 get_irkey_value(void)
{
    u8 tkey = 0xff;
    if (ir_code.bState != 32) {
        return tkey;
    }
    if ((((u8 *)&ir_code.wData)[0] ^ ((u8 *)&ir_code.wData)[1]) == 0xff) {
        /* if (ir_code.wUserCode == 0xFF00) */
        {
            /* log_info("<%d>",(u8)ir_code.wData); */
            tkey = (u8)ir_code.wData;
        }
    } else {
        ir_code.bState = 0;
    }
    return tkey;
}

u8 ir_busy_check(void)
{
    return ir_busy;
}



/**********ir_key 测试函数***********/
#if 0

#define CONFIG_IRFLT_TEST
#ifdef CONFIG_IRFLT_TEST
IRFLT_PLATFORM_DATA_BEGIN(irflt_data)
.irflt_io 		= IO_PORTA_09,
     .timer 			= SEL_TIMER2,
            IRFLT_PLATFORM_DATA_END()
            /* REGISTER_DEVICE(irflt_dev) = { */
            /* .name 			= "irflt", */
            /* .ops 			= &irflt_dev_ops, */
            /* .priv_data 		= (void *) &irflt_data, */
            /* }; */
#endif

            int irflt_test(void)
{
    u8 ir_key = 0xff;
    irflt_init(0, (void *)&irflt_data);
    while (1) {
        delay(100);
        ir_key = get_irkey_value();
        if (ir_key != 0xff) {
            log_info("ir_key:0x%x", ir_key);
            ir_key = 0xff;
            ir_code.bState = 0;
            ir_busy = 0;
            ir_code.wData = 0;
        }
        wdt_clear();
    }
    return 0;
}
#endif

