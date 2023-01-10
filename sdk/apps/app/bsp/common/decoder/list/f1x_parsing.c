#include "f1a_api.h"
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "decoder_api.h"
#include "vfs.h"
#include "circular_buf.h"
#include "errno-base.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

#define F1X_MAX_UNIT  10

typedef struct _f1x_ctl_ {
    u16 index;
    /* u16 max_unit; */
    u8  cnt;
    u8  loop_tab[F1X_MAX_UNIT];
    u32 buff[F1X_MAX_UNIT];
} f1x_ctl;

f1x_ctl g_f1x_ctl[2] AT(.f1a_1_buf);



u32 set_play_file(void *priv, u32 *startpos, u32 *endpos)
{
    f1x_ctl *p_f1x = priv;
    if ((u16) - 1 != p_f1x->index) {
        if (0xff != p_f1x->loop_tab[p_f1x->index]) {
            p_f1x->cnt++;
        }
        /* log_info(" loop %d; curr %d; index %d", p_f1x->loop_tab[p_f1x->index], p_f1x->cnt, p_f1x->index); */
    }
    /* log_info_hexdump(&p_f1x->loop_tab[0], 10); */
    if (((u16) - 1 == p_f1x->index) || (p_f1x->loop_tab[p_f1x->index] <= p_f1x->cnt)) {
        while (1) {
            p_f1x->index++;
            /* log_info(" index %d", p_f1x->index); */
            /* if(p_f1x->index >= p_f1x->max_unit) */
            if (p_f1x->index >= F1X_MAX_UNIT) {
                *startpos = -1;
                *endpos =  -1;
                return -1;
            }
            if (0 != p_f1x->loop_tab[p_f1x->index]) {
                p_f1x->cnt = 0;
                break;
            }
        }
    }
    *startpos = p_f1x->buff[p_f1x->index];
    /* if( (p_f1x->index + 1) >= p_f1x->max_unit) */
    if ((p_f1x->index + 1) >= F1X_MAX_UNIT) {
        *endpos = -1;
    } else {
        if ((p_f1x->buff[p_f1x->index + 1]) && (-1 != p_f1x->buff[p_f1x->index + 1])) {
            *endpos = p_f1x->buff[p_f1x->index + 1] - 1;
        } else {
            *endpos = p_f1x->buff[p_f1x->index + 1];
        }
    }
    //log_info("*startpos 0x%x; *endpos 0x%x", *startpos, *endpos);
    return 0;
}

//struct
const EX_PlayFile_STRUCT f1x_io[MAX_F1A_CHANNEL] = {
    {
        &g_f1x_ctl[0],      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
        set_play_file,

    },
#if (MAX_F1A_CHANNEL > 1)
    {
        &g_f1x_ctl[1],      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL
        set_play_file,
    }
#endif

};


int f1x_parsing(void *pfile, u32 *buff, u32 sum)
{

    u8 tbuff[4];
    u32 h_len;
    u32 unit_sum;
    u32 unit_len;


    vfs_seek(pfile, 4, SEEK_SET);
    /* vfs_seek(pfile, 0, SEEK_SET); */
    /* vfs_read(pfile,&tbuff[0], 2); */
    if (2 == vfs_read(pfile, &tbuff[0], 2)) {
        h_len = tbuff[0] | (tbuff[1] << 8) ;
    } else {
        return E_F1X_NO_HLEN;
    }
    /* log_info_hexdump(&tbuff[0], 2); */
    if (1 == vfs_read(pfile, &tbuff[0], 1)) {
        /* log_info_hexdump(&tbuff[0], 1); */
        unit_len = tbuff[0];
        if ((0 == h_len) || (0 == unit_len)) {
            return E_F1X_HU_ZERO;
        }
        if (unit_len > 4) {
            return E_F1X_U2LONG;
        }
        h_len -= 1;//减去存放单元头大小的位置
        if ((h_len >= unit_len) && (0 == (h_len % unit_len))) {
            unit_sum = h_len / unit_len;
        } else {
            return E_F1X_HU_ERR;
        }
    } else {
        return E_F1X_NO_ULEN;
    }

    /* unit_sum  = unit_sum > sum ? sum : unit_sumit_sum; */
    /* log_info(" sum %d, unit_sum %d", sum, unit_sum); */
    for (u32 i = 0; i < sum; i++) {
        if (i < unit_sum) {
            vfs_read(pfile, &tbuff[0], unit_len);
            buff[i] = 0;
            for (u32 j = 0; j < unit_len; j++) {
                buff[i] |= tbuff[j] << (8 * j);
            }
        } else {
            buff[i] = -1;
        }
    }
    /* log_info("%s succ\n", __func__); */
    return 0;
}

void *f1x_play_api(void *pfile, u8 *loop_tab, u32 size, u8 index, u32 addr)
{
    f1x_ctl *p_f1x;
    if (0 != index) {
        index = 1;
    }
    p_f1x = &g_f1x_ctl[index];
    memset(p_f1x, 0, sizeof(f1x_ctl));
    for (u32 i = 0; i < F1X_MAX_UNIT; i++) {
        p_f1x->buff[i] = -1;
    }

    size = size > F1X_MAX_UNIT ? F1X_MAX_UNIT : size;
    memcpy(&p_f1x->loop_tab[0], loop_tab, size);

    u32 err = f1x_parsing(pfile, &p_f1x->buff[0], size);
    if (0 != err) {
        log_error("f1x parsing 0x%x\n", err);
        return 0;
    }
    p_f1x->index = -1;
    if (0 != addr) {
        for (u32 i = 0; i < (F1X_MAX_UNIT - 2); i++) {
            if ((addr >= p_f1x->buff[i]) && (addr < p_f1x->buff[i + 1])) {
                p_f1x->index = i;
                break;
            }
        }
    }
    /* log_info("f1x parsing succ\n"); */
    return (void *)&f1x_io[index];
}








