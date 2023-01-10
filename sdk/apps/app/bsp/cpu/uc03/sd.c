#include "sd.h"
#include "gpio.h"
#include "clock.h"
#include "asm/power/p33_app.h"
#include "asm/power/p33.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[sd]"
#include "log.h"

extern void delay(u32 i);
extern const struct sdmmc_platform_data sd0_data;



#define PORT_IN     BIT(0)
#define PORT_OUT    BIT(1)
#define PORT_DIR    BIT(2)
#define PORT_DIE    BIT(3)
#define PORT_DIEH   BIT(4)
#define PORT_PU0    BIT(5)
#define PORT_PD0    BIT(6)
#define PORT_HD0    BIT(7)
#define PORT_HD1    BIT(8)

#define PORTA 0
#define PORTB 1
#define PORTC 2
#define PORTD 3

#define EQU_PORT(n,m,i)    ((n==m)?BIT(i) : 0)
#define GPIO_CONST_TAB(NAME,port,i)   \
const int NAME[4] = {           \
	EQU_PORT(PORTA, port, i),         \
	EQU_PORT(PORTB, port, i),         \
	EQU_PORT(PORTC, port, i),         \
	EQU_PORT(PORTD, port, i)          \
}

#define porta_value(value, clk, cmd, data0) \
		value = clk[0] | cmd[0] | data0[0];

#define portb_value(value, clk, cmd, data0) \
		value = clk[1] | cmd[1] | data0[1];

#define portc_value(value, clk, cmd, data0) \
		value = clk[2] | cmd[2] | data0[2];

#define portd_value(value, clk, cmd, data0) \
		value = clk[3] | cmd[3] | data0[3];

GPIO_CONST_TAB(sdmmc_cmd,   SDMMC_CMD_PORT, SDMMC_CMD_PORT_IM);
GPIO_CONST_TAB(sdmmc_clk,   SDMMC_CLK_PORT, SDMMC_CLK_PORT_IM);
GPIO_CONST_TAB(sdmmc_data0, SDMMC_DAT_PORT, SDMMC_DAT_PORT_IM);



#define PORT_INIR_STATUS(port, n)  \
    port->DIE |= n;  \
    port->OUT |= n;  \
    port->DIR &= ~n;  \
    port->HD1 &= ~n; \
    port->PU0 |= n;  \
    port->PD0 &= ~n;

__noinline
static void sd_gpio_all_init(const struct sdmmc_platform_data *data)
{
    /* #if 0 */
    /* for (u8 i = 0; i < 3; i ++) { */
    /* gpio_set_die(data->port[i], 1); */
    /* gpio_direction_output(data->port[i], 1); */
    /* gpio_set_hd(data->port[i], 0); */
    /* gpio_set_pull_up(data->port[i], 1); */
    /* gpio_set_pull_down(data->port[i], 0); */
    /* } */
    /* #else  */
    /* #endif */
#if 1
    u32 tmp;

    porta_value(tmp, sdmmc_cmd, sdmmc_clk, sdmmc_data0);
    if (tmp) {
        PORT_INIR_STATUS(JL_PORTA, tmp);
    }
    portb_value(tmp, sdmmc_cmd, sdmmc_clk, sdmmc_data0);
    if (tmp) {
        PORT_INIR_STATUS(JL_PORTB, tmp);
    }
    portc_value(tmp, sdmmc_cmd, sdmmc_clk, sdmmc_data0);
    if (tmp) {
        PORT_INIR_STATUS(JL_PORTC, tmp);
    }
    portd_value(tmp, sdmmc_cmd, sdmmc_clk, sdmmc_data0);
    if (tmp) {
        PORT_INIR_STATUS(JL_PORTD, tmp);
    }
#else
    JL_PORTC->DIE |= (BIT(1) | BIT(2) | BIT(0));
    JL_PORTC->DIR &= ~(BIT(1) | BIT(2) | BIT(0));
    JL_PORTC->OUT |= (BIT(1) | BIT(2) | BIT(0));
    JL_PORTC->HD1 &= ~(BIT(1) | BIT(2) | BIT(0));
    JL_PORTC->PU0 |= (BIT(1) | BIT(2) | BIT(0));
    JL_PORTC->PD0 &= ~(BIT(1) | BIT(2) | BIT(0));
#endif
}

