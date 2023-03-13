/***********************************Jieli tech************************************************
  File : nvm_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2022-12-26
********************************************************************************************/
#include "typedef.h"
#include "errno-base.h"
#include "new_vm.h"
#include "device.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[nvm_api]"
#include "log.h"

NEW_VM_OBJ g_nvm_obj;

u32 new_vm_buff[NVM_BUFF_SIZE / 4]; //需要32bit对齐
/******************************************************/
void *nvm_buf_for_lib(NEW_VM_OBJ *p_nvm, u32 *p_len)
{
    *p_len = sizeof(new_vm_buff);
    return new_vm_buff;
}

/******************************************************/

#define NVM_CACHE_ENABLE    1
#define NVM_CACHE_NUMBER    6

#if NVM_CACHE_ENABLE

NVM_ENTRY g_nvm_entry[NVM_CACHE_NUMBER];
NVM_CACHE g_nvm_cache;

NVM_CACHE *nvm_cache_init_api(void)
{
    memset(&g_nvm_entry[0], 0, sizeof(g_nvm_entry));
    memset(&g_nvm_cache, 0, sizeof(g_nvm_cache));
    g_nvm_cache.entries = &g_nvm_entry[0];
    g_nvm_cache.number_entry = NVM_CACHE_NUMBER;
    return &g_nvm_cache;
}

#endif
/******************************************************/


u32 nvm_init_api(u32 addr, u32 size)
{
    memset(&g_nvm_obj, 0, sizeof(NEW_VM_OBJ));

    /* if (NULL == g_nvm_obj.device) { */
    g_nvm_obj.device = dev_open(__SFC_NANE, 0);
    if (NULL == g_nvm_obj.device) {
        return E_NVM_OPEN_DEVICE;
    }
    /* } */
#if NVM_CACHE_ENABLE
    g_nvm_obj.cache = nvm_cache_init_api();
#endif
    return nvm_init(&g_nvm_obj, addr, size);
}

u32 nvm_read_api(u32 id, u8 *buf, u32 len)
{
    u32 err = nvm_read(&g_nvm_obj, id, buf, len);
    if (err) {
        return err;
    } else {
        return len;
    }
}

u32 nvm_write_api(u32 id, u8 *buf, u32 len)
{
    u32 err = nvm_write(&g_nvm_obj, id, buf, len);
    if (err) {
        return err;
    } else {
        return len;
    }
}
/******************************************************
 * 用法：此函数在系统空闲时在主循环调用
 * 作用：将接下来的1个会用到的区域擦除掉；将另外一个区域的低1个区域擦除掉；
 * 注：已经擦过了的将不会再次被擦除；
 * */
void nvm_erasure_next_api(void)
{
    nvm_pre_erasure_next(&g_nvm_obj, 1, 1);
}



/***************** 测试验证代码 ***********/
#if 0

#include "common.h"

u8 nvm_demo_buf[128];
u8 nvm_test_buf[128];
void test_nvm(void)
{
    u32 i, j, err;
    u32 retry = 2046;
    while (retry--) {
        wdt_clear();
        /* log_char('A'); */
        log_info("A %d\n", retry);
        j = sizeof(nvm_demo_buf);
        for (i = 0; i < sizeof(nvm_demo_buf); i++) {
            nvm_demo_buf[i] = JL_RAND->R64L;
        }
        memcpy(nvm_test_buf, nvm_demo_buf, sizeof(nvm_test_buf));
        err = nvm_write_api(1, nvm_demo_buf, sizeof(nvm_demo_buf));

        if (0 != err) {
            log_info("nvm_write_api err : 0x%x", err);
            break;
        }

        /* log_char('B'); */
        log_info("B\n");

        memset(nvm_demo_buf, 0, sizeof(nvm_demo_buf));
        nvm_read_api(1, nvm_demo_buf, sizeof(nvm_demo_buf));
        if (0 != err) {
            log_info("nvm_read_api err : 0x%x", err);
            break;
        }

        /* log_char('C'); */
        log_info("C\n");
        err = memcmp(nvm_demo_buf, nvm_test_buf, sizeof(nvm_demo_buf));
        if (0 != err) {
            log_info("save buf :");
            log_info_hexdump(nvm_test_buf, sizeof(nvm_demo_buf));
            log_info("read buf :");
            log_info_hexdump(nvm_demo_buf, sizeof(nvm_demo_buf));
            break;
        }
        /* log_char('.'); */
        if (0 != err) {
            break;
        }
        log_info("D\n");

    }

    log_info("--- over ---, %d\n", retry);
    while (1) {
        wdt_clear();
    }
}

