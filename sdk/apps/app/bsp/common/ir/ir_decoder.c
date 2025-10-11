#include "ir_decoder.h"
#include "gptimer.h"
#include "sys_timer.h"
#include "key_ir.h"
#include "key.h"
#include "tick_timer_driver.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[ir_encode]"
#include "log.h"


#define IRFLT_TEST_ENABLE   0

#define NEC_DATA_CNT        32
#define NEC_TIME_DEVIATION  10 //允许10%偏差
#define NEC_UNIT_TIME       563
#define NEC_UNIT_TIME_MAX   (NEC_UNIT_TIME * (100 + NEC_TIME_DEVIATION) / 100)
#define NEC_UNIT_TIME_MIN   (NEC_UNIT_TIME * (100 - NEC_TIME_DEVIATION) / 100)
#define NEC_HEADER_TIME     ((16 + 8) * NEC_UNIT_TIME)
#define NEC_HEADER_TIME_MAX (NEC_HEADER_TIME * (100 + NEC_TIME_DEVIATION) / 100)
#define NEC_HEADER_TIME_MIN (NEC_HEADER_TIME * (100 - NEC_TIME_DEVIATION) / 100)
#define NEC_BIT_0_TIME      ((1 + 1) * NEC_UNIT_TIME)
#define NEC_BIT_0_TIME_MAX  (NEC_BIT_0_TIME * (100 + NEC_TIME_DEVIATION) / 100)
#define NEC_BIT_0_TIME_MIN  (NEC_BIT_0_TIME * (100 - NEC_TIME_DEVIATION) / 100)
#define NEC_BIT_1_TIME      ((3 + 1) * NEC_UNIT_TIME)
#define NEC_BIT_1_TIME_MAX  (NEC_BIT_1_TIME * (100 + NEC_TIME_DEVIATION) / 100)
#define NEC_BIT_1_TIME_MIN  (NEC_BIT_1_TIME * (100 - NEC_TIME_DEVIATION) / 100)
#define NEC_REPEAT_TIME     110
#define NEC_REPEAT_TIME_MAX (NEC_REPEAT_TIME * (100 + NEC_TIME_DEVIATION) / 100)
#define NEC_REPEAT_TIME_MIN (NEC_REPEAT_TIME * (100 - NEC_TIME_DEVIATION) / 100)
#define NEC_TIMEOUT         70 //65.0625ms
#define NEC_JIFFIES_MS      10 //1个jiffies为10ms

enum nec_state {
    NEC_STATE_IDLE,
    NEC_STATE_WAIT_HEADER,
    NEC_STATE_DATA,
};
enum nec_data_type {
    NEC_DATA_ERROR = 0,
    NEC_DATA_HEADER,
    NEC_DATA_BIT_1,
    NEC_DATA_BIT_0,
    NEC_DATA_REPEAT,
};
struct irft_data {
    u8 state;
    u8 cnt;
    u32 jiffies;
    u32 data;
    u8 address;
    u8 address_not;
    u8 command;
    u8 command_not;
};
static int ir_timer_id;
static struct irft_data *ir_data;
static struct irft_data _ir_data;
static u32 irflt_get_jiffies()
{
    return NEC_JIFFIES_MS * maskrom_get_jiffies();
}
static u32 irflt_pulse_check(u32 us)
{
    u32 ret;
    /* log_info("us = %d\n", us); */
    if ((NEC_HEADER_TIME_MIN < us) && (us < NEC_HEADER_TIME_MAX)) {
        ret = NEC_DATA_HEADER;
    } else if ((NEC_BIT_0_TIME_MIN < us) && (us < NEC_BIT_0_TIME_MAX)) {
        ret = NEC_DATA_BIT_0;
    } else if ((NEC_BIT_1_TIME_MIN < us) && (us < NEC_BIT_1_TIME_MAX)) {
        ret = NEC_DATA_BIT_1;
    } else if ((NEC_REPEAT_TIME_MIN < us) && (us < NEC_REPEAT_TIME_MAX)) { //ms
        ret = NEC_DATA_REPEAT;
    } else {
        ret = NEC_DATA_ERROR;
    }
    return ret;
}

