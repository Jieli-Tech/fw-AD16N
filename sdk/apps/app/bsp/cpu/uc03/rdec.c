#include "config.h"
#include "common.h"
#include "gpio.h"
#include "clock.h"
#include "rdec.h"
#include "log.h"

#define LOG_TAG             "[rdec]"

struct  rdec {
    u8 init;
    const struct rdec_platform_data  *user_data;
};
static  struct  rdec _rdec = {0};
#define  __this     (&_rdec)

typedef struct {
    u32 con;
    int dat;
    /* int smp; */
} RDEC_REG;

RDEC_REG *rdec_get_reg(u8 index)
{
    RDEC_REG *reg = NULL;
    reg = (RDEC_REG *)JL_RDEC0;
    return reg;
}

static void __rdec_port_init(u8 port)
{
    gpio_set_pull_down(port, 0);
    gpio_set_pull_up(port, 1);
    gpio_set_die(port, 1);
    gpio_set_direction(port, 1);

}
___interrupt
static void rdec_isr()
{
    s8 rdat = 0;
    s8 rdat1 = 0;
    RDEC_REG *reg = NULL;
    reg = rdec_get_reg(0);
    if (reg->con & BIT(7)) {
        asm("csync");
        rdat = reg->dat;
        reg->con |= BIT(6);//clr pending,DAT reg will update as well
        asm("csync");
        rdat1 = reg->dat;
        log_debug("RDEC isr,dat: %d,%d", rdat, rdat1);
    }
}
static void rdec_port_init(const struct rdec_device *rdec)
{
    __rdec_port_init(rdec->sin_port0);
    __rdec_port_init(rdec->sin_port1);

    log_info("rdec->sin_port0 = %d\n", rdec->sin_port0);
    log_info("rdec->sin_port1 = %d\n", rdec->sin_port1);

    gpio_set_fun_input_port(rdec->sin_port0, PFI_RDEC0_DAT0);
    gpio_set_fun_input_port(rdec->sin_port1, PFI_RDEC0_DAT1);
}
static void log_rdec_info()
{
    RDEC_REG *reg = NULL;
    reg = rdec_get_reg(0);
    log_debug("RDEC CON = 0x%x\n", reg->con);
    log_info("FI_RDEC_DA0 = 0x%x\n", JL_IMAP->FI_RDEC0_DAT0);
    log_info("FI_RDEC_DA1 = 0x%x\n", JL_IMAP->FI_RDEC0_DAT1);

    log_info("PA_DIR = 0x%x\n", JL_PORTA->DIR);
    log_info("PA_OUT = 0x%x\n", JL_PORTA->OUT);
    log_info("PA_PU = 0x%x\n", JL_PORTA->PU0);
    log_info("PA_PD = 0x%x\n", JL_PORTA->PD0);

}

int rdec_init(const struct rdec_platform_data *user_data)
{
    RDEC_REG *reg = NULL;
    log_debug("rdec init...");
    __this->init = 0;
    if (user_data == NULL) {
        return -1;
    }
    reg = rdec_get_reg(0);
    rdec_port_init(&(user_data->rdec[0]));
    //rdec init
    reg->con = 0;
    u32 lsb_clk = clk_get("lsb") / 1000;
    u8 rdec_spd = 15;
    while (rdec_spd < 0x10) {
        u32 t = 1 << rdec_spd;
        if ((10 * t / lsb_clk < 2 * 10) || (rdec_spd == 0)) {
            break;
        }
        rdec_spd --;
    }
    /* log_info("rdec_spd = %d\n",rdec_spd);  */
    reg->con |= rdec_spd << 2;//Tsr：0.68-2ms
    reg->con &= ~BIT(1); //io pull up
    reg->con |= BIT(6);//clear pending
    reg->con |= BIT(8);//mode:mouse time
    reg->con |= BIT(0);//rdec en

    request_irq(IRQ_RDEC0_IDX, 1, rdec_isr, 0);
    __this->init = 1;
    __this->user_data = user_data;
    log_rdec_info();
    return 0;
}


const struct rdec_device rdec_device_list[] = {
    {
        .index = 0,
        .sin_port0 = IO_PORTA_01,
        .sin_port1 = IO_PORTA_02,
        .key_event = 2,

    },
};

RDEC_PLATFORM_DATA_BEGIN(rdec_data)
.enable = 1,
 .num = ARRAY_SIZE(rdec_device_list),
  .rdec = rdec_device_list,
   RDEC_PLATFORM_DATA_END()

   void rdec_test()
{
    rdec_init(&rdec_data);
    while (1);
}
