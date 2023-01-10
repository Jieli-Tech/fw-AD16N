

#pragma bss_seg(".malloc.data.bss")
#pragma data_seg(".malloc.data")
#pragma const_seg(".malloc.text.const")
#pragma code_seg(".malloc.text")
#pragma str_literal_override(".malloc.text.const")

#include "my_malloc.h"


#define LOG_TAG_CONST       HEAP
#define LOG_TAG             "[malloc]"
#include "log.h"

/* void *pvPortMalloc( size_t xWantedSize ) */
/* u32 example(u32 dat) */
/* { */
/* if (dat & 0x80) { */
/* dat += 1; */
/* } else { */
/* dat <<= 1; */
/* } */
/* return dat; */
/* } */

const u16 configHEAP_BEST_SIZE = 100;

void *my_malloc(u32 size, mm_type xType)
{
    void *res;
    /* return pvPortMalloc(size); */
    res =  pvPortMalloc(size, xType);
    if (NULL != res) {
        log_info(" malloc res 0x%x, size 0x%x,type 0x%x\n", (u32)res, size, xType);
        memset(res, 0, size);
#if MALLOC_INFO_PRINTF
        mem_malloc_info_printf((u32)res, size, xType);
#endif
    } else {
        log_info(" err malloc empty, size 0x%x,type 0x%x\n", size, xType);
    }
    return res;
}

void *my_free(void *pv)
{
    if (NULL != pv) {
        vPortFree(pv);
#if MALLOC_INFO_PRINTF
        mem_free_info_printf((u32)pv);
#endif
    }
    return NULL;
}


extern const u8 _free_start[];
extern const u8 _free_end[];

void my_malloc_init(void)
{
    u32 len = (u32)&_free_end[0] - (u32)&_free_start[0]  + 1;
    log_info(" HEAP----: 0x%x; 0x%x\n", (u32)&_free_end[0], (u32)&_free_start[0]);
    memset((void *)&_free_start[0], 0, len);
    vPortInit((void *)&_free_start[0], len);
}
void xTaskResumeAll(void)
{
    ;
}


#if MALLOC_INFO_PRINTF

#define MAX_MALLOC_NUM  10
struct _mem_table {
    u32 addr;
    u32 size;
    u8  type;
    u8  active;
} _GNU_PACKED_;
static struct _mem_table mem_table[MAX_MALLOC_NUM] = {0};
static u8 mem_cnt = 0;

void mem_printf(void)
{
    log_info("addr\t\tsize\ttype\tactive\tremain");
    for (int i = 0; i < mem_cnt; i++) {
        log_info("0x%x\t0x%x\t%d\t%d\t0x%x", mem_table[i].addr, mem_table[i].size, mem_table[i].type, mem_table[i].active, ((u32)&_free_end[0] - mem_table[i].addr - mem_table[i].size));
    }
    log_char('\n');
}

void mem_malloc_info_printf(u32 addr, u32 size, mm_type xType)
{
    for (int i = 0; i < mem_cnt; i++) {
        if (addr == mem_table[i].addr) {
            mem_table[i].active += 1;
            return;
        }
    }
    mem_table[mem_cnt].addr = addr;
    mem_table[mem_cnt].size = size;
    mem_table[mem_cnt].type = xType;
    mem_table[mem_cnt].active += 1;
    mem_cnt++;
    if (mem_cnt > MAX_MALLOC_NUM) {
        log_info("%s() %d\n", __func__, __LINE__);
        while (1);
    }
}

void mem_free_info_printf(u32 addr)
{
    for (int i = 0; i < mem_cnt; i++) {
        if (addr == mem_table[i].addr) {
            mem_table[i].active -= 1;
        }
    }
}
#endif
