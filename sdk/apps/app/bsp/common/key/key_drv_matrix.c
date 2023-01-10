#include "key_drv_matrix.h"
#include "app_config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[key_matrix]"
#include "log.h"


#if KEY_MATRIX_EN

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
/**@brief   matrix按键初始化
   @param   void
   @param   void
   @return  void
   @note    void matrix_key_init(void)
*/
/*----------------------------------------------------------------------------*/
void matrix_key_init(void)
{
    u8 i = 0;
#if 0
    for (i = 0; i < sizeof(matrix_key_row); i++) { //key 输出模式
        gpio_direction_output(matrix_key_row[i], !(KEY_DETECT_LEVEL));
    }
#else
    for (i = 0; i < sizeof(matrix_key_row); i++) { //key 高祖模式
        gpio_set_pull_up(matrix_key_row[i], 0);
        gpio_set_pull_down(matrix_key_row[i], 0);
        gpio_set_direction(matrix_key_row[i], 1);
        gpio_set_die(matrix_key_row[i], 0);
        gpio_set_dieh(matrix_key_row[i], 0);
    }
#endif
    for (i = 0; i < sizeof(matrix_key_col); i++) {
        gpio_set_direction(matrix_key_col[i], 1);
#if KEY_DETECT_LEVEL
        gpio_set_pull_up(matrix_key_col[i], 0);
        gpio_set_pull_down(matrix_key_col[i], 1);
#else
        gpio_set_pull_up(matrix_key_col[i], 1);
        gpio_set_pull_down(matrix_key_col[i], 0);
#endif
        gpio_set_die(matrix_key_col[i], 1);
    }
}
void matrix_key_suspend()
{
    matrix_key_init();
}
void matrix_key_release()
{
    u8 i = 0;
    for (i = 0; i < sizeof(matrix_key_row); i++) {
        gpio_set_pull_up(matrix_key_row[i], 0);
        gpio_set_pull_down(matrix_key_row[i], 0);
        gpio_set_direction(matrix_key_row[i], 1);
        gpio_set_die(matrix_key_row[i], 0);
        gpio_set_dieh(matrix_key_row[i], 0);
    }
    for (i = 0; i < sizeof(matrix_key_col); i++) {
        gpio_set_pull_up(matrix_key_col[i], 0);
        gpio_set_pull_down(matrix_key_col[i], 0);
        gpio_set_direction(matrix_key_col[i], 1);
        gpio_set_die(matrix_key_col[i], 0);
        gpio_set_dieh(matrix_key_col[i], 0);
    }
}


#if 0
u8 matrix_key_scan()//key 输出模式
{
    /*{{{*/
    u8 i = 0;
    u8 j = 0;
    u8 key_val = 0xff;
    for (i = 0; i < sizeof(matrix_key_row); i++) {
        gpio_write(matrix_key_row[i], KEY_DETECT_LEVEL);

        for (j = 0; j < sizeof(matrix_key_col); j++) {
            if (gpio_read(matrix_key_col[j]) == KEY_DETECT_LEVEL) {
                key_val = i * sizeof(matrix_key_col) + j;
                gpio_write(matrix_key_row[i], !(KEY_DETECT_LEVEL));
                printf("(%d %d,%d)", key_val, i, j);
                return key_val;
            }
        }
        gpio_write(matrix_key_row[i], !(KEY_DETECT_LEVEL));
    }
    printf("- ");
    return key_val;
}/*}}}*/
#else
u8 matrix_key_scan()//key 高祖模式
{
    local_irq_disable();
    u8 i = 0;
    u8 j = 0;
    u8 key_val = NO_KEY;
    for (i = 0; i < sizeof(matrix_key_row); i++) {
        gpio_direction_output(matrix_key_row[i], KEY_DETECT_LEVEL);

        for (j = 0; j < sizeof(matrix_key_col); j++) {
            if (gpio_read(matrix_key_col[j]) == KEY_DETECT_LEVEL) {
                matrix_key_row_value &= ~ 1 << i;
                matrix_key_col_value &= ~ 1 << j;
                key_val = i * sizeof(matrix_key_col) + j;
                /* key_puts("(%d %d,%d)",key_val,i,j); */

                /* gpio_set_direction(matrix_key_row[i],1); */
                /* local_irq_enable(); */
                /* return key_val; */
            }
        }
        gpio_set_direction(matrix_key_row[i], 1);
        /* delay(250);//500:62.8us */
    }
    /* key_puts("- "); */
    local_irq_enable();
    return key_val;
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief   获取matrix按键电平值
   @param   void
   @param   void
   @return  key_num:matrix按键号
   @note    u8 _get_matrixkey_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 _get_matrixkey_value(void)
{
    //key_puts("_get_matrixkey_value\n");
#if 0//KEY_MATRIX_LCD_REUSE_EN
    u8 key_num = 0xff;
    static u8 key_new = 0xff, key_old = 0xff;
    static u8 key_state_cnt = 0;
    key_new = matrix_key_scan();
    if (key_old == key_new) {
        key_state_cnt++;
    } else {
        key_state_cnt = 0;
    }
    if (key_state_cnt > 5) {
        key_num = key_new;
        key_state_cnt = 0;
    }
    key_old = key_new;

    return key_num;
#else
    u8 key_num = matrix_key_scan();
    return key_filter(key_num);
#endif
}
u8 get_matrixkey_value(void)
{
    //key_puts("_get_matrixkey_value\n");
#if KEY_MATRIX_LCD_REUSE_EN
    u8 get_lcd_matrix_key_value();
    return get_lcd_matrix_key_value();
#else
    u8 key_num = matrix_key_scan();
    return key_filter(key_num);
#endif
}

const key_interface_t key_matrix_info = {
    .key_type = KEY_TYPE_MATRIX,
    .key_init = matrix_key_init,
    .key_get_value = get_matrixkey_value,
};

#endif/*KEY_MATRIX_EN*/
