#ifndef _SYS_MEMORY_H_
#define _SYS_MEMORY_H_
#include "typedef.h"

struct btif_item {
    u16 id;
    u16 data_len;
};

typedef enum {
    //================系统内部 & 蓝牙库内使用，不可修改顺序===========================//
    SYSMEM_INDEX_DEMO = 0,
    // 系统lib使用，预留64个id，不可修改顺序
    LIB_SYSMEM_OLD_RTC_TIME     = 1,
    LIB_SYSMEM_OLD_REAL_TIME    = 2,
    LIB_SYSMEM_RTC_ALARM_TIME   = 3,
    CFG_BT_TRIM_INFO            = 4,
    VM_BLE_LOCAL_INFO           = 5,
    CFG_BT_FRE_OFFSET           = 6,    //BT 频偏
    VM_PMU_VOLTAGE              = 7,
    LIB_SYSMEM_VIR_RTC_TIME     = 8,
    LIB_SYSMEM_VIR_RTC_ALM      = 9,
    LIB_SYSMEM_VIR_RTC_CNT      = 10,
    LIB_SYSMEM_SDK_LASTEST      = 11,

    //蓝牙类配置项[]
    CFG_BLE_MODE_INFO           = 15,
    CFG_TWS_PAIR_AA,
    CFG_TWS_CONNECT_AA,
    CFG_TWS_LOCAL_ADDR,
    VM_DMA_RAND,
    VM_TME_AUTH_COOKIE,
    CFG_REMOTE_DB_00,
    VM_BLE_REMOTE_DB_INFO,
    VM_BLE_REMOTE_DB_00,
    VM_BLE_REMOTE_DB_01,
    VM_BLE_REMOTE_DB_02,
    VM_BLE_REMOTE_DB_03,
    VM_BLE_REMOTE_DB_04,
    VM_BLE_REMOTE_DB_05,
    VM_BLE_REMOTE_DB_06,
    VM_BLE_REMOTE_DB_07,
    VM_BLE_REMOTE_DB_08,
    VM_BLE_REMOTE_DB_09,

    LIB_SYSMEM_END = 64,
    //================================================================================//

    //=========================btif & cfg_tool.bin====================================//
    CFG_BT_NAME,
    CFG_BT_MAC_ADDR,
    CFG_BT_RF_POWER_ID,
    CFG_LRC_ID,
    //================================================================================//
    //
    // 以下用户可以任意修改顺序或添加
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

    // SYSMEM_LAST_INDEX = SYSMEM_INDEX_MAX,
    //===============================================================================//
    //                           蓝牙应用层库使用到                                  //
    //===============================================================================//
    CFG_AAP_MODE_INFO,
    CFG_BLE_BONDING_REMOTE_INFO,
    CFG_BLE_BONDING_REMOTE_INFO2,

} SYSMEM_INDEX;

int sysmem_init_api(u32 mem_addr, u32 mem_size);
int sysmem_read_api(u32 id, u8 *data_buf, u16 len);
int sysmem_write_api(u32 id, u8 *data_buf, u16 len);
void sysmem_pre_erase_api(void);
void sysmem_delete_api(u32 *delete_map, u32 delete_bits);

// 蓝牙vm使用接口
int syscfg_write(u16 item_id, const void *buf, u16 len);
int syscfg_read(u16 item_id, void *buf, u16 len);

#endif

