#ifndef __MIO_API_H__
#define __MIO_API_H__
#include "typedef.h"
#include "app_modules.h"
//-- number of MIO channel
#define MIO_MAX_CHL         16
#define MIO_MAX_CHL_PWM     1	//4
#define MIO_MAX_CHL_IO      (MIO_MAX_CHL-MIO_MAX_CHL_PWM)
#define MIO_MAX_RBUF        (MIO_MAX_CHL_PWM+(MIO_MAX_CHL_IO+7)/8)

//-- MIO logo
#define MIO_LOGO            0X55AA1212
#define MIO_VER_V1_1        0X00000200


#define B_MIO_EN     BIT(0)
#define B_MIO_START  BIT(1)
#define B_MIO_KICK   BIT(2)

#define B_MIO_ERR    BIT(7)


//-- MIO struct
struct mio_info {
    u32 logo;
    u32 version;
    u32 data_len;
    u8 level;
    u8 remain;
    u16 rate;
    u8 pwm_total;
    u8 io_total;
    u8 remain1[2];
};

typedef struct _sound_mio_obj {
    u8 status;
    u16 io_mask;
    u8  r_buf[MIO_MAX_RBUF];
    u16 r_size;
    u16 dac_step;
    u32 dac_cnt;
    u32 dac_used_cnt;
    struct mio_info info;
    void *pfile;
    u32(*read)(void *, u8 *, u32);
    void (*pwm_init)(u32);
    void (*pwm_run)(u32, u32);
    void (*io_init)(u32);
    void (*io_run)(u32, u32);
} sound_mio_obj;



void mio_a_hook_init(sound_mio_obj *obj);

__attribute__((weak)) void mio_module_init(void);
__attribute__((weak)) void mio_start(void *mio_obj);
__attribute__((weak)) void mio_kick(void *mio_obj, u32 dac_packt);
__attribute__((weak)) void mio_run(void);
__attribute__((weak)) bool mio_open(void **pp_obj, void *pfile, void *);
__attribute__((weak)) void mio_close(void **pp_obj);


// extern const char MIO_ENABLE;
// #define d_mio_module_init() if(MIO_ENABLE) { mio_module_init();}
// #define d_mio_start(n)      if(MIO_ENABLE) { mio_start(n);}
// #define d_mio_kick(n,m)     if(MIO_ENABLE) { mio_kick(n,m);}
// #define d_mio_run()         if(MIO_ENABLE) { mio_run();}
// #define d_mio_open(n,m,k)   if(MIO_ENABLE) { mio_open(n,m,k);}
// #define d_mio_close(n)      if(MIO_ENABLE) { mio_close(n);}

#if HAS_MIO_EN

#define d_mio_module_init   mio_module_init
#define d_mio_a_hook_init   mio_a_hook_init
#define d_mio_start         mio_start
#define d_mio_kick          mio_kick
#define d_mio_run           mio_run
#define d_mio_open          mio_open
#define d_mio_close         mio_close

#else
#define d_mio_module_init(...)
#define d_mio_a_hook_init(...)
#define d_mio_start(...)
#define d_mio_kick(...)
#define d_mio_run(...)
#define d_mio_open(...)
#define d_mio_close(...)

#endif




#endif

