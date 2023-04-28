/***********************************Jieli tech************************************************
  File : new_vm.h
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2022-12-26
********************************************************************************************/
#ifndef __NEW_VM_H__
#define __NEW_VM_H__

#include "typedef.h"


/*************************************************
 * newi vm cache
 * */
typedef struct __nvm_entry {
    u16 id;
    u16 rw_cnt;
    u32 offset;
} NVM_ENTRY;

typedef struct __nvm_cache {
    u16 rw_cnt;
    u16 number_entry;
    NVM_ENTRY *entries;
} NVM_CACHE;

u32 nvm_cache_cnt(NVM_ENTRY *entries, u32 len);
u32 nvm_write_cache(NVM_CACHE *cache, u16 id, u32 offset);
u32 nvm_read_cache(NVM_CACHE *cache, u16 id);
u32 nvm_clear_cache(NVM_CACHE *cache);


/****************************************************/

#define BIT_MAP  (32 * 16)
#define BIT_MAP_SIZE  (BIT_MAP / 8)

#define NVM_MAX_LEN     128

#define NVM_BUFF_SIZE   (NVM_MAX_LEN + BIT_MAP_SIZE)


// #define NVM_MULTIPLE_READ

typedef struct __new_vm_obj {
    void *device;
    NVM_CACHE *cache;
    u32 addr;
    u32 reserve : 7;
    u32 bool_block : 1;
    u32 block_size : 24;
    u32 area_len;
    u32 w_offset;
    u16 pre_sec_a;
    u16 pre_sec_b;
// #ifdef NVM_MULTIPLE_READ
    u16 id;
    u16 offset;
// #endif
} NEW_VM_OBJ;


/*********************************************************
 * 库接口
 * ******/
u32 nvm_init(NEW_VM_OBJ *p_nvm, u32 addr, u32 size);
u32 nvm_format_another(NEW_VM_OBJ *p_nvm);
u32 nvm_read(NEW_VM_OBJ *p_nvm, u32 id, u8 *buf, u32 len);
u32 nvm_write(NEW_VM_OBJ *p_nvm, u32 id, u8 *buf, u32 len);
void nvm_pre_erasure_next(NEW_VM_OBJ *p_nvm, u16 using_next, u16 idle_next);

/**********************************************************
 * 提供给库的回调函数
 * *****/
void *nvm_buf_for_lib(NEW_VM_OBJ *p_nvm, u32 *p_len);

/**********************************************************
 * 应用接口
 ******/
u32 nvm_init_api(u32 addr, u32 size);
u32 nvm_format_anotheri_api(void);
u32 nvm_read_api(u32 id, u8 *buf, u32 len);
u32 nvm_write_api(u32 id, u8 *buf, u32 len);
void nvm_erasure_next_api(void);

#endif

