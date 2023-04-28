#include "includes.h"
#include "asm/power/p33.h"
#include "ledc.h"
#include "gpio.h"
#include "stdlib.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[LEDC]"
#include "log.h"

static void (*ledc_isr_cbfun)(void) = NULL;
volatile u8 ledc_busy = 0;
___interrupt
void ledc_isr(void)
{
    if (JL_LEDC->CON & BIT(7)) {
        JL_LEDC->CON |= BIT(6);
        if (ledc_isr_cbfun) {
            ledc_isr_cbfun();
        }
    }
    ledc_busy = 0;
}

static u8 t_div[9] = {1, 2, 3, 6, 12, 24, 48, 96, 192};
void ledc_init(const struct ledc_platform_data *arg)
{
    gpio_set_die(arg->port, 1);
    gpio_set_direction(arg->port, 0);
    gpio_set_pull_up(arg->port, 0);
    gpio_set_pull_down(arg->port, 0);

    /* gpio_set_fun_output_port(arg->port, FO_GP_OCH5, 0, 1); */
    /* SFR(JL_IOMC->OCH_CON0, 25, 5, 9);  //ledc_out to och5 */

    /* JL_OMAP->PA4_OUT |= ((0x5 << 2) | (0x3 << 0)); //OCH5 to PA4 */
    /* JL_IOMC->OCH_CON0 |= (0x9 << 25);  //LEDC_OUT to OCH5 */

    gpio_och_sel_output_signal(arg->port, OUTPUT_CH_SIGNAL_GP_LEDC);

    //std_48M
    JL_LEDCK->CLK &= ~(0b11 << 0);
    JL_LEDCK->CLK |= (0b10 << 0);
    //set div
    JL_LEDCK->CLK &= ~(0xff << 8);
    JL_LEDCK->CLK |= ((t_div[arg->t_unit] - 1) << 8);

    JL_LEDC->CON = BIT(6);  //CPND

    if (arg->cbfun) {
        JL_LEDC->CON |= BIT(5); //IE
        request_irq(IRQ_LEDC_IDX, IRQ_LEDC_IP, ledc_isr, 0);
        ledc_isr_cbfun = arg->cbfun;
    }

    if (arg->idle_level) {
        JL_LEDC->CON |= BIT(4);
    }
    if (arg->out_inv) {
        JL_LEDC->CON |= BIT(3);
    }

    JL_LEDC->CON |= (arg->bit_inv << 1);

    JL_LEDC->TIX = 0;
    JL_LEDC->TIX |= ((arg->t1h_cnt - 1) << 24);
    JL_LEDC->TIX |= ((arg->t1l_cnt - 1) << 16);
    JL_LEDC->TIX |= ((arg->t0h_cnt - 1) << 8);
    JL_LEDC->TIX |= ((arg->t0l_cnt - 1) << 0);

    JL_LEDC->RSTX = 0;
    JL_LEDC->RSTX |= (arg->t_rest_cnt << 8);

    log_info("JL_LEDCK->CLK = 0x%x\n", JL_LEDCK->CLK);
    log_info("JL_LEDC->CON = 0x%x\n", JL_LEDC->CON);
    log_info("JL_LEDC->TIX = 0x%x\n", JL_LEDC->TIX);
    log_info("JL_LEDC->RSTX = 0x%x\n", JL_LEDC->RSTX);
}

void ledc_send_rgbbuf(u8 *rgbbuf, u32 buf_len, u16 again_cnt)
{
    if (JL_LEDC->CON & BIT(5)) {  //IE
        while (ledc_busy);
    }
    JL_LEDC->ADR = (u32)rgbbuf;
    JL_LEDC->FD = buf_len * 8;
    JL_LEDC->LP = again_cnt;
    ledc_busy = 1;    	   //防止打断上一次发送的数据,用于异步工作方式
    JL_LEDC->CON |= BIT(0);//启动
    if (!(JL_LEDC->CON & BIT(5))) {
        while (!(JL_LEDC->CON & BIT(7)));
        JL_LEDC->CON |= BIT(6);
    }
}
#if 0
// *INDENT-OFF*
/*******************************    参考示例 ***********************************/
#define LED_NUM  10
static u8 ledc_test_buf[LED_NUM * 3] __attribute__((aligned(4)));
static void ledc_callback()
{
	putchar('c');
}

LEDC_PLATFORM_DATA_BEGIN(ledc0_data)
    .port = IO_PORTA_04,
    .idle_level = 0,
    .out_inv = 0,
    .bit_inv = 1,
    .t_unit = t_42ns,
    .t1h_cnt = 24,
    .t1l_cnt = 7,
    .t0h_cnt = 7,
    .t0l_cnt = 24,
    .t_rest_cnt = 200,
    .cbfun = ledc_callback,
LEDC_PLATFORM_DATA_END()

void delay_10ms(u32 tick);
void wdt_clear(void);
void ledc_test(void)
{
    log_info("*************  ledc test  **************\n");

    ledc_init(&ledc0_data);

	/* ledc_test_buf[0] = 0; */
	/* ledc_test_buf[1] = 85; */
	/* ledc_test_buf[2] = 170; */
	for (u8 i = 0; i < sizeof(ledc_test_buf); i++) {
		ledc_test_buf[i] = (u8)rand();
	}
	u16 again_cnt = 0;
    while (1) {
		wdt_clear();
		for (u8 i = 0; i < sizeof(ledc_test_buf); i++) {
			ledc_test_buf[i] = (u8)rand();
		}
		ledc_send_rgbbuf(ledc_test_buf, sizeof(ledc_test_buf), again_cnt);
		delay_10ms(10);
    }
}
#endif