#define TNVM_MAX_ID 16
#define TNVM_LEN    128

u8 tnvm_id_buf[TNVM_MAX_ID][TNVM_LEN];
u8 tnvm_rw_buf[TNVM_LEN];
void test_nvm_0(void)
{
    u32 i, err;
    u32 retry = 1024;
    u32 write_id = 0;
    u32 read_id = 0;
    u32 id_list = 0;
    u32 len = 0;
    while (retry--) {
        wdt_clear();
        /* log_char('A'); */
        write_id = JL_RAND->R64L % TNVM_MAX_ID;
        if (0 == write_id) {
            write_id = 1;
        }
        if (7 == write_id) {
            write_id = 1;
        }
        if (15 == write_id) {
            write_id = 1;
        }
        log_char('\n');
        log_info("A %d ", retry);
        nvm_erasure_next_api();
        /* j = TNVM_LEN; */
        len = (JL_RAND->R64L % (TNVM_LEN - 1)) + 1;
        memset(tnvm_rw_buf, 0, TNVM_LEN);
        for (i = 0; i < len; i++) {
            tnvm_rw_buf[i] = JL_RAND->R64L;
        }
        log_info_hexdump(tnvm_rw_buf, sizeof(tnvm_rw_buf));
        memcpy(&tnvm_id_buf[write_id][0], tnvm_rw_buf, TNVM_LEN);
        err = nvm_write_api(write_id, tnvm_rw_buf, len);
        if (0 != err) {
            log_info("nvm_write_api err : 0x%x", err);
            break;
        }
        id_list |= BIT(write_id);
        log_info("write id: %d, 0x%x, len %d", write_id, id_list, len);

        log_info("A-1");
        read_id = write_id;
        memset(tnvm_rw_buf, 0, TNVM_LEN);
        nvm_read_api(read_id, tnvm_rw_buf, TNVM_LEN);
        if (0 != err) {
            log_info("nvm_read_api err : 0x%x", err);
            break;
        }

        err = memcmp(tnvm_rw_buf, &tnvm_id_buf[read_id][0], TNVM_LEN);
        if (0 != err) {
            log_info("-save buf :");
            log_info_hexdump(tnvm_rw_buf, sizeof(nvm_demo_buf));
            log_info("-read buf :");
            log_info_hexdump(&tnvm_id_buf[read_id][0], TNVM_LEN);
            break;
        }
        if (0 != err) {
            break;
        }
        log_info("B");

        read_id = JL_RAND->R64L % TNVM_MAX_ID;
        memset(tnvm_rw_buf, 0, TNVM_LEN);
        err = nvm_read_api(read_id, tnvm_rw_buf, TNVM_LEN);
        if (0 != err) {
            log_info("err read id %d : 0x%x", read_id, err);
            /* break; */
            /* continue; */
        } else {
            log_info("read_id : %d", read_id);
        }

        err = memcmp(tnvm_rw_buf, &tnvm_id_buf[read_id][0], TNVM_LEN);
        if (0 != err) {
            log_info("save buf :");
            log_info_hexdump(tnvm_rw_buf, sizeof(nvm_demo_buf));
            log_info("read buf :");
            log_info_hexdump(&tnvm_id_buf[read_id][0], TNVM_LEN);
            if (id_list & (BIT(read_id))) {
                break;
            }
            log_info("NULL, 0x%x, %d", id_list, read_id);
        }
        log_info("D\n");

    }

    log_info("--- over ---, %d\n", retry);
    while (1) {
        wdt_clear();
    }
}

