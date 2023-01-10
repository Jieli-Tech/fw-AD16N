

#pragma bss_seg(".decoder_point.data.bss")
#pragma data_seg(".decoder_point.data")
#pragma const_seg(".decoder_point.text.const")
#pragma code_seg(".decoder_point.text")
#pragma str_literal_override(".decoder_point.text.const")

#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "decoder_api.h"
#include "crc.h"
#include "errno-base.h"


/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[d_poit]"
#include "log.h"

#define CRC16 chip_crc16
bool get_dp(dec_obj *obj, dp_buff *dbuff)
{
    u32 t_size;
    decoder_ops_t *ops;

    if ((0 == obj) || (0 == dbuff)) {
        log_info(" get_dp null\n");
        return false;
    }
    u32 size = (u32)&dbuff->buff[0] - (u32)dbuff;
    size = sizeof(dp_buff) - size;
    /* log_info("----dp_buff 0x%x,buff_size 0x%x\n", (u32)dbuff, size); */
    ops = obj->dec_ops;

    bool bres;
    local_irq_disable();
    t_size = ops->need_bpbuf_size();
    /* log_info("-----need_bpbuf_size : 0x%x\n", t_size); */
    if (t_size <= size) {
        memcpy(&dbuff->buff[0], (void *)ops->get_bp_inf(obj->p_dbuf), t_size);
        dbuff->len = t_size;
        dbuff->crc = CRC16(&dbuff->len, dbuff->len + 2);
        bres = true;
    } else {
        log_info(" err dp size %d %d\n", t_size, size);
        bres = false;
    }
    local_irq_enable();
    /* log_info("-----get_dp over\n"); */
    return bres;
}
u32 dp_buff_len(void)
{
    dp_buff *p_dbuf = (void *)0;
    return sizeof(dp_buff) - ((u32)&p_dbuf->buff[0] - (u32)p_dbuf);
}
void *check_dp(dp_buff *dbuff)
{
    u16 crc;
    u32 len = dbuff->len;
    if ((0 == dbuff) || (0 == len) || (len > dp_buff_len())) {
        /* log_info("  check decoder point buf null\n"); */
        return 0;
    }
    crc = CRC16(&dbuff->len, dbuff->len + 2);
    if ((0 != crc) && (crc == dbuff->crc)) {
        /* log_info(" decoder point check succ\n"); */
        /* log_info_hexdump(&dbuff->buff[0], dbuff->len); */
        return &dbuff->buff[0];
    } else {
        log_info("check_dp err,crc: 0x%x 0x%x\n", crc, dbuff->crc);
        /* log_info_hexdump(&dbuff->buff[0], dbuff->len); */
        return 0;
    }
}
void clear_dp(dp_buff *dbuff)
{
    u16 crc;
    if (0 == dbuff) {
        return;
    }
    memset(&dbuff->crc, 0, sizeof(dp_buff) - 4);
}
void clear_dp_buff(void *buff)
{
    u16 crc;
    if (0 == buff) {
        return;
    }
    memset(buff, 0, sizeof(dp_buff) - 8);
}
