#include "pl_cnt.h"
static const struct pl_cnt_platform_data *__this = NULL;
static JL_PORT_TypeDef *PL_CNT_IO_PORTx = NULL;
static u8 PL_CNT_IO_xx;


void pl_cnt_iomc(u8 port)
{
    u8 input_start;
    if (port > IO_MAX_NUM) {
        return;
    } else if (port < 17)  {
        input_start = 1;
        PL_CNT_IO_PORTx = JL_PORTA;
    } else if ((16 < port) & (port < 28)) {
        input_start = 17;
        PL_CNT_IO_PORTx = JL_PORTB;
    } else if ((27 < port) & (port < 34)) {
        input_start = 28;
        PL_CNT_IO_PORTx = JL_PORTC;
    } else {
        return;
    }
    PL_CNT_IO_xx = port % 16;
    //放电计数引脚选择intputchannel0

    JL_IOMC->ICH_CON1 &= ~(0b1111 << 0);
    JL_IOMC->ICH_CON1 |= 5;    //选择功能输入通道GP_ICH5
    JL_IMAP->FI_GP_ICH5 = 0;
    JL_IMAP->FI_GP_ICH5 |= ((input_start + PL_CNT_IO_xx) << 0);        //通道选择端口
}

/*
   @brief   引脚放电计数模块初始化
*/
void pl_cnt_init(const struct pl_cnt_platform_data *pdata)
{
    __this = pdata;
    if (!__this) {
        return;
    }

    for (u8 i = 0; i < pdata->port_num; i ++) {
        pl_cnt_iomc(pdata->port[i]);
        //引脚先给寄生电容充电
        PL_CNT_IO_PORTx->DIE |=  BIT(PL_CNT_IO_xx);
        PL_CNT_IO_PORTx->OUT |=  BIT(PL_CNT_IO_xx);
        PL_CNT_IO_PORTx->DIR &= ~BIT(PL_CNT_IO_xx);
        //放电时，关上拉，开下拉
        PL_CNT_IO_PORTx->PU0 &= ~BIT(PL_CNT_IO_xx);
        PL_CNT_IO_PORTx->PD0 |=  BIT(PL_CNT_IO_xx);
    }
    JL_PCNT->CON = 0;
    JL_PCNT->CON |= (0b11 << 2);      //最好选择PLL_d1p0为时钟源
    JL_PCNT->CON |= BIT(1);           //引脚放电计数使能

}


extern void delay(volatile u32 t);
u32 get_pl_cnt_value(u8 ch)
{
    if (!__this) {
        return 0;
    }
    pl_cnt_iomc(__this->port[ch]);
    u32 tmp_val, start_val, end_val, sum_val = 0;
    for (u8 i = 0; i < __this->sum_num; i ++) {
        delay(__this->charge_time);                     //确保充满电
        start_val = JL_PCNT->VAL;                     //保存开始计数值
        PL_CNT_IO_PORTx->DIR |= BIT(PL_CNT_IO_xx);      //切换方向，下拉电阻起作用，此时开始放电，放电计数模块开始计数
        delay(__this->charge_time);                     //确保放电结束
        end_val = JL_PCNT->VAL;                       //保存结束计数值
        PL_CNT_IO_PORTx->DIR &= ~BIT(PL_CNT_IO_xx);     //切换方向，输出1，即又开始对寄生电容充电
        if (end_val > start_val) {
            tmp_val = end_val - start_val;
        } else {
            tmp_val = (u32) - start_val + end_val;
        }
        sum_val += tmp_val;
    }
    sum_val = sum_val / __this->sum_num;
    return sum_val;
}



