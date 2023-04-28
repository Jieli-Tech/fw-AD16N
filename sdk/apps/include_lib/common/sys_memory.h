#ifndef _SYS_MEMORY_H_
#define _SYS_MEMORY_H_
#include "typedef.h"

#define USE_NEW_VM      1
#define USE_OLD_VM      2
#define SYS_MEMORY_SELECT   USE_NEW_VM

#if (SYS_MEMORY_SELECT == USE_NEW_VM)
//新版vm，支持预擦除
#include "new_vm.h"
#define sysmem_phy_init(addr, size)         nvm_init_api(addr, size)
#define sysmem_phy_read(id, buf, len)       nvm_read_api(id, buf, len)
#define sysmem_phy_write(id, buf, len)      nvm_write_api(id, buf, len)
#define sysmem_phy_pre_erase()              nvm_erasure_next_api()
#define SYSMEM_INDEX_MAX                    BIT_MAP
#elif (SYS_MEMORY_SELECT == USE_OLD_VM)
//旧版vm
#include "vm.h"
#define sysmem_phy_init(addr, size)         syscfg_vm_init_phy(addr, size)
#define sysmem_phy_read(id, buf, len)       vm_read_phy(id, buf, len)
#define sysmem_phy_write(id, buf, len)      vm_write_phy(id, buf, len)
#define sysmem_phy_pre_erase(...)
#define SYSMEM_INDEX_MAX                    128
#else
#define sysmem_phy_init(addr, size)         -1
#define sysmem_phy_read(id, buf, len)       -1
#define sysmem_phy_write(id, buf, len)      -1
#define sysmem_phy_pre_erase(...)
#define SYSMEM_INDEX_MAX                    128
#endif

typedef enum {
    SYSMEM_INDEX_DEMO = 0,
    // 系统lib使用，预留32个id，不可修改顺序
    LIB_SYSMEM_OLD_RTC_TIME     = 1,
    LIB_SYSMEM_OLD_REAL_TIME    = 2,
    LIB_SYSMEM_RTC_ALARM_TIME   = 3,
    LIB_SYSMEM_END = 32,

    // 用户可以使用
    SYSMEM_INDEX_SONG,
    SYSMEM_INDEX_ENG,
    SYSMEM_INDEX_POETRY,
    SYSMEM_INDEX_STORY,
    SYSMEM_INDEX_F1X,
    SYSMEM_INDEX_EXT_SONG,
    SYSMEM_INDEX_VOL,
#if 1
    SYSMEM_INDEX_SONG_BP,
    SYSMEM_INDEX_ENG_BP,
    SYSMEM_INDEX_POETRY_BP,
    SYSMEM_INDEX_STORY_BP,
    SYSMEM_INDEX_F1X_BP,
    SYSMEM_INDEX_EXT_SONG_BP,
#endif
    /*mbox*/
    SYSMEM_INDEX_SYSMODE,
    SYSMEM_INDEX_UDISK_BP,
    SYSMEM_INDEX_UDISK_INDEX,
    SYSMEM_INDEX_SDMMCA_BP,
    SYSMEM_INDEX_SDMMCA_INDEX,
    SYSMEM_INDEX_SDMMCB_BP,
    SYSMEM_INDEX_SDMMCB_INDEX,
    SYSMEM_INDEX_INRFLSH_BP,
    SYSMEM_INDEX_INRFLSH_INDEX,
    SYSMEM_INDEX_EXTFLSH_BP,
    SYSMEM_INDEX_EXTFLSH_INDEX,
    SYSMEM_INDEX_ACTIVE_DEV,
    SYSMEM_INDEX_FM_FRE,
    SYSMEM_INDEX_FM_CHAN,
    SYSMEM_INDEX_FM_CHANNL,//SYSMEM_INDEX_FM_CHANNL和SYSMEM_INDEX_FM_CHANNL_END之间不能插入其它INDEX
    SYSMEM_INDEX_FM_CHANNL_END = SYSMEM_INDEX_FM_CHANNL + 28,
    /*mbox*/
    SYSMEM_INDEX_FLASH_UPDATE,
    SYSMEM_INDEX_AUTO_BP,
    // eq
    SYSMEM_INDEX_EQ,

    SYSMEM_LAST_INDEX = SYSMEM_INDEX_MAX,
} SYSMEM_INDEX;

int sysmem_init_api(u32 mem_addr, u32 mem_size);
int sysmem_read_api(u32 id, u8 *data_buf, u16 len);
int sysmem_write_api(u32 id, u8 *data_buf, u16 len);
void sysmem_pre_erase_api(void);

// vm_sfc接口
typedef u32(*flash_code_protect_cb_t)(u32 offset, u32 len);
u32 flash_code_protect_callback(u32 offset, u32 len);
extern volatile u8 vm_busy;
// vm擦写时可放出多个中断
void vm_isr_response_index_register(u8 index);
void vm_isr_response_index_unregister(u8 index);
u32 get_vm_isr_response_index_h(void);//获取放出中断的高32位(index 32-63)
u32 get_vm_isr_response_index_l(void);//获取放出中断的低32位(index 0-31)
#endif

