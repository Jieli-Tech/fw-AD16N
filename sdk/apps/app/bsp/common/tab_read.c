#pragma bss_seg(".tab_read.data.bss")
#pragma data_seg(".tab_read.data")
#pragma const_seg(".tab_read.text.const")
#pragma code_seg(".tab_read.text")
#pragma str_literal_override(".tab_read.text.const")

#include "tab_read.h"
#include "string.h"

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[tread]"
#include "log.h"

void tab_init(rtab_obj *stab, void *tab, u32 size)
{
    memset(stab, 0, sizeof(rtab_obj));
    stab->tab = tab;
    stab->size = size;
    log_info("stab->tab  : 0x%x\n", (u32)stab->tab);
    log_info("stab->size : %d\n", stab->size);

    /* log_info("size %d\n", size); */
    /* log_info_hexdump(stab->tab, size * 2); */

}

/* AT(.dac_oput_code) */
AT(.audio_isr_text)
u32 tab_read(void *buff, rtab_obj *stab, u32 len)
{
    u8 *rbuff = buff;
    u8 *rtab = stab->tab;
    u32 max_len = stab->size;
    u32 rlen = 0;
    u32 offset = stab->offset;

    /* log_info("stab :0x%x", (u32)stab); */
    /* log_info("max_len  : 0x%x\n", max_len); */
    /* log_info("offset : 0x%x\n", offset); */
    /* log_info("rtab   : 0x%x\n", (u32)rtab); */
    /* log_info("len   : 0x%x\n", (u32)len); */

    while (len && stab->cnt) {
        /* while (len && stab->cnt) { */
        rlen = max_len - offset;

        /* rlen = rlen > len ? len : rlen; */
        if (rlen > len) {
            rlen = len;
        }

        memcpy(rbuff, &rtab[offset], rlen);
        len    -= rlen;
        offset += rlen;
        rbuff  += rlen;
        if (offset >= max_len) {
            offset = 0;
            if (((u16) - 1) != stab->cnt) {
                if (0 != stab->cnt) {
                    /* log_info("stab->cnt  %d\n", stab->cnt); */
                    stab->cnt--;
                    /* log_char('k'); */
                }
            }
        }
    }
    stab->offset = offset;
    /* log_info("remain len : %d\n",len); */
    return len;
}

