#include "pa_mute.h"
#include "cpu.h"

#define PA_MUTE_ENABLE  1

#define MUTE_PORT   JL_PORTA
#define MUTE_BIT    BIT(4)

void pa_mute(u8 mute)
{
#if PA_MUTE_ENABLE
    if (mute) {
        //开发板mute脚默认为高电平，I/O口设为高阻即可
        MUTE_PORT->PU0 &= ~MUTE_BIT;
        MUTE_PORT->PD0 &= ~MUTE_BIT;
        MUTE_PORT->DIE &= ~MUTE_BIT;
        MUTE_PORT->DIR |= MUTE_BIT;
    } else {
        //输出低电平
        MUTE_PORT->DIR &= ~MUTE_BIT;
        MUTE_PORT->OUT &= ~MUTE_BIT;
    }
#endif
}