void nvm_cache_demo(void);
void nvm_demo(u32 start, u32 size)
{
    u32 i, j, err;
    /* nvm_cache_demo(); */
    log_info("new vm demo, 0x%x, 0x%x", start, size);
    err = nvm_init_api(start, size);
    if (0 != err) {
        log_info("nvm_init_api err : 0x%x", err);
        /* while (1) { */
        /* wdt_clear(); */
        /* } */

    } else {
        /* test_nvm(); */
    }
    test_nvm_0();

    j = sizeof(nvm_demo_buf);
    for (i = 0; i < sizeof(nvm_demo_buf); i++) {
        nvm_demo_buf[i] = j--;
    }
    log_info_hexdump(nvm_demo_buf, sizeof(nvm_demo_buf));
    err = nvm_write_api(1, nvm_demo_buf, sizeof(nvm_demo_buf));
    if (0 != err) {
        log_info("nvm_write_api err : 0x%x", err);
        while (1) {
            wdt_clear();
        }
    }
    memset(nvm_demo_buf, 0, sizeof(nvm_demo_buf));
    log_info_hexdump(nvm_demo_buf, sizeof(nvm_demo_buf));
    nvm_read_api(1, nvm_demo_buf, sizeof(nvm_demo_buf));
    if (0 != err) {
        log_info("nvm_read_api err : 0x%x", err);
        while (1) {
            wdt_clear();
        }
    }

    log_info("new vm read");
    log_info_hexdump(nvm_demo_buf, sizeof(nvm_demo_buf));

    while (1) {
        wdt_clear();
    }
}

void nvm_cache_demo_0(void)
{
    memset(&g_nvm_entry[0], 0, sizeof(g_nvm_entry));
    memset(&g_nvm_cache, 0, sizeof(g_nvm_cache));
    g_nvm_cache.entries = &g_nvm_entry[0];
    g_nvm_cache.number_entry = NVM_CACHE_NUMBER;
    u32 i;
    for (i = 0; i < NVM_CACHE_NUMBER; i++) {
        if (i != 1) {
            g_nvm_entry[i].rw_cnt = JL_RAND->R64L & 0xff;
            g_nvm_entry[i].offset = 0x55aa;
        }
    }

    for (i = 0; i < NVM_CACHE_NUMBER; i++) {
        log_info("index %d, rw_cnt %3d, 0x%x", i, g_nvm_entry[i].rw_cnt, g_nvm_entry[i].offset);
    }

    nvm_cache_cnt(&g_nvm_entry[0], NVM_CACHE_NUMBER);

    log_char('\n') ;
    for (i = 0; i < NVM_CACHE_NUMBER; i++) {
        log_info("index %d, rw_cnt %d", i, g_nvm_entry[i].rw_cnt);
    }

    while (1) {
        wdt_clear();
    }

}

void nvm_cache_demo(void)
{
    memset(&g_nvm_entry[0], 0, sizeof(g_nvm_entry));
    memset(&g_nvm_cache, 0, sizeof(g_nvm_cache));
    g_nvm_cache.entries = &g_nvm_entry[0];
    g_nvm_cache.number_entry = NVM_CACHE_NUMBER;
    u32 i;
    for (i = 0; i < NVM_CACHE_NUMBER; i++) {
        if (i != 1) {
            g_nvm_entry[i].rw_cnt = JL_RAND->R64L & 0xff;
            g_nvm_entry[i].offset = 0x5500 + i;
            g_nvm_entry[i].id = JL_RAND->R64L & 0xf;
        }
    }

    for (i = 0; i < NVM_CACHE_NUMBER; i++) {
        log_info("index %d, rw_cnt %3d, 0x%x", i, g_nvm_entry[i].rw_cnt, g_nvm_entry[i].offset);
    }

    g_nvm_cache.rw_cnt = nvm_cache_cnt(&g_nvm_entry[0], NVM_CACHE_NUMBER);
    log_char('\n');
    for (i = 0; i < NVM_CACHE_NUMBER; i++) {
        log_info("index %d, rw_cnt %3d, 0x%x", i, g_nvm_entry[i].rw_cnt, g_nvm_entry[i].offset);
    }

    log_info("g_nvm_cache.rw_cnt %d", g_nvm_cache.rw_cnt);
    u32 j = 16;
    u32 id, r_id, offset;
    log_info("/**********************/\n");
    while (j--) {
        log_char('\n');
        id = JL_RAND->R64L & 0xf;
        nvm_write_cache(&g_nvm_cache, id, j + 0x100);
        log_info("write curr id %d, 0x%x", id, j + 0x100);

        for (i = 0; i < NVM_CACHE_NUMBER; i++) {
            log_info("index %d, rw_cnt %2d, id %2d, 0x%x",
                     i,
                     g_nvm_entry[i].rw_cnt,
                     g_nvm_entry[i].id,
                     g_nvm_entry[i].offset);
        }

        r_id = JL_RAND->R64L & 0x1f;
        offset = nvm_read_cache(&g_nvm_cache, r_id);
        log_info("read id %d, 0x%x", r_id, offset);

    }
    while (1) {
        wdt_clear();
    }

}



#endif



