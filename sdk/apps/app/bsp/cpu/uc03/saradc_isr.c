#include "asm/power/p33_app.h"
#include "asm/power/p33.h"
#include "clock.h"
#include "gpio.h"
#include "saradc.h"

#define LOG_TAG_CONST         NORM
#define LOG_TAG               "[adc]"
#include "log.h"

struct _adc_info {
    u16 ch;         //ADC通道号
    u16 value;      //输出AD值
};

struct _adc_info adc_info[AD_MAX_CH];

static volatile u8 cur_ch = 0;
static volatile u8 init_flag = 0;
static volatile u8 time_div = 0;
static volatile u8 vbg_flag = 0;        //当JL_ADC->RES的数据为VBG的数据，该标志位至1
static u16 vbg_value = 0;
const u8 adc_clk_div_t[] = {
    1,     /*000*/
    6,     /*001*/
    12,    /*010*/
    24,    /*011*/
    48,    /*100*/
    72,    /*101*/
    96,    /*110*/
    128    /*111*/
};
#define ADC_MAX_CLK    1000000L

u8 adc_ch2port(u16 ch)
{
    if (ch > ADC_CH_USBDM) {
        return IO_PORT_MAX;
    }
    const u8 io_adcch_map_table[] = {
        IO_PORTA_00,
        IO_PORTA_04,
        IO_PORTA_06,
        IO_PORTA_08,
        IO_PORTA_10,
        IO_PORTB_00,
        IO_PORTB_02,
        IO_PORTB_04,
        IO_PORTB_06,
        IO_PORTB_07,
        IO_PORTB_08,
        IO_PORTC_00,
        IO_PORTC_01,
        IO_PORTC_03,
        IO_PORT_DP,
        IO_PORT_DM,
    };

    return io_adcch_map_table[ch];
}

void saradc_add_sample_ch(u16 real_ch)
{
    local_irq_disable();
    for (u8 i = 0; i < AD_MAX_CH; i++) {
        if (adc_info[i].ch == real_ch) {
            break;
        } else if (adc_info[i].ch == ((u16) - 1)) {
            adc_info[i].ch = real_ch;
            adc_info[i].value = AD_VALUE_NONE;
            log_info("add sample ch %x, chidx %d\n", real_ch, i);
            break;
        }
    }
    local_irq_enable();
}

void saradc_remove_sample_ch(u16 real_ch) //此处ch为头文件里的通道号
{
    local_irq_disable();
    for (u8 i = 0; i < AD_MAX_CH; i++) {
        if (adc_info[i].ch == real_ch) {
            adc_info[i].ch = -1;
            break;
        }
    }
    local_irq_disable();
}

static void adc_pmu_ch_select(u16 ch)
{
    u8 pmu_sub_ch;
//    P33_CON_SET(P3_PMU_ADC0, 0, 1, 1);
    P33_CON_SET(P3_PMU_ADC0, 1, 4, (ch & 0xf));
    if ((ch & 0xf) == 0) {
        pmu_sub_ch = (ch & ADC_PMU_CAL) >> 10;
        P33_CON_SET(P3_PMU_ADC1, 2, 2, pmu_sub_ch);
    }
    P33_CON_SET(P3_PMU_ADC0, 0, 1, 1);
}

static u8 adc_get_next_ch(u8 now_ch)
{
    for (u8 i = now_ch + 1; i < AD_MAX_CH; i++) {
        if (adc_info[i].ch != ((u16) - 1)) {
            return i;
        }
    }
    return 0;
}

SET(interrupt(""))
void saradc_isr()
{
    if (vbg_flag) {
        vbg_value = JL_ADC->RES;
        //log_info("ch %d, vbg %d\n", cur_ch, vbg_value);
        vbg_flag = 0;
    } else {
        adc_info[cur_ch].value = JL_ADC->RES;
        //adc_save_value(cur_ch, JL_ADC->RES);
    }

    //putchar('t');
    JL_ADC->CON = BIT(6);
    SFR(JL_ADDA->ADDA_CON0, 0, 10, 0);
    P33_CON_SET(P3_PMU_ADC0, 0, 1, 0);

}

static void adc_sample(u16 real_ch)
{
    u32 adc_con = 0;
    adc_con |= (0xf << 12);
    adc_con |= BIT(3);
    adc_con |= BIT(6);
    adc_con |= BIT(5);
    /**************通道选择*******************/
    if ((real_ch & ADC_MUX_CAL) == ADC_MUX_IO) {
        //log_info("sample io\n");
        SFR(adc_con, 8, 4, real_ch & 0xf);
        SFR(adc_con, 21, 3, 0b001);        //IO通道
    } else if ((real_ch & ADC_MUX_CAL) == ADC_MUX_AN) {
        //log_info("sample mux an\n");
        SFR(adc_con, 21, 3, 0b010);        //内部模拟通道
        if ((real_ch & ADC_AN_CAL) == ADC_AN_PMU) {
            //log_info("sample pmu\n");
            SFR(adc_con, 18, 3, 0b000);    //PMU
            if (real_ch != ADC_CH_PMU_MBG08) {
                adc_pmu_ch_select(real_ch);
            }
        }
    }

    //log_info("real_ch %x\n", real_ch);
    /*****************************************/
    adc_con |= time_div & 0x7;//
    JL_ADC->CON = adc_con;
    //log_info("JL_ADC->CON : 0x%x\n", JL_ADC->CON);
    JL_ADC->CON |= BIT(4);  //ctrl en
    JL_ADC->CON |= BIT(6);  //clr pnd
}

