#ifndef __VM_API_H__
#define __VM_API_H__
#include "typedef.h"
#include "new_vm.h"
#include "vm.h"

typedef enum {
    VM_INDEX_DEMO = 0,//用户可以使用
    VM_INDEX_SONG,
    VM_INDEX_ENG,
    VM_INDEX_POETRY,
    VM_INDEX_STORY,
    VM_INDEX_F1X,
    VM_INDEX_EXT_SONG,
    VM_INDEX_VOL,
#if 1
    VM_INDEX_SONG_BP,
    VM_INDEX_ENG_BP,
    VM_INDEX_POETRY_BP,
    VM_INDEX_STORY_BP,
    VM_INDEX_F1X_BP,
    VM_INDEX_EXT_SONG_BP,
#endif
    /*mbox*/
    VM_INDEX_SYSMODE,
    VM_INDEX_UDISK_BP,
    VM_INDEX_UDISK_INDEX,
    VM_INDEX_SDMMCA_BP,
    VM_INDEX_SDMMCA_INDEX,
    VM_INDEX_SDMMCB_BP,
    VM_INDEX_SDMMCB_INDEX,
    VM_INDEX_INRFLSH_BP,
    VM_INDEX_INRFLSH_INDEX,
    VM_INDEX_EXTFLSH_BP,
    VM_INDEX_EXTFLSH_INDEX,
    VM_INDEX_ACTIVE_DEV,
    VM_INDEX_FM_FRE,
    VM_INDEX_FM_CHAN,
    VM_INDEX_FM_CHANNL,//VM_INDEX_FM_CHANNL和VM_INDEX_FM_CHANNL_END之间不能插入其它INDEX
    VM_INDEX_FM_CHANNL_END = VM_INDEX_FM_CHANNL + 28,
    /*mbox*/
    VM_INDEX_FLASH_UPDATE,
    VM_INDEX_AUTO_BP,
    // rtc
    VM_OLD_RTC_TIME,
    VM_OLD_REAL_TIME,
    VM_RTC_ALARM_TIME,
    VM_INDEX_MAX = 128,
} VM_INDEX;

// vm_sfc
typedef u32(*flash_code_protect_cb_t)(u32 offset, u32 len);
u32 flash_code_protect_callback(u32 offset, u32 len);
extern volatile u8 vm_busy;
// vm擦写时可放出多个中断
void vm_isr_response_index_register(u8 index);
void vm_isr_response_index_unregister(u8 index);
u32 get_vm_isr_response_index_h(void);//获取放出中断的高32位(index 32-63)
u32 get_vm_isr_response_index_l(void);//获取放出中断的低32位(index 0-31)


// vm_api接口
u32 syscfg_vm_init(u32 addr, u32 size);
u32 vm_read(u32 id, u8 *buf, u32 len);
u32 vm_write(u32 id, u8 *buf, u32 len);
u32 vm_pre_erase();
#endif
