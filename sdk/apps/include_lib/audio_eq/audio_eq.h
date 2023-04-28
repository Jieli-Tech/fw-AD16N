#ifndef __EQ_H__
#define __EQ_H__
#include "typedef.h"
#include "hw_eq.h"
#include "sound_mge.h"

#define EQ_MAX_POINT    (128)
#define EQ_SECTION_MAX  10
#define EQ_COEFF_BASE   (0x2F0000)

#define EQ_RUN_MODE         NORMAL
#define EQ_IN_FORMAT        DATI_SHORT
#define EQ_OUT_FORMAT       DATO_SHORT
#define EQ_DATA_IN_MODE     SEQUENCE_DAT_IN
#define EQ_DATA_OUT_MODE    SEQUENCE_DAT_OUT

#define B_EQ_START          BIT(0)
#define B_EQ_FADE_IN        BIT(1)
#define B_EQ_FADE_OUT       BIT(2)

typedef struct _EQ_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} EQ_IO_CONTEXT;

typedef struct _EQ_PARA_STRUCT_ {
    struct eq_seg_info *seg;
    struct eq_seg_info *next_seg;
    float global_gain;
    float next_global_gain;
    unsigned short sample_rate;
    unsigned char max_nsection;
    unsigned char ch;
} EQ_PARA_STRUCT;

typedef struct  _EQ_STUCT_API_ {
    unsigned int(*need_buf)(int max_eq_seg, int ch);
    int *(*open)(unsigned int *ptr, EQ_PARA_STRUCT *para, EQ_IO_CONTEXT *rsio);
    int (*run)(unsigned int *ptr, short *inbuf, int len);
    u32(*config)(void *work_buf, u32 cmd, void *parm);
} EQ_STUCT_API;

typedef struct _EQ_BUFF {
    EQ_IO_CONTEXT   *io;
    EQ_STUCT_API    *ops;
    EQ_PARA_STRUCT  para;
    sound_fade_obj  fade;
    u16             rmlen;
    u16             rmcnt;
    u8              flag;
    u8              res[3];
    u8              buff[EQ_MAX_POINT * 2];
    float           eq_oeff_tab[0];//缓存运算后系数表
} EQ_BUFF;

typedef struct _EQ_UPDATE {
    void *seg;
    float global_gain;
    u8 eq_mode;
} EQ_UPDATE;

typedef struct _EQ_HDL {
    EFFECT_OBJ obj;
    sound_in_obj si;
    EQ_IO_CONTEXT io;
    EQ_BUFF buff;
} EQ_HDL;

enum EQ_CMD {
    EQ_CMD_NONE = 0,
    EQ_CMD_SWITCH_TAB,
    EQ_GET_CURR_MODE,
};

struct music_eq_tool {
    float global_gain;
    int seg_num;
    int enable_section;
    struct eq_seg_info seg[EQ_SECTION_MAX];
};
extern struct music_eq_tool music_eq_tool_online;
extern const EQ_STUCT_API *get_eq_context(void);
void *link_eq_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr, u8 ch);
void *eq_hld_malloc();
void eq_online_sw(void *parm);
void eq_mode_sw(void *parm);
void eq_reless(void **ppeffect);
extern u8 get_eq_mode_total_num();
static int eq_run_api(void *hld, short *inbuf, int len);
static void *eq_api(void *obuf, EQ_PARA_STRUCT *p_para, void **ppsound);
#endif
