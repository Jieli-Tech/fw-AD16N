#ifndef __SRC_H__
#define __SRC_H__

#include "typedef.h"
#include "src_para.h"
#define SRC_MAX_POINT  (200)
#define SRC_MAX_OVER_POINT  10//(SRC_MAX_POINT * 10 / 100)
#define SRC_MAX_LEN    (SRC_MAX_POINT + 40)
#define SRC_MAX_BUFF   (SRC_MAX_LEN + 4)

#define SRC_RAM0_ADDR   (0x2f0340 + (96*4))
// #define SRC_RAM0_ADDR   (0x2f0340)
#define SRC_RAM0_LEN    (0x2f07c0 - SRC_RAM0_ADDR)
// #define SRC_RAM0_LEN    (96 * 2 * 4)(0x2f07c0 - SRC_RAM0_ADDR)

typedef struct _SRC_INFO {
    u32 phase;
    // u16 usfcoef_adr;
    u16 ramusf_adr;
    u8  ramflt_adr;
    u32 olen;
    // u32 i_addr;
    // u32 o_addr;
    u8  ram0[SRC_RAM0_LEN];
    u32 ram1[55];
    u8  flag;
    // s16 fltb_buf[24];
} SRC_INFO;

typedef struct _SRC_IO_CONTEXT {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} SRC_IO_CONTEXT;


#define BF_SRC_24BIT BIT(0)
typedef struct _SRC_PARA_STRUCT_ {
    unsigned short insample;
    unsigned short outsample;
    unsigned char flag;//BIT[0] == bits24
    unsigned char ch;
    signed short insample_inc;
    unsigned short max_in;
} SRC_PARA_STRUCT;

typedef struct  _SRC_STUCT_API_ {
    unsigned int(*need_buf)();
    int *(*open)(unsigned int *ptr, SRC_PARA_API *para, SRC_IO_CONTEXT *rsio);
    int (*run)(unsigned int *ptr, short *inbuf, int len);
    u32(*config)(void *work_buf, u32 cmd, void *parm);
} SRC_STUCT_API;


typedef struct _SRC_BUFF {
    SRC_IO_CONTEXT *io;
    SRC_PARA_STRUCT para;
    SRC_STUCT_API   *ops;
    SRC_INFO        info;
    u16             rmlen;
    u16             rmcnt;
    u8              buff[SRC_MAX_BUFF * 2];
} SRC_BUFF;

const SRC_STUCT_API *get_src_context(void);

enum SRC_CMD {
    SRC_CMD_NONE = 0,
    SRC_CMD_INSR_SET,
    SRC_CMD_INSR_INC_SET,
    SRC_CMD_INSR_INC_INIT,

};



#endif