//ticktimer
void saradc_scan(void)
{
    if ((init_flag == 0) || vbg_flag) {
        return;
    }
    cur_ch = adc_get_next_ch(cur_ch); //轮询已注册通道
    //log_info(" %d\n",cur_ch);
    adc_sample(adc_info[cur_ch].ch);
}

u16 saradc_get_value(u16 real_ch)
{
    u8 i;
    if (real_ch == ADC_CH_PMU_MBG08) {
        return vbg_value;
    }

    for (i = 0; i < ARRAY_SIZE(adc_info); i++) {
        if (adc_info[i].ch == real_ch) {
            return adc_info[i].value;
        }
    }

    return ((u16) - 1);
}

void saradc_init(void)
{
    log_info("adc_init\n");
    u32 con = 0;
    memset(adc_info, 0xff, sizeof(adc_info));
    JL_ADC->CON = 0;
    int clk = clk_get("lsb");
    for (int j = 0; j < sizeof(adc_clk_div_t) / sizeof(adc_clk_div_t[0]); j++) {
        if (clk / adc_clk_div_t[j] <= ADC_MAX_CLK) {
            time_div |= j;
            //log_info("time_div 0x%x\n",time_div);
            break;
        }
    }

    adc_pmu_ch_select(ADC_CH_PMU_MBG08);
    P33_CON_SET(P3_PMU_ADC1, 0, 1, 1);//开启BUFFER
    udelay(100);
    P33_CON_SET(P3_PMU_ADC1, 1, 1, 1);//开启VBG2ADC
    adc_sample(ADC_CH_PMU_MBG08);
    for (u8 i = 0; i < 10; i++) {
        while (!(JL_ADC->CON & BIT(7)));
        vbg_value += JL_ADC->RES;
        JL_ADC->CON |= BIT(6);
    }
    vbg_value /= 10;
    P33_CON_SET(P3_PMU_ADC1, 1, 1, 0);//关闭VBG2ADC
    udelay(100);
    P33_CON_SET(P3_PMU_ADC1, 0, 1, 0);//关闭BUFFER

    //JL_ADC->CON &= ~BIT(4);  //ctrl en
    log_info("vbg value = %d\n", vbg_value);
    HWI_Install(IRQ_ADC_IDX, (u32)saradc_isr, IRQ_ADC_IP);
    init_flag = 1;
}

void adc_sample_vbg(void)
{
    if (init_flag == 0) {
        return;
    }
    local_irq_disable();
    JL_ADC->CON = BIT(6);
    vbg_flag = 1;
    //log_info("JL_ADC->CON : 0x%x\n", JL_ADC->CON);
    local_irq_enable();
    adc_pmu_ch_select(ADC_CH_PMU_MBG08);
    P33_CON_SET(P3_PMU_ADC1, 0, 1, 1);//开启BUFFER
    udelay(100);
    P33_CON_SET(P3_PMU_ADC1, 1, 1, 1);//开启VBG2ADC
    adc_sample(ADC_CH_PMU_MBG08);
    while (vbg_flag);
    P33_CON_SET(P3_PMU_ADC1, 1, 1, 0);//关闭VBG2ADC
    udelay(100);
    P33_CON_SET(P3_PMU_ADC1, 0, 1, 0);//关闭BUFFER
}

u32 saradc_value2voltage(u32 adc_vbg, u32 adc_ch_val)
{
    u32 adc_res = adc_ch_val;
    adc_res = adc_res * 800 / adc_vbg;
    return adc_res;
}

u16 saradc_get_voltage(u16 ch)
{
    u32 adc_res = saradc_get_value(ch);
    u32 adc_vbg = saradc_get_value(ADC_CH_PMU_MBG08);
    if (adc_vbg == 0) {
        return 0;
    }
    //log_info("adc_res %d, adc_vbg %d\n", adc_res, adc_vbg);
    return saradc_value2voltage(adc_vbg, adc_res);
}

u32 saradc_get_vbat_voltage(void)
{
    adc_sample_vbg();
    u32 vbat_voltage = 4 * saradc_get_voltage(ADC_CH_PMU_1_4_VBAT);
    return vbat_voltage;
}