static void sd_gpio_iomap_init(const struct sdmmc_platform_data *data)
{
    /* data = &sd0_data;  */
    //CMD
    gpio_set_fun_output_port(data->port[0], FO_SD0_CMD, 1, 1);
    gpio_set_fun_input_port(data->port[0], PFI_SD0_CMD);
    //CLK
    gpio_set_fun_output_port(data->port[1], FO_SD0_CLK, 1, 1);
    //DA0
    gpio_set_fun_output_port(data->port[2], FO_SD0_DA0, 1, 1);
    gpio_set_fun_input_port(data->port[2], PFI_SD0_DA0);
}

static void sd_gpio_init_0(const struct sdmmc_platform_data *data)
{
    /* data = &sd0_data;  */
    sd_gpio_all_init(data);
    sd_gpio_iomap_init(data);
}

#define PORT_HI_Z(port, n)  \
    port->DIE &= ~n;  \
    port->DIR |= n;  \
    port->PU0 &= ~n;  \
    port->PD0 &= ~n;

static void sd_gpio_uninit_3(const struct sdmmc_platform_data *data)
{
    /* data = &sd0_data;  */


    gpio_disable_fun_input_port(PFI_SD0_CMD);
    gpio_disable_fun_input_port(PFI_SD0_DA0);
    for (u8 i = 0; i < 3; i ++) {
        gpio_disable_fun_output_port(data->port[i]);
        /* gpio_set_pull_up(data->port[i], 0); */
        /* gpio_set_pull_down(data->port[i], 0); */
        /* gpio_set_direction(data->port[i], 1); */
        /* gpio_set_die(data->port[i], 0); */
    }
#if 1
    u32 tmp;

    porta_value(tmp, sdmmc_cmd, sdmmc_clk, sdmmc_data0);
    if (tmp) {
        PORT_HI_Z(JL_PORTA, tmp);
    }
    portb_value(tmp, sdmmc_cmd, sdmmc_clk, sdmmc_data0);
    if (tmp) {
        PORT_HI_Z(JL_PORTB, tmp);
    }
    portc_value(tmp, sdmmc_cmd, sdmmc_clk, sdmmc_data0);
    if (tmp) {
        PORT_HI_Z(JL_PORTC, tmp);
    }
    portd_value(tmp, sdmmc_cmd, sdmmc_clk, sdmmc_data0);
    if (tmp) {
        PORT_HI_Z(JL_PORTD, tmp);
    }
    /* #else  */
    /* JL_PORTC->DIE &= ~(BIT(1) | BIT(2) | BIT(0)); */
    /* JL_PORTC->DIR |= (BIT(1) | BIT(2) | BIT(0)); */
    /* JL_PORTC->PU0 &= ~(BIT(1) | BIT(2) | BIT(0)); */
    /* JL_PORTC->PD0 &= ~(BIT(1) | BIT(2) | BIT(0)); */

#endif
}

static void sd_gpio_suspend_4(const struct sdmmc_platform_data *data)
{
    /* data = &sd0_data;  */
    gpio_disable_fun_output_port(data->port[0]);
    gpio_disable_fun_output_port(data->port[1]);
    gpio_disable_fun_output_port(data->port[2]);
    gpio_disable_fun_input_port(PFI_SD0_CMD);
    gpio_disable_fun_input_port(PFI_SD0_DA0);
}

static void sd_gpio_resume_5(const struct sdmmc_platform_data *data)
{
    /* data = &sd0_data;  */
    sd_gpio_all_init(data);
    struct gpio_reg *g = gpio2reg(data->port[1]);
    u32 mask = BIT(data->port[1] % 16);
    for (u32 i = 0; i < 50; i ++) {
        g->out ^= mask;
    }
    sd_gpio_iomap_init(data);
}

