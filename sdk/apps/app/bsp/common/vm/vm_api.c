#include "vm_api.h"
#include "app_modules.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

extern const char config_use_new_vm;

u32 syscfg_vm_init(u32 addr, u32 size)
{
    u32 res = 0;
    if (config_use_new_vm) {
        res = nvm_init_api(addr, size);
    } else {
        res = syscfg_vm_init_phy(addr, size);
    }

    return res;
}
u32 vm_read(u32 id, u8 *buf, u32 len)
{
    u32 res = 0;
    if (config_use_new_vm) {
        res = nvm_read_api(id, buf, len);
    } else {
        res = vm_read_phy(id, buf, len);
    }

    return res;
}
u32 vm_write(u32 id, u8 *buf, u32 len)
{
    u32 res = 0;
    if (config_use_new_vm) {
        res = nvm_write_api(id, buf, len);
    } else {
        res = vm_write_phy(id, buf, len);
    }

    return res;
}
u32 vm_pre_erase()
{
    u32 res = 0;
    if (config_use_new_vm) {
        nvm_erasure_next_api();
    } else {
        res = -1;
        /* log_info("old vm version dosen't have this "); */
    }

    return res;
}

#define LABEL_INDEX_LEN_CRC_SIZE        (4)
static u8 vm_buff[sizeof(VM_INDEX_BUFF) + LABEL_INDEX_LEN_CRC_SIZE];
u16 vm_buff_alloc(u8 **buf)
{
    if (buf == NULL) {
        return 0;
    }

    *buf = vm_buff;
    return sizeof(vm_buff);
}