static void ir_isr_cb(int tid)
{
    ASSERT(ir_data != NULL, "%s()\n", __func__);
    u32 ret = 0;
    u32 cnt = gptimer_get_capture_count(ir_timer_id);
    u32 us = gptimer_tick2us(ir_timer_id, cnt);
    gptimer_set_count(ir_timer_id, 0);
    int time_sub = irflt_get_jiffies() - ir_data->jiffies;
    if (time_sub > NEC_TIMEOUT + 10) {
        ir_data->state = NEC_STATE_IDLE;
        ir_data->jiffies = irflt_get_jiffies();
    }
    switch (ir_data->state) {
    case NEC_STATE_IDLE:
        if (time_sub > NEC_TIMEOUT) {
            if (irflt_pulse_check(time_sub) == NEC_DATA_REPEAT) {
                ir_data->address = (ir_data->data >> 0) & 0xff;
                ir_data->address_not = (ir_data->data >> 8) & 0xff;
                ir_data->command = (ir_data->data >> 16) & 0xff;
                ir_data->command_not = (ir_data->data >> 24) & 0xff;
#if IRFLT_TEST_ENABLE
                log_info("repeat_data:0x%x\n", ir_data->data);
                log_info("address:0x%x\n", ir_data->address);
                log_info("address_not:0x%x\n", ir_data->address_not);
                log_info("command:0x%x\n", ir_data->command);
                log_info("command_not:0x%x\n", ir_data->command_not);
#endif
            } else {
                ir_data->state = NEC_STATE_WAIT_HEADER;
            }
            ir_data->jiffies = irflt_get_jiffies();
        }
        break;
    case NEC_STATE_WAIT_HEADER:
        ret = irflt_pulse_check(us);
        if (ret == NEC_DATA_HEADER) {
            ir_data->state = NEC_STATE_DATA;
            ir_data->cnt = 0;
        } else {
            ir_data->state = NEC_STATE_IDLE;
        }
        break;
    case NEC_STATE_DATA:
        ret = irflt_pulse_check(us);
        if (ret == NEC_DATA_BIT_1) {
            ir_data->data >>= 1;
            ir_data->data |= 0x80000000;
            ir_data->cnt++;
        } else if (ret == NEC_DATA_BIT_0) {
            ir_data->data >>= 1;
            ir_data->cnt++;
        } else {
            ir_data->state = NEC_STATE_IDLE;
            /* ir_data->data = 0; */
            ir_data->cnt = 0;
        }
        if (ir_data->cnt == NEC_DATA_CNT) {
            ir_data->cnt = 0;
            ir_data->state = NEC_STATE_IDLE;
            ir_data->address = (ir_data->data >> 0) & 0xff;
            ir_data->address_not = (ir_data->data >> 8) & 0xff;
            ir_data->command = (ir_data->data >> 16) & 0xff;
            ir_data->command_not = (ir_data->data >> 24) & 0xff;
            ir_data->jiffies = irflt_get_jiffies();
#if IRFLT_TEST_ENABLE
            log_info("data:0x%x\n", ir_data->data);
            log_info("address:0x%x\n", ir_data->address);
            log_info("address_not:0x%x\n", ir_data->address_not);
            log_info("command:0x%x\n", ir_data->command);
            log_info("command_not:0x%x\n", ir_data->command_not);
#endif
        }
        break;
    default:
        break;
    }

}

#if IRFLT_TEST_ENABLE
static u32 key_cnt = 0;
static void irflt_test_func(void *priv)
{
    u8 key = irflt_get_command_value();
    if (key != NEC_DATA_ERROR) {
        key_cnt++;
        log_info("key:0x%x,  num%d\n", key, key_cnt);
    }
}
#endif

void irflt_init(u32 port)
{
    log_info("%s()\n", __func__);
    if (ir_data == NULL) {
        /* ir_data = malloc(sizeof(struct irft_data)); */
        ir_data = &_ir_data;
        ASSERT(ir_data != NULL, "%s()\n", __func__);
        memset(ir_data, 0, sizeof(struct irft_data));
        ir_data->state = NEC_STATE_IDLE;
        ir_data->jiffies = irflt_get_jiffies();
    }
    struct gptimer_capture_config ir_capture_config;
    ir_capture_config.port = port / 16;
    ir_capture_config.pin = port % 16;
    ir_capture_config.edge_type = GPTIMER_CAPTRUE_EDGE_FAILL;
    ir_capture_config.irq_cb = ir_isr_cb;
    ir_capture_config.filter = 38000;//93750; //48m/512
    ir_capture_config.tid = IR_KEY_TIMER_IDX;
    ir_capture_config.irq_priority = IRQ_IRTMR_IP;

    ir_timer_id = gptimer_capture_init(&ir_capture_config);
    log_info("ir_timer_id = %d\n", ir_timer_id);
    gpio_set_mode(IO_PORT_SPILT(port), PORT_INPUT_PULLUP_10K);//开上拉
    gptimer_start(ir_timer_id);

#if IRFLT_TEST_ENABLE
    /* JL_PORTA->DIR &= ~BIT(10); */
    log_info("header:%d, header_min:%d, header_max:%d\n",
             NEC_HEADER_TIME, NEC_HEADER_TIME_MIN, NEC_HEADER_TIME_MAX);
    log_info("bit0:%d, bit0_min:%d, bit0_max:%d\n",
             NEC_BIT_0_TIME, NEC_BIT_0_TIME_MIN, NEC_BIT_0_TIME_MAX);
    log_info("bit1:%d, bit1_min:%d, bit1_max:%d\n",
             NEC_BIT_1_TIME, NEC_BIT_1_TIME_MIN, NEC_BIT_1_TIME_MAX);
    log_info("repeat:%d, repeat_min:%d, repeat_max:%d\n",
             NEC_REPEAT_TIME, NEC_REPEAT_TIME_MIN, NEC_REPEAT_TIME_MAX);
    /* usr_timer_add(NULL, irflt_test_func, 10, 1); */
#endif
}

u8 irflt_get_command_value(void)
{
    if (ir_data == NULL) {
        log_error("irflt not init!\n");
        return NO_KEY;
    }
    u8 command = ir_data->command;
    u8 command_not = ir_data->command_not;
    /* log_info("command = 0x%x, command_not = 0x%x\n", command, command_not); */
    if (command != NEC_DATA_ERROR) {
        if ((command ^ command_not) == 0xff) {
            int time_sub = irflt_get_jiffies() - ir_data->jiffies;
            if (time_sub > NEC_TIMEOUT + 10) {
                ir_data->command = NEC_DATA_ERROR;
                ir_data->command_not = NEC_DATA_ERROR;
            }
            return command;
        }
    }
    return NO_KEY;
}

