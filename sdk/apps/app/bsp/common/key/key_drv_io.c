#include "key_drv_io.h"
#include "app_config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[key_io]"
#include "log.h"


#if KEY_IO_EN

/*----------------------------------------------------------------------------*/
/**@brief   按键去抖函数，输出稳定键值
   @param   key：键值
   @return  稳定按键
   @note    u8 key_filter(u8 key)
*/
/*----------------------------------------------------------------------------*/
static u8 key_filter(u8 key)
{
    static u8 used_key = NO_KEY;
    static u8 old_key;
    static u8 key_counter;

    if (old_key != key) {
        key_counter = 0;
        old_key = key;
    } else {
        key_counter++;
        if (key_counter == KEY_BASE_CNT) {
            used_key = key;
        }
    }
    return used_key;
}

/*----------------------------------------------------------------------------*/
/**@brief   io按键初始化
   @param   void
   @param   void
   @return  void
   @note    void io_key_init(void)
*/
/*----------------------------------------------------------------------------*/
void io_key_init(void)
{
    KEY_INIT();
}

/*----------------------------------------------------------------------------*/
/**@brief   获取IO按键电平值
   @param   void
   @param   void
   @return  key_num:io按键号
   @note    u8 get_iokey_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 get_iokey_value(void)
{
    //key_puts("get_iokey_value\n");
    u8 key_num = NO_KEY;

    if (IS_KEY0_DOWN()) {
        key_puts(" KEY0 ");
        key_num = 0;
    } else if (IS_KEY1_DOWN()) {
        key_puts(" KEY1 ");
        key_num = 1;
    } else if (IS_KEY2_DOWN()) {
        key_puts(" KEY2 ");
        key_num = 2;
    } else if (IS_KEY3_DOWN()) {
        key_puts(" KEY3 ");
        key_num = 3;
    } else if (IS_KEY4_DOWN()) {
        key_puts(" KEY4 ");
        key_num = 4;
    } else if (IS_KEY5_DOWN()) {
        key_puts(" KEY5 ");
        key_num = 5;
    } else if (IS_KEY6_DOWN()) {
        key_puts(" KEY6 ");
        key_num = 6;
    } else if (IS_KEY7_DOWN()) {
        key_puts(" KEY7 ");
        key_num = 7;
    }

    return key_filter(key_num);
}

const key_interface_t key_io_info = {
    .key_type = KEY_TYPE_IO,
    .key_init = io_key_init,
    .key_get_value = get_iokey_value,
};

#endif/*KEY_IO_EN*/
