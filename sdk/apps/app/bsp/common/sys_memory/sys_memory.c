#include "sys_memory.h"
#include "app_config.h"
#include "cpu.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[SYS_MEM]"
#include "log.h"

int sysmem_init_api(u32 mem_addr, u32 mem_size)
{
    return sysmem_phy_init(mem_addr, mem_size);
}

int sysmem_read_api(u32 id, u8 *data_buf, u16 len)
{
    return sysmem_phy_read(id, data_buf, len);
}

int sysmem_write_api(u32 id, u8 *data_buf, u16 len)
{
    return sysmem_phy_write(id, data_buf, len);
}

void sysmem_pre_erase_api(void)
{
    sysmem_phy_pre_erase();
}