void sdmmc_0_port_init(const struct sdmmc_platform_data *data, int mode)
{
    data = &sd0_data;
    if (mode == 0) {
        sd_gpio_init_0(data);
    } else if (mode == 3)	{
        sd_gpio_uninit_3(data);
    } else if (mode == 4)	{
        sd_gpio_suspend_4(data);
    } else if (mode == 5)	{
        sd_gpio_resume_5(data);
    }
}
int sdmmc_0_clk_detect(const struct sdmmc_platform_data *data)
{
    int val = 0;
    u8 res = 0;
    data = &sd0_data;
    u32 *sd_clk_outreg = gpio2crossbar_outreg(data->port[1]);
    u32 data_dir_ctl = (*sd_clk_outreg) & 0b11;
    if (data_dir_ctl) {
        (*sd_clk_outreg) &= ~(0b11);
        res = 1;
    }
    gpio_set_die(data->port[1], 1);
    gpio_set_direction(data->port[1], 1);
    if (data->detect_io_level) {
        gpio_set_pull_up(data->port[1], 0);
        gpio_set_pull_down(data->port[1], 1);
        delay(100);
        val = gpio_read(data->port[1]);
        gpio_set_pull_down(data->port[1], 0);
    } else {
        gpio_set_pull_up(data->port[1], 1);
        gpio_set_pull_down(data->port[1], 0);
        delay(100);
        val = gpio_read(data->port[1]);
        val = !val;
        gpio_set_pull_up(data->port[1], 0);
    }
    if (res) {
        (*sd_clk_outreg) |= data_dir_ctl;
        if (((*sd_clk_outreg) >> 2) == FO_SD0_CLK) {
            gpio_write(data->port[1], 0);
            gpio_set_direction(data->port[1], 0);
        }
    }
    return val;
}

int sdmmc_0_io_detect(const struct sdmmc_platform_data *data)
{
    data = &sd0_data;
    int val = 0;
    u8 res = 0;
    u32 *det_io_outreg = gpio2crossbar_outreg(data->detect_io);
    u32 data_dir_ctl = (*det_io_outreg) & 0b11;
    if (data_dir_ctl) {
        (*det_io_outreg) &= ~(0b11);
        res = 1;
    }
    gpio_set_die(data->detect_io, 1);
    gpio_set_direction(data->detect_io, 1);
    if (data->detect_io_level) {
        gpio_set_pull_up(data->detect_io, 0);
        gpio_set_pull_down(data->detect_io, 1);
        delay(100);
        val = gpio_read(data->detect_io);
        gpio_set_pull_down(data->detect_io, 0);
    } else {
        gpio_set_pull_up(data->detect_io, 1);
        gpio_set_pull_down(data->detect_io, 0);
        delay(100);
        val = gpio_read(data->detect_io);
        val = !val;
        gpio_set_pull_up(data->detect_io, 0);
    }
    if (res) {
        (*det_io_outreg) |= data_dir_ctl;
    }
    return val;
}

int sdmmc_0_cmd_detect(const struct sdmmc_platform_data *data)
{
    return CMD_DECT_WITHOUT_TRANS;
}

static void sdpg_config(int enable)
{
    if (enable) {
        P33_CON_SET(P3_PGSD_CON, 0, 4, 0);
        JL_PORTC->DIE |=  BIT(3);
        JL_PORTC->HD0 &= ~BIT(3);
        JL_PORTC->HD1 &= ~BIT(3);
        JL_PORTC->OUT |=  BIT(3);
        JL_PORTC->DIR &= ~BIT(3);
        udelay(10000);//10ms
        JL_PORTC->HD0 |=  BIT(3);
        udelay(10000);//10ms
        JL_PORTC->HD1 |=  BIT(3);
        udelay(10000);//10ms
        P33_CON_SET(P3_PGSD_CON, 0, 4, 1);
        udelay(10000);//10ms
    } else {
        P33_CON_SET(P3_PGSD_CON, 0, 4, 0);
        JL_PORTC->DIR |=  BIT(3);
        JL_PORTC->PU0 &= ~BIT(3);
        JL_PORTC->PD0 &= ~BIT(3);
        JL_PORTC->HD0 &= ~BIT(3);
        JL_PORTC->HD1 &= ~BIT(3);
        JL_PORTC->DIE &= ~BIT(3);
    }
}

int set_sd_power(int en)
{
    static u8 power_old = 0xff;
    if (power_old != en) {
        power_old = en;
        sdpg_config(en);
    }
    /* log_info("P3_PGSD_CON : 0x%x\n", P3_PGSD_CON); */
    return 0;
}
